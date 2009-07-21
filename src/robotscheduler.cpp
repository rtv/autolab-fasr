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
#include "robotscheduler.h"
#include "stdio.h"
#include "global.h"

/** Time constant for trip round trip time low pass filter */
const float TAU_TRIP_TIME = 0.02;

//-----------------------------------------------------------------------------
CRobotScheduler::CRobotScheduler()
{
  mNumReassignments = 0;
}
//-----------------------------------------------------------------------------
CRobotScheduler::~CRobotScheduler()
{
}
//-----------------------------------------------------------------------------
CRobotScheduler* CRobotScheduler::getInstance()
{
  static CRobotScheduler* instance = NULL;

  if ( instance == NULL )
    instance = new CRobotScheduler();

  return instance;
}
//-----------------------------------------------------------------------------
void CRobotScheduler::setExperiencedTaskCompletionTime(
  ITransportTaskInterface* task, float time )
{
  for ( unsigned int i = 0; i < mTaskList.size(); i++ ) {
    if ( mTaskList[i].task == task ) {
      mTaskList[i].roundTripTime += TAU_TRIP_TIME *
                                    ( time - mTaskList[i].roundTripTime );
      return;
    }
  }
  PRT_WARN0( "Task not found" );
}
//-----------------------------------------------------------------------------
ITransportTaskInterface* CRobotScheduler::getTask(
  IRobotSchedulerInterface* robot )
{
  std::list<IRobotSchedulerInterface*>::iterator roIt;
  CTransportationTask* assignedTask;

  // currently assigned task
  assignedTask = static_cast<CTransportationTask*>( robot->getTask() );

  // iterate over all tasks
  for ( unsigned int i = 0; i < mTaskList.size(); i++ ) {
    //****************************************************
    // find optimal number of worker robots for each task
    mTaskList[i].optNumWorkers = ( unsigned short ) ceil(
                                   mTaskList[i].roundTripTime *
                                   mTaskList[i].task->getProductionRate() /
                                   ( float ) robot->getCargoBayCapacity() );
/*
    printf( "robot %d -> task %6s: rtt %1.1f assigned %2d opt %2d free %2d\n",
            robot->getRobotId(), mTaskList[i].task->getName().c_str(),
            mTaskList[i].roundTripTime,
            mTaskList[i].assignedRobotList.size(),
            mTaskList[i].optNumWorkers,
            mFreeRobotList.size() );
*/
    //****************************************************
    // check if this task has too many robots
    if ( mTaskList[i].optNumWorkers < mTaskList[i].assignedRobotList.size() ) {
      printf( "remove robot from task\n" );
      // is the requesting robot serving this task ?
      // if so remove it from the task
      if ( robot->getTask() == mTaskList[i].task ) {
        // remove robot from assignment list
        removeRobotFromTask( mTaskList[i].task, robot );
        // add robot to free list
        mFreeRobotList.push_back( robot );
        assignedTask = NULL; // no task
        break;
      }
    }
    //****************************************************
    // this task has the correct number of workers
    if ( mTaskList[i].optNumWorkers == mTaskList[i].assignedRobotList.size() ) {
      // we don't have to change anything, if the robot is currently executing
      // the in question task
      if ( assignedTask == mTaskList[i].task ) {
        printf( "no change !\n" );
        return assignedTask;
      }
    }
    //****************************************************
    // this tasks need more workers
    if ( mTaskList[i].optNumWorkers > mTaskList[i].assignedRobotList.size() ) {

      // if the free list is empty we have to assign the requesting robot to a
      // better task. Since the tasks are ordered by quality better tasks are
      // first in the list
      if ( mFreeRobotList.size() == 0 ) {
        if ( assignedTask != mTaskList[i].task ) {
          printf( "add robot to task\n" );
          // remove robot from current task
          removeRobotFromTask( assignedTask, robot );
          assignedTask = mTaskList[i].task;
          mTaskList[i].assignedRobotList.push_back( robot );
          mNumReassignments ++;
        }
        return assignedTask;
      }

      // if the requesting robot is in the free list, assign it to the task
      for ( roIt = mFreeRobotList.begin(); roIt != mFreeRobotList.end(); roIt++ ) {
        if ( *roIt == robot ) {
          roIt = mFreeRobotList.erase( roIt );
          mTaskList[i].assignedRobotList.push_back( robot );
          assignedTask = mTaskList[i].task;
          mNumReassignments ++;
          printf( "get robot from free list\n" );
          return assignedTask;
        }
      }
    }

  } // for tasks

  return assignedTask;
}
//-----------------------------------------------------------------------------
void CRobotScheduler::removeRobotFromTask( CTransportationTask* task,
    IRobotSchedulerInterface* robot )
{
  if ( task ) {
    for ( unsigned int i = 0;  i < mTaskList.size(); i++ ) {
      if ( task == mTaskList[i].task ) {
        removeRobotFromTask( &mTaskList[i], robot );
        return;
      }
    }
  }
}
//-----------------------------------------------------------------------------
void CRobotScheduler::removeRobotFromTask( tTaskData* task,
    IRobotSchedulerInterface* robot )
{
  std::list<IRobotSchedulerInterface*>::iterator roIt;

  for ( roIt  = task->assignedRobotList.begin();
        roIt != task->assignedRobotList.end(); roIt++ ) {
    if ( *roIt == robot ) {
      roIt = task->assignedRobotList.erase( roIt );
      return;
    }
  } // for
}
//-----------------------------------------------------------------------------
void CRobotScheduler::addTransportationTask( ITransportTaskInterface* task )
{
  tTaskData taskData;
  for ( unsigned int i = 0; i < mTaskList.size(); i++ ) {
    if ( task == mTaskList[i].task )
      return;  // task already in list
  }
  taskData.task = static_cast<CTransportationTask*>( task );
  taskData.roundTripTime = 2 * taskData.task->getSourceSinkDistance() /
                           PLANNING_SPEED;
  taskData.assignedRobotList.clear();
  taskData.optNumWorkers = 0;

  mTaskList.push_back( taskData );
}
//-----------------------------------------------------------------------------
void CRobotScheduler::registerRobot( IRobotSchedulerInterface* robot )
{
  mRobotList.push_back( robot );
  mFreeRobotList.push_back( robot );
}
//-----------------------------------------------------------------------------
bool CRobotScheduler::keepWaiting( ITransportTaskInterface* task,
                                   IRobotSchedulerInterface* robot )
{
  if ( static_cast<CTransportationTask*>( task )->getProductionRate() > 0 )
    return true;  // yes keep waiting

  return false; // no
}
//-----------------------------------------------------------------------------
void CRobotScheduler::robotIsCharging ( IRobotSchedulerInterface* robot )
{
  std::list<IRobotSchedulerInterface*>::iterator it;

  // remove robot from assignment list
  for ( unsigned int i = 0; i < mTaskList.size(); i++ ) {
    if ( mTaskList[i].task == robot->getTask() ) {
      for ( it  = mTaskList[i].assignedRobotList.begin();
            it != mTaskList[i].assignedRobotList.end(); it++ ) {
        if ( robot == *it ) {
          it = mTaskList[i].assignedRobotList.erase ( it );
          mFreeRobotList.push_back ( robot );
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------