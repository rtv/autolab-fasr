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
 * $Log: replanpolicyrobot.cpp,v $
 * Revision 1.6  2009-04-03 16:21:30  jwawerla
 * Move wavefront map specification to world file
 *
 * Revision 1.5  2009-04-03 15:10:02  jwawerla
 * *** empty log message ***
 *
 * Revision 1.4  2009-03-31 23:52:59  jwawerla
 * Moved cell index from float to int math
 *
 * Revision 1.3  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.2  2009-03-28 21:53:57  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1  2009-03-28 00:54:38  jwawerla
 * *** empty log message ***
 *
 *
 ***************************************************************************/
#include "replanpolicyrobotctrl.h"



//-----------------------------------------------------------------------------
CReplanPolicyRobotCtrl::CReplanPolicyRobotCtrl ( ARobot* robot )
    : ABaseRobotCtrl ( robot )
{
  rprintf ( "Loading CReplanPolicyRobotCtrl\n" );
  robotScheduler.registerRobot ( this );
  mFgFirstInDepot = false;
}
//-----------------------------------------------------------------------------
CReplanPolicyRobotCtrl::~CReplanPolicyRobotCtrl()
{
}
//-----------------------------------------------------------------------------
IWorkTaskRobotInterface* CReplanPolicyRobotCtrl::getTask()
{
  return mCurrentWorkTask;
}
//-----------------------------------------------------------------------------
int CReplanPolicyRobotCtrl::getRobotId()
{
  return mRobotId;
}
//-----------------------------------------------------------------------------
bool CReplanPolicyRobotCtrl::isUnAllocated()
{
  if ( ( ( mState == UNALLOCATED ) && ( mFgFirstInDepot == true ) ) ||
       ( mState == GOTO_DEPOT ) ||
       ( mState == START ) )
    return true;

  return false;
}
//-----------------------------------------------------------------------------
bool CReplanPolicyRobotCtrl::isWorking()
{
  if ( ( mState == GOTO_SOURCE ) ||
       ( mState == GOTO_SINK ) ||
       ( mState == DELIVER_LOAD )  ||
       ( mState == PICKUP_LOAD ) )
    return true;

  return false;
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::addWorkTask ( IWorkTaskRobotInterface* task )
{
  ABaseRobotCtrl::addWorkTask ( task );
  robotScheduler.addWorkTask ( task );
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::selectWorkTaskPolicy ( float dt )
{
  IWorkTaskRobotInterface* task;

  task = robotScheduler.getWorkTask ( this );

  if ( task == NULL ) {
    if ( mState != UNALLOCATED )
      mState = GOTO_DEPOT;
  } else {
    mState = GOTO_SOURCE;
    if ( task != mCurrentWorkTask )
      rprintf ( "new task %s \n", task->getName() );
    mCurrentWorkTask = task;
  }

  mFgFirstInDepot = false;
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::startupPolicy ( float dt )
{
// select first charger
  mCurrentChargerDestination = mChargerDestinationList.front();

  selectWorkTaskPolicy ( dt );
}
//-----------------------------------------------------------------------------
bool CReplanPolicyRobotCtrl::chargingPolicy ( float dt )
{
  if ( ABaseRobotCtrl::chargingPolicy ( dt ) == true ) {
    robotScheduler.robotIsCharging ( this );
    mCurrentWorkTask = NULL;
    return true;
  }
  return false;
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::leaveChargerPolicy ( float dt )
{
  selectWorkTaskPolicy ( dt );
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::waitAtSourcePolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::waitAtChargerPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::unallocatedPolicy ( float dt )
{
  mFgFirstInDepot = true;
  selectWorkTaskPolicy ( dt );
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::pickupCompletedPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CReplanPolicyRobotCtrl::waitingAtPickupPolicy ( float dt )
{
  //rprintf("production rate %f \n",mCurrentWorkTask->getProductionRate() );
  if ( mCurrentWorkTask->getProductionRate() == 0.0 ) {
    selectWorkTaskPolicy( dt );
  } else if ( mPickupDuration > 2.0 / mCurrentWorkTask->getProductionRate() ) {
    //selectWorkTaskPolicy();
  }
}
//-----------------------------------------------------------------------------



