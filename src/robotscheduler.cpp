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
 * $Log: robotscheduler.cpp,v $
 * Revision 1.7  2009-04-03 15:10:02  jwawerla
 * *** empty log message ***
 *
 * Revision 1.6  2009-03-31 04:27:33  jwawerla
 * Some bug fixing
 *
 * Revision 1.5  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.4  2009-03-29 00:54:27  jwawerla
 * Replan ctrl seems to work now
 *
 * Revision 1.3  2009-03-28 22:08:15  vaughan
 * added selecting policy from worldfile
 *
 * Revision 1.2  2009-03-28 21:53:57  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1  2009-03-28 00:54:39  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "robotscheduler.h"

/** Speed of robot, to plan round trip time [m/s] */
const float PLANNING_SPEED = 0.40;

//-----------------------------------------------------------------------------
CRobotScheduler::CRobotScheduler()
{
}
//-----------------------------------------------------------------------------
CRobotScheduler::~CRobotScheduler()
{
}
//-----------------------------------------------------------------------------
void CRobotScheduler::registerRobot ( IReplanPolicyRobotInterface* robot )
{
  mRobotVector.push_back ( robot );
  mFreeRobotList.push_back ( robot );
}
//-----------------------------------------------------------------------------
void CRobotScheduler::addWorkTask ( IWorkTaskRobotInterface* task )
{
  tTaskData taskData;

  for ( unsigned int i = 0; i < mTaskVector.size(); i++ ) {
    if ( mTaskVector[i].task == task )
      return;
  }

  task->preSetExperiencedTravelDuration ( task->getSrcSinkDistance() /
                                          PLANNING_SPEED );
  taskData.task = task;
  mTaskVector.push_back ( taskData );
}
//-----------------------------------------------------------------------------
void CRobotScheduler::robotIsCharging ( IReplanPolicyRobotInterface* robot )
{
  std::list<IReplanPolicyRobotInterface*>::iterator it;

  // remove robot from assignment list
  for ( unsigned int i = 0; i < mTaskVector.size(); i++ ) {
    if ( mTaskVector[i].task == robot->getTask() ) {
      for ( it  = mTaskVector[i].robotsAssignedList.begin();
            it != mTaskVector[i].robotsAssignedList.end(); it++ ) {
        if ( robot == *it ) {
          it = mTaskVector[i].robotsAssignedList.erase ( it );
          mFreeRobotList.push_back ( robot );
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------
IWorkTaskRobotInterface* CRobotScheduler::getWorkTask ( IReplanPolicyRobotInterface* robot )
{
  std::list<IReplanPolicyRobotInterface*>::iterator it;
  IWorkTaskRobotInterface* taskAssignment = robot->getTask();

  // find optimal number of worker robots for each task
  for ( unsigned int i = 0; i < mTaskVector.size(); i++ ) {

    mTaskVector[i].optNumOfWorkers = ( unsigned short ) ceil ( 2.0 *
                                     mTaskVector[i].task->getExperiencedTravelDuration() *
                                     mTaskVector[i].task->getProductionRate() /
                                     ( float ) robot->getCargoBayCapacity() );

    // check if this task has too many robots
    if ( mTaskVector[i].optNumOfWorkers < mTaskVector[i].robotsAssignedList.size() ) {
      // is the requesting robot serving this task ? if so remove it from the task
      if ( robot->getTask() == mTaskVector[i].task ) {
        // remove robot from assignment list
        for ( it  = mTaskVector[i].robotsAssignedList.begin();
              it != mTaskVector[i].robotsAssignedList.end(); it++ ) {
          if ( *it == robot ) {
            it = mTaskVector[i].robotsAssignedList.erase ( it );
            mFreeRobotList.push_back ( robot );
            taskAssignment = NULL; // no task
            break;
          }
        } // for
      }
    }
    // does this task have not enough works ?
    if ( mTaskVector[i].optNumOfWorkers > mTaskVector[i].robotsAssignedList.size() ) {

      // if the free list is empty we have to assign the requesting robot to a better task
      if ( mFreeRobotList.size() == 0 ) {
        taskAssignment = mTaskVector[i].task;
        mTaskVector[i].robotsAssignedList.push_back ( robot );
        return taskAssignment;
      }

      // if the requesting robot is in the free list, assign it to the task
      for ( it = mFreeRobotList.begin(); it != mFreeRobotList.end(); it++ ) {
        if ( *it == robot ) {
          it = mFreeRobotList.erase ( it );
          mTaskVector[i].robotsAssignedList.push_back ( robot );
          taskAssignment = mTaskVector[i].task;
          return taskAssignment;
        }
      }
    }

  } // for tasks

  return taskAssignment;
}
//-----------------------------------------------------------------------------
