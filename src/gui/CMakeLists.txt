
ADD_LIBRARY(autolab-fasr-gui STATIC
  fasrgui.cpp
  analysttaskwidget.cpp
  robotschedulerdialog.cpp
  taskdatawidget.cpp
  analystdialog.cpp
)

TARGET_LINK_LIBRARIES( autolab-fasr-gui
  ${RAPI_LIBRARIES}
  ${RAPI_GUI_LIBRARIES}
  ${QT_LIBRARIES}
)