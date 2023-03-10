/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGradientFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/*----------------------------------------------------------------------------
 Copyright (c) Sandia Corporation
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.
----------------------------------------------------------------------------*/

#include "vtkGradientFilter.h"

#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkCellDataToPointData.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkIdList.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredGrid.h"
#include "vtkUnstructuredGrid.h"

#include <limits>
#include <vector>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkGradientFilter);

namespace
{
// special template macro for only float and double types since we will never
// have other data types for output arrays and this helps with reducing template expansion
#define vtkFloatingPointTemplateMacro(call)         \
  vtkTemplateMacroCase(VTK_DOUBLE, double, call);   \
  vtkTemplateMacroCase(VTK_FLOAT, float, call);

// helper function to replace the gradient of a vector
// with the vorticity/curl of that vector
//-----------------------------------------------------------------------------
  template<class data_type>
  void ComputeVorticityFromGradient(data_type* gradients, data_type* vorticity)
  {
    vorticity[0] = gradients[7] - gradients[5];
    vorticity[1] = gradients[2] - gradients[6];
    vorticity[2] = gradients[3] - gradients[1];
  }

  template<class data_type>
  void ComputeDivergenceFromGradient(data_type* gradients, data_type* divergence)
  {
    divergence[0] = gradients[0]+gradients[4]+gradients[8];
  }

  template<class data_type>
  void ComputeQCriterionFromGradient(data_type* gradients, data_type* qCriterion)
  {
    // see http://public.kitware.com/pipermail/paraview/2015-May/034233.html for
    // paper citation and formula on Q-criterion.
    qCriterion[0] =
      - (gradients[0]*gradients[0]+gradients[4]*gradients[4]+gradients[8]*gradients[8])/2.
      - (gradients[1]*gradients[3]+gradients[2]*gradients[6]+gradients[5]*gradients[7]);
  }

  // Functions for unstructured grids and polydatas
  template<class data_type>
  void ComputePointGradientsUG(
    vtkDataSet *structure, vtkDataArray *array, data_type *gradients,
    int numberOfInputComponents, data_type* vorticity, data_type* qCriterion,
    data_type* divergence, int highestCellDimension, int contributingCellOption);

  int GetCellParametricData(
    vtkIdType pointId, double pointCoord[3], vtkCell *cell, int & subId,
    double parametricCoord[3]);

  template<class data_type>
  void ComputeCellGradientsUG(
    vtkDataSet *structure, vtkDataArray *array, data_type *gradients,
    int numberOfInputComponents, data_type* vorticity, data_type* qCriterion,
    data_type* divergence);

  // Functions for image data and structured grids
  template<class Grid, class data_type>
  void ComputeGradientsSG(Grid output, vtkDataArray* array, data_type* gradients,
                          int numberOfInputComponents, int fieldAssociation,
                          data_type* vorticity, data_type* qCriterion,
                          data_type* divergence);

  bool vtkGradientFilterHasArray(vtkFieldData *fieldData,
                                 vtkDataArray *array)
  {
    int numarrays = fieldData->GetNumberOfArrays();
    for (int i = 0; i < numarrays; i++)
    {
      if (array == fieldData->GetArray(i))
      {
        return true;
      }
    }
    return false;
  }

  // generic way to get the coordinate for either a cell (using
  // the parametric center) or a point
  void GetGridEntityCoordinate(vtkDataSet* grid, int fieldAssociation,
                               vtkIdType index, double coords[3])
  {
    if(fieldAssociation == vtkDataObject::FIELD_ASSOCIATION_POINTS)
    {
      grid->GetPoint(index, coords);
    }
    else
    {
      vtkCell* cell = grid->GetCell(index);
      double pcoords[3];
      int subId = cell->GetParametricCenter(pcoords);
      std::vector<double> weights(cell->GetNumberOfPoints()+1);
      cell->EvaluateLocation(subId, pcoords, coords, &weights[0]);
    }
  }

  template<class data_type>
  void Fill(vtkDataArray* array, data_type vtkNotUsed(data), int replacementValueOption)
  {
    switch (replacementValueOption)
    {
    case vtkGradientFilter::Zero:
      array->Fill(0);
      return;
    case vtkGradientFilter::NaN:
      array->Fill(vtkMath::Nan());
      return;
    case vtkGradientFilter::DataTypeMin:
      array->Fill(std::numeric_limits<data_type>::min());
      return;
    case vtkGradientFilter::DataTypeMax:
      array->Fill(std::numeric_limits<data_type>::max());
      return;
    }
  }
  template<class data_type>
  int GetOutputDataType(data_type vtkNotUsed(data))
  {
    if(sizeof(data_type)>=8)
    {
      return VTK_DOUBLE;
    }
    return VTK_FLOAT;
  }
} // end anonymous namespace

//-----------------------------------------------------------------------------
vtkGradientFilter::vtkGradientFilter()
{
  this->ResultArrayName = nullptr;
  this->DivergenceArrayName = nullptr;
  this->VorticityArrayName = nullptr;
  this->QCriterionArrayName = nullptr;
  this->FasterApproximation = 0;
  this->ComputeGradient = 1;
  this->ComputeDivergence = 0;
  this->ComputeVorticity = 0;
  this->ComputeQCriterion = 0;
  this->ContributingCellOption = vtkGradientFilter::All;
  this->ReplacementValueOption = vtkGradientFilter::Zero;
  this->SetInputScalars(vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS,
                        vtkDataSetAttributes::SCALARS);
}

//-----------------------------------------------------------------------------
vtkGradientFilter::~vtkGradientFilter()
{
  this->SetResultArrayName(nullptr);
  this->SetDivergenceArrayName(nullptr);
  this->SetVorticityArrayName(nullptr);
  this->SetQCriterionArrayName(nullptr);
}

