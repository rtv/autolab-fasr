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
#ifndef ANALYST_H
#define ANALYST_H

#include "baserobotctrl.h"
#include "transportationtask.h"
#include "stage.hh"
#include <vector>

/** Type definition for task data structure */
typedef struct {
  CTransportationTask* task;
  unsigned int numRobots;
  unsigned int optNumWorkers;
  float roundTripTime;
} tAnalystTaskData;

/**
 * This class keeps simple statistics for a fasr experiment and log it to a
 * file. Update is handled by attaching this class to a stage model.
 * @author Jens Wawerla
 */
class CAnalyst
{
  friend class CAnalystDialog;

  public:
    /** Default destructor */
    ~CAnalyst();
    /**
     * Singleton pattern, gets the only instance of this class
     * @param mod stage model for update callback, note this can only be set by the very first call
     * @param filename of log file, note this can only be set by the very first call
     * @return instance of this class
     */
    static CAnalyst* getInstance( Stg::Model* mod = NULL, std::string filename = "fasr_analyst.log" );
    /** Update method for stage */
    static int stgUpdate( Stg::Model* mod, CAnalyst* analyst );
    /**
     * Register a robot with the analyst
     * @param robot to register
     */
    void registerRobot( ABaseRobotCtrl* robot );
    /**
     * Register a transportation task with the analyst
     * @param task to register
     */
    void registerTask( CTransportationTask* task );

  protected:
    /**
     * Update the analysis
     */
    void update();

  private:
    /**
     * Default constructor
     * @param mod stage model for update callback
     * @param filename of log file
     */
    CAnalyst( Stg::Model* mod, std::string filename );
    /** List of robots */
    std::vector<ABaseRobotCtrl*> mRobotList;
    /** List of tasks */
    std::vector<tAnalystTaskData*> mTaskList;
    /** Stage model for this controller */
    Stg::Model* mStgModel;
    /** Robot cargo capacity [Flags] */
    unsigned int mRobotCargoCapacity;
    /** Number of robots in depot */
    unsigned int mNumRobotsInDepot;
    /** Number of robot charging */
    unsigned int mNumRobotCharging;
    /** Total reward obtained by all robots */
    float mTotalReward;
    /** Overall number of flags delivered */
    unsigned int mTotalNumFlags;
    /** Total number of task switches */
    unsigned int mTotalNumTaskSwitches;
    /** Overall energy expended */
    float mTotalEnergy;
    /** File pointer for log file */
    FILE* mFpLog;
};

#endif
