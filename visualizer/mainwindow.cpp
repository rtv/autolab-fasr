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
 * $Log: mainwindow.cpp,v $
 * Revision 1.6  2009-03-27 00:27:37  jwawerla
 * *** empty log message ***
 *
 * Revision 1.5  2009-03-26 05:15:30  jwawerla
 * *** empty log message ***
 *
 * Revision 1.4  2009-03-25 14:49:03  jwawerla
 * Task visualization works now
 *
 * Revision 1.3  2009-03-24 03:52:22  jwawerla
 * Pause button works now
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
 ***************************************************************************/
#include "mainwindow.h"
#include <QMetaType>

/** Update interval [ms] */
const int UPDATE_INTERVAL = 500;

//-----------------------------------------------------------------------------
CMainWindow::CMainWindow ( QString hostname, int port )
    : QMainWindow()
{
  QString title;

  qRegisterMetaType<tTaskMsg>("tTaskMsg");
  qRegisterMetaType<tDataMsg>("tDataMsg");


  mFgMapVisEabled = false;
  title = title.sprintf("%d", port);
  title = "RobotVis@"+hostname+":"+title;
  setWindowTitle ( title );

  mCentralWidget = new QWidget( this );
  QVBoxLayout* layout = new QVBoxLayout ( mCentralWidget );
  mCentralWidget->setLayout( layout );

  mTabWidget = new QTabWidget ( mCentralWidget );
  layout->addWidget( mTabWidget );

  mTaskFrame = new QFrame( this );
  mTaskFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
  mTaskFrame->setWindowTitle("Task");
  layout->addWidget( mTaskFrame );

  setCentralWidget ( mCentralWidget );

  mVisClient = new CVisClient ( this, hostname, port );

  mUpdateTimer = new QTimer ( this );
  mUpdateTimer->setInterval ( UPDATE_INTERVAL );
  mUpdateTimer->setSingleShot ( false );

  connect ( mVisClient, SIGNAL ( errorMessage ( QString ) ),
            this, SLOT ( displayError ( QString ) ) );
  connect ( mVisClient, SIGNAL ( newConfigMessage() ),
            this, SLOT ( newConfiguration() ) );
  connect ( mTabWidget, SIGNAL ( currentChanged ( int ) ),
            mVisClient, SLOT ( requestRobotData ( int ) ) );
  connect ( mVisClient, SIGNAL ( newRobotDataMessage ( tDataMsg ) ),
            this, SLOT ( newRobotData ( tDataMsg ) ) );
  connect ( mVisClient, SIGNAL ( newTaskDataMessage (tTaskMsg) ),
            this, SLOT (newTaskData( tTaskMsg ) ) );
  connect ( mUpdateTimer, SIGNAL ( timeout () ),
            this, SLOT ( updateTimeout() ) );

  mUpdateTimer->start();
}
//-----------------------------------------------------------------------------
CMainWindow::~CMainWindow()
{
}
//-----------------------------------------------------------------------------
void CMainWindow::updateTimeout()
{
  mUpdateCounter ++;

  if ( ( mUpdateCounter % 10 == 0 ) && ( mFgMapVisEabled == true ) ) {
    mVisClient->requestRobotMap ( mTabWidget->currentIndex() );
  }
  else  {
    if ( mUpdateCounter % 5 == 0 )
      mVisClient->requestTaskData ( );
    else
      mVisClient->requestRobotData ( mTabWidget->currentIndex() );
  }
}
//-----------------------------------------------------------------------------
void CMainWindow::addTabWidget ( QWidget* widget, QString title )
{
  mTabWidget->addTab ( widget, title );
}
//-----------------------------------------------------------------------------
void CMainWindow::displayError ( QString msg )
{
  QMessageBox::critical ( this, "RobotVis Client", msg );
  close();
}
//-----------------------------------------------------------------------------
void CMainWindow::newConfiguration()
{
  CTaskWidget* taskWidget;
  QHBoxLayout* taskLayout;
  CBaseRobotWidget* widget;
  QString title;

  for ( int i = 0; i < mVisClient->mConfigMsg.numRobots; i++ ) {
    title = title.sprintf ( "R%0d", i + 1 );
    widget = new CBaseRobotWidget ( i, this );
    addTabWidget (  widget, title );
    connect ( widget, SIGNAL(pauseRobot(int, bool)), mVisClient, SLOT(pauseRobot(int, bool)));;
  }

  taskLayout = new QHBoxLayout ( mTaskFrame );
  mTaskFrame->setLayout( taskLayout );

  for ( int i = 0; i < mVisClient->mConfigMsg.numTasks; i++ ) {
    taskWidget = new CTaskWidget( mTaskFrame );
    taskLayout->addWidget( taskWidget );
    mTaskWidgetVector.push_back( taskWidget );
  }
}
//-----------------------------------------------------------------------------
void CMainWindow::newRobotData ( tDataMsg dataMsg )
{
  CBaseRobotWidget* widget = NULL;

  if ( dataMsg.id <= mTabWidget->count() ) {
    widget = ( CBaseRobotWidget* ) mTabWidget->widget ( dataMsg.id );
    if ( widget )
      widget->udateData ( & ( dataMsg ) );
  }
}
//-----------------------------------------------------------------------------
void CMainWindow::newTaskData ( tTaskMsg taskMsg )
{
  if ( (taskMsg.id >= 0) && ( taskMsg.id < mTaskWidgetVector.size() ) )
    mTaskWidgetVector[taskMsg.id]->updateData( &taskMsg );
}
//-----------------------------------------------------------------------------

