set(SOURCES
  avtANSYSFileFormat.C
  )

ADD_VISIT_READER(VisItANSYSReader "1.0"
  VISIT_READER_TYPE "STSD"
  VISIT_READER_NAME "avtANSYSFileFormat"
  SERVER_SOURCES ${SOURCES}
  )
