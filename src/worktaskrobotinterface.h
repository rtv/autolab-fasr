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
 * $Log: worktaskrobotinterface.h,v $
 * Revision 1.3  2009-04-08 22:40:41  jwawerla
 * Hopefully ND interface issue solved
 *
 * Revision 1.2  2009-04-03 15:10:03  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1  2009-03-31 03:00:20  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 **************************************************************************/

#ifndef WORKTASK_ROBOT_INTERFACE_H
#define WORKTASK_ROBOT_INTERFACE_H

#include "destination.h"
#include "stage.hh"

class IWorkTaskRobotInterface
{
  public:
    /** Default destructor */
    virtual ~IWorkTaskRobotInterface() {};

    /** Locks the mutex */
    virtual void lock() = 0;
    /** unlocks the mutex */
    virtual void unlock() = 0;
    /**
     * Set the reward rate
     * @param rate [1/s]
     */
    virtual void setExperiencedRewardRate ( double rate ) = 0;
    /**
     * Gets the filtered reward rate
     * @return [1/s]
     */
    virtual double getExperiencedRewardRate()  = 0;
    /**
     * Set the task duration
     * @param duration [s]
     */
    virtual void setExperiencedTaskDuration ( double duration ) = 0;
    /**
     * Gets the filtered task duration
     * @return [s]
     */
    virtual double getExperiencedTaskDuration() = 0;
    /**
     * Set the travel duration
     * @param duration [s]
     */
    virtual void setExperiencedTravelDuration ( double duration ) = 0;
    /**
     * Pre set the travel duration without going through the filter
     * @param duration [s]
     */
    virtual void preSetExperiencedTravelDuration ( double duration ) = 0;
    /**
     * Gets the filtered travel duration
     * @return [s]
     */
    virtual double getExperiencedTravelDuration()  = 0;
    /**
     * Subcribe robot to a task, this is used for statistical purposes
     * @param robot subscribing
     */
    virtual void subscribe(IRobotWaitInterface* robot ) = 0;
    /**
     * Unsubcribe robot from a task, this is used for statistical purposes
     * @param robot unsubscribing
     */
    virtual void unsubscribe(IRobotWaitInterface* robot ) = 0;
    /**
     * Gets the number of subcribed robots
     * @return number of robots
     */
    virtual unsigned int getNumSubcribers()  = 0;
    /**
     * Gets the name of the task
     * @return name
     */
    virtual char* getName()  = 0 ;
    /**
     * Gets the reward given for a successfull task completion
     * @return reward
     */
    virtual float getReward() = 0;
    /**
     * Get the ideal distance between source and sink, based on planner
     * @return  [m]
     */
    virtual float getSrcSinkDistance()  = 0;
    /**
     * Get the rate of production
     * @return [1/s]
     */
    virtual float getProductionRate() = 0;
    /**
     * Gets the id of this task
     */
    virtual unsigned int getTaskId() = 0;
    /** Source for transported goods */
    CDestination* mSource;
    /** Stage source model */
    Stg::Model* mStgSource;
    /** Sink for transported goods */
    CDestination* mSink;
    /** Stage sink model */
    Stg::Model* mStgSink;

  protected:
    /** Default constructor */
    IWorkTaskRobotInterface() {};
};

#endif
