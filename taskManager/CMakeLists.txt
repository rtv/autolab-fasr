
link_directories( ${STAGE_LIBRARY_DIRS} )

# set_source_files_properties( taskmanager.cpp PROPERTIES COMPILE_FLAGS "${FLTK_CFLAGS}" )

#build a shared library
ADD_LIBRARY(taskmanager MODULE
  main.cpp
  taskmanager.cpp
  ../src/transportationtask.cpp
  ../src/destination.cpp

)

# delete the "lib" prefix from the plugin libraries
SET_TARGET_PROPERTIES( taskmanager PROPERTIES 
							  PREFIX ""
							  LINK_FLAGS "${STAGE_LDFLAGS_MORE}"
 )

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES(taskmanager
  ${STAGE_LIBRARIES}
  ${RAPI_LIBRARIES}
  ${WP_LIBRARIES}
)

INSTALL(TARGETS taskmanager
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)
