#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# On older Macs, 10 is too low. Due to what looks like a driver bug
# spectral lighting behaves sort of weird and produces small differences
threshold = 30
points = vtk.vtkPoints()
points.InsertNextPoint(2,4,0)
points.InsertNextPoint(2.6,2.6,0)
points.InsertNextPoint(4,2,0)
points.InsertNextPoint(1.4,4,1.4)
points.InsertNextPoint(2,3,1)
points.InsertNextPoint(3,2,1)
points.InsertNextPoint(4,1.4,1.4)
points.InsertNextPoint(0,4,2)
points.InsertNextPoint(1,3,2)
points.InsertNextPoint(2,2,2)
points.InsertNextPoint(3,1,2)
points.InsertNextPoint(4,0,2)
points.InsertNextPoint(0,2.6,2.6)
points.InsertNextPoint(1,2,3)
points.InsertNextPoint(2,1,3)
points.InsertNextPoint(2.6,0,2.6)
points.InsertNextPoint(0,2,4)
points.InsertNextPoint(1.4,1.4,4)
points.InsertNextPoint(2,0,4)
faces = vtk.vtkCellArray()
faces.InsertNextCell(3)
faces.InsertCellPoint(0)
faces.InsertCellPoint(3)
faces.InsertCellPoint(4)
faces.InsertNextCell(3)
faces.InsertCellPoint(0)
faces.InsertCellPoint(4)
faces.InsertCellPoint(1)
faces.InsertNextCell(3)
faces.InsertCellPoint(1)
faces.InsertCellPoint(4)
faces.InsertCellPoint(5)
faces.InsertNextCell(3)
faces.InsertCellPoint(1)
faces.InsertCellPoint(5)
faces.InsertCellPoint(2)
faces.InsertNextCell(3)
faces.InsertCellPoint(2)
faces.InsertCellPoint(5)
faces.InsertCellPoint(6)
faces.InsertNextCell(3)
faces.InsertCellPoint(3)
faces.InsertCellPoint(7)
faces.InsertCellPoint(8)
faces.InsertNextCell(3)
faces.InsertCellPoint(3)
faces.InsertCellPoint(8)
faces.InsertCellPoint(4)
faces.InsertNextCell(3)
faces.InsertCellPoint(4)
faces.InsertCellPoint(8)
faces.InsertCellPoint(9)
faces.InsertNextCell(3)
faces.InsertCellPoint(4)
faces.InsertCellPoint(9)
faces.InsertCellPoint(5)
faces.InsertNextCell(3)
faces.InsertCellPoint(5)
faces.InsertCellPoint(9)
faces.InsertCellPoint(10)
faces.InsertNextCell(3)
faces.InsertCellPoint(5)
faces.InsertCellPoint(10)
faces.InsertCellPoint(6)
faces.InsertNextCell(3)
faces.InsertCellPoint(6)
faces.InsertCellPoint(10)
faces.InsertCellPoint(11)
faces.InsertNextCell(3)
faces.InsertCellPoint(7)
faces.InsertCellPoint(12)
faces.InsertCellPoint(8)
faces.InsertNextCell(3)
faces.InsertCellPoint(8)
faces.InsertCellPoint(12)
faces.InsertCellPoint(13)
faces.InsertNextCell(3)
faces.InsertCellPoint(8)
faces.InsertCellPoint(13)
faces.InsertCellPoint(9)
faces.InsertNextCell(3)
faces.InsertCellPoint(9)
faces.InsertCellPoint(13)
faces.InsertCellPoint(14)
faces.InsertNextCell(3)
faces.InsertCellPoint(9)
faces.InsertCellPoint(14)
faces.InsertCellPoint(10)
faces.InsertNextCell(3)
faces.InsertCellPoint(10)
faces.InsertCellPoint(14)
faces.InsertCellPoint(15)
faces.InsertNextCell(3)
faces.InsertCellPoint(10)
faces.InsertCellPoint(15)
faces.InsertCellPoint(11)
faces.InsertNextCell(3)
faces.InsertCellPoint(12)
faces.InsertCellPoint(16)
faces.InsertCellPoint(13)
faces.InsertNextCell(3)
faces.InsertCellPoint(13)
faces.InsertCellPoint(16)
faces.InsertCellPoint(17)
faces.InsertNextCell(3)
faces.InsertCellPoint(13)
faces.InsertCellPoint(17)
faces.InsertCellPoint(14)
faces.InsertNextCell(3)
faces.InsertCellPoint(14)
faces.InsertCellPoint(17)
faces.InsertCellPoint(18)
faces.InsertNextCell(3)
faces.InsertCellPoint(14)
faces.InsertCellPoint(18)
faces.InsertCellPoint(15)
model = vtk.vtkPolyData()
model.SetPolys(faces)
model.SetPoints(points)
rn = vtk.vtkMath()
cellColors = vtk.vtkUnsignedCharArray()
cellColors.SetNumberOfComponents(3)
cellColors.SetNumberOfTuples(model.GetNumberOfCells())
i = 0
while i < model.GetNumberOfCells():
    cellColors.InsertComponent(i,0,rn.Random(100,255))
    cellColors.InsertComponent(i,1,rn.Random(100,255))
    cellColors.InsertComponent(i,2,rn.Random(100,255))
    i = i + 1

