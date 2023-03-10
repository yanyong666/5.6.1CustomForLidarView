//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================
#include <vtkm/cont/RuntimeDeviceTracker.h>
#include <vtkm/cont/openmp/DeviceAdapterOpenMP.h>
#include <vtkm/cont/testing/TestingDeviceAdapter.h>

int UnitTestOpenMPDeviceAdapter(int argc, char* argv[])
{
  auto& tracker = vtkm::cont::GetRuntimeDeviceTracker();
  tracker.ForceDevice(vtkm::cont::DeviceAdapterTagOpenMP{});
  return vtkm::cont::testing::TestingDeviceAdapter<vtkm::cont::DeviceAdapterTagOpenMP>::Run(argc,
                                                                                            argv);
}
