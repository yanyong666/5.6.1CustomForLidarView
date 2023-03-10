vtk_module(vtkIOImage
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkCommonSystem
    vtkCommonMath
    vtkCommonMisc
    vtkCommonTransforms
    vtkIOCore
  PRIVATE_DEPENDS
    vtkjpeg
    vtkpng
    vtktiff
    vtkMetaIO
    #vtkDICOMParser
    vtksys
  #TEST_DEPENDS
  #  vtkTestingCore
  #  vtkImagingSources
  #  vtkImagingMath
  #  vtkRenderingContext2D
  #  vtkRenderingCore
  #  vtkTestingCore
  #  vtkTestingRendering
  KIT
    vtkIO
  )
