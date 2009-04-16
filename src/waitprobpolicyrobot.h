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
 * $Log: waitprobpolicyrobot.h,v $
 * Revision 1.6  2009-04-03 16:21:30  jwawerla
 * Move wavefront map specification to world file
 *
 * Revision 1.5  2009-03-31 01:42:01  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.4  2009-03-29 00:54:27  jwawerla
 * Replan ctrl seems to work now
 *
 * Revision 1.3  2009-03-27 01:32:58  jwawerla
 * wait prob controller improved
 *
 * Revision 1.2  2009-03-27 00:27:37  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.1  2009-03-24 01:04:46  jwawerla
 * switching cost robot added
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 **************************************************************************/
#ifndef WAITPROBPOLICYROBOT_H
#define WAITPROBPOLICYROBOT_H

#include <baserobot.h>

/**
 * Implements a robot with a policy based on probability task switching. Where
 * the probability to switch is proportional to the ratio of waiting time to
 * overall trip time
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CWaitProbPolicyRobot : public ABaseRobot
{
  public:
    /**
     * Default constructor
     * @param modPos stage position model
     */
    CWaitProbPolicyRobot ( Stg::ModelPosition* modPos );
    /** Default destructor */
    ~CWaitProbPolicyRobot();
    /** Chooses the next work task according to some policy */
    virtual void selectWorkTaskPolicy();
    /** Policy for start up */
    virtual void startupPolicy();
    /** Policy for leaving a charger */
    virtual void leaveChargerPolicy();
    /** Policy for waiting at source */
    virtual void waitAtSourcePolicy();
    /** Policy for waiting at charger */
    virtual void waitAtChargerPolicy();
    /** Policy for the first unallocated robot */
    virtual void unallocatedPolicy();
    /** Policy for a successfull pick up */
    virtual void pickupCompletedPolicy();
    /** Policy for waiting at the pickup patch, note not in the queue */
    virtual void waitingAtPickupPolicy();
    /**
     * Gets the type of the controller
     * @return controller type
     */
    virtual tControllerType getControllerType() { return WAITPROB_CTRL; };
    /**
     * Get the data from a robot to send to robotVis
     * @param id of robot
     * @param buffer to fill with data
     * @param len of buffer
     */
    virtual void getRobotVisData( int id, char* buffer, int &len );

  protected:
    /**
     * Selects a new work task by setting mCurrentWorkTask or goes to the depot
     */
    void selectNewTask();

  private:
    /** Time spend waiting [s] */
    float mWaitDuration;
    /** Time we are willing to wait [s] */
    float mWaitDurationThreshold;
    /** Time stamp when we started the last task [s] */
    float mTaskStartedTimestamp;
    /** Probability of switching */
    float mSwitchProbabilty;
    /** Index of current task */
    unsigned int mTaskIndex;
    /**
     * Flags if robot is sent to depot or not, we use this to figure out what
     * to do after recharging.
     */
    bool mFgSendToDepot;

};

#endif
