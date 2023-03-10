/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkQWidgetRepresentation.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkQWidgetRepresentation.h"

#include <QtWidgets/QWidget>
#include "vtkActor.h"
#include "vtkCellPicker.h"
#include "vtkEventData.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLState.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLTexture.h"
#include "vtkPickingManager.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkQWidgetTexture.h"
#include "vtkRenderer.h"

#include "vtk_glew.h"

vtkStandardNewMacro(vtkQWidgetRepresentation);

//----------------------------------------------------------------------------
vtkQWidgetRepresentation::vtkQWidgetRepresentation()
{
  this->PlaneSource = vtkPlaneSource::New();
  this->PlaneSource->SetOutputPointsPrecision(
    vtkAlgorithm::DOUBLE_PRECISION);

  this->PlaneMapper = vtkPolyDataMapper::New();
  this->PlaneMapper->SetInputConnection(
    this->PlaneSource->GetOutputPort());

  this->QWidgetTexture = vtkQWidgetTexture::New();
  this->PlaneTexture = vtkOpenGLTexture::New();
  this->PlaneTexture->SetTextureObject(this->QWidgetTexture);

  this->PlaneActor = vtkActor::New();
  this->PlaneActor->SetMapper(this->PlaneMapper);
  this->PlaneActor->SetTexture(this->PlaneTexture);
  this->PlaneActor->GetProperty()->SetAmbient(1.0);
  this->PlaneActor->GetProperty()->SetDiffuse(0.0);

  // Define the point coordinates
  double bounds[6];
  bounds[0] = -0.5;
  bounds[1] = 0.5;
  bounds[2] = -0.5;
  bounds[3] = 0.5;
  bounds[4] = -0.5;
  bounds[5] = 0.5;

  // Initial creation of the widget, serves to initialize it
  this->PlaceWidget(bounds);

  //Manage the picking stuff
  this->Picker = vtkCellPicker::New();
  this->Picker->SetTolerance(0.005);
  this->Picker->AddPickList(this->PlaneActor);
  this->Picker->PickFromListOn();
}

//----------------------------------------------------------------------------
vtkQWidgetRepresentation::~vtkQWidgetRepresentation()
{
  this->PlaneSource->Delete();
  this->PlaneMapper->Delete();
  this->PlaneActor->Delete();
  this->PlaneTexture->Delete();
  this->QWidgetTexture->Delete();

  this->Picker->Delete();
}

void vtkQWidgetRepresentation::SetWidget(QWidget *w)
{
  // just pass down to the QWidgetTexture
  this->QWidgetTexture->SetWidget(w);
  this->Modified();
}

// see if the event hits the widget rep, if so set the WidgetCoordinates
// and move to Inside state
int vtkQWidgetRepresentation::ComputeComplexInteractionState(
  vtkRenderWindowInteractor *,
  vtkAbstractWidget *,
  unsigned long , void *calldata, int )
{
  vtkEventData *edata = static_cast<vtkEventData *>(calldata);
  vtkEventDataDevice3D *edd = edata->GetAsEventDataDevice3D();
  if (edd)
  {
    double pos[3];
    double ori[4];
    edd->GetWorldPosition(pos);
    edd->GetWorldOrientation(ori);

    // does the ray hit the widget rep?
    if (!this->Picker->Pick3DRay(pos, ori, this->Renderer))
    {
      // if not just return
      this->InteractionState = vtkQWidgetRepresentation::Outside;
      return this->InteractionState;
    }

    // the ray hit the widget
    this->ValidPick = 1;
    this->InteractionState = vtkQWidgetRepresentation::Inside;

    // compute the widget coordinates
    vtkPoints *pp = this->Picker->GetPickedPositions();
    double ppos[3];
    pp->GetPoint(0,ppos);

    double origin[3];
    this->PlaneSource->GetOrigin(origin);
    double axis0[3];
    this->PlaneSource->GetPoint1(axis0);
    double axis1[3];
    this->PlaneSource->GetPoint2(axis1);
    for (int i = 0; i < 3; ++i)
    {
      ppos[i] -= origin[i];
      axis0[i] -= origin[i];
      axis1[i] -= origin[i];
    }
    double lengtha0 = vtkMath::Normalize(axis0);
    double lengtha1 = vtkMath::Normalize(axis1);
    double wCoords[2] = { 0.0, 0.0 };
    for (int i = 0; i < 3; ++i)
    {
      wCoords[0] += ppos[i] * axis0[i];
      wCoords[1] += ppos[i] * axis1[i];
    }
    QWidget *widget = this->QWidgetTexture->GetWidget();
    this->WidgetCoordinates[0] = wCoords[0]*widget->width() / lengtha0;
    this->WidgetCoordinates[1] = wCoords[1]*widget->height() / lengtha1;
    this->WidgetCoordinates[1] = widget->height() - this->WidgetCoordinates[1];
  }

  return this->InteractionState;
}

