if (PARAVIEW_BUILD_QT_GUI AND BUILD_SHARED_LIBS)
  set(_paraview_add_tests_default_test_data_target ParaViewData)
  ExternalData_Expand_Arguments(ParaViewData _
    "DATA{${CMAKE_CURRENT_SOURCE_DIR}/Data/Gmsh/,REGEX:viz_.*}"
    "DATA{${CMAKE_CURRENT_SOURCE_DIR}/Data/Gmsh/naca0012/,REGEX:.*}")

  paraview_add_client_tests(
    LOAD_PLUGIN "GmshReader"
    BASELINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Data/Baseline"
    TEST_DATA_TARGET ParaViewData
    TEST_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/GmshReaderTest.xml)

  paraview_add_client_server_tests(
    LOAD_PLUGIN "GmshReader"
    BASELINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Data/Baseline"
    TEST_DATA_TARGET ParaViewData
    TEST_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/GmshReaderMeshOnlyTest.xml)
endif()
