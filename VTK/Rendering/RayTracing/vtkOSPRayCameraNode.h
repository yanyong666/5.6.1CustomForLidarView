/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOSPRayCameraNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkOSPRayCameraNode
 * @brief   links vtkCamera to OSPRay
 *
 * Translates vtkCamera state into OSPRay rendering calls
*/

#ifndef vtkOSPRayCameraNode_h
#define vtkOSPRayCameraNode_h

#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkCameraNode.h"

class vtkInformationIntegerKey;
class vtkCamera;

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayCameraNode :
  public vtkCameraNode
{
public:
  static vtkOSPRayCameraNode* New();
  vtkTypeMacro(vtkOSPRayCameraNode, vtkCameraNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Make ospray calls to render me.
   */
  virtual void Render(bool prepass) override;

protected:
  vtkOSPRayCameraNode();
  ~vtkOSPRayCameraNode();

private:
  vtkOSPRayCameraNode(const vtkOSPRayCameraNode&) = delete;
  void operator=(const vtkOSPRayCameraNode&) = delete;
};

#endif
