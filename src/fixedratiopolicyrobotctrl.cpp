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
#include "fixedratiopolicyrobotctrl.h"

//-----------------------------------------------------------------------------
CFixedRatioPolicyRobotCtrl::CFixedRatioPolicyRobotCtrl( ARobot* robot )
    : ABaseRobotCtrl( robot )
{
  static unsigned int id = 0;

  // generate a unique ID
  id ++;
  mRobotId = id;
  mCurrentTask = NULL;
}
//-----------------------------------------------------------------------------
CFixedRatioPolicyRobotCtrl::~CFixedRatioPolicyRobotCtrl()
{
}
//-----------------------------------------------------------------------------
void CFixedRatioPolicyRobotCtrl::addTransportationTask(
  ITransportTaskInterface* task )
{
  ABaseRobotCtrl::addTransportationTask( task );
}
//-----------------------------------------------------------------------------
void CFixedRatioPolicyRobotCtrl::startPolicy()
{
  char* name = new char[20];
  int num;
  unsigned int id = 0;
  FILE* fp;

  fp = fopen( "ratio.dat", "r" );
  if ( fp == NULL ) {
    PRT_WARN0( "No ratio.dat file found" );
    exit( -1 );
  }

  for ( unsigned int i = 0; i < mTaskVector.size(); i++ ) {
    if ( fscanf( fp, "TASK: %s = %d\n", name, &num ) != 2 ) {
      PRT_WARN0( "Failure while reading ratio.dat" );
      exit(-1);
    }

    if ( strcmp( name, mTaskVector[i]->getName().c_str() ) != 0 ) {
      PRT_WARN2( "Task name mismatch, got %s but expected %s", name,
                 mTaskVector[i]->getName().c_str() );
      exit(-1);
    }
    id += num;
    if ( mRobotId <= id ) {
      mCurrentTask = mTaskVector[i];
      break;
    }
  }
  if ( mCurrentTask )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_DEPOT;

  delete name;
}
//-----------------------------------------------------------------------------
void CFixedRatioPolicyRobotCtrl::leaveChargerPolicy( float dt )
{
  mState = GOTO_SOURCE;
}
//-----------------------------------------------------------------------------
void CFixedRatioPolicyRobotCtrl::deliveryCompletedPolicy( float dt )
{
  mState = GOTO_SOURCE;
}
//-----------------------------------------------------------------------------
void CFixedRatioPolicyRobotCtrl::unallocatedPolicy( float dt )
{
}
//-----------------------------------------------------------------------------
void CFixedRatioPolicyRobotCtrl::waitingAtSourcePolicy( float dt )
{
}
//-----------------------------------------------------------------------------
void CFixedRatioPolicyRobotCtrl::pickupCompletedPolicy( float dt )
{
}
//-----------------------------------------------------------------------------
void CFixedRatioPolicyRobotCtrl::waitingAtPickupPolicy( float dt )
{
}
//-----------------------------------------------------------------------------
