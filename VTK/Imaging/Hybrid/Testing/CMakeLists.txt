vtk_module_test_data(
  Data/headsq/,REGEX:.*)

add_subdirectory(Cxx)

if (VTK_WRAP_PYTHON)
  add_subdirectory(Python)
endif ()
