//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================

#include <vtkm/filter/internal/CreateResult.h>
#include <vtkm/worklet/DispatcherMapField.h>

#include <vtkm/filter/WarpScalar.h>

namespace vtkm
{
namespace filter
{

//-----------------------------------------------------------------------------
inline VTKM_CONT WarpScalar::WarpScalar(vtkm::FloatDefault scaleAmount)
  : vtkm::filter::FilterField<WarpScalar>()
  , Worklet()
  , NormalFieldName("normal")
  , NormalFieldAssociation(vtkm::cont::Field::Association::ANY)
  , ScalarFactorFieldName("scalarfactor")
  , ScalarFactorFieldAssociation(vtkm::cont::Field::Association::ANY)
  , ScaleAmount(scaleAmount)
{
  this->SetOutputFieldName("warpscalar");
}

//-----------------------------------------------------------------------------
template <typename T, typename StorageType, typename DerivedPolicy>
inline VTKM_CONT vtkm::cont::DataSet WarpScalar::DoExecute(
  const vtkm::cont::DataSet& inDataSet,
  const vtkm::cont::ArrayHandle<vtkm::Vec<T, 3>, StorageType>& field,
  const vtkm::filter::FieldMetadata& fieldMetadata,
  vtkm::filter::PolicyBase<DerivedPolicy> policy)
{
  using vecType = vtkm::Vec<T, 3>;
  auto normalF = inDataSet.GetField(this->NormalFieldName, this->NormalFieldAssociation);
  auto sfF = inDataSet.GetField(this->ScalarFactorFieldName, this->ScalarFactorFieldAssociation);
  vtkm::cont::ArrayHandle<vecType> result;
  this->Worklet.Run(
    field,
    vtkm::filter::ApplyPolicy(normalF, policy, vtkm::filter::FilterTraits<WarpScalar>()),
    vtkm::filter::ApplyPolicy(
      sfF, policy, vtkm::filter::FilterTraits<WarpScalar, WarpScalarScalarFieldTag>()),
    this->ScaleAmount,
    result);

  return internal::CreateResult(inDataSet,
                                result,
                                this->GetOutputFieldName(),
                                fieldMetadata.GetAssociation(),
                                fieldMetadata.GetCellSetName());
}
}
}
