#!/usr/bin/env python
import vtk

htg = vtk.vtkUniformHyperTreeGrid()
htg.Initialize()

scalarArray = vtk.vtkDoubleArray()
scalarArray.SetName('scalar')
scalarArray.SetNumberOfValues(0)
htg.GetPointData().AddArray(scalarArray)
htg.GetPointData().SetActiveScalars('scalar')

htg.SetDimensions([4, 3, 1])
htg.SetBranchFactor(2)
htg.SetOrigin([-1., -1., -2])
htg.SetGridScale([1., 1., 1.])

# Let's split the various trees
cursor = vtk.vtkHyperTreeGridNonOrientedCursor()
offsetIndex = 0

# ROOT CELL 0
htg.InitializeNonOrientedCursor(cursor, 0, True)
cursor.SetGlobalIndexStart(offsetIndex)

idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 10)

cursor.SubdivideLeaf()

# ROOT CELL 0/[0-3]
cursor.ToChild(0)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 100)
cursor.ToParent()

cursor.ToChild(1)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 101)
cursor.ToParent()

cursor.ToChild(2)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 101)
cursor.ToParent()

cursor.ToChild(3)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 103)
cursor.ToParent()

# ROOT CELL 0

offsetIndex += cursor.GetTree().GetNumberOfVertices()

# ROOT CELL 1
htg.InitializeNonOrientedCursor(cursor, 1, True)
cursor.SetGlobalIndexStart(offsetIndex)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 11)

offsetIndex += cursor.GetTree().GetNumberOfVertices()

# ROOT CELL 2
htg.InitializeNonOrientedCursor(cursor, 2, True)
cursor.SetGlobalIndexStart(offsetIndex)

idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 12)

cursor.SubdivideLeaf()

# ROOT CELL 2/[0-3]
cursor.ToChild(0)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 120)
cursor.ToParent()

cursor.ToChild(1)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 121)
cursor.ToParent()

cursor.ToChild(2)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 122)
cursor.ToParent()

cursor.ToChild(3)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 123)
cursor.ToParent()

# ROOT CELL 2

offsetIndex += cursor.GetTree().GetNumberOfVertices()

# ROOT CELL 3
htg.InitializeNonOrientedCursor(cursor, 3, True)
cursor.SetGlobalIndexStart(offsetIndex)

idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 13)

offsetIndex += cursor.GetTree().GetNumberOfVertices()

# ROOT CELL 4
htg.InitializeNonOrientedCursor(cursor, 4, True)
cursor.SetGlobalIndexStart(offsetIndex)

idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 14)

cursor.SubdivideLeaf()

# ROOT CELL 4/[0-3]
cursor.ToChild(0)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 140)
cursor.ToParent()

cursor.ToChild(1)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 141)
cursor.ToParent()

cursor.ToChild(2)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 142)
cursor.ToParent()

cursor.ToChild(3)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 143)

cursor.SubdivideLeaf()

# ROOT CELL 4/3/[0-3]
cursor.ToChild(0)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 1430)

cursor.SubdivideLeaf()

# ROOT CELL 4/3/0/[0-3]
cursor.ToChild(0)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 14300)
cursor.ToParent()

cursor.ToChild(1)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 14301)
cursor.ToParent()

cursor.ToChild(2)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 14302)
cursor.ToParent()

cursor.ToChild(3)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 14303)
cursor.ToParent()

# ROOT CELL 4/3/0
cursor.ToParent()

# ROOT CELL 4/3/[1-3]
cursor.ToChild(1)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 1431)
cursor.ToParent()

cursor.ToChild(2)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 1432)
cursor.ToParent()

cursor.ToChild(3)
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 1433)
cursor.ToParent()

# ROOT CELL 4/3

cursor.ToParent()

# ROOT CELL 4

offsetIndex += cursor.GetTree().GetNumberOfVertices()

