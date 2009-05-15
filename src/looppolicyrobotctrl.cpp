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
 * $Log: looppolicyrobot.cpp,v $
 * Revision 1.7  2009-04-03 16:21:30  jwawerla
 * Move wavefront map specification to world file
 *
 * Revision 1.6  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.5  2009-03-27 00:27:37  jwawerla
 * *** empty log message ***
 *
 * Revision 1.4  2009-03-24 01:04:46  jwawerla
 * switching cost robot added
 *
 * Revision 1.3  2009-03-21 02:31:50  jwawerla
 * *** empty log message ***
 *
 * Revision 1.2  2009-03-20 03:05:23  jwawerla
 * dynamic obstacles added to wavefront
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/

#include "looppolicyrobotctrl.h"

//-----------------------------------------------------------------------------
CLoopPolicyRobotCtrl::CLoopPolicyRobotCtrl ( ARobot* robot )
    : ABaseRobotCtrl ( robot )
{
  rprintf ( "Loading CLoopPolicyRobotCtrl\n" );
}
//-----------------------------------------------------------------------------
CLoopPolicyRobotCtrl::~CLoopPolicyRobotCtrl()
{
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::selectWorkTaskPolicy ( float dt )
{
  IWorkTaskRobotInterface* task;
  std::vector<IWorkTaskRobotInterface*>::iterator it;

  for ( it = mWorkTaskVector.begin(); it != mWorkTaskVector.end(); it++ ) {
    task = ( IWorkTaskRobotInterface* ) ( *it );
    if ( task == mCurrentWorkTask ) {
      it++;
      if ( it != mWorkTaskVector.end() )
        mCurrentWorkTask = ( IWorkTaskRobotInterface* ) ( *it );
      else
        mCurrentWorkTask = ( mWorkTaskVector.front() );

      break;  // found what we were looking for and we are out of here
    }
  }
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::startupPolicy ( float dt )
{
  mCurrentWorkTask = mWorkTaskVector.back();
  mCurrentChargerDestination = mChargerDestinationList.front();
  if ( mCargoLoad == 0 )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_SINK;
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::leaveChargerPolicy ( float dt )
{
  if ( mCargoLoad == 0 )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_SINK;
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::waitAtSourcePolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::waitAtChargerPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::unallocatedPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::pickupCompletedPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CLoopPolicyRobotCtrl::waitingAtPickupPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
