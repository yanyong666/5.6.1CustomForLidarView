//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================
#include <GL/glew.h>
#include <vtkm/interop/testing/TestingTransferFancyHandles.h>
#include <vtkm/rendering/CanvasEGL.h>

int UnitTestFancyTransferEGL(int argc, char* argv[])
{
  vtkm::cont::Initialize(argc, argv);

  //get egl canvas to construct a context for us
  vtkm::rendering::CanvasEGL canvas(1024, 1024);
  canvas.Initialize();
  canvas.Activate();

  //get glew to bind all the opengl functions
  glewInit();

  return vtkm::interop::testing::TestingTransferFancyHandles::Run();
}
