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
#include "looppolicyrobotctrl.h"


//-----------------------------------------------------------------------------
CLoopPolicyRobotCtrl::CLoopPolicyRobotCtrl( ARobot* robot )
    : ABaseRobotCtrl( robot )
{
}
//-----------------------------------------------------------------------------
CLoopPolicyRobotCtrl::~CLoopPolicyRobotCtrl()
{
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::startPolicy()
{
  if ( mTaskVector.empty() ) {
    rprintf( "No tasks available\n" );
    return;
  }
  mCurrentTask = mTaskVector.front();
  rprintf( "Selected task: %s \n", mCurrentTask->getName().c_str() );

  mState = GOTO_SOURCE;
  mState = GOTO_DEPOT;
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::leaveChargerPolicy( float dt )
{
  mState = GOTO_SOURCE;
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::deliveryCompletedPolicy( float dt )
{
  ITransportTaskInterface* task;
  std::vector<ITransportTaskInterface*>::iterator it;

  for ( it = mTaskVector.begin(); it != mTaskVector.end(); it++ ) {
    task = ( ITransportTaskInterface* )( *it );
    if ( task == mCurrentTask ) {
      it++;
      if ( it != mTaskVector.end() )
        mCurrentTask = ( ITransportTaskInterface* )( *it );
      else
        mCurrentTask = ( mTaskVector.front() );

      break;  // found what we were looking for and we are out of here
    }
  }

  // should we charge or work
  if ( chargingPolicy( dt ) ) {
    mState = GOTO_CHARGER;
  }
  else {
    mState = GOTO_SOURCE;
  }
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::unallocatedPolicy( float dt )
{
  if ( mElapsedStateTime > 120.0 )
    mState = GOTO_SOURCE;
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::waitingAtSourcePolicy( float dt)
{
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::pickupCompletedPolicy( float dt )
{
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::waitingAtPickupPolicy( float dt )
{
}
//-----------------------------------------------------------------------------