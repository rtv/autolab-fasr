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
#ifndef TRANSPORTATIONTASK_H
#define TRANSPORTATIONTASK_H

#include <RapiCore>
#include <string>
#include "transporttaskinterface.h"
#include "stage.hh"
#include "destination.h"

using namespace Rapi;

/**
 * Defines a transportation task
 * @author Jens Wawerla
 */
class CTransportationTask : public ITransportTaskInterface
{
  public:
    /**
     * Default constructor
     * @param stgSource stage source model
     * @param stgSink stage sink model
     * @param name of task
     */
    CTransportationTask ( Stg::Model* stgSource,
                          Stg::Model* stgSink,
                          std::string name );
    /** Default destructor */
    ~CTransportationTask();
    /**
     * Sets the production rate of the source
     * @param rate [flags/s]
     */
    void setProductionRate( float rate );
    /**
     * Gets the production rate of the source
     * @return rate [flags/s]
     */
    float getProductionRate( ) const { return mProductionRate; };
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
     * Gets the name of a task
     * @return task name
     */
    std::string getName() { return mName; };
    /**
     * Gets the source destination
     * @return source destination
     */
    virtual CDestination* getSource() { return mSourceDestination; };
    /**
     * Gets the sink destination
     * @return sink destination
     */
    virtual CDestination* getSink() { return mSinkDestination; };
    /**
     * Gets the reward for the transported goods
     * @return [$]
     */
    virtual float getReward() const { return mReward; };
    /**
     * Sets the distance between source and sink
     * @param dist [m]
     */
    void setSourceSinkDistance(float dist);
    /**
     * Gets the distance between source and sink based on the map
     * @return [m]
     */
    float getSourceSinkDistance() const { return mSourceSinkDistance; };
    /**
     * Increments the task complete counter, this is used for statistical
     * purposes only
     */
    void incrementTaskCompletedCount();
    /**
     * Gets the number of completed tasks
     * @return number of completed tasks
     */
    unsigned int getTaskCompletedCount() const { return mTaskCompleteCounter; };

  protected:
    /** Name of task */
    std::string mName;
    /** Rate of production [1/s] */
    float mProductionRate;
    /** Reward given for a successfull task completion */
    float mReward;
    /** Storage capacity of source [flags] */
    int mStorageCapacity;
    /** Source destination */
    CDestination* mSourceDestination;
    /** Sink destination */
    CDestination* mSinkDestination;
    /** Distance between source and sink based on the map [m] */
    float mSourceSinkDistance;
    /** Number of completed tasks */
    unsigned int mTaskCompleteCounter;
};

#endif
