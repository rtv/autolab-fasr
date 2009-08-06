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
#include "epsilonsamplerrobotctrl.h"

//-----------------------------------------------------------------------------
CEpsilonSamplerRobotCtrl::CEpsilonSamplerRobotCtrl(ARobot* robot)
 : ABaseRobotCtrl(robot)
{
  mTauGainRate = 0.2;
  mEpsilon = 0.1;
  mLpfGainRate = 0.0;

  mDataLogger->addVar( &mLpfGainRate, "lpfGainRate" );
}
//-----------------------------------------------------------------------------
CEpsilonSamplerRobotCtrl::~CEpsilonSamplerRobotCtrl()
{
}
//-----------------------------------------------------------------------------
void CEpsilonSamplerRobotCtrl::startPolicy()
{
  if ( mTaskVector.empty() ) {
    rprintf ( "No tasks available\n" );
    mState = GOTO_DEPOT;
    return;
  }

  // assign first task at random
  mTaskIndex = ( int ) round ( drand48() * ( mTaskVector.size() - 1 ) );
  mPrevTaskIndex = mTaskIndex;
  mCurrentTask = mTaskVector[mTaskIndex];

  if ( mCurrentTask ) {
    rprintf ( "CEpsilonSamplerRobotCtrl: Selected task: %s \n",
              mCurrentTask->getName().c_str() );
    mState = GOTO_SOURCE;
  }
  else {
    mState = GOTO_DEPOT;
  }
}
//-----------------------------------------------------------------------------
void CEpsilonSamplerRobotCtrl::leaveChargerPolicy( float dt )
{
}
//-----------------------------------------------------------------------------
void CEpsilonSamplerRobotCtrl::deliveryCompletedPolicy( float dt )
{
  float r;
  unsigned int nextTaskIndex;

  // check if we sampled (current task != previous task) and if we got
  // a higher reward this time then the long term moving average
  if ( (mTaskIndex != mPrevTaskIndex ) &&
       (mRewardRate > mLpfGainRate) ) {
    // we are better off if we stay with the new (sampled task)
    nextTaskIndex = mTaskIndex;
  }
  else {
    // no this one was no good, lets switch back to the task before sampling
    nextTaskIndex = mPrevTaskIndex;
  }

  if (mNumTrips == 1)
    mLpfGainRate = mRewardRate;
  else
    mLpfGainRate = mLpfGainRate + mTauGainRate * ( mRewardRate - mLpfGainRate );

  r = drand48();
  rprintf("gain %f lpf %f r %f \n", mRewardRate, mLpfGainRate, r);
  // with a small probability we sample other patches
  if (r < mEpsilon ) {
    // assign first task at random
    do {
      nextTaskIndex = ( int ) round ( drand48() * ( mTaskVector.size() - 1 ) );
    } while ( nextTaskIndex == mTaskIndex );
  }

  mPrevTaskIndex = mTaskIndex;
  mTaskIndex = nextTaskIndex;
  mCurrentTask = mTaskVector[mTaskIndex];

  // should we charge or work
  if ( chargingPolicy ( dt ) ) {
    mState = GOTO_CHARGER;
  }
  else {
    mState = GOTO_SOURCE;
  }
}
//-----------------------------------------------------------------------------
void CEpsilonSamplerRobotCtrl::unallocatedPolicy( float dt )
{
  // nothing to do, we are always allocated
}
//-----------------------------------------------------------------------------
void CEpsilonSamplerRobotCtrl::waitingAtSourcePolicy( float dt )
{
}
//-----------------------------------------------------------------------------
void CEpsilonSamplerRobotCtrl::pickupCompletedPolicy( float dt )
{
}
//-----------------------------------------------------------------------------
void CEpsilonSamplerRobotCtrl::waitingAtPickupPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------



