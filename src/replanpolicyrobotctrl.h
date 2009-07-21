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
#ifndef REPLANPOLICYROBOTCTRL_H
#define REPLANPOLICYROBOTCTRL_H

#include "staticpolicyrobotctrl.h"

/**
 * Extends CStaticPolicyRobotCtrl. This policy requests a task from the
 * scheduler every time a task was completed.
 * @author Jens Wawerla
 */
class CReplanPolicyRobotCtrl : public CStaticPolicyRobotCtrl
{
  public:
    /**
     * Default constructor
     * @param robot to control
     */
    CReplanPolicyRobotCtrl(ARobot* robot);
    /** Default destructor */
    ~CReplanPolicyRobotCtrl();
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
    void waitingAtSourcePolicy( float dt);
    /**
     * Policy for leaving a charger, this must set mState to what ever we
     * should do next
     * @param dt length of time step [s]
     */
    void leaveChargerPolicy( float dt );
    /**
     * Make a decision about recharging, it only makes sense to go charging
     * if at the sink, therefore this method gets called after a puck delivery
     * @param dt length of time step [s]
     * @return return true if charging needed, false otherwise
     */
    bool chargingPolicy ( float dt );

  protected:
     /** Flags if we were in the depot before this task */
     bool mFgWasInDepot;
     /** Flags if we were charging */
     bool mFgWasCharging;
};

#endif