//-----------------------------------------------------------------------------
void vtkGradientFilter::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ResultArrayName:"
     << (this->ResultArrayName ? this->ResultArrayName : "Gradients") << endl;
  os << indent << "DivergenceArrayName:"
     << (this->DivergenceArrayName ? this->DivergenceArrayName : "Divergence") << endl;
  os << indent << "VorticityArrayName:"
     << (this->VorticityArrayName ? this->VorticityArrayName : "Vorticity") << endl;
  os << indent << "QCriterionArrayName:"
     << (this->QCriterionArrayName ? this->QCriterionArrayName : "Q-criterion") << endl;
  os << indent << "FasterApproximation:" << this->FasterApproximation << endl;
  os << indent << "ComputeGradient:"  << this->ComputeGradient << endl;
  os << indent << "ComputeDivergence:"  << this->ComputeDivergence << endl;
  os << indent << "ComputeVorticity:" << this->ComputeVorticity << endl;
  os << indent << "ComputeQCriterion:" << this->ComputeQCriterion << endl;
  os << indent << "ContributingCellOption:" << this->ContributingCellOption << endl;
  os << indent << "ReplacementValueOption:" << this->ReplacementValueOption << endl;
}

//-----------------------------------------------------------------------------
void vtkGradientFilter::SetInputScalars(int fieldAssociation, const char *name)
{
  if (   (fieldAssociation != vtkDataObject::FIELD_ASSOCIATION_POINTS)
      && (fieldAssociation != vtkDataObject::FIELD_ASSOCIATION_CELLS)
      && (fieldAssociation != vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS) )
  {
    vtkErrorMacro("Input Array must be associated with points or cells.");
    return;
  }

  this->SetInputArrayToProcess(0, 0, 0, fieldAssociation, name);
}

//-----------------------------------------------------------------------------
void vtkGradientFilter::SetInputScalars(int fieldAssociation,
                                        int fieldAttributeType)
{
  if (   (fieldAssociation != vtkDataObject::FIELD_ASSOCIATION_POINTS)
      && (fieldAssociation != vtkDataObject::FIELD_ASSOCIATION_CELLS)
      && (fieldAssociation != vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS) )
  {
    vtkErrorMacro("Input Array must be associated with points or cells.");
    return;
  }

  this->SetInputArrayToProcess(0, 0, 0, fieldAssociation, fieldAttributeType);
}

//-----------------------------------------------------------------------------
int vtkGradientFilter::RequestUpdateExtent(vtkInformation *vtkNotUsed(request),
                                           vtkInformationVector **inputVector,
                                           vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // Technically, this code is only correct for pieces extent types.  However,
  // since this class is pretty inefficient for data types that use 3D extents,
  // we'll punt on the ghost levels for them, too.
  int piece, numPieces, ghostLevels;

  piece = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
  numPieces = outInfo->Get(
                   vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());
  ghostLevels = outInfo->Get(
             vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS());

  if (numPieces > 1)
  {
    ++ghostLevels;
  }

  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER(), piece);
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES(),
              numPieces);
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS(),
              ghostLevels);
  inInfo->Set(vtkStreamingDemandDrivenPipeline::EXACT_EXTENT(), 1);

  return 1;
}

//-----------------------------------------------------------------------------
int vtkGradientFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                   vtkInformationVector **inputVector,
                                   vtkInformationVector *outputVector)
{
  vtkDebugMacro("RequestData");

  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkDataSet *input
    = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkDataSet *output
    = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkDataArray *array = this->GetInputArrayToProcess(0, inputVector);

  if (input->GetNumberOfCells() == 0)
  {
    // need cells to compute the gradient so if we don't have cells. we can't compute anything.
    // if we have points and an array with values provide a warning letting the user know that
    // no gradient will be computed because of the lack of cells. otherwise the dataset is
    // assumed empty and we can skip providing a warning message to the user.
    if (input->GetNumberOfPoints() && array && array->GetNumberOfTuples())
    {
      vtkWarningMacro("Cannot compute gradient for datasets without cells");
    }
    output->ShallowCopy(input);
    return 1;
  }

  if (array == nullptr)
  {
    vtkErrorMacro("No input array. If this dataset is part of a composite dataset"
                  << " check to make sure that all non-empty blocks have this array.");
    return 0;
  }
  if (array->GetNumberOfComponents() == 0)
  {
    vtkErrorMacro("Input array must have at least one component.");
    return 0;
  }

  // we can only compute vorticity and Q criterion if the input
  // array has 3 components. if we can't compute them because of
  // this we only mark internally the we aren't computing them
  // since we don't want to change the state of the filter.
  bool computeVorticity = this->ComputeVorticity != 0;
  bool computeDivergence = this->ComputeDivergence != 0;
  bool computeQCriterion = this->ComputeQCriterion != 0;
  if( (this->ComputeQCriterion || this->ComputeVorticity ||
       this->ComputeDivergence) && array->GetNumberOfComponents() != 3)
  {
    vtkWarningMacro("Input array must have exactly three components with "
                    << "ComputeDivergence, ComputeVorticity or ComputeQCriterion flag enabled."
                    << "Skipping divergence, vorticity and Q-criterion computation.");
    computeVorticity = false;
    computeQCriterion = false;
    computeDivergence = false;
  }

  int fieldAssociation;
  if (vtkGradientFilterHasArray(input->GetPointData(), array))
  {
    fieldAssociation = vtkDataObject::FIELD_ASSOCIATION_POINTS;
  }
  else if (vtkGradientFilterHasArray(input->GetCellData(), array))
  {
    fieldAssociation = vtkDataObject::FIELD_ASSOCIATION_CELLS;
  }
  else
  {
    vtkErrorMacro("Input arrays do not seem to be either point or cell arrays.");
    return 0;
  }

  output->CopyStructure(input);
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());

  if(output->IsA("vtkImageData") || output->IsA("vtkStructuredGrid") ||
          output->IsA("vtkRectilinearGrid") )
  {
    this->ComputeRegularGridGradient(
      array, fieldAssociation, computeVorticity, computeQCriterion,
      computeDivergence, output);
  }
  else
  {
    this->ComputeUnstructuredGridGradient(
      array, fieldAssociation, input, computeVorticity, computeQCriterion,
      computeDivergence, output);
  }

  return 1;
}

