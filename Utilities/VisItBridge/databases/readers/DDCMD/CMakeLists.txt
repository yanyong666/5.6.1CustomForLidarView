set(SOURCES
  avtDDCMDFileFormat.C
  object.C
  error.C
  )

ADD_VISIT_READER(VisItDDCMDReader "1.0"
  VISIT_READER_TYPE "STMD"
  VISIT_READER_NAME "avtDDCMDFileFormat"
  SERVER_SOURCES ${SOURCES}
  )