# ROOT CELL 5
htg.InitializeNonOrientedCursor(cursor, 5, True)
cursor.SetGlobalIndexStart(offsetIndex)

idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 15)

print('#',scalarArray.GetNumberOfTuples())
print('DataRange: ',scalarArray.GetRange())

isFilter = False

# Depth Limiter Filter
depth = None
if True:
  print('With Depth Limiter Filter (HTG)')
  depth = vtk.vtkHyperTreeGridDepthLimiter()
  depth.SetInputData(htg)
  depth.SetDepth(1)
  isFilter = True
else:
  print('Without Depth Limiter Filter (HTG)')
  depth = htg

# Threshold Filter
threshold = None
if True:
  print('With Threshold Filter (HTG)')
  threshold = vtk.vtkHyperTreeGridThreshold()
  if isFilter:
    threshold.SetInputConnection(depth.GetOutputPort())
  else:
    threshold.SetInputData(depth)
  #[9,99] :  x
  #         x x
  #threshold.ThresholdBetween(9,99)
  #[19,999] : xx  xx       * = xx\/
  #           xx  xx           xx/\
  #             xx             \/\/
  #             x*             /\/\
  threshold.ThresholdBetween(9,9999)
  isFilter = True
else:
  print('No Threshold Filter (HTG)')
  threshold = depth

# Axis reflection
reflection = None
if True:
  print('With AxisReflection Filter (HTG)')
  reflection = vtk.vtkHyperTreeGridAxisReflection()
  if isFilter:
    reflection.SetInputConnection(threshold.GetOutputPort())
  else:
    reflection.SetInputData(threshold)
  reflection.SetPlaneToY()
  reflection.SetCenter(0)
  isFilter = True
else:
  print('No AxisReflection Filter (HTG)')
  reflection = threshold

# Geometries
geometry = vtk.vtkHyperTreeGridGeometry()
if isFilter:
  geometry.SetInputConnection(reflection.GetOutputPort())
else:
  geometry.SetInputData(reflection)
print('With Geometry Filter (HTG to NS)')

# Shrink Filter
if True:
  print('With Shrink Filter (NS)')
  # En 3D, le shrink ne doit pas se faire sur la geometrie car elle ne represente que la peau
  shrink = vtk.vtkShrinkFilter()
  shrink.SetInputConnection(geometry.GetOutputPort())
  shrink.SetShrinkFactor(.8)
else:
  print('No Shrink Filter (NS)')
  shrink = geometry

# LookupTable
lut = vtk.vtkLookupTable()
lut.SetHueRange(0.66, 0)
lut.Build()

# Mappers
mapper = vtk.vtkDataSetMapper()
mapper.SetInputConnection(shrink.GetOutputPort())

shrink.Update()
dataRange = shrink.GetOutput().GetCellData().GetArray('scalar').GetRange()
print('DataRange (after shrink): ', dataRange)

mapper.SetLookupTable(lut)
mapper.SetColorModeToMapScalars()
mapper.SetScalarModeToUseCellFieldData()
mapper.SelectColorArray('scalar')
mapper.SetScalarRange(dataRange[0], dataRange[1])

# Actors
actor = vtk.vtkActor()
actor.SetMapper(mapper)

# Camera
bd = htg.GetBounds()
camera = vtk.vtkCamera()
camera.SetClippingRange(1., 100.)
focal = []
for i in range(3):
  focal.append(bd[ 2 * i ] + (bd[ 2 * i + 1 ] - bd[ 2 * i]) / 2.)
camera.SetFocalPoint(focal)
camera.SetPosition(focal[0], focal[1], focal[2] + 4.)

# Renderer
renderer = vtk.vtkRenderer()
renderer.SetActiveCamera(camera)
renderer.AddActor(actor)

# Render window
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(renderer)
renWin.SetSize(600, 400)

# Render window interactor
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

# render the image
renWin.Render()
# iren.Start()

# prevent the tk window from showing up then start the event loop
# --- end of script --