//-----------------------------------------------------------------------------
int vtkGradientFilter::ComputeUnstructuredGridGradient(
  vtkDataArray* array, int fieldAssociation, vtkDataSet* input,
  bool computeVorticity, bool computeQCriterion, bool computeDivergence,
  vtkDataSet* output)
{
  int arrayType = this->GetOutputArrayType(array);
  int numberOfInputComponents = array->GetNumberOfComponents();
  vtkSmartPointer<vtkDataArray> gradients = nullptr;
  if(this->ComputeGradient)
  {
    gradients.TakeReference(vtkDataArray::CreateDataArray(arrayType));
    gradients->SetNumberOfComponents(3*numberOfInputComponents);
    gradients->SetNumberOfTuples(array->GetNumberOfTuples());
    switch (arrayType)
    {
      vtkFloatingPointTemplateMacro(Fill(gradients, static_cast<VTK_TT>(0), this->ReplacementValueOption));
    }
    if (this->ResultArrayName)
    {
      gradients->SetName(this->ResultArrayName);
    }
    else
    {
      gradients->SetName("Gradients");
    }
  }
  vtkSmartPointer<vtkDataArray> divergence = nullptr;
  if(computeDivergence)
  {
    divergence.TakeReference(vtkDataArray::CreateDataArray(arrayType));
    divergence->SetNumberOfTuples(array->GetNumberOfTuples());
    switch (arrayType)
    {
      vtkFloatingPointTemplateMacro(Fill(divergence, static_cast<VTK_TT>(0), this->ReplacementValueOption));
    }
    if (this->DivergenceArrayName)
    {
      divergence->SetName(this->DivergenceArrayName);
    }
    else
    {
      divergence->SetName("Divergence");
    }
  }
  vtkSmartPointer<vtkDataArray> vorticity;
  if(computeVorticity)
  {
    vorticity.TakeReference(vtkDataArray::CreateDataArray(arrayType));
    vorticity->SetNumberOfComponents(3);
    vorticity->SetNumberOfTuples(array->GetNumberOfTuples());
    switch (arrayType)
    {
      vtkFloatingPointTemplateMacro(Fill(vorticity, static_cast<VTK_TT>(0), this->ReplacementValueOption));
    }
    if (this->VorticityArrayName)
    {
      vorticity->SetName(this->VorticityArrayName);
    }
    else
    {
      vorticity->SetName("Vorticity");
    }
  }
  vtkSmartPointer<vtkDataArray> qCriterion;
  if(computeQCriterion)
  {
    qCriterion.TakeReference(vtkDataArray::CreateDataArray(arrayType));
    qCriterion->SetNumberOfTuples(array->GetNumberOfTuples());
    switch (arrayType)
    {
      vtkFloatingPointTemplateMacro(Fill(qCriterion, static_cast<VTK_TT>(0), this->ReplacementValueOption));
    }
    if (this->QCriterionArrayName)
    {
      qCriterion->SetName(this->QCriterionArrayName);
    }
    else
    {
      qCriterion->SetName("Q-criterion");
    }
  }

  int highestCellDimension = 0;
  if (this->ContributingCellOption == vtkGradientFilter::DataSetMax)
  {
    int maxDimension = input->IsA("vtkPolyData") == 1 ? 2 : 3;
    for (vtkIdType i=0;i<input->GetNumberOfCells();i++)
    {
      int dim = input->GetCell(i)->GetCellDimension();
      if (dim > highestCellDimension)
      {
        highestCellDimension = dim;
        if (highestCellDimension == maxDimension)
        {
          break;
        }
      }
    }
  }

  if (fieldAssociation == vtkDataObject::FIELD_ASSOCIATION_POINTS)
  {
    if (!this->FasterApproximation)
    {
      switch (arrayType)
      { // ok to use template macro here since we made the output arrays ourselves
        vtkFloatingPointTemplateMacro(ComputePointGradientsUG(
                           input, array,
                           (gradients == nullptr ? nullptr :
                            static_cast<VTK_TT *>(gradients->GetVoidPointer(0))),
                           numberOfInputComponents,
                           (vorticity == nullptr ? nullptr :
                            static_cast<VTK_TT *>(vorticity->GetVoidPointer(0))),
                           (qCriterion == nullptr ? nullptr :
                            static_cast<VTK_TT *>(qCriterion->GetVoidPointer(0))),
                           (divergence == nullptr ? nullptr :
                            static_cast<VTK_TT *>(divergence->GetVoidPointer(0))),
                           highestCellDimension, this->ContributingCellOption));
      }
      if(gradients)
      {
        output->GetPointData()->AddArray(gradients);
      }
      if(divergence)
      {
        output->GetPointData()->AddArray(divergence);
      }
      if(vorticity)
      {
        output->GetPointData()->AddArray(vorticity);
      }
      if(qCriterion)
      {
        output->GetPointData()->AddArray(qCriterion);
      }
    }
    else // this->FasterApproximation
    {
      // The cell computation is faster and works off of point data anyway.  The
      // faster approximation is to use the cell algorithm and then convert the
      // result to point data.
      vtkSmartPointer<vtkDataArray> cellGradients = nullptr;
      if(gradients)
      {
        cellGradients.TakeReference(vtkDataArray::CreateDataArray(arrayType));
        cellGradients->SetName(gradients->GetName());
        cellGradients->SetNumberOfComponents(3*array->GetNumberOfComponents());
        cellGradients->SetNumberOfTuples(input->GetNumberOfCells());
      }
      vtkSmartPointer<vtkDataArray> cellDivergence = nullptr;
      if(divergence)
      {
        cellDivergence.TakeReference(vtkDataArray::CreateDataArray(arrayType));
        cellDivergence->SetName(divergence->GetName());
        cellDivergence->SetNumberOfTuples(input->GetNumberOfCells());
      }
      vtkSmartPointer<vtkDataArray> cellVorticity = nullptr;
      if(vorticity)
      {
        cellVorticity.TakeReference(vtkDataArray::CreateDataArray(arrayType));
        cellVorticity->SetName(vorticity->GetName());
        cellVorticity->SetNumberOfComponents(3);
        cellVorticity->SetNumberOfTuples(input->GetNumberOfCells());
      }
      vtkSmartPointer<vtkDataArray> cellQCriterion = nullptr;
      if(qCriterion)
      {
        cellQCriterion.TakeReference(vtkDataArray::CreateDataArray(arrayType));
        cellQCriterion->SetName(qCriterion->GetName());
        cellQCriterion->SetNumberOfTuples(input->GetNumberOfCells());
      }

      switch (arrayType)
      { // ok to use template macro here since we made the output arrays ourselves
        vtkFloatingPointTemplateMacro(
          ComputeCellGradientsUG(
            input, array,
            (cellGradients == nullptr ? nullptr :
             static_cast<VTK_TT *>(cellGradients->GetVoidPointer(0))),
            numberOfInputComponents,
            (vorticity == nullptr ? nullptr :
             static_cast<VTK_TT *>(cellVorticity->GetVoidPointer(0))),
            (qCriterion == nullptr ? nullptr :
             static_cast<VTK_TT *>(cellQCriterion->GetVoidPointer(0))),
            (divergence == nullptr ? nullptr :
             static_cast<VTK_TT *>(cellDivergence->GetVoidPointer(0)))));
      }

      // We need to convert cell Array to points Array.
      vtkSmartPointer<vtkDataSet> dummy;
      dummy.TakeReference(input->NewInstance());
      dummy->CopyStructure(input);
      if(cellGradients)
      {
        dummy->GetCellData()->AddArray(cellGradients);
      }
      if(divergence)
      {
        dummy->GetCellData()->AddArray(cellDivergence);
      }
      if(vorticity)
      {
        dummy->GetCellData()->AddArray(cellVorticity);
      }
      if(qCriterion)
      {
        dummy->GetCellData()->AddArray(cellQCriterion);
      }

      vtkNew<vtkCellDataToPointData> cd2pd;
      cd2pd->SetInputData(dummy);
      cd2pd->PassCellDataOff();
      cd2pd->SetContributingCellOption(this->ContributingCellOption);
      cd2pd->Update();

      // Set the gradients array in the output and cleanup.
      if(gradients)
      {
        output->GetPointData()->AddArray(
          cd2pd->GetOutput()->GetPointData()->GetArray(gradients->GetName()));
      }
      if(qCriterion)
      {
        output->GetPointData()->AddArray(
          cd2pd->GetOutput()->GetPointData()->GetArray(qCriterion->GetName()));
      }
      if(divergence)
      {
        output->GetPointData()->AddArray(
          cd2pd->GetOutput()->GetPointData()->GetArray(divergence->GetName()));
      }
      if(vorticity)
      {
        output->GetPointData()->AddArray(
          cd2pd->GetOutput()->GetPointData()->GetArray(vorticity->GetName()));
      }
    }
  }
  else  // fieldAssociation == vtkDataObject::FIELD_ASSOCIATION_CELLS
  {
    // We need to convert cell Array to points Array.
    vtkSmartPointer<vtkDataSet> dummy;
    dummy.TakeReference(input->NewInstance());
    dummy->CopyStructure(input);
    dummy->GetCellData()->SetScalars(array);

    vtkNew<vtkCellDataToPointData> cd2pd;
    cd2pd->SetInputData(dummy);
    cd2pd->PassCellDataOff();
    cd2pd->SetContributingCellOption(this->ContributingCellOption);
    cd2pd->Update();
    vtkDataArray *pointScalars
      = cd2pd->GetOutput()->GetPointData()->GetScalars();
    pointScalars->Register(this);

    switch (arrayType)
    { // ok to use template macro here since we made the output arrays ourselves
      vtkFloatingPointTemplateMacro(ComputeCellGradientsUG(
                         input, pointScalars,
                         (gradients == nullptr ? nullptr :
                          static_cast<VTK_TT *>(gradients->GetVoidPointer(0))),
                         numberOfInputComponents,
                         (vorticity == nullptr ? nullptr :
                          static_cast<VTK_TT *>(vorticity->GetVoidPointer(0))),
                         (qCriterion == nullptr ? nullptr :
                          static_cast<VTK_TT *>(qCriterion->GetVoidPointer(0))),
                         (divergence == nullptr ? nullptr :
                          static_cast<VTK_TT *>(divergence->GetVoidPointer(0)))));
    }

    if(gradients)
    {
      output->GetCellData()->AddArray(gradients);
    }
    if(vorticity)
    {
      output->GetCellData()->AddArray(vorticity);
    }
    if(divergence)
    {
      output->GetCellData()->AddArray(divergence);
    }
    if(qCriterion)
    {
      output->GetCellData()->AddArray(qCriterion);
    }
    pointScalars->UnRegister(this);
  }

  return 1;
}