model.GetCellData().SetScalars(cellColors)
t0 = vtk.vtkTransform()
t0.Identity()
tf0 = vtk.vtkTransformPolyDataFilter()
tf0.SetTransform(t0)
tf0.SetInputData(model)
t1 = vtk.vtkTransform()
t1.Identity()
t1.RotateZ(90)
tf1 = vtk.vtkTransformPolyDataFilter()
tf1.SetTransform(t1)
tf1.SetInputData(model)
t2 = vtk.vtkTransform()
t2.Identity()
t2.RotateZ(180)
tf2 = vtk.vtkTransformPolyDataFilter()
tf2.SetTransform(t2)
tf2.SetInputData(model)
t3 = vtk.vtkTransform()
t3.Identity()
t3.RotateZ(270)
tf3 = vtk.vtkTransformPolyDataFilter()
tf3.SetTransform(t3)
tf3.SetInputData(model)
af = vtk.vtkAppendPolyData()
af.AddInputConnection(tf0.GetOutputPort())
af.AddInputConnection(tf1.GetOutputPort())
af.AddInputConnection(tf2.GetOutputPort())
af.AddInputConnection(tf3.GetOutputPort())
t4 = vtk.vtkTransform()
t4.Identity()
t4.RotateX(180)
tf4 = vtk.vtkTransformPolyDataFilter()
tf4.SetTransform(t4)
tf4.SetInputConnection(af.GetOutputPort())
af2 = vtk.vtkAppendPolyData()
af2.AddInputConnection(af.GetOutputPort())
af2.AddInputConnection(tf4.GetOutputPort())
t5 = vtk.vtkTransform()
t5.Identity()
t5.Translate(0,0,-8)
tf5 = vtk.vtkTransformPolyDataFilter()
tf5.SetTransform(t5)
tf5.SetInputConnection(af2.GetOutputPort())
af3 = vtk.vtkAppendPolyData()
af3.AddInputConnection(af2.GetOutputPort())
af3.AddInputConnection(tf5.GetOutputPort())
t6 = vtk.vtkTransform()
t6.Identity()
t6.Translate(0,-8,0)
tf6 = vtk.vtkTransformPolyDataFilter()
tf6.SetTransform(t6)
tf6.SetInputConnection(af3.GetOutputPort())
af4 = vtk.vtkAppendPolyData()
af4.AddInputConnection(af3.GetOutputPort())
af4.AddInputConnection(tf6.GetOutputPort())
# Create the RenderWindow, Renderer and both Actors
#
ren1 = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren1)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)
clean = vtk.vtkCleanPolyData()
clean.SetTolerance(.001)
clean.SetInputData(model)
clean.SetInputConnection(af2.GetOutputPort())
clean.SetInputConnection(af3.GetOutputPort())
clean.SetInputConnection(af4.GetOutputPort())
subdivide = vtk.vtkButterflySubdivisionFilter()
subdivide.SetInputConnection(clean.GetOutputPort())
subdivide.SetNumberOfSubdivisions(3)
mapper = vtk.vtkDataSetMapper()
mapper.SetInputConnection(subdivide.GetOutputPort())
surface = vtk.vtkActor()
surface.SetMapper(mapper)
fe = vtk.vtkFeatureEdges()
fe.SetInputConnection(subdivide.GetOutputPort())
fe.SetFeatureAngle(100)
feStripper = vtk.vtkStripper()
feStripper.SetInputConnection(fe.GetOutputPort())
feTubes = vtk.vtkTubeFilter()
feTubes.SetInputConnection(feStripper.GetOutputPort())
feTubes.SetRadius(.1)
feMapper = vtk.vtkPolyDataMapper()
feMapper.SetInputConnection(feTubes.GetOutputPort())
edges = vtk.vtkActor()
edges.SetMapper(feMapper)
# Add the actors to the renderer, set the background and size
#
ren1.AddActor(surface)
ren1.AddActor(edges)
backP = vtk.vtkProperty()
backP.SetDiffuseColor(1,1,.3)
surface.SetBackfaceProperty(backP)
edges.GetProperty().SetDiffuseColor(.2,.2,.2)
surface.GetProperty().SetDiffuseColor(1,.4,.3)
surface.GetProperty().SetSpecular(.4)
surface.GetProperty().SetDiffuse(.8)
surface.GetProperty().SetSpecularPower(40)
ren1.SetBackground(0.1,0.2,0.4)
renWin.SetSize(300,300)
# render the image
#
ren1.ResetCamera()
cam1 = ren1.GetActiveCamera()
cam1.Azimuth(90)
ren1.ResetCamera()
cam1.Zoom(1.5)
iren.Initialize()
# prevent the tk window from showing up then start the event loop
# --- end of script --
