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
#include "destination.h"
#include "assert.h"
#include "printerror.h"
#include "utilities.h"

//-----------------------------------------------------------------------------
CDestination::CDestination( Stg::Model* model )
{
  float dir;
  Stg::Pose pose;

  assert( model );
  mStgModel = model;

  pose = mStgModel->GetPose();
  mLocation.mX = pose.x;
  mLocation.mY = pose.y;

  if ( !  mStgModel->GetPropertyFloat ( "queuedir", &dir, 0.0 ) )
    PRT_WARN1 ( "queuedir not specified for destination %s\n",
                mStgModel->Token() );

  mQueueDirection = D2R ( dir );
  mName = mStgModel->Token();
}
//-----------------------------------------------------------------------------
CDestination::~CDestination()
{
}
//-----------------------------------------------------------------------------
void CDestination::enterQueue( ARobotCtrl* robotCtrl )
{
  ARobotCtrl* ctrl;
  std::list<ARobotCtrl*>::iterator it;

  for ( it = mWaitingQueue.begin(); it != mWaitingQueue.end(); it++ ) {
    ctrl = *it;
    if ( ctrl == robotCtrl ) {
      PRT_WARN1( "Robot is already in this queue %s",
                 robotCtrl->getRobot()->getName().c_str() );
      return;
    }
  }
  mWaitingQueue.push_back( robotCtrl );
}
//-----------------------------------------------------------------------------
void CDestination::leaveQueue( ARobotCtrl* robotCtrl )
{
  ARobotCtrl* ctrl;
  std::list<ARobotCtrl*>::iterator it;

  for ( it = mWaitingQueue.begin(); it != mWaitingQueue.end(); it++ ) {
    ctrl = *it;
    if ( ctrl == robotCtrl ) {
      mWaitingQueue.erase( it );
      return;
    }
  }
  // robot was not in queue lets add it to the queue
  PRT_WARN1( "Robot %s was not in the queue",
             robotCtrl->getRobot()->getName().c_str() );

}
//-----------------------------------------------------------------------------
int CDestination::getQueuePosition( ARobotCtrl* robotCtrl, CPose2d &pose )
{
  bool fgFound = false;;
  int index = 0;
  ARobotCtrl* ctrl;
  std::list<ARobotCtrl*>::iterator it;

  for ( it = mWaitingQueue.begin(); it != mWaitingQueue.end(); it++ ) {
    index ++;
    ctrl = *it;
    if ( ctrl == robotCtrl ) {
      fgFound = true;
      break;
    }
  }

  // robot was not in queue lets add it to the queue
  if ( fgFound == false ) {
    PRT_WARN1( "Robot %s was not in the queue",
               robotCtrl->getRobot()->getName().c_str() );
    index ++;
    enterQueue( robotCtrl );
  }

  pose.mX = mLocation.mX + cos( mQueueDirection ) * ( index )
            * WAITING_SPACING;
  pose.mY = mLocation.mY + sin( mQueueDirection ) * ( index )
            * WAITING_SPACING;
  pose.mYaw = normalizeAngle( mQueueDirection - PI );

  return index;
}
//-----------------------------------------------------------------------------
void CDestination::pushFlag( Stg::Flag* flag )
{
  if ( flag ) {
    mStgModel->Lock();
    mStgModel->PushFlag( flag );
    mStgModel->Unlock();
  }
}
//-----------------------------------------------------------------------------
Stg::Flag* CDestination::popFlag()
{
  Stg::Flag* flag = NULL;

  if ( mStgModel->GetFlagCount() > 0 ) {
    mStgModel->Lock();
    flag = mStgModel->PopFlag();
    mStgModel->Unlock();
  }

  return flag;
}
//-----------------------------------------------------------------------------
bool CDestination::hasNumFlags()
{
  if ( mStgModel->GetFlagCount() > 0 )
    return true; // yes flags available

  return false; // no flags available
}
//-----------------------------------------------------------------------------
CPose2d CDestination::getLocation()
{
  CPose2d pose;

  pose = mLocation;
  pose.mYaw = normalizeAngle(mQueueDirection + PI);
  return pose;
}
//-----------------------------------------------------------------------------
bool CDestination::isNearEofQueue( CPose2d pose )
{
  float dist;
  dist = CLOSENESS_THRESHOLD + mWaitingQueue.size() * WAITING_SPACING;

  dist = limit ( dist, 0.0, MAX_CLOSENESS_DISTANCE );

  if ( euclidian(pose.mX, pose.mY, mLocation.mX, mLocation.mY ) < dist )
    return true;

  return false;
}
//---------------------------------------------------------------------------