//-----------------------------------------------------------------------------
int vtkGradientFilter::ComputeRegularGridGradient(
  vtkDataArray* array, int fieldAssociation, bool computeVorticity,
  bool computeQCriterion, bool computeDivergence, vtkDataSet* output)
{
  int arrayType = this->GetOutputArrayType(array);
  int numberOfInputComponents = array->GetNumberOfComponents();
  vtkSmartPointer<vtkDataArray> gradients = nullptr;
  if(this->ComputeGradient)
  {
    gradients.TakeReference(vtkDataArray::CreateDataArray(arrayType));
    gradients->SetNumberOfComponents(3*numberOfInputComponents);
    gradients->SetNumberOfTuples(array->GetNumberOfTuples());
    if (this->ResultArrayName)
    {
      gradients->SetName(this->ResultArrayName);
    }
    else
    {
      gradients->SetName("Gradients");
    }
  }
  vtkSmartPointer<vtkDataArray> divergence = nullptr;
  if(computeDivergence)
  {
    divergence.TakeReference(vtkDataArray::CreateDataArray(arrayType));
    divergence->SetNumberOfTuples(array->GetNumberOfTuples());
    if (this->DivergenceArrayName)
    {
      divergence->SetName(this->DivergenceArrayName);
    }
    else
    {
      divergence->SetName("Divergence");
    }
  }
  vtkSmartPointer<vtkDataArray> vorticity;
  if(computeVorticity)
  {
    vorticity.TakeReference(vtkDataArray::CreateDataArray(arrayType));
    vorticity->SetNumberOfComponents(3);
    vorticity->SetNumberOfTuples(array->GetNumberOfTuples());
    if (this->VorticityArrayName)
    {
      vorticity->SetName(this->VorticityArrayName);
    }
    else
    {
      vorticity->SetName("Vorticity");
    }
  }
  vtkSmartPointer<vtkDataArray> qCriterion;
  if(computeQCriterion)
  {
    qCriterion.TakeReference(vtkDataArray::CreateDataArray(arrayType));
    qCriterion->SetNumberOfTuples(array->GetNumberOfTuples());
    if (this->QCriterionArrayName)
    {
      qCriterion->SetName(this->QCriterionArrayName);
    }
    else
    {
      qCriterion->SetName("Q-criterion");
    }
  }

  if(vtkStructuredGrid* structuredGrid = vtkStructuredGrid::SafeDownCast(output))
  {
    switch (arrayType)
    { // ok to use template macro here since we made the output arrays ourselves
      vtkFloatingPointTemplateMacro(ComputeGradientsSG(
                         structuredGrid, array,
                         (gradients == nullptr ? nullptr :
                          static_cast<VTK_TT *>(gradients->GetVoidPointer(0))),
                         numberOfInputComponents, fieldAssociation,
                         (vorticity == nullptr ? nullptr :
                          static_cast<VTK_TT *>(vorticity->GetVoidPointer(0))),
                         (qCriterion == nullptr ? nullptr :
                          static_cast<VTK_TT *>(qCriterion->GetVoidPointer(0))),
                         (divergence == nullptr ? nullptr :
                          static_cast<VTK_TT *>(divergence->GetVoidPointer(0)))));

    }
  }
  else if(vtkImageData* imageData = vtkImageData::SafeDownCast(output))
  {
    switch (arrayType)
    { // ok to use template macro here since we made the output arrays ourselves
      vtkFloatingPointTemplateMacro(ComputeGradientsSG(
                         imageData, array,
                         (gradients == nullptr ? nullptr :
                          static_cast<VTK_TT *>(gradients->GetVoidPointer(0))),
                         numberOfInputComponents, fieldAssociation,
                         (vorticity == nullptr ? nullptr :
                          static_cast<VTK_TT *>(vorticity->GetVoidPointer(0))),
                         (qCriterion == nullptr ? nullptr :
                          static_cast<VTK_TT *>(qCriterion->GetVoidPointer(0))),
                         (divergence == nullptr ? nullptr :
                          static_cast<VTK_TT *>(divergence->GetVoidPointer(0)))));
    }
  }
  else if(vtkRectilinearGrid* rectilinearGrid = vtkRectilinearGrid::SafeDownCast(output))
  {
    switch (arrayType)
    { // ok to use template macro here since we made the output arrays ourselves
      vtkFloatingPointTemplateMacro(ComputeGradientsSG(
                         rectilinearGrid, array,
                         (gradients == nullptr ? nullptr :
                          static_cast<VTK_TT *>(gradients->GetVoidPointer(0))),
                         numberOfInputComponents, fieldAssociation,
                         (vorticity == nullptr ? nullptr :
                          static_cast<VTK_TT *>(vorticity->GetVoidPointer(0))),
                         (qCriterion == nullptr ? nullptr :
                          static_cast<VTK_TT *>(qCriterion->GetVoidPointer(0))),
                         (divergence == nullptr ? nullptr :
                          static_cast<VTK_TT *>(divergence->GetVoidPointer(0)))));

    }
  }
  if(fieldAssociation == vtkDataObject::FIELD_ASSOCIATION_POINTS)
  {
    if(gradients)
    {
      output->GetPointData()->AddArray(gradients);
    }
    if(vorticity)
    {
      output->GetPointData()->AddArray(vorticity);
    }
    if(qCriterion)
    {
      output->GetPointData()->AddArray(qCriterion);
    }
    if(divergence)
    {
      output->GetPointData()->AddArray(divergence);
    }
  }
  else if(fieldAssociation == vtkDataObject::FIELD_ASSOCIATION_CELLS)
  {
    if(gradients)
    {
      output->GetCellData()->AddArray(gradients);
    }
    if(vorticity)
    {
      output->GetCellData()->AddArray(vorticity);
    }
    if(qCriterion)
    {
      output->GetCellData()->AddArray(qCriterion);
    }
    if(divergence)
    {
      output->GetCellData()->AddArray(divergence);
    }
  }
  else
  {
    vtkErrorMacro("Bad fieldAssociation value " << fieldAssociation << endl);
  }

  return 1;
}

