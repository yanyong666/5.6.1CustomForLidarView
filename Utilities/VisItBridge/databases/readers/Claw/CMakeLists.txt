SET(SOURCES
  avtClawFileFormat.C
  )

ADD_VISIT_READER(VisItClawReader "1.0"
  VISIT_READER_TYPE "MTMD"
  VISIT_READER_NAME "avtClawFileFormat"
  SERVER_SOURCES ${SOURCES}
  )