//----------------------------------------------------------------------
double *vtkQWidgetRepresentation::GetBounds()
{
  this->BuildRepresentation();
  return this->PlaneActor->GetBounds();
}

//----------------------------------------------------------------------------
void vtkQWidgetRepresentation::GetActors(vtkPropCollection *pc)
{
  this->PlaneActor->GetActors(pc);
}

//----------------------------------------------------------------------------
void vtkQWidgetRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
  this->PlaneActor->ReleaseGraphicsResources(w);
  this->PlaneMapper->ReleaseGraphicsResources(w);
  this->PlaneTexture->ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------------
int vtkQWidgetRepresentation::RenderOpaqueGeometry(vtkViewport *v)
{
  vtkInformation *info = this->GetPropertyKeys();
  this->PlaneActor->SetPropertyKeys(info);

  vtkOpenGLRenderWindow *renWin =
    static_cast<vtkOpenGLRenderWindow *>(this->Renderer->GetRenderWindow());
  vtkOpenGLState *ostate = renWin->GetState();

  // always draw over the rest
  ostate->vtkglDepthFunc(GL_ALWAYS);
  int result = this->PlaneActor->RenderOpaqueGeometry(v);
  ostate->vtkglDepthFunc(GL_LEQUAL);

  return result;
}

//-----------------------------------------------------------------------------
int vtkQWidgetRepresentation::RenderTranslucentPolygonalGeometry(vtkViewport *)
{
  return 0;
}

//-----------------------------------------------------------------------------
vtkTypeBool vtkQWidgetRepresentation::HasTranslucentPolygonalGeometry()
{
  return false;
}

//----------------------------------------------------------------------------
void vtkQWidgetRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  // this->InteractionState is printed in superclass
  // this is commented to avoid PrintSelf errors
}

//----------------------------------------------------------------------------
void vtkQWidgetRepresentation::PlaceWidget(double bds[6])
{
  this->PlaneSource->SetOrigin(bds[0],bds[2],bds[4]);
  this->PlaneSource->SetPoint1(bds[1],bds[2],bds[4]);
  this->PlaneSource->SetPoint2(bds[0],bds[2],bds[5]);

  this->ValidPick = 1; // since we have positioned the widget successfully
}

//----------------------------------------------------------------------------
vtkPolyDataAlgorithm *vtkQWidgetRepresentation::GetPolyDataAlgorithm()
{
  return this->PlaneSource;
}

//----------------------------------------------------------------------------
void vtkQWidgetRepresentation::UpdatePlacement()
{
}

//----------------------------------------------------------------------------
void vtkQWidgetRepresentation::BuildRepresentation()
{
  // rep is always built via plane source and doesn't change
}

//----------------------------------------------------------------------
void vtkQWidgetRepresentation::RegisterPickers()
{
  vtkPickingManager* pm = this->GetPickingManager();
  if (!pm)
  {
    return;
  }
  pm->AddPicker(this->Picker, this);
}
