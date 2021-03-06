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
#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "baserobotctrl.h"
#include "transportationtask.h"
#include "wavefrontmap.h"
#include <stage.hh>
#include <list>

/**
 * Manages all task, by dynamically altering the reward and the production
 * rate. The data comes from a script
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CTaskManager
{
  public:
    /** Default destructor */
    ~CTaskManager();
    /** Update method for stage */
    static int stgUpdate ( Stg::Model* mod, CTaskManager* taskManager );
    /**
     * Gets the only instance of this class
     * @param mod stage model
     * @return object of this class
     */
    static CTaskManager* getInstance ( Stg::Model* mod = NULL );
    /**
     * Registers a robot with the task manager
     * @param robot to register
     */
    void registerRobot( ABaseRobotCtrl* robot );

  protected:
    /**
     * Default constructor
     * @param mod stage model
     */
    CTaskManager ( Stg::Model* mod );
    /**
     * Loads the script
     * @return 1 of successfull, 0 otherwise
     */
    int loadScript();

  private:
    /** Stage model for this controller */
    Stg::Model* mStgModel;
    /** File name and path of script */
    std::string mScriptName;
    /** Current simulation time [s] */
    float mSimTime;
    /** Time of next task modification [s] */
    float mNextModificationTime;
    /** List of available tasks */
    std::list<CTransportationTask*> mTaskList;
    /** List of registered robots */
    std::list<ABaseRobotCtrl*> mRobotList;
    /** Map of the world to get distance between source and sink */
    CWaveFrontMap* mWaveFrontMap;

};

#endif
