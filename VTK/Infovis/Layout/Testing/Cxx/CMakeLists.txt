vtk_add_test_cxx(vtkInfovisLayoutCxxTests tests
  # TestAssignCoordinates.cxx # todo (unsatisfied deps)
  TestChacoGraphReader.cxx
  TestCirclePackLayoutStrategy.cxx
  # TestCosmicTreeLayoutStrategy.cxx # todo (unsatisfied deps)
  TestGraphLayoutStrategy.cxx,NO_VALID
  TestIncrementalForceLayout.cxx,NO_VALID
  TestTreeMapLayoutStrategy.cxx
  )
vtk_test_cxx_executable(vtkInfovisLayoutCxxTests tests)
