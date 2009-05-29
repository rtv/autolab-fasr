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
 * $Log: destination.cpp,v $
 * Revision 1.10  2009-04-03 15:10:02  jwawerla
 * *** empty log message ***
 *
 * Revision 1.9  2009-03-31 04:27:32  jwawerla
 * Some bug fixing
 *
 * Revision 1.8  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.7  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.6  2009-03-24 01:04:46  jwawerla
 * switching cost robot added
 *
 * Revision 1.5  2009-03-21 02:31:50  jwawerla
 * *** empty log message ***
 *
 * Revision 1.4  2009-03-20 03:05:23  jwawerla
 * dynamic obstacles added to wavefront
 *
 * Revision 1.3  2009-03-17 03:11:30  jwawerla
 * Another day no glory
 *
 * Revision 1.2  2009-03-16 14:27:18  jwawerla
 * robots still get stuck
 *
 * Revision 1.1.1.1  2009-03-15 03:52:03  jwawerla
 * First commit
 *
 *
 ***************************************************************************/

#include "destination.h"
#include "utilities.h"
#include "string.h"


//-----------------------------------------------------------------------------
CDestination::CDestination ( double x, double y, double queueDirection,
                             const char* name )
{
  mStgModel = NULL;

  if ( name == NULL )
    snprintf ( mName, 20, "noname" );
  else
    strncpy ( mName, name, 20 );

  mAccessMutex = NULL;

  mQueueDirection = queueDirection;
  mWaitingQueue.clear();

  mLocation.mX = x;
  mLocation.mY = y;
}
//-----------------------------------------------------------------------------
CDestination::CDestination ( const char* name )
{
  mStgModel = NULL;

  if ( name == NULL )
    snprintf ( mName, 20, "noname" );
  else
    strncpy ( mName, name, 20 );

  mAccessMutex = NULL;

  mQueueDirection = 0.0;
  mWaitingQueue.clear();

  mLocation.mX = 0.0;
  mLocation.mY = 0.0;
}
//-----------------------------------------------------------------------------
CDestination::CDestination ( Stg::Model* mod )
{
  float dir;
  mStgModel = mod;
  mAccessMutex = NULL;

  if ( !  mStgModel->GetPropertyFloat ( "queuedir", &dir, 0.0 ) )
    PRT_WARN1 ( "queuedir not specified for destination %s\n",
                mStgModel->Token() );

  mQueueDirection = D2R ( dir );
  mLocation.mX = mStgModel->GetPose().x;
  mLocation.mY = mStgModel->GetPose().y;
  strncpy ( mName, mStgModel->Token(), 20 );
}
//-----------------------------------------------------------------------------
CDestination::~CDestination()
{
}
//-----------------------------------------------------------------------------
char* CDestination::getName()
{
  return mName;
}
//-----------------------------------------------------------------------------
CPoint2d CDestination::getLocation()
{
  return mLocation;
}
//-----------------------------------------------------------------------------
void CDestination::lock()
{
  if ( mAccessMutex == NULL )
    mAccessMutex = g_mutex_new();

  assert ( mAccessMutex );
  g_mutex_lock ( mAccessMutex );
}
//-----------------------------------------------------------------------------
void CDestination::unlock()
{
  assert ( mAccessMutex );
  g_mutex_unlock ( mAccessMutex );
}
//---------------------------------------------------------------------------
void CDestination::enterWaitingQueue ( IRobotWaitInterface* robot )
{
  mWaitingQueue.push_back ( robot );
}
//---------------------------------------------------------------------------
void CDestination::leaveWaitingQueue ( IRobotWaitInterface* robot )
{
  bool fgFound = false;
  IRobotWaitInterface* bot;
  std::list<IRobotWaitInterface*>::iterator it;

  for ( it = mWaitingQueue.begin(); it != mWaitingQueue.end(); it++ ) {
    bot = *it;
    if ( bot == robot ) {
      fgFound = true;
      mWaitingQueue.erase ( it );
      break;
    }
  }
  // robot was not in queue lets add it to the queue
  if ( fgFound == false ) {
    PRT_WARN1 ( "Robot %s was not in the queue", robot->getName() );
  }
}
//---------------------------------------------------------------------------
int CDestination::getWaitingPosition ( IRobotWaitInterface* robot,
                                       CPose2d &pose )
{
  bool fgFound = false;;
  int index = 0;
  IRobotWaitInterface* bot;
  std::list<IRobotWaitInterface*>::iterator it;

  for ( it = mWaitingQueue.begin(); it != mWaitingQueue.end(); it++ ) {
    index ++;
    bot = *it;
    if ( bot == robot ) {
      fgFound = true;
      break;
    }
  }

  // robot was not in queue lets add it to the queue
  if ( fgFound == false ) {
    PRT_WARN1 ( "Robot %s was not in the queue", robot->getName() );
    index ++;
    enterWaitingQueue ( robot );
  }

  pose.mX = mLocation.mX + cos ( mQueueDirection ) * ( index )
            * WAITING_SPACING;
  pose.mY = mLocation.mY + sin ( mQueueDirection ) * ( index )
            * WAITING_SPACING;
  pose.mYaw = normalizeAngle ( mQueueDirection - PI );

  return index;
}
//---------------------------------------------------------------------------
bool CDestination::isNear ( CPose2d pose )
{
  float dist;
  dist = CLOSENESS_THRESHOLD + mWaitingQueue.size() * WAITING_SPACING;

  dist = limit ( dist, 0.0, MAX_CLOSENESS_DISTANCE );

  if ( getDistance ( pose ) < dist )
    return true;

  return false;
}
//---------------------------------------------------------------------------
float CDestination::getDistance ( CPose2d pose )
{
  return euclidian ( mLocation.mX, mLocation.mY, pose.mX, pose.mY );
}
//---------------------------------------------------------------------------
CPose2d CDestination::getPose()
{
  return CPose2d ( mLocation.mX, mLocation.mY, mQueueDirection );
}
//---------------------------------------------------------------------------
void CDestination::setLocation ( CPoint2d point )
{
  mLocation.mX = point.mX;
  mLocation.mY = point.mY;
}
//---------------------------------------------------------------------------
void CDestination::setLocation ( float x, float y )
{
  mLocation.mX = x;
  mLocation.mY = y;
}
//---------------------------------------------------------------------------
void CDestination::setQueueDirection ( float dir )
{
  mQueueDirection = dir;
}
//---------------------------------------------------------------------------


