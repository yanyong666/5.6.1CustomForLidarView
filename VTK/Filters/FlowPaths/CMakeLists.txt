set(classes
  vtkAbstractInterpolatedVelocityField
  vtkAMRInterpolatedVelocityField
  vtkCachingInterpolatedVelocityField
  vtkCellLocatorInterpolatedVelocityField
  vtkCompositeInterpolatedVelocityField
  vtkEvenlySpacedStreamlines2D
  vtkInterpolatedVelocityField
  vtkLagrangianParticle
  vtkLagrangianParticleTracker
  vtkLagrangianBasicIntegrationModel
  vtkLagrangianMatidaIntegrationModel
  vtkModifiedBSPTree
  vtkParticlePathFilter
  vtkParticleTracer
  vtkParticleTracerBase
  vtkStreaklineFilter
  vtkStreamTracer
  vtkTemporalInterpolatedVelocityField)

if (NOT VTK_LEGACY_REMOVE)
  list(APPEND classes vtkTemporalStreamTracer)
endif()
vtk_module_add_module(VTK::FiltersFlowPaths
  CLASSES ${classes})
