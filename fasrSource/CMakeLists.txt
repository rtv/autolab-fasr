
link_directories( ${STAGE_LIBRARY_DIRS} )

set_source_files_properties( fasrsource.cpp PROPERTIES COMPILE_FLAGS "${FLTK_CFLAGS}" )

#build a shared library
ADD_LIBRARY(fasrsource MODULE
  main.cpp
  fasrsource.cpp
  ../src/error.c
  ../src/utilities.cpp
)

# delete the "lib" prefix from the plugin libraries
SET_TARGET_PROPERTIES( fasrsource PROPERTIES PREFIX "" )

TARGET_LINK_LIBRARIES(fasrsource ${STAGE_LIBRARIES} )

INSTALL(TARGETS fasrsource
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
)
