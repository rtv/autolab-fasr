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
 * $Log: broadcast.cpp,v $
 * Revision 1.3  2009-03-31 04:27:32  jwawerla
 * Some bug fixing
 *
 * Revision 1.2  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.1  2009-03-27 01:32:58  jwawerla
 * wait prob controller improved
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "broadcast.h"

/** Only instance of this class */
CBroadCast* boadCastInstance = NULL;

//-----------------------------------------------------------------------------
CBroadCast::CBroadCast ( float ttl )
{
  mTimeToLive = ttl;
  mWorkerRequestList.clear();
}
//-----------------------------------------------------------------------------
CBroadCast::~CBroadCast()
{
}
//-----------------------------------------------------------------------------
CBroadCast* CBroadCast::getInstance( float ttl )
{
  if (boadCastInstance == NULL)
    boadCastInstance = new CBroadCast(ttl);

  return boadCastInstance;
}
//-----------------------------------------------------------------------------
bool CBroadCast::hasMessage ( float timestamp )
{
  removeOldMessages ( timestamp );

  if ( mWorkerRequestList.size() == 0 )
    return false;

  return true;
}
//-----------------------------------------------------------------------------
void CBroadCast::removeOldMessages ( float timestamp )
{
  tBCMsg msg;
  std::list<tBCMsg>::iterator it;

  if ( mWorkerRequestList.size() == 0 )
    return;

  for ( it = mWorkerRequestList.begin(); it != mWorkerRequestList.end(); it++ ) {
    msg = *it;
    if ( fabs ( msg.timestamp - timestamp ) > mTimeToLive )
      it = mWorkerRequestList.erase ( it );
  }
}
//-----------------------------------------------------------------------------
void CBroadCast::addMessage ( IWorkTaskRobotInterface* task, float timestamp )
{
  tBCMsg msg;
  removeOldMessages ( timestamp );

  msg.timestamp = timestamp;
  msg.workTask = task;

  mWorkerRequestList.push_back( msg );
}
//-----------------------------------------------------------------------------
IWorkTaskRobotInterface* CBroadCast::popMessage()
{
  IWorkTaskRobotInterface* task = NULL;

  task = mWorkerRequestList.front().workTask;
  mWorkerRequestList.pop_front();

  return task;
}
//-----------------------------------------------------------------------------
int CBroadCast::getNumMessage(float timestamp)
{
  removeOldMessages ( timestamp );
  return mWorkerRequestList.size();
}
//-----------------------------------------------------------------------------

