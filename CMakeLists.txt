
PROJECT(fasrrobot)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
ADD_DEFINITIONS(-g -Wall -O2)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE( STAGE REQUIRED stage )
PKG_SEARCH_MODULE( ND    REQUIRED autolab-nd )
PKG_SEARCH_MODULE( WP    REQUIRED autolab-wp )

INCLUDE_DIRECTORIES( src
                     fasrSource
                     taskmanager
                     ${STAGE_INCLUDE_DIRS}
                     ${ND_INCLUDE_DIRS}
                     ${WP_INCLUDE_DIRS}
)

LINK_DIRECTORIES( ${STAGE_LIBRARY_DIRS} 
						${ND_LIBRARY_DIRS}
						${WP_LIBRARY_DIRS}
)


ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(fasrSource)
ADD_SUBDIRECTORY(taskmanager)

