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
 * $Log: replanpolicyrobot.h,v $
 * Revision 1.4  2009-04-03 16:21:30  jwawerla
 * Move wavefront map specification to world file
 *
 * Revision 1.3  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.2  2009-03-28 21:53:57  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1  2009-03-28 00:54:39  jwawerla
 * *** empty log message ***
 *
 *
 **************************************************************************/
#ifndef REPLANPOLICYROBOT_H
#define REPLANPOLICYROBOT_H

#include "baserobotctrl.h"
#include "robotscheduler.h"
#include "replanpolicyrobotinterface.h"

/** Static list of working robots */
static CRobotScheduler robotScheduler;

/**
 * Implements a replanning policy, this robot replans the optimal task
 * allocation after completion of each task using the latest experience.
 * @author Jens Wawerla <jwawerla@sfu.ca>
*/
class CReplanPolicyRobotCtrl : public ABaseRobotCtrl, public IReplanPolicyRobotInterface
{
  public:
    /**
     * Default constructor
     * @param robot we controll
     */
    CReplanPolicyRobotCtrl ( ARobot* robot );
    /** Default destructor */
    ~CReplanPolicyRobotCtrl();
    /** Checks if this robot is working or not */
    virtual bool isWorking();
    /** Checks if a robot is in state unallocated */
    virtual bool isUnAllocated();
    /**
     * Gets the currently assigned task
     * @return task
     */
    virtual IWorkTaskRobotInterface* getTask();
    /** Gets the id of the robot */
    virtual int getRobotId();
    /**
     * Chooses the next work task according to some policy
     * @param dt length of time step [s]
     */
    virtual void selectWorkTaskPolicy ( float dt );
    /**
     * Policy for start up
     * @param dt length of time step [s]
     */
    virtual void startupPolicy ( float dt );
    /**
     * Policy for leaving a charger
     * @param dt length of time step [s]
     */
    virtual void leaveChargerPolicy ( float dt );
    /**
     * Policy for waiting at source
     * @param dt length of time step [s]
     */
    virtual void waitAtSourcePolicy ( float dt );
    /**
     * Policy for waiting at charger
     * @param dt length of time step [s]
     */
    virtual void waitAtChargerPolicy ( float dt );
    /**
     * Policy for the first unallocated robot
     * @param dt length of time step [s]
     */
    virtual void unallocatedPolicy ( float dt );
    /**
     * Policy for a successfull pick up
     * @param dt length of time step [s]
     */
    virtual void pickupCompletedPolicy ( float dt );
    /**
     * Policy for waiting at the pickup patch, note not in the queue
     * @param dt length of time step [s]
     */
    virtual void waitingAtPickupPolicy ( float dt );
    /**
     * Adds a task to the list of work tasks, the list is sorted by reward rate
     * @param task to be added
     */
    virtual void addWorkTask ( IWorkTaskRobotInterface* task );
    /**
     * Gets the capacity of the cargo bay
     * @return [flags]
     */
    virtual int getCargoBayCapacity() { return mCargoBayCapacity; };

  protected:
    /**
     * Overloaded charging policy, this one removes a charging robot from the
     * workerlist
     * @param dt length of time step [s]
     */
    bool chargingPolicy ( float dt );


  private:
    /** Flags if robot is first in waiting queue at depot */
    bool mFgFirstInDepot;

};

#endif
