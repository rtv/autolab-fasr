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
#ifndef FASRGUI_H
#define FASRGUI_H

#include "RapiGui"
#include "robotschedulerdialog.h"
#include "analystdialog.h"

/**
 * Custom gui for fasr probject
 * @author Jens Wawerla
 */
class CFasrGui
{
  public:
    /** Default destructor */
    ~CFasrGui();
    /**
     * Gets the only instance of this class
     * @return class instance
     */
    static CFasrGui* getInstance( int argc, char* argv[] );
    /**
     * Registers a robot with this gui, so it can be visualized
     * @param robot to visualize
     */
    void registerRobot( ARobot* robot );

  protected:
    /** Main for the thread */
    static void* threadMain( void* arg );
    /** Main window */
    CMainWindow* mMainWindow;

  private:
    /** Default constructor */
    CFasrGui( int argc=0, char* argv[]=NULL );
    /** Robot scheduler dialog */
    CRobotSchedulerDialog* mSchedulerDialog;
    /** Analyst dialog */
    CAnalystDialog* mAnalystDialog;
    /** Thread structure */
    pthread_t mPThread;
    /** Command line parameters for QT */
    int mArgc;
    char** mArgv;


};

#endif
