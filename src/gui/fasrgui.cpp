/***************************************************************************
 * Project: FASR                                                           *
 * Author:  Jens Wawerla (jwawerla@sfu.ca)                                 *
 * $Id: $
 ***************************************************************************
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
 **************************************************************************/
#include "fasrgui.h"
/** Qt Application */
QApplication* mQtApp = NULL;

//-----------------------------------------------------------------------------
CFasrGui::CFasrGui( int argc, char* argv[] )
{
  mSchedulerDialog = NULL;
  mArgc = argc;
  mArgv = argv;
  pthread_create( &mPThread, NULL, &threadMain, this );

  sleep( 2 );  // need to wait for thread to finish creating the Qt App
}
//-----------------------------------------------------------------------------
CFasrGui::~CFasrGui()
{
  if (mQtApp) {
    mQtApp->quit();
    delete mQtApp;
  }
  
}
//-----------------------------------------------------------------------------
CFasrGui* CFasrGui::getInstance( int argc, char* argv[] )
{
  static CFasrGui* cguiInstance = NULL;

  if ( cguiInstance == NULL )
    cguiInstance = new CFasrGui( argc, argv );

  return cguiInstance;
}
//-----------------------------------------------------------------------------
void CFasrGui::registerRobot( ARobot* robot )
{
  mMainWindow->addRobot( robot );
}
//-----------------------------------------------------------------------------
void* CFasrGui::threadMain( void* arg )
{
  CFasrGui* gui = ( CFasrGui* )arg;
  mQtApp = new QApplication( gui->mArgc, gui->mArgv );
  mQtApp->setStyle( "plastique" );
  mQtApp->setOrganizationName( "Autolab" );
  mQtApp->setOrganizationDomain( "autolab.cmpt.sfu.ca" );
  mQtApp->setApplicationName( "RapiGui" );
  gui->mMainWindow = new CMainWindow();
  gui->mMainWindow->show();
  //gui->mSchedulerDialog = new CRobotSchedulerDialog( gui->mMainWindow );
  gui->mAnalystDialog = new CAnalystDialog( gui->mMainWindow );
  gui->mAnalystDialog->show();
  //gui->mMainWindow->addCustomDialog( gui->mSchedulerDialog );
  gui->mMainWindow->addCustomDialog( gui->mAnalystDialog );

  mQtApp->exec();

  return NULL;
}
//-----------------------------------------------------------------------------
