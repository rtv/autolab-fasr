
LINK_DIRECTORIES( ${STAGE_LIBRARY_DIRS} )

set_source_files_properties( fasrrobot.cpp PROPERTIES COMPILE_FLAGS "${FLTK_CFLAGS}" )

#build a shared library
ADD_LIBRARY(autolab-fasr MODULE
  main.cpp
  baserobotctrl.cpp
  robotscheduler.cpp
  looppolicyrobotctrl.cpp
  transportationtask.cpp
  destination.cpp
  charger.cpp
  broadcast.cpp
  analyst.cpp
  staticpolicyrobotctrl.cpp
  replanpolicyrobotctrl.cpp
  waitprobpolicyrobotctrl.cpp
  waitpolicyrobotctrl.cpp
  fixedratiopolicyrobotctrl.cpp
  epsilonsamplerrobotctrl.cpp
  ../taskManager/taskmanager.cpp

)

IF (QT_FOUND)
  ADD_SUBDIRECTORY(gui)
  ADD_DEFINITIONS(-DGUI)
  SET( FASRGUI_LIBRARIES
    ${RAPI_GUI_LIBRARIES}
    autolab-fasr-gui
  )
ENDIF (QT_FOUND)

# delete the "lib" prefix from the plugin libraries
SET_TARGET_PROPERTIES( autolab-fasr
  PROPERTIES PREFIX ""
  LINK_FLAGS "${STAGE_LDFLAGS_MORE}"
)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES( autolab-fasr
  ${STAGE_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${RAPI_LIBRARIES}
  ${FASRGUI_LIBRARIES}
  ${QT_LIBRARIES}
  ${ND_LIBRARIES}
  ${WP_LIBRARIES}
)

INSTALL(TARGETS autolab-fasr
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)
