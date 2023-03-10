cmake_minimum_required(VERSION 3.3...3.12 FATAL_ERROR)

#
# Set the project name.
#

project (VTKMY)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

#
# Load CMake commands that you probably should not modify.
#

include (${VTKMY_SOURCE_DIR}/CMakeOptions.cmake)

include_directories(${VTKMY_BINARY_DIR})

#
# You can put your include path(s) here
#

include_directories(${VTKMY_SOURCE_DIR}/Common)
include_directories(${VTKMY_SOURCE_DIR}/Imaging)
include_directories(${VTKMY_SOURCE_DIR}/Unsorted)

#
# Build examples too ?
#

option(BUILD_EXAMPLES "Build examples." ON)
if (BUILD_EXAMPLES)
  add_subdirectory(Examples)
endif ()

#
# Common configuration settings
#
# Do not worry about this one.
#

configure_file(
  ${VTKMY_SOURCE_DIR}/vtkmyConfigure.h.in
  ${VTKMY_BINARY_DIR}/vtkmyConfigure.h
)

#
# Here is where you can list the sub-directories holding your local
# classes. Sorting classes by 'package' type like VTK does (Common,
# Rendering, Filtering, Imaging, IO, etc.) is a good thing and prevents
# numerous dependencies problems.
#
add_subdirectory(Common)
add_subdirectory(Imaging)
add_subdirectory(Unsorted)
add_subdirectory(Utilities)
