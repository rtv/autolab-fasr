

# build a loadable shared library module 
ADD_LIBRARY(fasrrobot MODULE
            ../taskmanager/taskmanager.cpp
            baserobot.cpp
            broadcast.cpp
            charger.cpp
            destination.cpp
            individualswcostpolicyrobot.cpp
            logwriter.cpp
            looppolicyrobot.cpp
            main.cpp
            replanpolicyrobot.cpp
            robotscheduler.cpp
            stagelaser.cpp
            staticpolicyrobot.cpp
            swcostcompolicyrobot.cpp
            visserver.cpp
            waitprobpolicyrobot.cpp
            worktask.cpp
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
