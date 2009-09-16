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
#include "analyst.h"
#include "global.h"
#include <assert.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
CAnalyst::CAnalyst( Stg::Model* mod, std::string filename )
{
  if ( mod == NULL ) {
    PRT_ERR0( "No Stage model given " );
    mStgModel = NULL;
  }
  else {
    mStgModel = mod;
    mStgModel->AddUpdateCallback(( Stg::stg_model_callback_t ) stgUpdate, this );
    mStgModel->Subscribe();
  }

  mTotalNumFlags = 0;
  mTotalEnergy = 0.0;
  mTotalReward = 0.0;
  mTotalNumTaskSwitches = 0;

  mFpLog = fopen( filename.c_str(), "w" );
  if ( mFpLog == NULL )
    PRT_WARN1( "CAnalyst: failed to open log file %s\n", filename.c_str() );
}
//-----------------------------------------------------------------------------
CAnalyst::~CAnalyst()
{
  fclose( mFpLog );
}
//-----------------------------------------------------------------------------
void CAnalyst::update()
{
  bool fgWorking;

  // clear data
  for ( unsigned int i = 0; i < mTaskList.size(); i++ ) {
    mTaskList[i]->numRobots = 0;
  }
  mNumRobotCharging = 0;
  mNumRobotsInDepot = 0;
  mTotalNumFlags = 0;
  mTotalEnergy = 0.0;
  mTotalReward = 0.0;
  mTotalNumTaskSwitches = 0;

  for ( unsigned int r = 0; r < mRobotList.size(); r++ ) {
    fgWorking = false;
    mTotalNumTaskSwitches += mRobotList[r]->mNumTaskSwitches;
    mTotalNumFlags += mRobotList[r]->mNumTrips;
    mTotalEnergy += mRobotList[r]->mPowerPack->getTotalEnergyDissipated();
    mTotalReward += mRobotList[r]->mAccumulatedReward;

    if (( mRobotList[r]->mState == GOTO_DEPOT ) ||
        ( mRobotList[r]->mState == UNALLOCATED ) ) {
      mNumRobotsInDepot ++;
    }
    else if (( mRobotList[r]->mState == CHARGE ) ||
             ( mRobotList[r]->mState == GOTO_CHARGER ) ||
             ( mRobotList[r]->mState == DOCK ) ||
             ( mRobotList[r]->mState == UNDOCK ) ||
             ( mRobotList[r]->mState == WAITING_AT_CHARGER ) ) {
      mNumRobotCharging ++;
    }
    else {
      fgWorking = true;
    }

    for ( unsigned int t = 0; t < mTaskList.size(); t++ ) {

      // calculate theoretical optimal number of robots
      mTaskList[t]->optNumWorkers = ( unsigned short ) ceil(
                                      mTaskList[t]->roundTripTime *
                                      mTaskList[t]->task->getProductionRate() /
                                      ( float ) mRobotCargoCapacity );

      if ( fgWorking ) {
        // find number of robots serving this task or
        if ( mRobotList[r]->mCurrentTask == mTaskList[t]->task ) {
          mTaskList[t]->numRobots ++;
        }
      }
    } // for tasks
  } // for robots

  fprintf( mFpLog, "%d, %d, %d, %0.1f, %d, %.14f",
           mNumRobotsInDepot, mNumRobotCharging, mTotalNumTaskSwitches,
           mTotalReward, mTotalNumFlags, mTotalEnergy );

  for ( unsigned int t = 0; t < mTaskList.size(); t++ ) {
    fprintf( mFpLog, ", %d, %d, %f, %f, %f, %d",
             mTaskList[t]->numRobots,
             mTaskList[t]->optNumWorkers,
             mTaskList[t]->roundTripTime,
             mTaskList[t]->task->getProductionRate(),
             mTaskList[t]->task->getReward(),
             mTaskList[t]->task->getTaskCompletedCount() );
  } // tasks
  fprintf( mFpLog, "\n" );
  fflush( mFpLog );
}
//-----------------------------------------------------------------------------
int CAnalyst::stgUpdate( Stg::Model* mod, CAnalyst* analyst )
{
  analyst->update();
  return 0; // ok
}
//-----------------------------------------------------------------------------
CAnalyst* CAnalyst::getInstance( Stg::Model* mod, std::string filename )
{
  static CAnalyst* instance = NULL;

  if ( instance == NULL )
    instance = new CAnalyst( mod, filename );

  return instance;
}
//-----------------------------------------------------------------------------
void CAnalyst::registerRobot( ABaseRobotCtrl* robot )
{
  assert( robot );

  mRobotList.push_back( robot );
  for ( unsigned int i = 0; i < robot->mTaskVector.size(); i++ )
    registerTask(( CTransportationTask* )( robot->mTaskVector[i] ) );

  mRobotCargoCapacity = robot->mCargoBayCapacity;
}
//-----------------------------------------------------------------------------
void CAnalyst::registerTask( CTransportationTask* task )
{
  tAnalystTaskData* taskData;

  // make sure we have only one instance of each task
  for ( unsigned int i = 0; i < mTaskList.size(); i++ ) {
    if ( mTaskList[i]->task == task )
      return;
  }

  taskData = new tAnalystTaskData;

  taskData->task = task;
  taskData->numRobots = 0;
  taskData->optNumWorkers = 0;
  taskData->roundTripTime = 2 * taskData->task->getSourceSinkDistance() /
                            PLANNING_SPEED;
  mTaskList.push_back( taskData );

  printf( "TASK: %s %f \n", taskData->task->getName().c_str(),
          taskData->task->getSourceSinkDistance() );
}
//-----------------------------------------------------------------------------

