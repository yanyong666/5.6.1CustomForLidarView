set(classes
  vtkArcSource
  vtkArrowSource
  vtkButtonSource
  vtkCellTypeSource
  vtkConeSource
  vtkCubeSource
  vtkCylinderSource
  vtkDiagonalMatrixSource
  vtkDiskSource
  vtkEllipseArcSource
  vtkEllipticalButtonSource
  vtkFrustumSource
  vtkGlyphSource2D
  vtkGraphToPolyData
  vtkHyperTreeGridSource
  vtkLineSource
  vtkOutlineCornerFilter
  vtkOutlineCornerSource
  vtkOutlineSource
  vtkParametricFunctionSource
  vtkPlaneSource
  vtkPlatonicSolidSource
  vtkPointSource
  vtkPolyLineSource
  vtkPolyPointSource
  vtkProgrammableDataObjectSource
  vtkProgrammableSource
  vtkRandomHyperTreeGridSource
  vtkRectangularButtonSource
  vtkRegularPolygonSource
  vtkSelectionSource
  vtkSphereSource
  vtkSuperquadricSource
  vtkTessellatedBoxSource
  vtkTextSource
  vtkTexturedSphereSource
  vtkUniformHyperTreeGridSource)

vtk_module_add_module(VTK::FiltersSources
  CLASSES ${classes})
