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
 * $Log: worktask.h,v $
 * Revision 1.9  2009-04-08 22:40:41  jwawerla
 * Hopefully ND interface issue solved
 *
 * Revision 1.8  2009-04-03 16:57:43  jwawerla
 * Some bug fixing
 *
 * Revision 1.7  2009-04-03 15:10:03  jwawerla
 * *** empty log message ***
 *
 * Revision 1.6  2009-03-31 01:42:01  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.5  2009-03-28 00:54:39  jwawerla
 * *** empty log message ***
 *
 * Revision 1.4  2009-03-27 00:27:37  jwawerla
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-26 04:09:46  jwawerla
 * Minor bug fixing
 *
 * Revision 1.2  2009-03-24 01:04:47  jwawerla
 * switching cost robot added
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 **************************************************************************/
#ifndef WORKTASK_H
#define WORKTASK_H

#include "destination.h"
#include "stage.hh"
#include "worktaskrobotinterface.h"
#include <assert.h>
#include <list>

/**
 * This class describes a work task by specifying a source and a sink
 * @author
 */
class CWorkTask : public IWorkTaskRobotInterface
{
  public:
    /**
     * Default constructor
     * @param stgSource stage source model
     * @param stgSink stage sink model
     * @param name of task
     */
    CWorkTask ( Stg::Model* stgSource,
                Stg::Model* stgSink,
                const char* name = NULL );
    /** Default destructor */
    ~CWorkTask();
    /** Locks the mutex */
    void lock();
    /** unlocks the mutex */
    void unlock();
    /**
     * Set the reward rate
     * @param rate [1/s]
     */
    void setExperiencedRewardRate ( double rate );
    /**
     * Gets the filtered reward rate
     * @return [1/s]
     */
    double getExperiencedRewardRate() { return mRewardRateFilt; };
    /**
     * Set the task duration
     * @param duration [s]
     */
    void setExperiencedTaskDuration ( double duration );
    /**
     * Gets the filtered task duration
     * @return [s]
     */
    double getExperiencedTaskDuration() { return mTaskDurationFilt; };
    /**
     * Set the travel duration
     * @param duration [s]
     */
    void setExperiencedTravelDuration ( double duration );
    /**
     * Pre set the travel duration without going through the filter
     * @param duration [s]
     */
    void preSetExperiencedTravelDuration ( double duration );
    /**
     * Gets the filtered travel duration
     * @return [s]
     */
    double getExperiencedTravelDuration() { return mTravelDurationFilt; };
    /**
     * Subcribe robot to a task, this is used for statistical purposes
     * @param robot subscribing
     */
    void subscribe(IRobotWaitInterface* robot );
    /**
     * Unsubcribe robot from a task, this is used for statistical purposes
     * @param robot unsubscribing
     */
    void unsubscribe(IRobotWaitInterface* robot );
    /**
     * Gets the number of subcribed robots
     * @return number of robots
     */
    unsigned int getNumSubcribers();
    /**
     * Gets the name of the task
     * @return name
     */
    char* getName() { return mName; } ;
    /**
     * Gets the reward given for a successfull task completion
     * @return reward
     */
    float getReward() { return mReward; };
    /**
     * Get the ideal distance between source and sink, based on planner
     * @return  [m]
     */
    float getSrcSinkDistance() { return mSrcSinkDistance; };
    /**
     * Set the ideal distance between source and sink, e.g. based on planner
     * @param dist  [m]
     */
    void setSrcSinkDistance( float dist ) { mSrcSinkDistance = dist; };
    /**
     * Get the rate of production
     * @return [1/s]
     */
    float getProductionRate() { return mProductionRate; };
    /** Source for transported goods */
    //CDestination* mSource;
    /** Stage source model */
    //Stg::Model* mStgSource;
    /** Sink for transported goods */
    //CDestination* mSink;
    /** Stage sink model */
    //Stg::Model* mStgSink;

    /**
     * Sets the production rate of the source
     * @param rate
     */
    void setProductionRate( float rate );
    /**
     * Sets the reward per unit transported
     * @param reward
     */
    void setReward( float reward );
    /**
     * Sets the storage capacity of the source
     * @param capacity
     */
    void setStorageCapacity( int capacity );
    /**
     * Gets the id of this task
     */
    unsigned int getTaskId() { return mTaskId; };

  private:
    /** Name of task */
    char mName[20];
    /** List of subcribed robots */
    std::list<IRobotWaitInterface*> mRobotServicingList;
    /** Access mutex */
    GMutex* mAccessMutex;
    /** Filtered reward rate [1/s] */
    float mRewardRateFilt;
    /** Filtered task round trip time including waiting [s] */
    float mTaskDurationFilt;
    /** Filtered duration of travel, no waiting time  [s] */
    float mTravelDurationFilt;
    /** Ideal distance between source and sink, based on planner [m] */
    float mSrcSinkDistance;
    /** Rate of production [1/s] */
    float mProductionRate;
    /** Reward given for a successfull task completion */
    float mReward;
    /** Storage capacity of source [flags] */
    int mStorageCapacity;
    /** Id of this task */
    unsigned int mTaskId;
};

#endif
