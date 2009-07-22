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
#include "replanpolicyrobotctrl.h"

//-----------------------------------------------------------------------------
CReplanPolicyRobotCtrl::CReplanPolicyRobotCtrl( ARobot* robot )
    : CStaticPolicyRobotCtrl( robot )
{
  mFgWasInDepot = false;
  rprintf("CReplanPolicyRobotCtrl\n");
}
//-----------------------------------------------------------------------------
CReplanPolicyRobotCtrl::~CReplanPolicyRobotCtrl()
{
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::deliveryCompletedPolicy( float dt )
{

  // only if we were not in the depot do we have a good idea how long it
  // took to complete the task and only then should we inform the scheduler
  if ( ( mFgWasInDepot == false) && ( mFgWasCharging == false) ) {
    mScheduler->setExperiencedTaskCompletionTime( mCurrentTask,
        mTaskCompletionTime - mSlowedDownTime - mSourceWaitingTime - mSinkWaitingTime);

    rprintf( "roundtrip %f slowedDown %f src %f sink %f => %f\n",
             mTaskCompletionTime, mSlowedDownTime, mSourceWaitingTime,
             mSinkWaitingTime,
             mTaskCompletionTime - mSourceWaitingTime - mSlowedDownTime -
             mSinkWaitingTime );
  }
  mCurrentTask = mScheduler->getTask( this );

  if ( mCurrentTask )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_DEPOT;

  mFgWasInDepot = false;
  mFgWasCharging = false;
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::unallocatedPolicy( float dt )
{
  mCurrentTask = mScheduler->getTask( this );
  if ( mCurrentTask ) {
    mState = GOTO_SOURCE;
    mFgWasInDepot = true;
  }
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::waitingAtSourcePolicy( float dt )
{
  if ( not mScheduler->keepWaiting( mCurrentTask, this ) ) {
    mCurrentTask = mScheduler->getTask( this );

    if ( mCurrentTask )
      mState = GOTO_SOURCE;
    else
      mState = GOTO_DEPOT;
  }
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::leaveChargerPolicy( float dt )
{
  mFgWasCharging = true;
  deliveryCompletedPolicy(dt);
}
//-----------------------------------------------------------------------------
bool CReplanPolicyRobotCtrl::chargingPolicy ( float dt )
{
  if ( ABaseRobotCtrl::chargingPolicy ( dt ) == true ) {
    mScheduler->robotIsCharging ( this );
    mCurrentTask = NULL;
    return true;
  }
  return false;
}
//-----------------------------------------------------------------------------
