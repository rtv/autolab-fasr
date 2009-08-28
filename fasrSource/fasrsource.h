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
#ifndef FASRSOURCE_H
#define FASRSOURCE_H


#include <stage.hh>
#include <list>
#include "printerror.h"
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
    /** Updates the source */
    void update();

  private:
    /** File name and path of script */
    std::string mScriptName;
    /** Model name */
    char mName[20];
    /** Stage model */
    Stg::Model* mStgModel;
    /** Storage capacity for source  [flags] */
    int mStorageCapacity;
    /** Production rate [1/s] */
    float mProductionRate;
    /** Update interval [s] */
    float mUpdateInterval;
    /** Reward per completed task */
    float mReward;
    /** Time stamp of last start of production */
    float mProductionStartedTimestamp;
    /** Flags if production was started */
    bool mFgProductionStarted;
    /** Simulation time [s] */
    float mSimTime;
    /** Time of next task modification [s] */
    float mNextModicificationTime;
    /** Timeline of changes of this source */
    std::list<tTaskData> mTaskTimeline;
};

#endif
