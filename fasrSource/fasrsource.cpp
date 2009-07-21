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

#include "fasrsource.h"

const double FLAGSZ = 0.4;

//-----------------------------------------------------------------------------
CFasrSource::CFasrSource( Stg::Model* mod )
{
  mNextModicificationTime = 0.1;
  mSimTime = 0.0;
  mStgModel = mod;
  mProductionStartedTimestamp = 0.0;
  mFgProductionStarted = false;
  mUpdateInterval = 10;
  mTaskTimeline.clear();

  mStgModel->AddUpdateCallback(( Stg::stg_model_callback_t ) stgUpdate, this );
  strcpy( mName, mStgModel->Token() );

  // get params from Stage model as specified in worldfile

  if ( ! mStgModel->GetPropertyStr( "scriptname", &mScriptName,  NULL ) )
    PRT_WARN0( "No script specified " );
  else {
    PRT_MSG1( 4, "Loading script %s", mScriptName );
    loadScript();
  }
}

//-----------------------------------------------------------------------------
CFasrSource::~CFasrSource()
{
}
//-----------------------------------------------------------------------------
int CFasrSource::stgUpdate( Stg::Model* mod, CFasrSource* source )
{
  source->update();
  return 0; // run again
}
//-----------------------------------------------------------------------------
void CFasrSource::update()
{
  mSimTime = mStgModel->GetWorld()->SimTimeNow() / 1e6;

  // time to change our configuration ?
  if ( mSimTime == mNextModicificationTime ) {
    nextTaskConfiguration();
  }

  // produce flag if necessary
  if (( mStgModel->GetFlagCount() < mStorageCapacity ) ||
      ( mStorageCapacity == -1 ) ) {
    if ( mFgProductionStarted ) {
      if (( mSimTime - mProductionStartedTimestamp ) >= mUpdateInterval ) {
        mFgProductionStarted = false;
        mStgModel->PushFlag( new Stg::Flag( Stg::Color( 1, 1, 0), FLAGSZ ) );
      }
    }
    else {
      mFgProductionStarted = true;
      mProductionStartedTimestamp = mSimTime;
    }

  }

}
//-----------------------------------------------------------------------------
int CFasrSource::loadScript()
{
  int line = 0;
  tTaskData taskData;
  char* name = new char[20];
  char* source = new char[20];
  char* sink = new char[20];
  FILE* fp = NULL;

  fp = fopen( mScriptName, "r" );
  if ( fp == NULL ) {
    PRT_ERR1( "Failed to open script file %s", mScriptName );
    return 0; // error
  }

  while ( feof( fp ) == false ) {
    line++;
    if ( fscanf( fp, "TASK: %f %s %s %s %d %f %f\n", &taskData.timestamp, name,
                 source, sink, &taskData.capacity, &taskData.productionRate,
                 &taskData.reward ) != 7 ) {
      PRT_ERR2( "While reading script file %s line %d", mScriptName, line );
    }
    else {
      if ( strcmp( mName, source ) == 0 ) {
        addTaskData( taskData );
      }
    }
  } // while

  fclose( fp );

  return 1; // success
}
//-----------------------------------------------------------------------------
void CFasrSource::addTaskData( tTaskData newTaskData )
{
  tTaskData taskData;

  std::list<tTaskData>::iterator it;

  if ( mTaskTimeline.size() == 0 ) {
    mTaskTimeline.push_back( newTaskData );
    return;
  }

  for ( it = mTaskTimeline.begin(); it != mTaskTimeline.end(); it++ ) {
    taskData = *it;
    if ( newTaskData.timestamp < taskData.timestamp )  {
      it = mTaskTimeline.insert( it, newTaskData );
      return;
    }
  }

  mTaskTimeline.push_back( newTaskData );
}
//-----------------------------------------------------------------------------
void CFasrSource::nextTaskConfiguration()
{
  tTaskData taskData;
  char buf[256];

  if ( mTaskTimeline.size() > 0 ) {
    taskData = mTaskTimeline.front();
    mTaskTimeline.pop_front();

    mProductionRate = taskData.productionRate;
    mReward = taskData.reward;
    mStorageCapacity = taskData.capacity;
    mUpdateInterval = ( int ) round( 1.0 / mProductionRate );

    if ( mTaskTimeline.size() > 0 )
      mNextModicificationTime = mTaskTimeline.front().timestamp;
  }

  snprintf( buf, 255, "Rate: %.3f Reward %.1f Capacity: %d", mProductionRate,
            mReward, mStorageCapacity );
  mStgModel->Say( buf );
}
//-----------------------------------------------------------------------------

