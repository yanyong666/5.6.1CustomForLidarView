set(SOURCES
  avtTecplotFileFormat.C
  avtTecplotOptions.C
  avtTecplotWriter.C
  TecplotPluginInfo.C
  TecplotEnginePluginInfo.C
  TecplotCommonPluginInfo.C
  TecplotFile.C
  )

set(BINARY_SOURCES
  avtTecplotBinaryFileFormat.C
  TecplotFile.C
  )

ADD_VISIT_READER(VisItTecplotReader "1.0"
  VISIT_READER_TYPE "STMD"
  VISIT_READER_NAME "avtTecplotFileFormat"
  VISIT_READER_USES_OPTIONS ON
  SERVER_SOURCES ${SOURCES}
  )

ADD_VISIT_READER(VisItTecplotBinaryReader "1.0"
  VISIT_READER_TYPE "STMD"
  VISIT_READER_NAME "avtTecplotBinaryFileFormat"
  VISIT_READER_USES_OPTIONS ON
  VISIT_READER_OPTIONS_NAME "avtTecplotFileFormat"
  SERVER_SOURCES ${BINARY_SOURCES}
  )
