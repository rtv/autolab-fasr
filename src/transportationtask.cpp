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
#include "transportationtask.h"
#include "assert.h"
//-----------------------------------------------------------------------------
CTransportationTask::CTransportationTask ( Stg::Model* stgSource,
    Stg::Model* stgSink, std::string name ) : ITransportTaskInterface()
{
  assert ( stgSource );
  assert ( stgSink );

  mName = name;

  mSinkDestination = new CDestination ( stgSink );
  mSourceDestination = new CDestination ( stgSource );

  mProductionRate = 0.0f;
  mReward = 0.0f;
  mStorageCapacity = 0;
  mTaskCompleteCounter = 0;
}
//-----------------------------------------------------------------------------
CTransportationTask::~CTransportationTask()
{
}
//-----------------------------------------------------------------------------
void CTransportationTask::setProductionRate ( float rate )
{
  mProductionRate = rate;
}
//-----------------------------------------------------------------------------
void CTransportationTask::setReward ( float reward )
{
  mReward = reward;
}
//-----------------------------------------------------------------------------
void CTransportationTask::setStorageCapacity ( int capacity )
{
  mStorageCapacity = capacity;
}
//----------------------------------------------------------------------------
void CTransportationTask::setSourceSinkDistance(float dist)
{
  mSourceSinkDistance = dist;
}
//----------------------------------------------------------------------------
void CTransportationTask::incrementTaskCompletedCount()
{
  mTaskCompleteCounter++;
}
//----------------------------------------------------------------------------

