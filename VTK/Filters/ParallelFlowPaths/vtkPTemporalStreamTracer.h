/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTemporalStreamTracer.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkTemporalStreamTracer
 * @brief   A Parallel Particle tracer for unsteady vector fields
 *
 * vtkTemporalStreamTracer is a filter that integrates a vector field to generate
 *
 *
 * @sa
 * vtkRibbonFilter vtkRuledSurfaceFilter vtkInitialValueProblemSolver
 * vtkRungeKutta2 vtkRungeKutta4 vtkRungeKutta45 vtkStreamTracer
 *
 * This class is deprecated.
 * Use instead one of the following classes: vtkParticleTracerBase
 * vtkParticleTracer vtkParticlePathFilter vtkStreaklineFilter
 * See https://blog.kitware.com/improvements-in-path-tracing-in-vtk/
*/

#ifndef vtkPTemporalStreamTracer_h
#define vtkPTemporalStreamTracer_h

#include "vtkConfigure.h" // For legacy defines
#include "vtkSetGet.h"    // For legacy macros
#ifndef VTK_LEGACY_REMOVE

#include "vtkFiltersParallelFlowPathsModule.h" // For export macro
#include "vtkSmartPointer.h" // For protected ivars.
#include "vtkTemporalStreamTracer.h"

#include <vector> // STL Header
#include <list>   // STL Header

class vtkMultiProcessController;

class vtkMultiBlockDataSet;
class vtkDataArray;
class vtkDoubleArray;
class vtkGenericCell;
class vtkIntArray;
class vtkTemporalInterpolatedVelocityField;
class vtkPoints;
class vtkCellArray;
class vtkDoubleArray;
class vtkFloatArray;
class vtkIntArray;
class vtkCharArray;
class vtkAbstractParticleWriter;

class VTKFILTERSPARALLELFLOWPATHS_EXPORT vtkPTemporalStreamTracer : public vtkTemporalStreamTracer
{
public:

    vtkTypeMacro(vtkPTemporalStreamTracer,vtkTemporalStreamTracer);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    /**
     * Construct object using 2nd order Runge Kutta
     */
    static vtkPTemporalStreamTracer *New();

    //@{
    /**
     * Set/Get the controller used when sending particles between processes
     * The controller must be an instance of vtkMPIController.
     */
    virtual void SetController(vtkMultiProcessController* controller);
    vtkGetObjectMacro(Controller, vtkMultiProcessController);
    //@}

  protected:

    VTK_LEGACY(vtkPTemporalStreamTracer());
    ~vtkPTemporalStreamTracer();

    //
    // Generate output
    //
    virtual int RequestData(vtkInformation* request,
                            vtkInformationVector** inputVector,
                            vtkInformationVector* outputVector) override;

//

//

    /**
     * all the injection/seed points according to which processor
     * they belong to. This saves us retesting at every injection time
     * providing 1) The volumes are static, 2) the seed points are static
     * If either are non static, then this step is skipped.
     */
    virtual void AssignSeedsToProcessors(
      vtkDataSet *source, int sourceID, int ptId,
      vtkTemporalStreamTracerNamespace::ParticleVector &LocalSeedPoints,
      int &LocalAssignedCount) override;

    /**
     * give each one a unique ID. We need to use MPI to find out
     * who is using which numbers.
     */
    virtual void AssignUniqueIds(
      vtkTemporalStreamTracerNamespace::ParticleVector &LocalSeedPoints) override;

    /**
     * this is used during classification of seed points and also between iterations
     * of the main loop as particles leave each processor domain
     */
    virtual void TransmitReceiveParticles(
      vtkTemporalStreamTracerNamespace::ParticleVector &outofdomain,
      vtkTemporalStreamTracerNamespace::ParticleVector &received,
      bool removeself) override;

    void AddParticleToMPISendList(
      vtkTemporalStreamTracerNamespace::ParticleInformation &info);

//

//

  // MPI controller needed when running in parallel
  vtkMultiProcessController* Controller;

private:
  vtkPTemporalStreamTracer(const vtkPTemporalStreamTracer&) = delete;
  void operator=(const vtkPTemporalStreamTracer&) = delete;
};

#endif // VTK_LEGACY_REMOVE

#endif
