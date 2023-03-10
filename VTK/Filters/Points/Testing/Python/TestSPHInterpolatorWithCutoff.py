#!/usr/bin/env python
try:
    import numpy as np
except ImportError:
    print("Numpy (http://numpy.scipy.org) not found.")
    print("This test requires numpy!")
    from vtk.test import Testing
    Testing.skip()

import vtk
import vtk.test.Testing
from vtk.util.misc import vtkGetDataRoot
from vtk.numpy_interface import dataset_adapter as dsa
VTK_DATA_ROOT = vtkGetDataRoot()

# Parameters for testing
res = 250

# Graphics stuff
ren0 = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren0)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

# create pipeline
#
reader = vtk.vtkXMLUnstructuredGridReader()
reader.SetFileName(VTK_DATA_ROOT + "/Data/SPH_Points.vtu")
reader.Update()
output = reader.GetOutput()
scalarRange = output.GetPointData().GetArray("Rho").GetRange()

output2 = dsa.WrapDataObject(output)
# the constant value in the Cutoff array below is equal to
# the spatialStep (0.1) set below, and the CutoffFactor (3) of the QuinticKernel
Cutoff = np.ones(output.GetNumberOfPoints()) * 3.0/10.0;
Mass = np.ones(output.GetNumberOfPoints()) * 1.0;
output2.PointData.append(Mass, "Mass")
output2.PointData.append(Cutoff, "Cutoff")

# Something to sample with
center = output.GetCenter()
bounds = output.GetBounds()
length = output.GetLength()

plane = vtk.vtkPlaneSource()
plane.SetResolution(res,res)
plane.SetOrigin(bounds[0],bounds[2],bounds[4])
plane.SetPoint1(bounds[1],bounds[2],bounds[4])
plane.SetPoint2(bounds[0],bounds[3],bounds[4])
plane.SetCenter(center)
plane.SetNormal(0,0,1)
plane.Push(1.15)
plane.Update()

# SPH kernel------------------------------------------------

sphKernel = vtk.vtkSPHQuinticKernel()
sphKernel.SetSpatialStep(0.1)

interpolator = vtk.vtkSPHInterpolator()
interpolator.SetInputConnection(plane.GetOutputPort())
interpolator.SetSourceConnection(reader.GetOutputPort())
interpolator.SetDensityArrayName("Rho")
interpolator.SetMassArrayName("Mass")
interpolator.SetCutoffArrayName("Cutoff")
interpolator.SetKernel(sphKernel)

# Time execution
timer = vtk.vtkTimerLog()
timer.StartTimer()
interpolator.Update()
timer.StopTimer()
time = timer.GetElapsedTime()
print("Interpolate Points (SPH): {0}".format(time))
intMapper = vtk.vtkPolyDataMapper()
intMapper.SetInputConnection(interpolator.GetOutputPort())
intMapper.SetScalarModeToUsePointFieldData()
intMapper.SelectColorArray("Rho")
intMapper.SetScalarRange(interpolator.GetOutput().GetPointData().GetArray("Rho").GetRange())

intActor = vtk.vtkActor()
intActor.SetMapper(intMapper)

# Create an outline
outline = vtk.vtkOutlineFilter()
outline.SetInputData(output)

outlineMapper = vtk.vtkPolyDataMapper()
outlineMapper.SetInputConnection(outline.GetOutputPort())

outlineActor = vtk.vtkActor()
outlineActor.SetMapper(outlineMapper)

ren0.AddActor(intActor)
ren0.AddActor(outlineActor)
ren0.SetBackground(0.1, 0.2, 0.4)

iren.Initialize()
renWin.Render()

iren.Start()