//-----------------------------------------------------------------------------
int vtkGradientFilter::GetOutputArrayType(vtkDataArray* array)
{
  int retType = VTK_DOUBLE;
  switch (array->GetDataType())
  {
    vtkTemplateMacro(retType = GetOutputDataType(static_cast<VTK_TT>(0)));
  }
  return retType;
}

namespace {
//-----------------------------------------------------------------------------
  template<class data_type>
  void ComputePointGradientsUG(
    vtkDataSet *structure, vtkDataArray *array, data_type *gradients,
    int numberOfInputComponents, data_type* vorticity, data_type* qCriterion,
    data_type* divergence, int highestCellDimension, int contributingCellOption)
  {
    vtkNew<vtkIdList> currentPoint;
    currentPoint->SetNumberOfIds(1);
    vtkNew<vtkIdList> cellsOnPoint;

    vtkIdType numpts = structure->GetNumberOfPoints();

    int numberOfOutputComponents = 3*numberOfInputComponents;
    std::vector<data_type> g(numberOfOutputComponents);

    // if we are doing patches for contributing cell dimensions we want to keep track of
    // the maximum expected dimension so we can exit out of the check loop quicker
    const int maxCellDimension = structure->IsA("vtkPolyData") ? 2 : 3;

    for (vtkIdType point = 0; point < numpts; point++)
    {
      currentPoint->SetId(0, point);
      double pointcoords[3];
      structure->GetPoint(point, pointcoords);
      // Get all cells touching this point.
      structure->GetCellNeighbors(-1, currentPoint, cellsOnPoint);
      vtkIdType numCellNeighbors = cellsOnPoint->GetNumberOfIds();

      for(int i=0;i<numberOfOutputComponents;i++)
      {
        g[i] = 0;
      }

      if (contributingCellOption == vtkGradientFilter::Patch)
      {
        highestCellDimension = 0;
        for (vtkIdType neighbor = 0; neighbor < numCellNeighbors; neighbor++)
        {
          int cellDimension = structure->GetCell(cellsOnPoint->GetId(neighbor))->GetCellDimension();
          if (cellDimension > highestCellDimension)
          {
            highestCellDimension = cellDimension;
            if (highestCellDimension == maxCellDimension)
            {
              break;
            }
          }
        }
      }
      vtkIdType numValidCellNeighbors = 0;

      // Iterate on all cells and find all points connected to current point
      // by an edge.
      for (vtkIdType neighbor = 0; neighbor < numCellNeighbors; neighbor++)
      {
        vtkCell *cell = structure->GetCell(cellsOnPoint->GetId(neighbor));
        if (cell->GetCellDimension() >= highestCellDimension)
        {
          int subId;
          double parametricCoord[3];
          if(GetCellParametricData(point, pointcoords, cell,
                                   subId, parametricCoord))
          {
            numValidCellNeighbors++;
            for(int inputComponent=0;inputComponent<numberOfInputComponents;inputComponent++)
            {
              int numberOfCellPoints = cell->GetNumberOfPoints();
              std::vector<double> values(numberOfCellPoints);
              // Get values of Array at cell points.
              for (int i = 0; i < numberOfCellPoints; i++)
              {
                values[i] = array->GetComponent(cell->GetPointId(i), inputComponent);
              }

              double derivative[3];
              // Get derivative of cell at point.
              cell->Derivatives(subId, parametricCoord, &values[0], 1, derivative);

              g[inputComponent*3] += static_cast<data_type>(derivative[0]);
              g[inputComponent*3+1] += static_cast<data_type>(derivative[1]);
              g[inputComponent*3+2] += static_cast<data_type>(derivative[2]);
            } // iterating over Components
          } // if(GetCellParametricData())
        } // if(cell->GetCellDimension () >= highestCellDimension
      } // iterating over neighbors

      if (numValidCellNeighbors > 0)
      {
        for(int i=0;i<3*numberOfInputComponents;i++)
        {
          g[i] /= numValidCellNeighbors;
        }

        if(vorticity)
        {
          ComputeVorticityFromGradient(&g[0], vorticity+3*point);
        }
        if(qCriterion)
        {
          ComputeQCriterionFromGradient(&g[0], qCriterion+point);
        }
        if(divergence)
        {
          ComputeDivergenceFromGradient(&g[0], divergence+point);
        }
        if(gradients)
        {
          for(int i=0;i<numberOfOutputComponents;i++)
          {
            gradients[point*numberOfOutputComponents+i] = g[i];
          }
        }
      }
    }  // iterating over points in grid
  }

//-----------------------------------------------------------------------------
  int GetCellParametricData(vtkIdType pointId, double pointCoord[3],
                            vtkCell *cell, int &subId, double parametricCoord[3])
  {
    // Watch out for degenerate cells.  They make the derivative calculation
    // fail.
    vtkIdList *pointIds = cell->GetPointIds();
    int timesPointRegistered = 0;
    for (int i = 0; i < pointIds->GetNumberOfIds(); i++)
    {
      if (pointId == pointIds->GetId(i))
      {
        timesPointRegistered++;
      }
    }
    if (timesPointRegistered != 1)
    {
      // The cell should have the point exactly once.  Not good.
      return 0;
    }

    double dummy;
    int numpoints = cell->GetNumberOfPoints();
    std::vector<double> values(numpoints);
    // Get parametric position of point.
    cell->EvaluatePosition(pointCoord, nullptr, subId, parametricCoord,
                           dummy, &values[0]/*Really another dummy.*/);

    return 1;
  }

//-----------------------------------------------------------------------------
  template<class data_type>
    void ComputeCellGradientsUG(
      vtkDataSet *structure, vtkDataArray *array, data_type *gradients,
      int numberOfInputComponents, data_type* vorticity, data_type* qCriterion,
      data_type* divergence)
  {
    vtkIdType numcells = structure->GetNumberOfCells();
    std::vector<double> values(8);
    std::vector<data_type> cellGradients(3*numberOfInputComponents);
    for (vtkIdType cellid = 0; cellid < numcells; cellid++)
    {
      vtkCell *cell = structure->GetCell(cellid);
      int subId;
      double cellCenter[3];
      subId = cell->GetParametricCenter(cellCenter);

      int numpoints = cell->GetNumberOfPoints();
      if(static_cast<size_t>(numpoints) > values.size())
      {
        values.resize(numpoints);
      }
      double derivative[3];
      for(int inputComponent=0;inputComponent<numberOfInputComponents;
          inputComponent++)
      {
        for (int i = 0; i < numpoints; i++)
        {
          values[i] = array->GetComponent(cell->GetPointId(i), inputComponent);
        }

        cell->Derivatives(subId, cellCenter, &values[0], 1, derivative);
        cellGradients[inputComponent*3] =
          static_cast<data_type>(derivative[0]);
        cellGradients[inputComponent*3+1] =
          static_cast<data_type>(derivative[1]);
        cellGradients[inputComponent*3+2] =
          static_cast<data_type>(derivative[2]);
      }
      if(gradients)
      {
        for(int i=0;i<3*numberOfInputComponents;i++)
        {
          gradients[cellid*3*numberOfInputComponents+i] = cellGradients[i];
        }
      }
      if(vorticity)
      {
        ComputeVorticityFromGradient(&cellGradients[0], vorticity+3*cellid);
      }
      if(qCriterion)
      {
        ComputeQCriterionFromGradient(&cellGradients[0], qCriterion+cellid);
      }
      if(divergence)
      {
        ComputeDivergenceFromGradient(&cellGradients[0], divergence+cellid);
      }
    }
  }

//-----------------------------------------------------------------------------
  template<class Grid, class data_type>
  void ComputeGradientsSG(Grid output, vtkDataArray* array, data_type* gradients,
                          int numberOfInputComponents, int fieldAssociation,
                          data_type* vorticity, data_type* qCriterion,
                          data_type* divergence)
  {
    int idx, idx2, inputComponent;
    double xp[3], xm[3], factor;
    xp[0] = xp[1] = xp[2] = xm[0] = xm[1] = xm[2] = factor = 0;
    double xxi, yxi, zxi, xeta, yeta, zeta, xzeta, yzeta, zzeta;
    yxi = zxi = xeta = yeta = zeta = xzeta = yzeta = zzeta = 0;
    double aj, xix, xiy, xiz, etax, etay, etaz, zetax, zetay, zetaz;
    xix = xiy = xiz = etax = etay = etaz = zetax = zetay = zetaz = 0;
    // for finite differencing -- the values on the "plus" side and
    // "minus" side of the point to be computed at
    std::vector<double> plusvalues(numberOfInputComponents);
    std::vector<double> minusvalues(numberOfInputComponents);

    std::vector<double> dValuesdXi(numberOfInputComponents);
    std::vector<double> dValuesdEta(numberOfInputComponents);
    std::vector<double> dValuesdZeta(numberOfInputComponents);
    std::vector<data_type> localGradients(numberOfInputComponents*3);

    int dims[3];
    output->GetDimensions(dims);
    if(fieldAssociation == vtkDataObject::FIELD_ASSOCIATION_CELLS)
    {
      // reduce the dimensions by 1 for cells
      for(int i=0;i<3;i++)
      {
        dims[i]--;
      }
    }
    int ijsize = dims[0]*dims[1];

    for (int k=0; k<dims[2]; k++)
    {
      for (int j=0; j<dims[1]; j++)
      {
        for (int i=0; i<dims[0]; i++)
        {
          //  Xi derivatives.
          if ( dims[0] == 1 ) // 2D in this direction
          {
            factor = 1.0;
            for (int ii=0; ii<3; ii++)
            {
              xp[ii] = xm[ii] = 0.0;
            }
            xp[0] = 1.0;
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = minusvalues[inputComponent] = 0;
            }
          }
          else if ( i == 0 )
          {
            factor = 1.0;
            idx = (i+1) + j*dims[0] + k*ijsize;
            idx2 = i + j*dims[0] + k*ijsize;
            GetGridEntityCoordinate(output, fieldAssociation, idx, xp);
            GetGridEntityCoordinate(output, fieldAssociation, idx2, xm);
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = array->GetComponent(idx, inputComponent);
              minusvalues[inputComponent] = array->GetComponent(idx2, inputComponent);
            }
          }
          else if ( i == (dims[0]-1) )
          {
            factor = 1.0;
            idx = i + j*dims[0] + k*ijsize;
            idx2 = i-1 + j*dims[0] + k*ijsize;
            GetGridEntityCoordinate(output, fieldAssociation, idx, xp);
            GetGridEntityCoordinate(output, fieldAssociation, idx2, xm);
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = array->GetComponent(idx, inputComponent);
              minusvalues[inputComponent] = array->GetComponent(idx2, inputComponent);
            }
          }
          else
          {
            factor = 0.5;
            idx = (i+1) + j*dims[0] + k*ijsize;
            idx2 = (i-1) + j*dims[0] + k*ijsize;
            GetGridEntityCoordinate(output, fieldAssociation, idx, xp);
            GetGridEntityCoordinate(output, fieldAssociation, idx2, xm);
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = array->GetComponent(idx, inputComponent);
              minusvalues[inputComponent] = array->GetComponent(idx2, inputComponent);
            }
          }

