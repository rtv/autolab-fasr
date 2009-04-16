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
 * $Log: fasrsource.h,v $
 * Revision 1.4  2009-03-31 01:41:59  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.3  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.2  2009-03-20 03:05:22  jwawerla
 * dynamic obstacles added to wavefront
 *
 * Revision 1.1.1.1  2009-03-15 03:52:03  jwawerla
 * First commit
 *
 *
 **************************************************************************/
#ifndef FASRSOURCE_H
#define FASRSOURCE_H


#include <stage.hh>
#include <list>
#include "error.h"
#include "utilities.h"

/**
 * A puck source for the FASR project
 * @author
 */
class CFasrSource
{
  public:
    /**
     * Default constructor
     * @param mod stage model
     */
    CFasrSource ( Stg::Model* mod );
    /** Default destructor */
    ~CFasrSource();
    /**
     * Callback for Stage to update the model
     * @param mod
     * @param source
     */
    static int stgUpdate ( Stg::Model* mod, CFasrSource* source );

  protected:
    /**
     * Loads the script
     * @return 1 of successfull, 0 otherwise
     */
    int loadScript();
    /** Data structur for task information */
    typedef struct {
      float timestamp;
      int capacity;
      float productionRate;
      float reward;
    } tTaskData;
    /**
     * Add task data in the correct temporal order
     * @param taskData to be added
     */
    void addTaskData ( tTaskData taskData );
    /**
     * Switches to the next task configuration
     */
    void nextTaskConfiguration();

  private:
    /** File name and path of script */
    char* mScriptName;
    /** Model name */
    char mName[20];
    /** Stage model */
    Stg::Model* mStgModel;
    /** Storage capacity for source  [flags] */
    int mStorageCapacity;
    /** Production rate [1/s] */
    float mProductionRate;
    /** Update interval in simulation steps */
    int mUpdateInterval;
    /** Reward per completed task */
    float mReward;
    /** Time stamp of last production */
    float mLastProducedUnitTimestamp;
    /** Simulation time [s] */
    float mSimTime;
    /** Time of next task modification [s] */
    float mNextModicificationTime;
    /** Timeline of changes of this source */
    std::list<tTaskData> mTaskTimeline;
};

#endif
