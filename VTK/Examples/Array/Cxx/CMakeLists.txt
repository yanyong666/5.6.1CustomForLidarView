PROJECT(Array)

IF(NOT VTK_BINARY_DIR)
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})
ENDIF()

IF(NOT VTK_USE_N_WAY_ARRAYS)
  MESSAGE(FATAL_ERROR "Example ${PROJECT_NAME} requires VTK_USE_N_WAY_ARRAYS.")
ENDIF()

ADD_EXECUTABLE(ArrayBasics ArrayBasics.cxx)
TARGET_LINK_LIBRARIES(ArrayBasics vtkCommon)

ADD_EXECUTABLE(IdentityMatrix IdentityMatrix.cxx)
TARGET_LINK_LIBRARIES(IdentityMatrix vtkCommon)

ADD_EXECUTABLE(ArrayIteration ArrayIteration.cxx)
TARGET_LINK_LIBRARIES(ArrayIteration vtkCommon)

IF(vtkViewsInfovis_LOADED AND VTK_USE_INFOVIS)
  ADD_EXECUTABLE(AdjacencyMatrix AdjacencyMatrix.cxx)
  TARGET_LINK_LIBRARIES(AdjacencyMatrix vtkInfovis vtkViews)
ENDIF()
