##============================================================================
##  Copyright (c) Kitware, Inc.
##  All rights reserved.
##  See LICENSE.txt for details.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##============================================================================
cmake_minimum_required(VERSION 3.8...3.14 FATAL_ERROR)
project(Clipping CXX)

#Find the VTK-m package
find_package(VTKm REQUIRED QUIET)

add_executable(Clipping Clipping.cxx)
target_link_libraries(Clipping PRIVATE vtkm_filter)

if(TARGET vtkm::cuda)
  set_source_files_properties(Clipping.cxx PROPERTIES LANGUAGE "CUDA")
endif()
