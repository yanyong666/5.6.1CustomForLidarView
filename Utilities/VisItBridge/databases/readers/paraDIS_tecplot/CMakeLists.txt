set(SOURCES
  avtparaDIS_tecplotFileFormat.C
)

ADD_VISIT_READER(VisItParaDISTecplotReader "1.0"
  VISIT_READER_TYPE "STSD"
  VISIT_READER_NAME "avtparaDIS_tecplotFileFormat"
  VISIT_READER_USES_OPTIONS OFF
  SERVER_SOURCES ${SOURCES}
  )
