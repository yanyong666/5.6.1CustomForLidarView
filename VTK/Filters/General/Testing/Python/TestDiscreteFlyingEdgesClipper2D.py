#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# Create the RenderWindow, Renderer and both Actors
#
ren1 = vtk.vtkRenderer()
ren1.SetBackground(1,1,1)
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren1)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

# Read the data. Note this creates a 3-component scalar.
red = vtk.vtkPNGReader()
red.SetFileName(VTK_DATA_ROOT + "/Data/RedCircle.png")
red.Update()

# Next filter can only handle RGB *(&&*@
extract = vtk.vtkImageExtractComponents()
extract.SetInputConnection(red.GetOutputPort())
extract.SetComponents(0,1,2)

# Quantize the image into an index
quantize = vtk.vtkImageQuantizeRGBToIndex()
quantize.SetInputConnection(extract.GetOutputPort())
quantize.SetNumberOfColors(3)

# Create the pipeline
discrete = vtk.vtkDiscreteFlyingEdgesClipper2D()
discrete.SetInputConnection(quantize.GetOutputPort())
discrete.SetValue(0,1)

# Polygons are displayed
polyMapper = vtk.vtkPolyDataMapper()
polyMapper.SetInputConnection(discrete.GetOutputPort())

polyActor = vtk.vtkActor()
polyActor.SetMapper(polyMapper)

ren1.AddActor(polyActor)

renWin.Render()
iren.Start()
