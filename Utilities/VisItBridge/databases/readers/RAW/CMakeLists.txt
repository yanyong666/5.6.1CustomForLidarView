set(SOURCES
  avtRAWFileFormat.C
  )

ADD_VISIT_READER(VisItRAWReader "1.0"
  VISIT_READER_TYPE "STMD"
  VISIT_READER_NAME "avtRAWFileFormat"
  SERVER_SOURCES ${SOURCES}
  )
