/***************************************************************************
 *   Copyright (C) 2009 by Jens
 *   jwawerla@sfu.ca
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 * $Log: mainwindow.h,v $
 * Revision 1.3  2009-03-25 14:49:03  jwawerla
 * Task visualization works now
 *
 * Revision 1.2  2009-03-23 06:24:22  jwawerla
 * RobotVis works now for data, maps still need more work
 *
 * Revision 1.1  2009-03-16 14:27:19  jwawerla
 * robots still get stuck
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 **************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "baserobotwidget.h"
#include "taskwidget.h"
#include "visclient.h"
#include <QtGui>
#include <QTimer>



/**
 * Main window for qt application, basically a the entry point for everything gui
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CMainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    /** Default constructor */
    CMainWindow(QString hostname = "localhost", int port = 7000);
    /** Default destructor */
    ~CMainWindow();
    /**
     * Adds a widget to the tab view
     * @param widget to be added
     * @param title of tab
     */
    void addTabWidget ( QWidget* widget, QString title );


  protected slots:
    /**
     * Slot for displaying error messages
     * @param msg message to inform user about error
     */
    void displayError ( QString msg );
    /** Slot to inform about a new configuration of the server */
    void newConfiguration();
    /** New data for robot with id robotId available */
    void newRobotData ( tDataMsg );
    /** Slot of timeout signal of update timer */
    void updateTimeout();
    /** Slot for new task data */
    void newTaskData ( tTaskMsg taskMsg );

  private:
    /** Central widget */
    QWidget* mCentralWidget;
    /** Frame for all the tasks */
    QFrame* mTaskFrame;
    /** Tab widget to hold the robot display widgets */
    QTabWidget* mTabWidget;
    /** Client for robotVis communication */
    CVisClient* mVisClient;
    /** Timer for updating the information */
    QTimer* mUpdateTimer;
    /** Count updates */
    int mUpdateCounter;
    /** Flags if we visualizes the map data or not */
    bool mFgMapVisEabled;
    /** Vector of task widgets */
    std::vector<CTaskWidget*> mTaskWidgetVector;
};

#endif
