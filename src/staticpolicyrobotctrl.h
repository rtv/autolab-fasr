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
#ifndef STATICPOLICYROBOTCTRL_H
#define STATICPOLICYROBOTCTRL_H

#include "baserobotctrl.h"
#include "robotschedulerinterface.h"
#include "robotscheduler.h"

/**
 * This robot controller uses a static policy. Under this policy the robot
 * initialy requests a task from the scheduler CRobotScheduler and sticks
 * with this task.
 * @author Jens Wawerla
 */
class CStaticPolicyRobotCtrl : public ABaseRobotCtrl, public IRobotSchedulerInterface
{
  public:
    /**
     * Default constructor
     * @param robot to control
     */
    CStaticPolicyRobotCtrl( ARobot* robot );
    /** Default destructor */
    virtual ~CStaticPolicyRobotCtrl();
    /** Start up policy */
    void startPolicy();
    /**
     * Policy for leaving a charger, this must set mState to what ever we
     * should do next
     * @param dt length of time step [s]
     */
    virtual void leaveChargerPolicy( float dt );
    /**
     * Policy for delivery completed, this policy determines what to do next,
     * after the cargo was delivered to the sink
     */
    void deliveryCompletedPolicy( float dt );
    /**
     * Policy for robot on rank 1 in waiting queue at depot. This is the robot
     * that is allowed to leave the queue next, the policy determines when
     * the robot leave the depot and what it does next
     */
    void unallocatedPolicy( float dt );
    /**
     * Policy for pickup completed, this policy allows the robot controller to
     * decide what to do after the load was picked up
     */
    void pickupCompletedPolicy( float dt );
    /**
     * Policy for waiting at the source
     */
    virtual void waitingAtSourcePolicy( float dt);
    /**
     * Policy for waiting at the pickup patch, note not in the queue
     * @param dt length of time step [s]
     */
    void waitingAtPickupPolicy ( float dt );
    /**
     * Gets the ID of the robot
     * @return robot id
     */
    virtual unsigned int getRobotId() const { return mRobotId; };
    /**
     * Gets the current task of the robot
     * @return task
     */
    virtual ITransportTaskInterface* getTask() const { return mCurrentTask; };
    /**
     * Gets the capacity of the cargo bay
     * return [flags]
     */
    unsigned int getCargoBayCapacity() const { return mCargoBayCapacity; };
    /**
     * Adds a transportation task to the robot
     * @param task to add
     */
    void addTransportationTask( ITransportTaskInterface* task );

  protected:
    /** Scheduler */
    CRobotScheduler* mScheduler;
    /** Id of robot */
    unsigned int mRobotId;
};

#endif
