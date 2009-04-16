

# build a loadable shared library module 
ADD_LIBRARY(fasrrobot MODULE
            main.cpp
            baserobot.cpp
            looppolicyrobot.cpp
            staticpolicyrobot.cpp
            replanpolicyrobot.cpp
            swcostcompolicyrobot.cpp
            waitprobpolicyrobot.cpp
            individualswcostpolicyrobot.cpp
            broadcast.cpp
            stagelaser.cpp
            worktask.cpp
            destination.cpp
            charger.cpp
            visserver.cpp
            logwriter.cpp
            robotscheduler.cpp
            ../taskmanager/taskmanager.cpp
            )

# delete the "lib" prefix from the plugin libraries
SET_TARGET_PROPERTIES( fasrrobot PROPERTIES PREFIX "" )

TARGET_LINK_LIBRARIES( fasrrobot
  ${ND_LIBRARIES}
  ${WP_LIBRARIES}
  ${STAGE_LIBRARIES}
  ${OPENGL_LIBRARIES}
)

INSTALL(TARGETS fasrrobot
	LIBRARY DESTINATION lib
)
