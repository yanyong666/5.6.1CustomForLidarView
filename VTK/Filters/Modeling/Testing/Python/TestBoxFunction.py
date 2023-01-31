#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

box = vtk.vtkBox()
box.SetXMin(0,2,4)
box.SetXMax(2,4,6)
sample = vtk.vtkSampleFunction()
sample.SetSampleDimensions(30,30,30)
sample.SetImplicitFunction(box)
sample.SetModelBounds(0,1.5,1,5,2,8)
sample.ComputeNormalsOn()
contours = vtk.vtkContourFilter()
contours.SetInputConnection(sample.GetOutputPort())
contours.GenerateValues(5,-0.5,1.5)
w = vtk.vtkPolyDataWriter()
w.SetInputConnection(contours.GetOutputPort())
w.SetFileName("junk.vtk")
#w Write
contMapper = vtk.vtkPolyDataMapper()
contMapper.SetInputConnection(contours.GetOutputPort())
contMapper.SetScalarRange(-0.5,1.5)
contActor = vtk.vtkActor()
contActor.SetMapper(contMapper)
# We'll put a simple outline around the data.
outline = vtk.vtkOutlineFilter()
outline.SetInputConnection(sample.GetOutputPort())
outlineMapper = vtk.vtkPolyDataMapper()
outlineMapper.SetInputConnection(outline.GetOutputPort())
outlineActor = vtk.vtkActor()
outlineActor.SetMapper(outlineMapper)
outlineActor.GetProperty().SetColor(0,0,0)
# The usual rendering stuff.
ren1 = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.SetMultiSamples(0)
renWin.AddRenderer(ren1)
renWin.SetSize(500,500)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)
ren1.SetBackground(1,1,1)
ren1.AddActor(contActor)
ren1.AddActor(outlineActor)
camera = vtk.vtkCamera()
camera.SetClippingRange(6.31875,20.689)
camera.SetFocalPoint(0.75,3,5)
camera.SetPosition(9.07114,-4.10065,-1.38712)
camera.SetViewAngle(30)
camera.SetViewUp(-0.580577,-0.802756,0.13606)
ren1.SetActiveCamera(camera)
iren.Initialize()
# --- end of script --
