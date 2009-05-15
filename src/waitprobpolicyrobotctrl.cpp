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
 * $Log: waitprobpolicyrobot.cpp,v $
 * Revision 1.9  2009-04-03 16:21:30  jwawerla
 * Move wavefront map specification to world file
 *
 * Revision 1.8  2009-04-03 15:10:02  jwawerla
 * *** empty log message ***
 *
 * Revision 1.7  2009-03-31 23:52:59  jwawerla
 * Moved cell index from float to int math
 *
 * Revision 1.6  2009-03-31 01:42:01  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.5  2009-03-29 00:54:27  jwawerla
 * Replan ctrl seems to work now
 *
 * Revision 1.4  2009-03-28 00:54:39  jwawerla
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-27 01:32:58  jwawerla
 * wait prob controller improved
 *
 * Revision 1.2  2009-03-27 00:27:37  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 *
 ***************************************************************************/
#include "waitprobpolicyrobotctrl.h"

/** Minimum time we wait at a pickup for a flag [s] */
const float MIN_PICKUP_WAIT_DURATION = 50.0;

//-----------------------------------------------------------------------------
CWaitProbPolicyRobotCtrl::CWaitProbPolicyRobotCtrl ( ARobot* robot )
    : ABaseRobotCtrl ( robot )
{
  mWaitDuration = 0;
  mTaskStartedTimestamp = 0;
  rprintf ( "Loading CWaitProbPolicyRobot\n" );
}
//-----------------------------------------------------------------------------
CWaitProbPolicyRobotCtrl::~CWaitProbPolicyRobotCtrl()
{
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::selectWorkTaskPolicy ( float dt )
{
  mWaitDuration = 0.0;
  mTaskStartedTimestamp = mSimTime;
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::selectNewTask()
{
  mTaskIndex = mTaskIndex + 1;
  if ( mTaskIndex >= mWorkTaskVector.size() ) {
    mTaskIndex = 0;
    mFgSendToDepot = true;
    if ( mPowerPack->getBatteryLevel() < 0.6 )
      mState = GOTO_CHARGER;
    else
      if ( mState != UNALLOCATED )
        mState = GOTO_DEPOT;
  } else {
    mFgSendToDepot = false;
    mState = GOTO_SOURCE;
  }
  mCurrentWorkTask = mWorkTaskVector[mTaskIndex];
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::startupPolicy ( float dt )
{
  // select a task at random with uniform probability
  mTaskIndex = ( int ) floor ( drand48() * mWorkTaskVector.size() );
  mCurrentWorkTask = mWorkTaskVector[mTaskIndex];

  // select first charger
  mCurrentChargerDestination = mChargerDestinationList.front();

  mState = GOTO_SOURCE;
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::leaveChargerPolicy ( float dt )
{
  if ( mFgSendToDepot )
    mState = GOTO_DEPOT;
  else
    mState = GOTO_SOURCE;
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::waitAtSourcePolicy ( float dt )
{
  float r;
  mWaitDuration = mWaitDuration + dt;

  if ( mFgStateChanged ) {
    r = drand48();
    mWaitDurationThreshold = r * mTravelDuration / ( 1 - r );
    //rprintf("mWaitDurationThreshold %f \n",mWaitDurationThreshold);
  }

  if ( mWaitDuration > mWaitDurationThreshold )
    selectNewTask();
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::waitAtChargerPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::unallocatedPolicy ( float dt )
{
  if ( mBroadCast->hasMessage ( mSimTime ) ) {
    mCurrentWorkTask = mBroadCast->popMessage();
    mState = GOTO_SOURCE;
  }
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::pickupCompletedPolicy ( float dt )
{
  if ( mPickupDuration < 2.0 ) {
    mBroadCast->addMessage ( mCurrentWorkTask, mSimTime );
    //rprintf("requesting more workers %f \n", mPickupDuration );
  }
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::waitingAtPickupPolicy ( float dt )
{
  float waitTime;

  waitTime = MAX ( 0.5 * mCurrentWorkTask->getExperiencedTaskDuration(),
                   MIN_PICKUP_WAIT_DURATION );

  if ( mPickupDuration > waitTime )
    selectNewTask();
}
//-----------------------------------------------------------------------------