          xxi = factor * (xp[0] - xm[0]);
          yxi = factor * (xp[1] - xm[1]);
          zxi = factor * (xp[2] - xm[2]);
          for(inputComponent=0;inputComponent<numberOfInputComponents;inputComponent++)
          {
            dValuesdXi[inputComponent] = factor *
              (plusvalues[inputComponent] - minusvalues[inputComponent]);
          }

          //  Eta derivatives.
          if ( dims[1] == 1 ) // 2D in this direction
          {
            factor = 1.0;
            for (int ii=0; ii<3; ii++)
            {
              xp[ii] = xm[ii] = 0.0;
            }
            xp[1] = 1.0;
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = minusvalues[inputComponent] = 0;
            }
          }
          else if ( j == 0 )
          {
            factor = 1.0;
            idx = i + (j+1)*dims[0] + k*ijsize;
            idx2 = i + j*dims[0] + k*ijsize;
            GetGridEntityCoordinate(output, fieldAssociation, idx, xp);
            GetGridEntityCoordinate(output, fieldAssociation, idx2, xm);
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = array->GetComponent(idx, inputComponent);
              minusvalues[inputComponent] = array->GetComponent(idx2, inputComponent);
            }
          }
          else if ( j == (dims[1]-1) )
          {
            factor = 1.0;
            idx = i + j*dims[0] + k*ijsize;
            idx2 = i + (j-1)*dims[0] + k*ijsize;
            GetGridEntityCoordinate(output, fieldAssociation, idx, xp);
            GetGridEntityCoordinate(output, fieldAssociation, idx2, xm);
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = array->GetComponent(idx, inputComponent);
              minusvalues[inputComponent] = array->GetComponent(idx2, inputComponent);
            }
          }
          else
          {
            factor = 0.5;
            idx = i + (j+1)*dims[0] + k*ijsize;
            idx2 = i + (j-1)*dims[0] + k*ijsize;
            GetGridEntityCoordinate(output, fieldAssociation, idx, xp);
            GetGridEntityCoordinate(output, fieldAssociation, idx2, xm);
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = array->GetComponent(idx, inputComponent);
              minusvalues[inputComponent] = array->GetComponent(idx2, inputComponent);
            }
          }

          xeta = factor * (xp[0] - xm[0]);
          yeta = factor * (xp[1] - xm[1]);
          zeta = factor * (xp[2] - xm[2]);
          for(inputComponent=0;inputComponent<numberOfInputComponents;inputComponent++)
          {
            dValuesdEta[inputComponent] = factor *
              (plusvalues[inputComponent] - minusvalues[inputComponent]);
          }

          //  Zeta derivatives.
          if ( dims[2] == 1 ) // 2D in this direction
          {
            factor = 1.0;
            for (int ii=0; ii<3; ii++)
            {
              xp[ii] = xm[ii] = 0.0;
            }
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = minusvalues[inputComponent] = 0;
            }
            xp[2] = 1.0;
          }
          else if ( k == 0 )
          {
            factor = 1.0;
            idx = i + j*dims[0] + (k+1)*ijsize;
            idx2 = i + j*dims[0] + k*ijsize;
            GetGridEntityCoordinate(output, fieldAssociation, idx, xp);
            GetGridEntityCoordinate(output, fieldAssociation, idx2, xm);
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = array->GetComponent(idx, inputComponent);
              minusvalues[inputComponent] = array->GetComponent(idx2, inputComponent);
            }
          }
          else if ( k == (dims[2]-1) )
          {
            factor = 1.0;
            idx = i + j*dims[0] + k*ijsize;
            idx2 = i + j*dims[0] + (k-1)*ijsize;
            GetGridEntityCoordinate(output, fieldAssociation, idx, xp);
            GetGridEntityCoordinate(output, fieldAssociation, idx2, xm);
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = array->GetComponent(idx, inputComponent);
              minusvalues[inputComponent] = array->GetComponent(idx2, inputComponent);
            }
          }
          else
          {
            factor = 0.5;
            idx = i + j*dims[0] + (k+1)*ijsize;
            idx2 = i + j*dims[0] + (k-1)*ijsize;
            GetGridEntityCoordinate(output, fieldAssociation, idx, xp);
            GetGridEntityCoordinate(output, fieldAssociation, idx2, xm);
            for(inputComponent=0;inputComponent<numberOfInputComponents;
                inputComponent++)
            {
              plusvalues[inputComponent] = array->GetComponent(idx, inputComponent);
              minusvalues[inputComponent] = array->GetComponent(idx2, inputComponent);
            }
          }

          xzeta = factor * (xp[0] - xm[0]);
          yzeta = factor * (xp[1] - xm[1]);
          zzeta = factor * (xp[2] - xm[2]);
          for(inputComponent=0;inputComponent<numberOfInputComponents;inputComponent++)
          {
            dValuesdZeta[inputComponent] = factor *
              (plusvalues[inputComponent] - minusvalues[inputComponent]);
          }

          // Now calculate the Jacobian.  Grids occasionally have
          // singularities, or points where the Jacobian is infinite (the
          // inverse is zero).  For these cases, we'll set the Jacobian to
          // zero, which will result in a zero derivative.
          //
          aj =  xxi*yeta*zzeta+yxi*zeta*xzeta+zxi*xeta*yzeta
            -zxi*yeta*xzeta-yxi*xeta*zzeta-xxi*zeta*yzeta;
          if (aj != 0.0)
          {
            aj = 1. / aj;
          }

          //  Xi metrics.
          xix  =  aj*(yeta*zzeta-zeta*yzeta);
          xiy  = -aj*(xeta*zzeta-zeta*xzeta);
          xiz  =  aj*(xeta*yzeta-yeta*xzeta);

          //  Eta metrics.
          etax = -aj*(yxi*zzeta-zxi*yzeta);
          etay =  aj*(xxi*zzeta-zxi*xzeta);
          etaz = -aj*(xxi*yzeta-yxi*xzeta);

          //  Zeta metrics.
          zetax=  aj*(yxi*zeta-zxi*yeta);
          zetay= -aj*(xxi*zeta-zxi*xeta);
          zetaz=  aj*(xxi*yeta-yxi*xeta);

          // Finally compute the actual derivatives
          idx = i + j*dims[0] + k*ijsize;
          for(inputComponent=0;inputComponent<numberOfInputComponents;inputComponent++)
          {
            localGradients[inputComponent*3] = static_cast<data_type>(
              xix*dValuesdXi[inputComponent]+etax*dValuesdEta[inputComponent]+
              zetax*dValuesdZeta[inputComponent]);

            localGradients[inputComponent*3+1] = static_cast<data_type>(
              xiy*dValuesdXi[inputComponent]+etay*dValuesdEta[inputComponent]+
              zetay*dValuesdZeta[inputComponent]);

            localGradients[inputComponent*3+2] = static_cast<data_type>(
              xiz*dValuesdXi[inputComponent]+etaz*dValuesdEta[inputComponent]+
              zetaz*dValuesdZeta[inputComponent]);
          }

          if(gradients)
          {
            for(int ii=0;ii<3*numberOfInputComponents;ii++)
            {
              gradients[idx*numberOfInputComponents*3+ii] = localGradients[ii];
            }
          }
          if(vorticity)
          {
            ComputeVorticityFromGradient(&localGradients[0], vorticity+3*idx);
          }
          if(qCriterion)
          {
            ComputeQCriterionFromGradient(&localGradients[0], qCriterion+idx);
          }
          if(divergence)
          {
            ComputeDivergenceFromGradient(&localGradients[0], divergence+idx);
          }
        }
      }
    }
  }

} // end anonymous namespace
