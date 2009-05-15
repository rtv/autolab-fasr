
PROJECT(autolab-fasr)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE OFF)

INCLUDE(FindPkgConfig)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-g -Wall -O2)

PKG_SEARCH_MODULE( GLIB REQUIRED gthread-2.0 )

IF( GLIB_FOUND )
  MESSAGE( STATUS ${INDENT} "Glib version ${GLIB_VERSION} detected at ${GLIB_PREFIX}" )
  MESSAGE( STATUS "    GLIB_CFLAGS = ${GLIB_CFLAGS}" )
  MESSAGE( STATUS "    GLIB_LDFLAGS = ${GLIB_LDFLAGS}" )
ELSE( GLIB_FOUND )
  MESSAGE( ${INDENT} "Glib not detected" )
ENDIF( GLIB_FOUND )

PKG_SEARCH_MODULE( STAGE REQUIRED stage )

IF( STAGE_FOUND )
  MESSAGE( STATUS "STAGE ${STAGE_LIBRARY_DIRS}" )
  MESSAGE( STATUS "STAGE ${STAGE_LIBRARIES}" )
  MESSAGE( STATUS "STAGEDIR ${STAGE_INCLUDE_DIRS}" )
ELSE( STAGE_FOUND )
  MESSAGE(  "Stage was not found" )
ENDIF( STAGE_FOUND )

PKG_SEARCH_MODULE( RAPI REQUIRED autolab-rapi-stage )

IF (RAPI_FOUND)
  MESSAGE(STATUS "autolab-rapi-stage version ${RAPI_VERSION} detected at ${RAPI_PREFIX}" )
ELSE (RAPI_FOUND)
  MESSAGE(STATUS "autolab-rapi-stage not detected")
ENDIF (RAPI_FOUND)

PKG_SEARCH_MODULE( RAPI_GUI REQUIRED autolab-rapi-gui )

IF (RAPI_GUI_FOUND)
  MESSAGE(STATUS "autolab-rapi-gui version ${RAPI_GUI_VERSION} detected at ${RAPI_GUI_PREFIX}" )
ELSE (RAPI_GUI_FOUND)
  MESSAGE(STATUS "autolab-rapi-gui not detected")
ENDIF (RAPI_GUI_FOUND)

PKG_SEARCH_MODULE( ND REQUIRED autolab-nd )

IF (ND_FOUND)
  MESSAGE(STATUS "autolab-nd version ${ND_VERSION} detected at ${ND_PREFIX}" )
ELSE (ND_FOUND)
  MESSAGE(STATUS "autolab-nd not detected")
ENDIF (ND_FOUND)

PKG_SEARCH_MODULE( WP REQUIRED autolab-wp )

IF (WP_FOUND)
  MESSAGE(STATUS "autolab-wp version ${WP_VERSION} detected at ${WP_PREFIX}" )
ELSE (WP_FOUND)
  MESSAGE(STATUS "autolab-wp not detected")
ENDIF (WP_FOUND)

FIND_PACKAGE(Qt4 REQUIRED)
IF (QT_FOUND)
  INCLUDE(${QT_USE_FILE})
  ADD_DEFINITIONS( ${QT_DEFINITIONS} )
ELSEIF(QT_FOUND)
  MESSAGE(STATUS "Qt4 not detected")
ENDIF(QT_FOUND)

INCLUDE_DIRECTORIES( ./src
                     ./fasrSource
                     ./taskManager
                     ${RAPI_INCLUDE_DIRS}
                     ${RAPI_GUI_INCLUDE_DIRS}
                     ${ND_INCLUDE_DIRS}
                     ${WP_INCLUDE_DIRS}
                     ${STAGE_INCLUDE_DIRS}
                     ${GLIB_INCLUDE_DIRS}
                     ${QT_INCLUDE_DIR}
)

LINK_DIRECTORIES( ${STAGE_LIBRARY_DIRS}
                  ${RAPI_LIBRARY_DIRS}
                  ${RAPI_GUI_LIBRARY_DIRS}
                  ${ND_LIBRARY_DIRS}
                  ${WP_LIBRARY_DIRS}
                  ${QT_LIBRARY_DIR}
)


ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(fasrSource)
ADD_SUBDIRECTORY(taskManager)