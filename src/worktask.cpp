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
 * $Log: worktask.cpp,v $
 * Revision 1.14  2009-04-08 22:40:41  jwawerla
 * Hopefully ND interface issue solved
 *
 * Revision 1.13  2009-04-03 16:57:43  jwawerla
 * Some bug fixing
 *
 * Revision 1.12  2009-04-03 15:10:03  jwawerla
 * *** empty log message ***
 *
 * Revision 1.11  2009-03-31 23:52:59  jwawerla
 * Moved cell index from float to int math
 *
 * Revision 1.10  2009-03-31 04:27:33  jwawerla
 * Some bug fixing
 *
 * Revision 1.9  2009-03-31 01:42:01  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.8  2009-03-28 21:53:57  jwawerla
 * *** empty log message ***
 *
 * Revision 1.7  2009-03-28 00:54:39  jwawerla
 * *** empty log message ***
 *
 * Revision 1.6  2009-03-27 19:05:27  vaughan
 * running on OS X and using new controller parameter method
 *
 * Revision 1.5  2009-03-27 00:27:37  jwawerla
 * *** empty log message ***
 *
 * Revision 1.4  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.3  2009-03-24 01:04:47  jwawerla
 * switching cost robot added
 *
 * Revision 1.2  2009-03-20 03:05:23  jwawerla
 * dynamic obstacles added to wavefront
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/

#include "worktask.h"

/** Filter constant for reward rate */
const double REWARD_RATE_FILT_CONST = 0.5;
/** Filter constant for task duration */
const double TASK_DURATION_FILT_CONST = 0.5;
/** Filter constant for travel duration */
const double TRAVEL_DURATION_FILT_CONST = 0.3;
/** Counts the tasks and assigned each a unique id */
static unsigned int taskId = 0;

//-----------------------------------------------------------------------------
CWorkTask::CWorkTask ( Stg::Model* stgSource,
                       Stg::Model* stgSink,
                       const char* name )
    : IWorkTaskRobotInterface()
{
  mTaskId = taskId;
  taskId = taskId + 1;

  assert( stgSource );
  assert( stgSink );

  mStgSource = stgSource;
  mStgSink = stgSink;

  if ( name == NULL )
    snprintf ( mName, 20, "noname" );
  else
    strncpy ( mName, name, 20 );

  mAccessMutex = NULL;
  mRobotServicingList.clear();

  mSource = new CDestination ( mStgSource );
  mSink = new CDestination ( mStgSink );

  mSrcSinkDistance = 1.0;
  mRewardRateFilt = 0.0;
  mTaskDurationFilt = 0.0;
  mTravelDurationFilt = 0.0;
}
//-----------------------------------------------------------------------------
CWorkTask::~CWorkTask()
{
}
//-----------------------------------------------------------------------------
void CWorkTask::lock()
{
  if ( mAccessMutex == NULL )
    mAccessMutex = g_mutex_new();

  assert ( mAccessMutex );
  g_mutex_lock ( mAccessMutex );
}
//-----------------------------------------------------------------------------
void CWorkTask::unlock()
{
  assert ( mAccessMutex );
  g_mutex_unlock ( mAccessMutex );
}
//-----------------------------------------------------------------------------
void CWorkTask::setProductionRate ( float rate )
{
  mProductionRate = rate;
}
//-----------------------------------------------------------------------------
void CWorkTask::setReward ( float reward )
{
  mReward = reward;
}
void CWorkTask::setStorageCapacity ( int capacity )
{
  mStorageCapacity = capacity;
}
//-----------------------------------------------------------------------------
void CWorkTask::setExperiencedRewardRate ( double rate )
{
  mRewardRateFilt = mRewardRateFilt +
                    REWARD_RATE_FILT_CONST * ( rate - mRewardRateFilt );
}
//-----------------------------------------------------------------------------
void CWorkTask::setExperiencedTaskDuration ( double duration )
{
  mTaskDurationFilt = mTaskDurationFilt +
                      TASK_DURATION_FILT_CONST * ( duration - mTaskDurationFilt );
}
//-----------------------------------------------------------------------------
void CWorkTask::preSetExperiencedTravelDuration ( double duration )
{
  mTravelDurationFilt = duration;
}
//-----------------------------------------------------------------------------
void CWorkTask::setExperiencedTravelDuration ( double duration )
{
  mTravelDurationFilt = mTravelDurationFilt +
                        TRAVEL_DURATION_FILT_CONST * ( duration - mTravelDurationFilt );
}
//-----------------------------------------------------------------------------
void  CWorkTask::subscribe ( IRobotWaitInterface* robot )
{
  // make sure we don't have any double subscribers
  unsubscribe ( robot );
  // add robot to the list
  mRobotServicingList.push_back ( robot );
}
//-----------------------------------------------------------------------------
void  CWorkTask::unsubscribe ( IRobotWaitInterface* robot )
{
  std::list<IRobotWaitInterface*>::iterator it;

  for ( it = mRobotServicingList.begin(); it != mRobotServicingList.end(); it++ ) {
    if ( *it == robot )
      it = mRobotServicingList.erase ( it );
  }
}
//-----------------------------------------------------------------------------
unsigned int CWorkTask::getNumSubcribers()
{
  return mRobotServicingList.size();
}
//-----------------------------------------------------------------------------




