vtk_module_test_data(
  Data/WineGlass.wrl)

add_subdirectory(Cxx)

if (VTK_WRAP_PYTHON)
  vtk_module_test_data(
    Data/bot2.wrl)

  add_subdirectory(Python)
endif ()
