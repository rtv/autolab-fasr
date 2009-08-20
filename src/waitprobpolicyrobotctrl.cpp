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
#include "waitprobpolicyrobotctrl.h"

/** Minimum time we wait at a pickup for a flag [s] */
const float MIN_PICKUP_WAIT_DURATION = 50.0;

//-----------------------------------------------------------------------------
CWaitProbPolicyRobotCtrl::CWaitProbPolicyRobotCtrl ( ARobot* robot,
    float probBroadcast )
    : ABaseRobotCtrl ( robot )
{
  mProbBroadcast = probBroadcast;
  mPickupDurationThreshold = 0.5;
  mWaitDurationThreshold = 0.0;
  mTaskIndex = 0;
  mBroadCast = CBroadCast::getInstance();
}
//-----------------------------------------------------------------------------
CWaitProbPolicyRobotCtrl::~CWaitProbPolicyRobotCtrl()
{
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::startPolicy()
{
  if ( mTaskVector.empty() ) {
    rprintf ( "No tasks available\n" );
    return;
  }

  // assign first task at random
  mTaskIndex = ( int ) round ( drand48() * ( mTaskVector.size() - 1 ) );
  mCurrentTask = mTaskVector[mTaskIndex];

  if ( mCurrentTask ) {
    rprintf ( "CWaitProbPolicyRobotCtrl: Selected task: %s \n",
              mCurrentTask->getName().c_str() );
    mState = GOTO_SOURCE;
  }
  else {
    mState = GOTO_DEPOT;
  }
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
void CWaitProbPolicyRobotCtrl::deliveryCompletedPolicy ( float dt )
{
  // should we charge or work
  if ( chargingPolicy ( dt ) ) {
    mState = GOTO_CHARGER;
  }
  else {
    mState = GOTO_SOURCE;
  }
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::unallocatedPolicy ( float dt )
{
  if ( mBroadCast->hasMessage ( mRobot->getCurrentTime() ) ) {
    mCurrentTask = mBroadCast->popMessage();
    mState = GOTO_SOURCE;
  }
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::waitingAtSourcePolicy ( float dt )
{
  float r;
  float time;

  if ( mFgStateChanged ) {
    r = drand48();
    time = mTaskCompletionTime - mSlowedDownTime - mSourceWaitingTime -
           mSinkWaitingTime;
    //mWaitDurationThreshold = r * mTaskCompletionTime / ( 1.0 - r );
    mWaitDurationThreshold = r * time;
    // wait at least 10 sec
    mWaitDurationThreshold = std::max ( mWaitDurationThreshold, (float)10.0 );
    //rprintf( "mWaitDurationThreshold %f \n", mWaitDurationThreshold );
  }

  // only consider task switching if we actually have some information about
  // the task competion time
  if ( mTaskCompletionTime > 0.0 ) {
    if ( mElapsedStateTime > mWaitDurationThreshold )
      selectNewTask();


    snprintf ( mStatusStr, 20, "wait %0.1f %01.f", mElapsedStateTime,
               mWaitDurationThreshold );
  }
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::pickupCompletedPolicy ( float dt )
{
  if ( mPickupTime < mPickupDurationThreshold ) {
    printf ( " %f %f = %f \n",mSlowedDownTime+mSourceWaitingTime , mTaskCompletionTime, mProbBroadcast *
             ( 1.0 - (mSlowedDownTime + mSourceWaitingTime) / mTaskCompletionTime ) );
    if ( drand48() < ( mProbBroadcast *
                       ( 1.0 - ( mSlowedDownTime + mSourceWaitingTime )
                         / mTaskCompletionTime ) ) )
      mBroadCast->addMessage ( mCurrentTask, mRobot->getCurrentTime() );
    /*
    rprintf( "requesting more workers %f for task %s \n", mPickupTime,
             mCurrentTask->getName().c_str() );
    */
  }
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::selectNewTask()
{

  if ( mCurrentTask )
    mCurrentTask->getSource()->leaveQueue ( this );

  mTaskIndex = mTaskIndex + 1;

  if ( mTaskIndex >= mTaskVector.size() ) {
    mTaskIndex = 0;
    mCurrentTask = NULL;
    mFgSendToDepot = true;
    if ( mPowerPack->getBatteryLevel() < 0.6 ) {
      mState = GOTO_CHARGER;
    }
    else {
      if ( mState != UNALLOCATED )
        mState = GOTO_DEPOT;
    }
  }
  else {
    mFgSendToDepot = false;
    mState = GOTO_SOURCE;
    mCurrentTask = mTaskVector[mTaskIndex];
  }
}
//-----------------------------------------------------------------------------
void CWaitProbPolicyRobotCtrl::waitingAtPickupPolicy ( float dt )
{
  float waitTime;

  waitTime = std::max ( mTaskCompletionTime,
                   MIN_PICKUP_WAIT_DURATION );

  if ( mElapsedStateTime > waitTime )
    selectNewTask();
}
//-----------------------------------------------------------------------------
