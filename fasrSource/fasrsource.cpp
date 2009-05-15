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
 * $Log: fasrsource.cpp,v $
 * Revision 1.10  2009-04-07 23:08:45  vaughan
 * small world works for rtv
 *
 * Revision 1.9  2009-04-07 20:42:04  vaughan
 * still crashy
 *
 * Revision 1.8  2009-03-31 23:52:58  jwawerla
 * Moved cell index from float to int math
 *
 * Revision 1.7  2009-03-31 01:41:59  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.6  2009-03-27 23:37:52  vaughan
 * added say string for sources
 *
 * Revision 1.5  2009-03-27 19:05:26  vaughan
 * running on OS X and using new controller parameter method
 *
 * Revision 1.4  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.3  2009-03-20 03:05:22  jwawerla
 * dynamic obstacles added to wavefront
 *
 * Revision 1.2  2009-03-16 14:27:17  jwawerla
 * robots still get stuck
 *
 * Revision 1.1.1.1  2009-03-15 03:52:03  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "fasrsource.h"

const double FLAGSZ = 0.4;

//-----------------------------------------------------------------------------
CFasrSource::CFasrSource ( Stg::Model* mod )
{
  mNextModicificationTime = 0.1;
  mSimTime = 0.0;
  mStgModel = mod;
  mLastProducedUnitTimestamp = 0.0;
  mUpdateInterval = 10;
  mTaskTimeline.clear();

  mStgModel->AddUpdateCallback ( ( Stg::stg_model_callback_t ) stgUpdate, this );
  strcpy ( mName, mStgModel->Token() );

  // get params from Stage model as specified in worldfile

  if ( ! mStgModel->GetPropertyStr ( "scriptname", &mScriptName,  NULL ) )
    PRT_WARN0 ( "No script specified " );
  else {
    PRT_MSG1 ( 4, "Loading script %s", mScriptName );
    loadScript();
  }
}

//-----------------------------------------------------------------------------
CFasrSource::~CFasrSource()
{
}
//-----------------------------------------------------------------------------
int CFasrSource::stgUpdate ( Stg::Model* mod, CFasrSource* source )
{
  source->mSimTime = mod->GetWorld()->SimTimeNow() / 1e6;

  // time to change our configuration ?
  if ( source->mSimTime == source->mNextModicificationTime ) {
    source->nextTaskConfiguration();
  }

  // produce flag if necessary
  if ( ( ( mod->GetWorld()->GetUpdateCount() % source->mUpdateInterval  == 0 ) &&
         ( source->mSimTime > 0 ) ) &&
       ( ( mod->GetFlagCount() < source->mStorageCapacity ) ||
         ( source->mStorageCapacity == -1 ) ) )
    mod->PushFlag ( new Stg::Flag ( Stg::stg_color_pack ( 1, 1, 0, 0 ), FLAGSZ ) );

  return 0; // run again
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

  fp = fopen ( mScriptName, "r" );
  if ( fp == NULL ) {
    PRT_ERR1 ( "Failed to open script file %s", mScriptName );
    return 0; // error
  }

  while ( feof ( fp ) == false ) {
    line++;
    if ( fscanf ( fp, "TASK: %f %s %s %s %d %f %f\n", &taskData.timestamp, name,
                  source, sink, &taskData.capacity, &taskData.productionRate,
                  &taskData.reward ) != 7 ) {
      PRT_ERR2 ( "While reading script file %s line %d", mScriptName, line );
    } else {
      if ( strcmp ( mName, source ) == 0 ) {
        addTaskData ( taskData );
      }
    }
  } // while

  fclose ( fp );

  return 1; // success
}
//-----------------------------------------------------------------------------
void CFasrSource::addTaskData ( tTaskData newTaskData )
{
  tTaskData taskData;

  std::list<tTaskData>::iterator it;

  if ( mTaskTimeline.size() == 0 ) {
    mTaskTimeline.push_back ( newTaskData );
    return;
  }

  for ( it = mTaskTimeline.begin(); it != mTaskTimeline.end(); it++ ) {
    taskData = *it;
    if ( newTaskData.timestamp < taskData.timestamp )  {
      it = mTaskTimeline.insert ( it, newTaskData );
      return;
    }
  }

  mTaskTimeline.push_back ( newTaskData );
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
    mUpdateInterval = ( int ) round ( 10.0 / mProductionRate );

    if ( mTaskTimeline.size() > 0 )
      mNextModicificationTime = mTaskTimeline.front().timestamp;
  }

  snprintf ( buf, 255, "Rate: %.2f Reward %.1f Capacity: %d", mProductionRate,
             mReward, mStorageCapacity );
  mStgModel->Say ( buf );
}
//-----------------------------------------------------------------------------

