add_executable(TestCatalystServerManager
  TestCatalystServerManager.cxx)
target_link_libraries(TestCatalystServerManager
  vtkPVClientServerCoreCore
  vtkPVServerManagerApplication
  vtkPVServerManagerCore)

add_test(NAME TestCatalystServerManager
  COMMAND TestCatalystServerManager)
