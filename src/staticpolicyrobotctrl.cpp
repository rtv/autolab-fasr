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
#include "staticpolicyrobotctrl.h"

//-----------------------------------------------------------------------------
CStaticPolicyRobotCtrl::CStaticPolicyRobotCtrl( ARobot* robot )
    : ABaseRobotCtrl( robot ), IRobotSchedulerInterface()
{
  static unsigned int id = 0;

  // generate a unique ID
  id ++;
  mRobotId = id;

  mScheduler = CRobotScheduler::getInstance();
  mScheduler->registerRobot(this);
}
//-----------------------------------------------------------------------------
CStaticPolicyRobotCtrl::~CStaticPolicyRobotCtrl()
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::addTransportationTask(
  ITransportTaskInterface* task )
{
  ABaseRobotCtrl::addTransportationTask( task );
  mScheduler->addTransportationTask( task );
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::startPolicy()
{
  mCurrentTask = mScheduler->getTask( this );
  if (mCurrentTask)
    mState = GOTO_SOURCE;
  else
    mState = GOTO_DEPOT;
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::leaveChargerPolicy( float dt )
{
  mState = GOTO_SOURCE;
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::deliveryCompletedPolicy( float dt )
{
  mState = GOTO_SOURCE;
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::unallocatedPolicy( float dt )
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::waitingAtSourcePolicy( float dt)
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::pickupCompletedPolicy( float dt )
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::waitingAtPickupPolicy( float dt )
{
}
//-----------------------------------------------------------------------------