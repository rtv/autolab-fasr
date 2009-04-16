
# list all qt header files here for moc
QT_WRAP_CPP(visualizer MOC_SRC
  mainwindow.h
  baserobotwidget.h
  visclient.h
  dataline.h
  dataled.h
)

# list all source files here
ADD_EXECUTABLE(visualizer
  main.cpp
  mainwindow.cpp
  baserobotwidget.cpp
  visclient.cpp
  dataled.cpp
  dataline.cpp
  cmdlineparser.cpp
  taskwidget.cpp
  ../src/error.c
  ${MOC_SRC}
)

link_directories( ${QT_LIBRARY_DIR} )


# TARGET_LINK_LIBRARIES(visualizer ${QT_LIBRARIES} ${SDL_LIBRARY} SDLmain )
TARGET_LINK_LIBRARIES(visualizer ${QT_LIBRARIES} ${SDL_LIBRARY} )




