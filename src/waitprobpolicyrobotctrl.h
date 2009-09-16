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
#ifndef WAITPROBPOLICYROBOTCTRL_H
#define WAITPROBPOLICYROBOTCTRL_H

#include "baserobotctrl.h"
#include "broadcast.h"

/**
 * Probabilistic robot controller based on the waiting times in various
 * situations
 * @author Jens Wawerla
 */
class CWaitProbPolicyRobotCtrl : public ABaseRobotCtrl
{
  public:
    /**
     * Default constructor
     * @param robot to control
     * @param probBroadcast probability to broadcast a new worker request
     */
    CWaitProbPolicyRobotCtrl( ARobot* robot, float probBroadcast);
    /** Default destructor */
    ~CWaitProbPolicyRobotCtrl();
    /** Start up policy */
    void startPolicy();
    /**
     * Policy for leaving a charger, this must set mState to what ever we
     * should do next
     * @param dt length of time step [s]
     */
    void leaveChargerPolicy( float dt );
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
     * Policy for waiting at the source
     */
    void waitingAtSourcePolicy( float dt );
    /**
     * Policy for pickup completed, this policy allows the robot controller to
     * decide what to do after the load was picked up
     */
    void pickupCompletedPolicy( float dt );
    /**
     * Policy for waiting at the pickup patch, note not in the queue
     * @param dt length of time step [s]
     */
    void waitingAtPickupPolicy ( float dt );

  protected:
    /**
     * Selects a new work task by setting mCurrentTask or goes to the depot
     */
    void selectNewTask();
    /** Threshold for waiting time [s] */
    float mWaitDurationThreshold;
    /** Threshold for pickup time [s] */
    float mPickupDurationThreshold;
    /** Index of the current task */
    unsigned int mTaskIndex;
    /** Flags if we are sent to the depot or not */
    bool mFgSendToDepot;
    /** Probability to broadcast a worker recuiting request */
    float mProbBroadcast;
    /** Broadcast system */
    CBroadCast* mBroadCast;
    /** Time we are willing to spend at the depot before returning to work */
    float mDepotWaitingTimeTheshold;
};

#endif
