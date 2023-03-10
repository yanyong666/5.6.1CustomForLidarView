##============================================================================
##  Copyright (c) Kitware, Inc.
##  All rights reserved.
##  See LICENSE.txt for details.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##============================================================================

#add the directory that contains the VTK-m config file to the cmake
#path so that our examples can find VTK-m
set(CMAKE_PREFIX_PATH ${VTKm_BINARY_DIR}/${VTKm_INSTALL_CONFIG_DIR})

add_subdirectory(clipping)
add_subdirectory(contour_tree)
add_subdirectory(contour_tree_augmented)
add_subdirectory(cosmotools)
add_subdirectory(demo)
add_subdirectory(game_of_life)
add_subdirectory(hello_world)
add_subdirectory(histogram)
add_subdirectory(isosurface)
add_subdirectory(lagrangian)
add_subdirectory(multi_backend)
add_subdirectory(oscillator)
add_subdirectory(particle_advection)
add_subdirectory(redistribute_points)
add_subdirectory(rendering)
add_subdirectory(streamline)
add_subdirectory(temporal_advection)
add_subdirectory(tetrahedra)
# add_subdirectory(unified_memory)


if (VTKm_ENABLE_TESTING)
  # These need to be fast to build as they will
  # be built each time we run the test
  vtkm_test_against_install(rendering)
  vtkm_test_against_install(histogram)
endif()
