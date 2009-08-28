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
#include "taskmanager.h"
#include "utilities.h"

//-----------------------------------------------------------------------------
CTaskManager::CTaskManager ( Stg::Model* mod )
{
  char* script;
  char* mapModelName = NULL;
  float wavefrontCellSize;
  mSimTime = 0;
  mNextModificationTime = 0.0;
  mTaskList.clear();
  mRobotList.clear();

  assert ( mod );


  mStgModel = mod;
  mStgModel->AddUpdateCallback ( ( Stg::stg_model_callback_t ) stgUpdate, this );

  // check if we got a task description file passed in on the command line
  for ( unsigned int i=0; i< Stg::World::args.size(); i++ ) {
    if ( Stg::World::args[i].compare ( 0, 3, "-td" ) == 0 ) {
      mScriptName = Stg::World::args[i].substr ( 4, Stg::World::args[i].size()-4 ).c_str();
      PRT_MSG1 ( 3, "World file task description file overruled by user, new file: %s",
                 mScriptName.c_str() );
      break;
    }
  }

  if ( mScriptName.empty() ) {
    if ( ! mStgModel->GetPropertyStr ( "scriptname", &script,  NULL ) )
      PRT_WARN0 ( "No script specified " );
    mScriptName = script;
  }

  if ( ! mod->GetPropertyFloat ( "wavefrontcellsize", &wavefrontCellSize,  0.1 ) )
    PRT_WARN1 ( "CTaskManager %s has no parameter wavefrontcellsize specified in worldfile.",
                mod->Token() );

  if ( ! mod->GetPropertyStr ( "mapmodel", &mapModelName,  NULL ) ) {
    PRT_ERR1 ( "robot %s has no mapmodel string specified in worldfile.",
               mod->Token() );
    exit ( -1 );
  }

  Stg::Model* mapModel = mod->GetWorld()->GetModel ( mapModelName );
  if ( ! mapModel ) {
    PRT_ERR1 ( "worldfile has no model named \"%s\"", mapModelName );
    exit ( -1 );
  }

  // get an occupancy grid from the Stage model
  Stg::Geom geom = mapModel->GetGeom();
  int width = geom.size.x / wavefrontCellSize;
  int height = geom.size.y / wavefrontCellSize;

  if ( ( width % 2 ) == 0 )
    width++;

  if ( ( height % 2 ) == 0 )
    height++;

  uint8_t* cells = new uint8_t[ width * height ];
  mapModel->Rasterize ( cells,
                        width, height,
                        wavefrontCellSize, wavefrontCellSize );
  mWaveFrontMap = new CWaveFrontMap ( new CGridMap ( cells,
                                      width, height,
                                      wavefrontCellSize ),
                                      "map" );
  if ( cells )
    delete[] cells;

  // enforce first script parsing
  PRT_MSG1 ( 5, "Loading script %s", mScriptName.c_str() );
  loadScript();
}
//-----------------------------------------------------------------------------
CTaskManager::~CTaskManager()
{
  CTransportationTask* task = NULL;
  std::list<CTransportationTask*>::iterator it;

  for ( it = mTaskList.begin(); it != mTaskList.end(); it++ ) {
    task = *it;
    if ( task )
      delete task;
    it = mTaskList.erase ( it );
  }
}
//-----------------------------------------------------------------------------
CTaskManager* CTaskManager::getInstance ( Stg::Model* mod )
{
  static CTaskManager* instance = NULL;

  if ( instance == NULL )
    instance = new CTaskManager ( mod );

  return instance;
}
//-----------------------------------------------------------------------------
void CTaskManager::registerRobot ( ABaseRobotCtrl* robot )
{
  std::list<CTransportationTask*>::iterator it;

  mRobotList.push_back ( robot );

  for ( it = mTaskList.begin(); it != mTaskList.end(); it++ ) {
    robot->addTransportationTask ( *it );
  }
}
//-----------------------------------------------------------------------------
int CTaskManager::stgUpdate ( Stg::Model* mod, CTaskManager* taskManager )
{
  taskManager->mSimTime = mod->GetWorld()->SimTimeNow() / 1e6;

  if ( taskManager->mSimTime == taskManager->mNextModificationTime )
    taskManager->loadScript();

  return 0; // ok
}
//-----------------------------------------------------------------------------
int CTaskManager::loadScript()
{
  CTransportationTask* newWorkTask = NULL;
  bool fgFound;
  std::list<CTransportationTask*>::iterator it;
  float timestamp = 0.0;
  int capacity;
  float productionRate = 0.0;
  float reward = 0.0;
  char* taskName = new char[20];
  char* sourceName = new char[20];
  char* sinkName = new char[20];
  FILE* fp = NULL;

  fp = fopen ( mScriptName.c_str(), "r" );
  if ( fp == NULL ) {
    PRT_ERR1 ( "Failed to open script file %s", mScriptName.c_str() );
    return 0; // error
  }

  mNextModificationTime = INFINITY;

  while ( feof ( fp ) == false ) {
    if ( fscanf ( fp, "TASK: %f %s %s %s %d %f %f\n", &timestamp, taskName,
                  sourceName, sinkName, &capacity, &productionRate,
                  &reward ) != 7 ) {
      PRT_ERR1 ( "While reading script file %s", mScriptName.c_str() );
    }

    // check if this task is already in the list of tasks, if not add it to the list
    fgFound = false;
    for ( it = mTaskList.begin(); it != mTaskList.end(); it++ ) {
      if ( strcmp ( ( *it )->getName().c_str(), taskName ) == 0 ) {
        fgFound = true;
        newWorkTask = *it;
      }
    }
    if ( fgFound == false ) {
      newWorkTask = new CTransportationTask ( mStgModel->GetWorld()->GetModel ( sourceName ),
                                              mStgModel->GetWorld()->GetModel ( sinkName ),
                                              taskName );
      mTaskList.push_back ( newWorkTask );

      mWaveFrontMap->calculateWaveFront ( newWorkTask->getSource()->getLocation(),
                                          CWaveFrontMap::USE_MAP_ONLY );
      newWorkTask->setSourceSinkDistance ( mWaveFrontMap->calculatePlanFrom (
                                             newWorkTask->getSink()->getLocation() ) );
    }

    if ( ( timestamp == mSimTime ) && ( newWorkTask != NULL ) ) {
      newWorkTask->setStorageCapacity ( capacity );
      newWorkTask->setReward ( reward );
      newWorkTask->setProductionRate ( productionRate );
    }

    if ( ( timestamp > mSimTime ) && ( timestamp < mNextModificationTime ) )
      mNextModificationTime = timestamp;

  } // while

  fclose ( fp );

  return 1; // success
}
//-----------------------------------------------------------------------------


