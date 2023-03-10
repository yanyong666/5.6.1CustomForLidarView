#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# this script tests vtkImageReslice with different interpolation modes
# Image pipeline
reader = vtk.vtkImageReader()
reader.ReleaseDataFlagOff()
reader.SetDataByteOrderToLittleEndian()
reader.SetDataExtent(0,63,0,63,1,93)
reader.SetDataSpacing(3.2,3.2,1.5)
reader.SetFilePrefix("" + str(VTK_DATA_ROOT) + "/Data/headsq/quarter")
reader.SetDataMask(0x7fff)
reslice1 = vtk.vtkImageReslice()
reslice1.SetInputConnection(reader.GetOutputPort())
reslice1.SetInterpolationModeToCubic()
reslice1.SetOutputSpacing(0.65,0.65,1.5)
reslice1.SetOutputOrigin(80,120,40)
reslice1.SetOutputExtent(0,63,0,63,0,0)
reslice2 = vtk.vtkImageReslice()
reslice2.SetInputConnection(reader.GetOutputPort())
reslice2.SetInterpolationModeToLinear()
reslice2.SetOutputSpacing(0.65,0.65,1.5)
reslice2.SetOutputOrigin(80,120,40)
reslice2.SetOutputExtent(0,63,0,63,0,0)
reslice3 = vtk.vtkImageReslice()
reslice3.SetInputConnection(reader.GetOutputPort())
reslice3.SetInterpolationModeToNearestNeighbor()
reslice3.SetOutputSpacing(0.65,0.65,1.5)
reslice3.SetOutputOrigin(80,120,40)
reslice3.SetOutputExtent(0,63,0,63,0,0)
reslice4 = vtk.vtkImageReslice()
reslice4.SetInputConnection(reader.GetOutputPort())
reslice4.SetInterpolationModeToLinear()
reslice4.SetOutputSpacing(3.2,3.2,1.5)
reslice4.SetOutputOrigin(0,0,40)
reslice4.SetOutputExtent(0,63,0,63,0,0)
mapper1 = vtk.vtkImageMapper()
mapper1.SetInputConnection(reslice1.GetOutputPort())
mapper1.SetColorWindow(2000)
mapper1.SetColorLevel(1000)
mapper1.SetZSlice(0)
mapper2 = vtk.vtkImageMapper()
mapper2.SetInputConnection(reslice2.GetOutputPort())
mapper2.SetColorWindow(2000)
mapper2.SetColorLevel(1000)
mapper2.SetZSlice(0)
mapper3 = vtk.vtkImageMapper()
mapper3.SetInputConnection(reslice3.GetOutputPort())
mapper3.SetColorWindow(2000)
mapper3.SetColorLevel(1000)
mapper3.SetZSlice(0)
mapper4 = vtk.vtkImageMapper()
mapper4.SetInputConnection(reslice4.GetOutputPort())
mapper4.SetColorWindow(2000)
mapper4.SetColorLevel(1000)
mapper4.SetZSlice(0)
actor1 = vtk.vtkActor2D()
actor1.SetMapper(mapper1)
actor2 = vtk.vtkActor2D()
actor2.SetMapper(mapper2)
actor3 = vtk.vtkActor2D()
actor3.SetMapper(mapper3)
actor4 = vtk.vtkActor2D()
actor4.SetMapper(mapper4)
imager1 = vtk.vtkRenderer()
imager1.AddActor2D(actor1)
imager1.SetViewport(0.5,0.0,1.0,0.5)
imager2 = vtk.vtkRenderer()
imager2.AddActor2D(actor2)
imager2.SetViewport(0.0,0.0,0.5,0.5)
imager3 = vtk.vtkRenderer()
imager3.AddActor2D(actor3)
imager3.SetViewport(0.5,0.5,1.0,1.0)
imager4 = vtk.vtkRenderer()
imager4.AddActor2D(actor4)
imager4.SetViewport(0.0,0.5,0.5,1.0)
imgWin = vtk.vtkRenderWindow()
imgWin.AddRenderer(imager1)
imgWin.AddRenderer(imager2)
imgWin.AddRenderer(imager3)
imgWin.AddRenderer(imager4)
imgWin.SetSize(150,128)
imgWin.Render()
# --- end of script --
