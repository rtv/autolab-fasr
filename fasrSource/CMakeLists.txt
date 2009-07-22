
link_directories( ${STAGE_LIBRARY_DIRS} )

set_source_files_properties( fasrsource.cpp PROPERTIES COMPILE_FLAGS "${FLTK_CFLAGS}" )

#build a shared library
ADD_LIBRARY(fasrsource MODULE
  main.cpp
  fasrsource.cpp
)

# delete the "lib" prefix from the plugin libraries
SET_TARGET_PROPERTIES( fasrsource PROPERTIES PREFIX "" )

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES(fasrsource
  ${STAGE_LIBRARIES}
  ${RAPI_LIBRARIES}
)

INSTALL(TARGETS fasrsource
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)
