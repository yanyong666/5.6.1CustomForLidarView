/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointHandleRepresentationSphere.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPointHandleRepresentationSphere
 * @brief   represent the position of a point in display coordinates
 *
 * This class is used to represent a vtkHandleWidget. It represents a
 * position in 2D world coordinates using a x-y cursor (the cursor defined by
 * an instance of vtkPolyData and generated by a vtkPolyDataAlgorithm).
 *
 * @sa
 * vtkHandleRepresentation vtkHandleWidget
*/

#ifndef vtkPointHandleRepresentationSphere_h
#define vtkPointHandleRepresentationSphere_h

#include "vtkHandleRepresentation.h"
#include "vtkPVVTKExtensionsDefaultModule.h" //needed for exports

class vtkActor;
class vtkGlyph3D;
class vtkPoints;
class vtkPolyData;
class vtkPolyDataAlgorithm;
class vtkPolyDataMapper;
class vtkProperty;

class VTKPVVTKEXTENSIONSDEFAULT_EXPORT vtkPointHandleRepresentationSphere
  : public vtkHandleRepresentation
{
public:
  /**
   * Instantiate this class.
   */
  static vtkPointHandleRepresentationSphere* New();

  //@{
  /**
   * Standard methods for instances of this class.
   */
  vtkTypeMacro(vtkPointHandleRepresentationSphere, vtkHandleRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  //@{
  /**
   * Specify the cursor shape with an instance of vtkPolyData. Note that
   * shape is assumed to be defined in the display coordinate system. By
   * default a sphere (the output of vtkSphereSource) shape is used.
   */
  void SetCursorShape(vtkPolyData* cursorShape);
  vtkPolyData* GetCursorShape();
  //@}

  /**
   * Set/Get the position of the point in display coordinates.  This overloads
   * the superclasses SetDisplayPosition in order to set the focal point
   * of the cursor.
   */
  void SetDisplayPosition(double xyz[3]) override;

  //@{
  /**
   * Set/Get the handle properties when unselected and selected.
   */
  void SetProperty(vtkProperty*);
  void SetSelectedProperty(vtkProperty*);
  vtkGetObjectMacro(Property, vtkProperty);
  vtkGetObjectMacro(SelectedProperty, vtkProperty);
  //@}

  //@{
  /**
   * Subclasses of vtkPointHandleRepresentationSphere must implement these
   * methods. These are the methods that the widget and its representation
   * use to communicate with each other.
   */
  double* GetBounds() override;
  void BuildRepresentation() override;
  void StartWidgetInteraction(double eventPos[2]) override;
  void WidgetInteraction(double eventPos[2]) override;
  int ComputeInteractionState(int X, int Y, int modify = 0) override;
  //@}

  //@{
  /**
   * Methods to make this class behave as a vtkProp.
   */
  void ShallowCopy(vtkProp* prop) override;
  void GetActors(vtkPropCollection*) override;
  void ReleaseGraphicsResources(vtkWindow*) override;
  int RenderOpaqueGeometry(vtkViewport* viewport) override;
  //@}

  //@{
  /**
   * Set/get the scalar value associated with this handle.
   */
  vtkSetMacro(Scalar, double);
  vtkGetMacro(Scalar, double);
  //@}

  //@{
  /**
   * Set/get the flag whether to add a circle (disk) source around the sphere.
   */
  void SetAddCircleAroundSphere(int);
  vtkGetMacro(AddCircleAroundSphere, int);
  vtkBooleanMacro(AddCircleAroundSphere, int);
  //@}

  /**
   * Toggle whether this handle should be highlighted.
   */
  void Highlight(int highlight) override;

protected:
  vtkPointHandleRepresentationSphere();
  ~vtkPointHandleRepresentationSphere() override;

  // Render the cursor
  vtkActor* Actor;
  vtkPolyDataMapper* Mapper;
  vtkGlyph3D* Glypher;
  vtkPolyData* CursorShape;
  vtkPolyData* FocalData;
  vtkPoints* FocalPoint;

  // Support picking
  double LastPickPosition[3];
  double LastEventPosition[2];

  // Methods to manipulate the cursor
  int ConstraintAxis;
  void Translate(double eventPos[2]);
  void Scale(double eventPos[2]);

  // A flag to use the disk source
  int AddCircleAroundSphere;
  vtkActor* DiskActor;
  vtkPolyDataMapper* DiskMapper;
  vtkGlyph3D* DiskGlypher;
  void CreateDefaultDiskSource();

  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty* Property;
  vtkProperty* SelectedProperty;
  void CreateDefaultProperties();

  // The size of the hot spot.
  int DetermineConstraintAxis(int constraint, double eventPos[2]);
  int WaitingForMotion;
  int WaitCount;

  double Scalar;

private:
  vtkPointHandleRepresentationSphere(const vtkPointHandleRepresentationSphere&) = delete;
  void operator=(const vtkPointHandleRepresentationSphere&) = delete;
};

#endif
