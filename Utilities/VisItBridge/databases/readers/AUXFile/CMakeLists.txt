set(SOURCES
  avtAUXFileFileFormat.C
  )

ADD_VISIT_READER(VisItAuxFileReader "1.0"
  VISIT_READER_TYPE "STSD"
  VISIT_READER_NAME "avtAUXFileFileFormat"
  SERVER_SOURCES ${SOURCES}
  )
