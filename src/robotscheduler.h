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
 * $Log: robotscheduler.h,v $
 * Revision 1.4  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.3  2009-03-29 00:54:27  jwawerla
 * Replan ctrl seems to work now
 *
 * Revision 1.2  2009-03-28 21:53:57  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1  2009-03-28 00:54:39  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 **************************************************************************/
#ifndef ROBOTSCHEDULER_H
#define ROBOTSCHEDULER_H

#include <vector>
#include <list>
#include "replanpolicyrobotinterface.h"
#include "worktask.h"

/**
 * Scheduler robot globaly
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CRobotScheduler
{
  public:
    /** Default constructor */
    CRobotScheduler();
    /** Default destructor */
    virtual ~CRobotScheduler();
    /**
     * Register a robot with the planner
     * @param robot to register
     */
    void registerRobot ( IReplanPolicyRobotInterface* robot );
    /**
     * Add a work task to the planner
     * @param task to be added
     */
    void addWorkTask ( IWorkTaskRobotInterface* task );
    /**
     * Gets the next work task for a given robot
     * @param robot to get task for
     * @return next task for robot
     */
    IWorkTaskRobotInterface* getWorkTask ( IReplanPolicyRobotInterface* robot );
    /**
     * Notifies the planner that a given robot is now charging
     * @param robot that is charging
     */
    void robotIsCharging ( IReplanPolicyRobotInterface* robot );

  protected:

    typedef struct {
      IWorkTaskRobotInterface* task;
      unsigned short optNumOfWorkers;
      std::list<IReplanPolicyRobotInterface*> robotsAssignedList;
    } tTaskData;

  private:
    /** Vector of all robots */
    std::vector<IReplanPolicyRobotInterface*> mRobotVector;
    /** List of free robots */
    std::list<IReplanPolicyRobotInterface*> mFreeRobotList;
    /** List of all tasks */
    std::vector<tTaskData> mTaskVector;

};

#endif
