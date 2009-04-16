

#include <stdio.h>
#include <stdlib.h>


#include "baserobot.h"
#include "stage.hh"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
//#include <QString>

/** Static variable to count the number of robots using this controller */
static int robotNumber = 0;

//-----------------------------------------------------------------------------
ABaseRobot::ABaseRobot ( Stg::ModelPosition* modPos )
    : IRobotWaitInterface()
{
  char* mapModel = NULL;
  float wavefrontCellSize;

  //***********************************
  // User parameters
  mCargoBayCapacity = 1;

  if ( ! modPos->GetPropertyFloat ( "wavefrontcellsize", &wavefrontCellSize,  0.1 ) )
    PRT_WARN2 ( "robot %s has no parameter wavefrontcellsize specified in "\
                "worldfile. Using %.2f meters",
                modPos->Token(), wavefrontCellSize );


  if ( ! modPos->GetPropertyStr ( "mapmodel", &mapModel,  NULL ) ) {
    PRT_ERR1 ( "robot %s has no mapmodel string specified in worldfile.",
               modPos->Token() );
    exit ( -1 );
  }

  Stg::Model* map_model = modPos->GetWorld()->GetModel ( mapModel );
  if ( ! map_model ) {
    PRT_ERR1 ( "worldfile has no model named \"%s\"", mapModel );
    exit ( -1 );
  }

  // get an occupancy grid from the Stage model
  Stg::Geom geom = map_model->GetGeom();
  int width = geom.size.x / wavefrontCellSize;
  int height = geom.size.y / wavefrontCellSize;

  if ( ( width % 2 ) == 0 )
	 width++;
  
  if ( ( height % 2 ) == 0 )
	 height++;
  
  uint8_t* cells = new uint8_t[ width * height ];

  map_model->Rasterize ( cells, 
								 width, height, 
								 wavefrontCellSize, wavefrontCellSize );

  mWaveFrontMap = new CWaveFrontMap ( new CGridMap ( cells,
																	  width, height,
																	  wavefrontCellSize ),
                                      "map" );
  if(cells)
	 delete[] cells;

  assert( mWaveFrontMap );
  // place a pointer to the map in the model where others (e.g. the MapVis object)  can find it
  modPos->SetProperty ( "wavefront_map", static_cast<void*>( mWaveFrontMap ));

  //***********************************
  // Member variables, not subject to user configuration
  robotNumber = robotNumber + 1;
  mRobotId = robotNumber;
  mWaveFrontMap->calculateWaveFront ( 7.0, 7.0 );
  mFgChargerDetected = false;
  mAvoidCount = 0;
  mBatteryLevel = 1.0;
  mState = START;
  mPrevState = START;
  mPrevTimestepState = START;
  mTotalReward = 0.0;
  mFgStateChanged = true;
  mWayPointList.clear();
  mWaitDestination = NULL;
  mNd = NULL;
  mTimer = 0.0;
  mSimTime = 0.0;
  mProgressTimer = 0.0;
  mStgLaserWrapper = NULL;
  mTranslationalSpeedCmd = 0.0;
  mRotationalSpeedCmd = 0.0;
  mStallRecoverSpeedCmd = 0.0;
  mStallRecoverTurnRateCmd = 0.0;
  mWorkTaskStartedTimeStamp = 0.0;
  mTaskCompletionTime = 0.0;
  mReward = 0.0;
  mRewardRate = 0.0;
  mTravelDuration = 0.0;
  mPickupDuration = 0.0;
  mTotalEnergyDissipated = 0.0;
  mCountTasksCompleted = 0;
  mTaskId = 0;
  mDepotDestination = NULL;
  mBroadCast = NULL;
  mCurrentWorkTask = NULL;
  mPowerPack = NULL;
  mFgPoweredUp  = true;
  mFgPublishTaskInformation = true;
  mLogWriter = CLogWriter::getInstance();
  mVisServer = CVisServer::getInstance();

  mVisServer->registerRobot ( this );

  mLogWriter->registerVariable ( &mTotalReward, CLogWriter::FLOAT, "SumReward", "" );
  mLogWriter->registerVariable ( &mCountTasksCompleted, CLogWriter::INT, "Trips", "" );
  mLogWriter->registerVariable ( &mTaskId, CLogWriter::INT, "task", "" );
  mLogWriter->registerVariable ( &mTotalEnergyDissipated, CLogWriter::FLOAT, "dissipated", "" );
  mLogWriter->registerVariable ( &mBatteryEnergyRemaining, CLogWriter::FLOAT, "battery", "" );
  //************************************
  // FSM
  strncpy ( mFsmText[START], "start", 20 );
  strncpy ( mFsmText[PAUSE], "pause", 20 );
  strncpy ( mFsmText[GOTO_SINK], "sink", 20 );
  strncpy ( mFsmText[GOTO_SOURCE], "source", 20 );
  strncpy ( mFsmText[GOTO_CHARGER], "charger", 20 );
  strncpy ( mFsmText[DOCK], "dock", 20 );
  strncpy ( mFsmText[UNDOCK], "undock", 20 );
  strncpy ( mFsmText[CHARGE], "refuel", 20 );
  strncpy ( mFsmText[PICKUP_LOAD], "pickup", 20 );
  strncpy ( mFsmText[DELIVER_LOAD], "deliver", 20 );
  strncpy ( mFsmText[WAITING_AT_SOURCE], "waiting", 20 );
  strncpy ( mFsmText[WAITING_AT_SINK], "waiting", 20 );
  strncpy ( mFsmText[WAITING_AT_CHARGER], "waiting", 20 );
  strncpy ( mFsmText[RECOVER], "recover", 20 );
  strncpy ( mFsmText[GOTO_DEPOT], "depot", 20 );
  strncpy ( mFsmText[UNALLOCATED], "unallocated", 20 );

  // take care for the postion model
  mPosition = modPos;
  mPosition->AddUpdateCallback ( ( Stg::stg_model_callback_t ) positionUpdate,
                                 this );
  mPosition->Subscribe();
  mPowerPack = mPosition->FindPowerPack();

  // Add a laser ranger finder to the robot
  mLaser = ( Stg::ModelLaser* ) modPos->GetUnusedModelOfType ( Stg::MODEL_TYPE_LASER );
  mLaser->AddUpdateCallback ( ( Stg::stg_model_callback_t ) laserUpdate, this );
  mLaser->Subscribe();
  mStgLaserWrapper = new CStageLaser ( mLaser );
  if ( mStgLaserWrapper->init() != 1 ) {
    PRT_ERR0 ( "Failed to initialize stage laser wrapper " );
    return;
  }

  // Add a fiducial device to the robot
  mFiducial = ( Stg:: ModelFiducial* ) modPos->GetUnusedModelOfType ( Stg::MODEL_TYPE_FIDUCIAL );
  mFiducial->AddUpdateCallback ( ( Stg::stg_model_callback_t ) fiducialUpdate, this );
  mFiducial->Subscribe();

#ifdef GRIPPER
  // Add a gripper to the robot
  printf ( "gripper\n" );
  mGripper = ( Stg::ModelGripper* ) modPos->GetUnusedModelOfType ( MODEL_TYPE_GRIPPER );
  mGripper->Subscribe();
#endif

  // Add nearest distance obstacle avoidance to robot
  mNd = new CNd ( mPosition->Token() );
  mNd->addRangeFinder ( mStgLaserWrapper );

  mWaveFrontMap->addRangeFinder ( mStgLaserWrapper );
  mWaveFrontMap->setRobotPose ( &mRobotPose );

  // comes in usec but we need sec
  mSimInterval = ( float ) ( mPosition->GetSimInterval() ) / 1e6;
}
//-----------------------------------------------------------------------------
ABaseRobot::~ABaseRobot()
{
  if ( mWaveFrontMap )
    delete mWaveFrontMap;

  if ( mNd )
    delete mNd;

  if ( mStgLaserWrapper )
    delete mStgLaserWrapper;
}
//-----------------------------------------------------------------------------
void ABaseRobot::getRobotVisData ( int id, char* buffer, int &len )
{
  tDataMsg msg;

  if ( len < ( int ) sizeof ( tDataMsg ) ) {
    len = 0;
    return;
  }

  msg.id = id;
  strncpy ( msg.fsmText, mFsmText[mState], 20 );
  if ( mCurrentWorkTask )
    strncpy ( msg.taskName, mCurrentWorkTask->getName(), 20 );
  else
    strncpy ( msg.taskName, "no task", 20 );
  msg.batteryLevel = mBatteryLevel;
  msg.fgCharging = mFgCharging;
  msg.travelTime = mTravelDuration;
  msg.fgStalled = mPosition->Stalled();
  msg.fgPoweredUp = mFgPoweredUp;
  msg.cargoLoad = ( float ) mCargoLoad / ( float ) mCargoBayCapacity;
  msg.rewardRate = mRewardRate;
  msg.taskCompletionTime = mTaskCompletionTime;
  msg.reward = mReward;
  msg.totalReward = mTotalReward;
  msg.numTasksCompleted = mCountTasksCompleted;
  msg.translationalSpeedCmd = mTranslationalSpeedCmd;
  msg.rotationalSpeedCmd = mRotationalSpeedCmd;

  memcpy ( buffer, ( char* ) &msg, sizeof ( tDataMsg ) );
  len = sizeof ( tDataMsg );
}
//-----------------------------------------------------------------------------
void ABaseRobot::getRobotVisMapData ( tMapMsg* msg, char* buffer, int& size )
{
  int mapSize;
  int wayPointSize;
  //char* map;

  mapSize = mWaveFrontMap->getNumCellsX() * mWaveFrontMap->getNumCellsY() * sizeof ( char );

  if ( size > mapSize ) {
    msg->width = mWaveFrontMap->getNumCellsX();
    msg->height = mWaveFrontMap->getNumCellsY();

    printf ( "copy map %d\n", mapSize );
    //map = mWaveFrontMap->getCompleteGrayScaleMap();
    //memcpy ( buffer, map, mapSize );
    printf ( "done\n" );
  }
  else {
    PRT_WARN0 ( "Buffer not large enough for map" );
    mapSize = 0;
    msg->width = 0;
    msg->height = 0;
  }

  wayPointSize = mWaveFrontMap->mWayPointList.size() * sizeof ( char ) * 2;

  if ( size > mapSize + wayPointSize ) {
    msg->numWaypoints = mWaveFrontMap->mWayPointList.size();
  }
  else {
    PRT_WARN0 ( "Buffer not large enough for waypoint list" );
    wayPointSize = 0;
    msg->numWaypoints = 0;
  }

  size = mapSize + wayPointSize;

  printf ( "getRobotVisMapData \n" );
}
//-----------------------------------------------------------------------------
void ABaseRobot::setRobotVisCommandMsg ( tCommandMsg msg )
{
  if ( msg.fgPause )
    mState = PAUSE;

  if ( ( msg.fgPause == false ) && ( mState == PAUSE ) )
    mState = mPrevState;
}
//-----------------------------------------------------------------------------
void ABaseRobot::removeWorkTask ( IWorkTaskRobotInterface* task )
{
  std::vector<IWorkTaskRobotInterface*>::iterator it;

  for ( it = mWorkTaskVector.begin(); it != mWorkTaskVector.end(); it++ ) {
    if ( task == *it )
      it = mWorkTaskVector.erase ( it );
  }
}
//-----------------------------------------------------------------------------
void ABaseRobot::addWorkTask ( IWorkTaskRobotInterface* newTask )
{
  IWorkTaskRobotInterface* task;
  std::vector<IWorkTaskRobotInterface*>::iterator it;
  float rewardRate;

  mVisServer->registerTask ( newTask );

  rewardRate = newTask->getReward() / newTask->getSrcSinkDistance();

  if ( mWorkTaskVector.size() == 0 ) {
    mWorkTaskVector.push_back ( newTask );
    return;
  }

  for ( it = mWorkTaskVector.begin(); it != mWorkTaskVector.end(); it++ ) {
    task = *it;
    if ( rewardRate > task->getReward() / task->getSrcSinkDistance() )  {
      it = mWorkTaskVector.insert ( it, newTask );
      return;
    }
  }

  mWorkTaskVector.push_back ( newTask );
}
//-----------------------------------------------------------------------------
void ABaseRobot::addCharger ( CCharger* charger )
{
  mChargerDestinationList.push_back ( charger );
}
//-----------------------------------------------------------------------------
void ABaseRobot::setDepot ( CDestination* depot )
{
  mDepotDestination = depot;
}
//-----------------------------------------------------------------------------
void ABaseRobot::setBroadCast ( CBroadCast* broadCast )
{
  mBroadCast = broadCast;
}
//-----------------------------------------------------------------------------
void ABaseRobot::printTaskStatistics()
{
  IWorkTaskRobotInterface* task;
  std::vector<IWorkTaskRobotInterface*>::iterator it;

  for ( it = mWorkTaskVector.begin(); it != mWorkTaskVector.end(); it++ ) {
    task = ( IWorkTaskRobotInterface* ) * it;
    printf ( "Task %s: #robots %2d reward rate %f\n", task->getName(),
             task->getNumSubcribers(),
             task->getExperiencedRewardRate() );
  }
}
//-----------------------------------------------------------------------------
void ABaseRobot::rprintf ( const char* format, ... )
{
  va_list args;
  fprintf ( stdout, "\033[1;37;42m[%s]\033[0m: ", mPosition->Token() );
  va_start ( args, format );
  vfprintf ( stdout, format, args );
  va_end ( args );

}
//-----------------------------------------------------------------------------
char* ABaseRobot::getName()
{
  return ( char* ) mPosition->Token();
}
//-----------------------------------------------------------------------------
bool ABaseRobot::noProgress ( float timeout )
{
  if ( fmod ( mElapsedStateTime, 2.0 ) == 0 ) {
    mProgressPose = mRobotPose;
  }

  if ( mRobotPose.distance ( mProgressPose ) < 0.2 ) {
    if ( mProgressTimer > timeout )
      return true;  // no progress :-(
  }
  else {
    mProgressTimer = 0;
    mProgressPose = mRobotPose;
  }
  return false;  // nop, we are still making some progress
}
//-----------------------------------------------------------------------------
void ABaseRobot::transferWaypointToStage()
{
  CPose pose;
  int i = 0;
  std::list<CPose>::iterator it;
  Stg::Waypoint* oldList = NULL;
  Stg::Waypoint* wpList = new Stg::Waypoint[mWayPointList.size() + 1 ];

  // add current waypoint
  wpList[i].pose.x = mCurrentWayPoint.mX;
  wpList[i].pose.y = mCurrentWayPoint.mY;
  wpList[i].pose.a = mCurrentWayPoint.mYaw;
  wpList[i].color = Stg::stg_color_pack ( 0, 1, 0, 0 );
  i++;

  for ( it = mWayPointList.begin(); it != mWayPointList.end(); it++ ) {
    pose = *it;
    wpList[i].pose.x = pose.mX;
    wpList[i].pose.y = pose.mY;
    wpList[i].pose.a = pose.mYaw;
    wpList[i].color = Stg::stg_color_pack ( 1, 0, 0, 0 );
    i ++;
  }

  mPosition->Lock();
  oldList = mPosition->SetWaypoints ( wpList, mWayPointList.size() );

  if ( oldList != NULL )
    delete[] oldList;
  mPosition->Unlock();
}
//-----------------------------------------------------------------------------
void ABaseRobot::stop()
{
  mRotationalSpeedCmd = 0.0;
  mTranslationalSpeedCmd = 0.0;
}
//-----------------------------------------------------------------------------
void ABaseRobot::powerUp ( bool on )
{
  if ( mFgPoweredUp != on ) {

    if ( on ) {
      //mPosition->Subscribe();
      mLaser->Subscribe();
      mFiducial->Subscribe();
    }
    else {
      //mPosition->Unsubscribe();  // cannot unsubscribe from the position model,
      // or we won't get updated
      mLaser->Unsubscribe();
      mFiducial->Unsubscribe();
    }
    mFgPoweredUp = on;
  }
}
//-----------------------------------------------------------------------------
int ABaseRobot::positionUpdate ( Stg::ModelPosition* pos, ABaseRobot* robot )
{
  pos->Lock();

  Stg::Pose pose = pos->GetPose();

  // update position data
  robot->mRobotPose.mX = pose.x;
  robot->mRobotPose.mY = pose.y;
  //robot->mRobotPose.mYaw = NORMALIZE_ANGLE ( pose.a );
  robot->mRobotPose.mYaw =  pose.a;
  // update power information
  robot->mFgCharging = robot->mPowerPack->GetCharging();
  robot->mBatteryLevel = robot->mPowerPack->ProportionRemaining();
  robot->mBatteryEnergyRemaining = JOULE_TO_WATTHOURS (
                                     robot->mPowerPack->GetStored() );
  robot->mBatteryCapacity = JOULE_TO_WATTHOURS (
                              robot->mPowerPack->GetCapacity() );
  robot->mTotalEnergyDissipated = JOULE_TO_WATTHOURS (
                                    robot->mPowerPack->GetDissipated() );

  // update cargo bay data
  robot->mCargoLoad = pos->GetFlagCount();

  pos->Unlock();

  robot->updateCtrl();

  return 0; // everything is fine
}
//-----------------------------------------------------------------------------
int ABaseRobot::laserUpdate ( Stg::ModelLaser* laser, ABaseRobot* robot )
{
  laser->Lock();
  robot->mStgLaserWrapper->update();
  laser->Unlock();

  return 0; // everything is fine
}
//-----------------------------------------------------------------------------
int ABaseRobot::sonarUpdate ( Stg::ModelRanger* sonar, ABaseRobot* robot )
{
  return 0; // everything is fine
}
//-----------------------------------------------------------------------------
int ABaseRobot::fiducialUpdate ( Stg::ModelFiducial* fiducial, ABaseRobot* robot )
{
  float minRange = INFINITY;
  Stg::stg_fiducial_t* fiducialData;

  fiducial->Lock();

  robot->mFgChargerDetected = false;
  robot->mFgRobotDetected = false;

  for ( unsigned int i = 0; i < fiducial->fiducial_count; i++ ) {
    fiducialData = &fiducial->fiducials[i];

    // check if this fiducial is a charging station
    if ( ( fiducialData->id == 2 ) && ( fiducialData->range < minRange ) ) {
      minRange = fiducialData->range;
      // keep the information for later
      robot->mFgChargerDetected = true;
      robot->mLastChargingStation.bearing = fiducialData->bearing;
      robot->mLastChargingStation.distance = fiducialData->range;
      robot->mLastChargingStation.orientation = fiducialData->geom.a;
    }

    if ( fiducialData->id == 1 )
      robot->mFgRobotDetected = true;
  }
  fiducial->Unlock();

  return 0; // everything is fine
}
//-----------------------------------------------------------------------------
void ABaseRobot::replan ( tPoint2d goal, bool force )
{
  float localUpdate = false;
  float angleResolution;
  float halfConeAngle;
  int k;
  int n;

  // check if replanning is required
  if ( force == false ) {
    angleResolution = mStgLaserWrapper->getFov() / ( float ) mStgLaserWrapper->mNumSamples;

    halfConeAngle = sin ( ( REPLANNING_BOX_WIDTH / 2.0 ) / REPLANNING_BOX_LENGTH );

    k = ( int ) ( ( mStgLaserWrapper->mNumSamples / 2 ) - ( halfConeAngle / angleResolution ) );
    n = ( int ) ( 2 * ceil ( halfConeAngle / angleResolution ) );

    for ( int i = k; i < k + n; i++ ) {
      if ( mStgLaserWrapper->mRangeData[i].range < REPLANNING_BOX_LENGTH ) {
        localUpdate = true;
        break;
      }
    }
    if ( localUpdate ) {
      //mWaveFrontMap->calculateLocalWaveFront ( true );
      mWaveFrontMap->calculateWaveFront ( goal, true );
    }
    else {
      return; // no replanning required - we are out of here
    }
  }
  else {
    mWaveFrontMap->calculateWaveFront ( goal, true );
  }

  if ( mWaveFrontMap->calculatePlanFrom ( mRobotPose ) != -1 ) {
    mWayPointList.clear();
    mFgWayPointListChanged = true;
    mWaveFrontMap->getWayPointList ( mWayPointList );
    mWayPointList.pop_front();
  }
  else {
    rprintf ( "Failed to plan path \n" );
  }
}
//-----------------------------------------------------------------------------
void ABaseRobot::leaveCurrentWaitingQueue()
{
  if ( mWaitDestination != NULL ) {
    mWaitDestination->leaveWaitingQueue ( this );
    mWaitDestination = NULL;
  }
}
//-----------------------------------------------------------------------------
bool ABaseRobot::chargingPolicy()
{
  float energyRequired;
  float distFromSinkToCharger;
  float distFromSourceToSink;
  float distFromHereToSource;

  if ( mCurrentWorkTask == NULL )
    return false;  // we dont have a task, which means we should go to the depot

  mWaveFrontMap->calculateWaveFront ( mCurrentWorkTask->mSource->getLocation(), false );
  distFromHereToSource = mWaveFrontMap->calculatePlanFrom ( mRobotPose );

  mWaveFrontMap->calculateWaveFront ( mCurrentWorkTask->mSink->getLocation(), false );
  distFromSourceToSink = mWaveFrontMap->calculatePlanFrom (
                           mCurrentWorkTask->mSource->getLocation() );

  distFromSinkToCharger = mWaveFrontMap->calculatePlanFrom (
                            mCurrentWorkTask->mSource->getLocation() );


  energyRequired = ( distFromSinkToCharger + distFromSourceToSink + distFromHereToSource ) *
                   ENERGY_PER_METER;

  if ( mBatteryEnergyRemaining > energyRequired )
    return false; // charging not required

  return true;  // charging required
}
//-----------------------------------------------------------------------------
bool ABaseRobot::actionDock()
{
  float heading;

#ifdef HAVE_GRIPPER
  // close the grippers so they can be pushed into the charger
  ModelGripper::config_t gripper_data = mGripper->GetConfig();

  if ( gripper_data.paddles != ModelGripper::PADDLE_CLOSED )
    mGripper->CommandClose();
  else  if ( gripper_data.lift != ModelGripper::LIFT_UP )
    mGripper->CommandUp();
#endif

  if ( mFgChargerDetected == true ) {
    heading = NORMALIZE_ANGLE ( mLastChargingStation.bearing );

    if ( mLastChargingStation.distance > 0.6 ) {
      if ( actionAvoidObstacle() == 0 ) {
        mTranslationalSpeedCmd =  CRUISE_SPEED ;
        mRotationalSpeedCmd = heading;
      }
    }
    else {
      mRotationalSpeedCmd = heading;
      mTranslationalSpeedCmd = DOCK_SPEED; // creep towards it

      //if ( mLastChargingStation.distance < 0.1 ) { // close enough
      if ( mFgCharging == true ) {
        stop();
        return true;   // successfull dock
      }

      if ( mPosition->Stalled() ) // touching
        mTranslationalSpeedCmd = -0.01; // back off a bit
    }
  }
  else {
    // can't see the charger any more, better stop
    stop();
  }

  return false; // not docked yet
}
//-----------------------------------------------------------------------------
bool ABaseRobot::actionUnDock()
{
  Stg::ModelGripper::config_t gripperData;

  if ( mFgChargerDetected == false )
    return true;  // we can't see the charger any more, lets assume we are undocked

  // back up a bit
  if ( mLastChargingStation.distance < BACK_OFF_DISTANCE ) {
    mTranslationalSpeedCmd = BACK_OFF_SPEED;
    mHeading = NORMALIZE_ANGLE ( mLastChargingStation.bearing - PI );
  }
  else {
    mTranslationalSpeedCmd = 0.0;
    mRotationalSpeedCmd = 0.5;
  }
#ifdef HAVE_GRIPPER
  // once we have backed off a bit, open and lower the gripper
  gripperData = mGripper->GetConfig();

  if ( mLastChargingStation.distance > GRIPPER_DISTANCE ) {
    if ( gripperData.paddles != ModelGripper::PADDLE_OPEN )
      mGripper->CommandOpen();
    else if ( gripperData.lift != ModelGripper::LIFT_DOWN )
      mGripper->CommandDown();
  }

  // if the gripper is down and open and we're away from the charger,
  // undock is finished
  if ( gripperData.paddles == ModelGripper::PADDLE_OPEN &&
       gripperData.lift == ModelGripper::LIFT_DOWN &&
       mLastChargingStation.distance > BACK_OFF_DISTANCE )
    return true;  // done undocking everything good
#else
  if ( ( mLastChargingStation.distance > BACK_OFF_DISTANCE ) &&
       ( epsilonEqual ( mRobotPose.mYaw, mHeading, D2R ( 10.0 ) ) == true ) )
    return true;  // done undocking everything good

#endif
  return false; // undock procedure not completed yet
}
//---------------------------------------------------------------------------
int ABaseRobot::actionAvoidObstacle()
{
  bool obstruction = false;
  bool stop = false;
  Stg::stg_laser_sample_t* scan;

  // find the closest distance to the left and right and check if
  // there's anything in front
  float minleft = 1e6;
  float minright = 1e6;

  // Get the data
  uint32_t sampleCount = 0;
  scan = mLaser->GetSamples ( &sampleCount );

  for ( uint32_t i = 0; i < sampleCount; i++ ) {

    if ( ( i > ( sampleCount / 4 ) )
         && ( i < ( sampleCount - ( sampleCount / 4 ) ) )
         && scan[i].range < MIN_FRONT_DISTANCE ) {
      obstruction = true;
    }

    if ( scan[i].range < STOP_DISTANCE ) {
      stop = true;
    }

    if ( i > sampleCount / 2 )
      minleft = MIN ( minleft, scan[i].range );
    else
      minright = MIN ( minright, scan[i].range );
  }

  if ( obstruction || stop || ( mAvoidCount > 0 ) ) {

    mTranslationalSpeedCmd = stop ? 0.0 : AVOID_SPEED;

    /* once we start avoiding, select a turn direction and stick
       with it for a few iterations */
    if ( mAvoidCount < 1 ) {
      mAvoidCount = randNo ( AVOID_DURATION , 2 * AVOID_DURATION );

      if ( minleft < minright ) {
        mRotationalSpeedCmd = -AVOID_TURN;
      }
      else {
        mRotationalSpeedCmd = + AVOID_TURN;
      }
    }
    mAvoidCount--;

    return 1; // busy avoding obstacles
  }

  return 0; // didn't have to avoid anything
}
//-----------------------------------------------------------------------------
bool ABaseRobot::actionPickupLoad()
{
  if ( mWaitDestination == NULL )
    return true;

  if ( mWaitDestination->getDistance ( mRobotPose ) <= 0.3 ) {
    stop();

    // protect access to source
    mCurrentWorkTask->mStgSource->Lock();
    if ( mPosition->GetFlagCount() < mCargoBayCapacity ) {
      // load flag
      mPosition->PushFlag ( mCurrentWorkTask->mStgSource->PopFlag() );
    }
    // unlock source
    mCurrentWorkTask->mStgSource->Unlock();
  }
  else {
    mNd->setEpsilonDistance ( 0.3 );
    mNd->setEpsilonAngle ( D2R ( 180 ) );
    mNd->setGoal ( mWaitDestination->getPose() );
    mNd->update ( mRobotPose );
    mTranslationalSpeedCmd = mNd->getRecommendedTranslationalVelocity();
    mRotationalSpeedCmd = mNd->getRecommendedRotationalVelocity();
  }

  if ( mPosition->GetFlagCount() >= mCargoBayCapacity ) {
    return true; // success, we are fully loaded
  }

  return false; // nop still space for more cargo
}
//---------------------------------------------------------------------------
bool ABaseRobot::actionDeliverLoad()
{
  if ( mWaitDestination == NULL )
    return true;

  if ( mWaitDestination->getDistance ( mRobotPose ) <= 0.3 ) {
    stop();

    if ( mPosition->GetFlagCount() > 0 ) {
      // protect access to sink
      mCurrentWorkTask->mStgSink->Lock();
      // unload flag
      mCurrentWorkTask->mStgSink->PushFlag ( mPosition->PopFlag() );
      // unlock sink
      mCurrentWorkTask->mStgSink->Unlock();
    }
    else {
      return true;  // we were already empty exit here and do not collect
      // any reward
    }
  }
  else {
    mNd->setEpsilonDistance ( 0.3 );
    mNd->setEpsilonAngle ( D2R ( 180 ) );
    mNd->setGoal ( mWaitDestination->getPose() );
    mNd->update ( mRobotPose );
    mTranslationalSpeedCmd = mNd->getRecommendedTranslationalVelocity();
    mRotationalSpeedCmd = mNd->getRecommendedRotationalVelocity();
  }


  if ( mPosition->GetFlagCount() == 0 ) {
    mReward = mCurrentWorkTask->getReward();
    mTotalReward += mReward;
    mTaskCompletionTime = mSimTime - mWorkTaskStartedTimeStamp;
    mRewardRate = mReward / mTaskCompletionTime;
    mWorkTaskStartedTimeStamp = mSimTime;
    mCountTasksCompleted = mCountTasksCompleted + 1;
    // make experienced work rate available to others
    if ( mFgPublishTaskInformation == true ) {
      mCurrentWorkTask->setExperiencedRewardRate ( mRewardRate );
      mCurrentWorkTask->setExperiencedTaskDuration ( mTaskCompletionTime );
      mCurrentWorkTask->setExperiencedTravelDuration ( mTravelDuration );
    }

    mFgPublishTaskInformation = true;

    return true;  // success, we are empty
  }

  return false; // nop still stuff in the cargo bay;
}
//---------------------------------------------------------------------------
bool ABaseRobot::actionRecover()
{
  if ( mFgStateChanged ) {
    mHeading = randNo ( -PI, PI );
    mRecoveryTimeout = randNo ( 2.0, 10.0 );
  }
  if ( !actionAvoidObstacle() ) {
    mTranslationalSpeedCmd = 0.2;
    mRotationalSpeedCmd = SIGN ( NORMALIZE_ANGLE ( mRobotPose.mYaw - mHeading ) ) * 0.5;
  }

  if ( mElapsedStateTime > mRecoveryTimeout )
    return true;

  return false;
}
//---------------------------------------------------------------------------
bool ABaseRobot::actionFollowWayPointList ( )
{
  if ( mNd->atGoal() == true  || mFgWayPointListChanged == true ) {
    mFgWayPointListChanged = false;
    if ( mWayPointList.size() > 0 ) {
      mCurrentWayPoint = mWayPointList.front();
      mWayPointList.pop_front();

      mNd->setGoal ( mCurrentWayPoint );
      if ( mWayPointList.size() == 0 ) {
        mNd->setEpsilonAngle ( D2R ( 25 ) );
        mNd->setEpsilonDistance ( 0.5 );
      }
      else {
        mNd->setEpsilonAngle ( D2R ( 180 ) );
        mNd->setEpsilonDistance ( 0.5 );
      }
    }
    else {
      stop();
      return true; // way point list is empty, this must mean we are at the goal
    }
  }
  if ( mNd->hasActiveGoal() == false ) {
    mNd->setGoal ( mCurrentWayPoint );
  }

  if ( mNd->isStalled() == true ) {
    stop();
    if ( mTimer > ND_STALL_TIMEOUT ) {
      mTranslationalSpeedCmd = BACK_OFF_SPEED;
      mRotationalSpeedCmd = 0.0;

      if ( mTimer > ND_STALL_TIMEOUT + ( BACK_OFF_DISTANCE / fabs ( BACK_OFF_SPEED ) ) ) {
        mNd->setGoal ( mCurrentWayPoint );
        mTimer = 0;
      }
    }
    return false;  // we are still in line and waiting
  }

  mNd->update ( mRobotPose );
  if ( mNd->isTurningInPlace() == false ) {
    mTranslationalSpeedCmd = mNd->getRecommendedTranslationalVelocity();
    mRotationalSpeedCmd = mNd->getRecommendedRotationalVelocity();
  }
  else {
    // HACK: Nd seems to turn in the wrong direction when turning in place
    mTranslationalSpeedCmd = 0.0;
    mRotationalSpeedCmd = SIGN ( mRobotPose.angleDifference ( mCurrentWayPoint ) ) * -0.5;
  }

  return false; // not at goal yet
}
//---------------------------------------------------------------------------
bool ABaseRobot::actionWaitInQueue ( bool fgEnforceWaitingPosition )
{
  CPose pose;
  float angle;

  if ( mWaitDestination == NULL ) {
    rprintf ( "actionWaitInQueue has no waiting destination in state %s\n",
              mFsmText[mState] );
    return false;
  }

  mWayPointList.clear();
  mQueueRank = mWaitDestination->getWaitingPosition ( this,  pose );

  if ( ( mNd->isStalled() == true )  || ( pose != mCurrentWayPoint ) ) {
    mCurrentWayPoint = pose;
    mNd->setGoal ( pose );
    mNd->setEpsilonDistance ( 0.3 );
    mNd->setEpsilonAngle ( D2R ( 10 ) );
  }

  if ( mNd->atGoal() == false ) {
    mNd->update ( mRobotPose );
    mTranslationalSpeedCmd = mNd->getRecommendedTranslationalVelocity();
    mRotationalSpeedCmd = mNd->getRecommendedRotationalVelocity();
    // HACK: ND doesn't always like to turn in place, lets force it
    if ( mNd->isTurningInPlace() == true ) {
      angle = mRobotPose.angleDifference ( pose );
      mRotationalSpeedCmd = SIGN ( angle ) * -0.3;
    }
  }
  else {
    // make sure we have the correct heading
    angle = mRobotPose.angleDifference ( pose );
    mTranslationalSpeedCmd = 0.0;
    if ( fabs ( angle ) > D2R ( 10 ) )
      mRotationalSpeedCmd = SIGN ( angle ) * -0.5;
    else
      mRotationalSpeedCmd = 0.0;
  }

  if ( mQueueRank == 1 ) {
    if ( mNd->atGoal() || fgEnforceWaitingPosition == false )
      return true;  // we made it to the front of the queue
  }
  return false; // still waiting in line
}
//---------------------------------------------------------------------------
void ABaseRobot::updateCtrl()
{
  char statusStr[20];
  float r;

  snprintf ( statusStr, 20, "%s", mFsmText[mState] );
  mWaveFrontMap->update ( mSimTime );
  // remove ourselfs from the current task counter
  if ( mCurrentWorkTask )
    mCurrentWorkTask->unsubscribe ( this );

  // assume we are not working, till proven otherwise
  mFgWorking = false;

  switch ( mState ) {

    case START:
      startupPolicy();
      break;

    case PAUSE:
      mTranslationalSpeedCmd = 0.0;
      mRotationalSpeedCmd = 0.0;
      break;

    case WAITING_AT_SOURCE:
      mFgWorking = true;
      snprintf ( statusStr, 20, "%s (%d)", mFsmText[mState], mQueueRank );
      if ( mWaitDestination == NULL ) {
        mState = GOTO_SOURCE;
      }
      else if ( actionWaitInQueue() == true ) {
        mState = PICKUP_LOAD;
      }
      waitAtSourcePolicy();
      break;

    case WAITING_AT_SINK:
      mFgWorking = true;
      snprintf ( statusStr, 20, "%s (%d)", mFsmText[mState], mQueueRank );
      if ( mWaitDestination == NULL ) {
        mState = GOTO_SINK;
      }
      else if ( actionWaitInQueue() == true ) {
        mState = DELIVER_LOAD;
      }
      break;

    case WAITING_AT_CHARGER:
      snprintf ( statusStr, 20, "%s (%d)", mFsmText[mState], mQueueRank );

      if ( mWaitDestination == NULL ) {
        mState = GOTO_CHARGER;
      }
      else if ( actionWaitInQueue ( false ) == true )  {
        if ( ( mFgRobotDetected == true ) && ( mFgChargerDetected == false ) )
          mState = WAITING_AT_CHARGER;
        if ( mFgChargerDetected == true ) {
          mState = DOCK;
        }
        if ( ( mFgRobotDetected == false ) && ( mFgChargerDetected == false ) )
          mState = GOTO_CHARGER;
      }
      waitAtChargerPolicy();
      break;

    case DELIVER_LOAD:
      mFgWorking = true;
      if ( actionDeliverLoad() == true ) {
        mState = GOTO_SOURCE;
        selectWorkTaskPolicy();
        //printTaskStatistics();
        mTravelStartTimeStamp = mSimTime;
        if ( chargingPolicy() == true ) {
          mState = GOTO_CHARGER;
        }
      }
      break;

    case PICKUP_LOAD:
      mPickupDuration = mPickupDuration + mSimInterval;
      mFgWorking = true;
      if ( mCurrentWorkTask->mSource->getDistance ( mRobotPose ) > LOADINGZONE_DISTANCE_LIMIT ) {
        mPickupDuration = 0.0;
        if ( mElapsedStateTime > LOADINGZONE_APPROACH_TIMEOUT )
          mState = GOTO_SOURCE;
      }

      if ( actionPickupLoad() == true ) {
        mState = GOTO_SINK;
        mTravelStartTimeStamp = mSimTime;
        pickupCompletedPolicy();
      }
      else {
        waitingAtPickupPolicy();
      }
      break;

    case GOTO_SOURCE:
      mFgWorking = true;
      if ( mFgStateChanged ) {
        replan ( mCurrentWorkTask->mSource->getLocation(), true );
      }
      replan ( mCurrentWorkTask->mSource->getLocation() );
      if ( mCurrentWorkTask->mSink->getDistance ( mRobotPose ) > 0.5 ) {
        leaveCurrentWaitingQueue();
      }
      if ( mCurrentWorkTask->mSource->isNear ( mRobotPose ) == true ) {
        mCurrentWorkTask->mSource->enterWaitingQueue ( this );
        mWaitDestination = mCurrentWorkTask->mSource;
        mState = WAITING_AT_SOURCE;
        mTravelDuration = mSimTime - mTravelStartTimeStamp;
      }

      if ( noProgress ( NO_PROGRESS_TIMEOUT ) == true ) {
        mState = RECOVER;
      }
      if ( actionFollowWayPointList() == true ) {
        mState = PICKUP_LOAD;
      }
      break;

    case GOTO_SINK:
      mFgWorking = true;
      if ( mFgStateChanged ) {
        replan ( mCurrentWorkTask->mSink->getLocation(), true );
      }
      replan ( mCurrentWorkTask->mSink->getLocation() );
      if ( mCurrentWorkTask->mSource->getDistance ( mRobotPose ) > 0.5 ) {
        leaveCurrentWaitingQueue();
      }

      if ( noProgress ( NO_PROGRESS_TIMEOUT ) == true ) {
        mState = RECOVER;
      }

      if ( mCurrentWorkTask->mSink->isNear ( mRobotPose ) == true ) {
        mCurrentWorkTask->mSink->enterWaitingQueue ( this );
        mWaitDestination = mCurrentWorkTask->mSink;
        mTravelDuration = mSimTime - mTravelStartTimeStamp;
        mState = WAITING_AT_SINK;
      }

      if ( actionFollowWayPointList() == true ) {
        mState = DELIVER_LOAD;
      }
      break;


    case GOTO_CHARGER:
      if ( mFgStateChanged ) {
        replan ( mCurrentChargerDestination->getLocation(), true );
        leaveCurrentWaitingQueue();
      }
      actionFollowWayPointList();
      if ( noProgress ( NO_PROGRESS_TIMEOUT ) == true ) {
        mState = RECOVER;
      }

      if ( mCurrentChargerDestination->isNear ( mRobotPose ) == true ) {
        mCurrentChargerDestination->enterWaitingQueue ( this );
        mWaitDestination = mCurrentChargerDestination;
        mState = WAITING_AT_CHARGER;
      }
      break;

    case GOTO_DEPOT:
      if ( mFgStateChanged ) {
        replan ( mDepotDestination->getLocation(), true );
        leaveCurrentWaitingQueue();
      }
      actionFollowWayPointList();
      if ( noProgress ( NO_PROGRESS_TIMEOUT ) == true ) {
        mState = RECOVER;
      }

      if ( mDepotDestination->isNear ( mRobotPose ) == true ) {
        mDepotDestination->enterWaitingQueue ( this );
        mWaitDestination = mDepotDestination;
        mState = UNALLOCATED;
      }
      break;

    case CHARGE:
      stop();
      leaveCurrentWaitingQueue();
      powerUp ( false );
      if ( ( mBatteryLevel > 0.99 ) || ( mFgCharging == false ) ) {
        mState = UNDOCK;
        powerUp ( true );
      }
      break;

    case DOCK:
      if ( actionDock() == true ) {
        mState = CHARGE;
      }
      if ( mFgChargerDetected == false ) {
        mState = GOTO_CHARGER;
      }
      break;

    case UNDOCK:
      if ( actionUnDock() == true ) {
        mFgPublishTaskInformation = false;
        leaveCurrentWaitingQueue();
        leaveChargerPolicy();
      }
      break;

    case RECOVER:
      leaveCurrentWaitingQueue();
      if ( actionRecover() == true ) {
        mProgressPose = mRobotPose;
        if ( mPrevState != RECOVER )
          mState = mPrevState;
        else
          mState = START;
      }
      break;

    case UNALLOCATED:
      snprintf ( statusStr, 20, "%s (%d)", mFsmText[mState], mQueueRank );

      if ( mWaitDestination == NULL ) {
        powerUp ( true );
        mState = GOTO_DEPOT;
      }
      else {
        if ( mNd->atGoal() )
          powerUp ( false );
        else
          powerUp ( true );

        if ( actionWaitInQueue ( false ) == true ) {
          unallocatedPolicy();
          if ( mState != UNALLOCATED )
            powerUp ( true );
        }
      }
      break;

    default:
      printf ( "CBaseRobot::updateCtrl(): unknown FSM state %d \n", mState );
      mState = START;
      break;
  } // switch


  transferWaypointToStage();
  //*****************************************
  // Emergency routines
  if ( mPosition->Stalled() ) {
    leaveCurrentWaitingQueue();
    if ( epsilonEqual ( fmodCorrect ( mSimTime, 3.0f ), 0.0f, 0.001f ) ) {
      r = drand48();
      if ( r <= 0.33 )
        mStallRecoverSpeedCmd = 0.0;
      else if ( r <= 0.66 )
        mStallRecoverSpeedCmd = STALL_RECOVER_SPEED;
      else
        mStallRecoverSpeedCmd = -STALL_RECOVER_SPEED;

      r = drand48();
      if ( r <= 0.33 )
        mStallRecoverTurnRateCmd = 0.0;
      else if ( r <= 0.66 )
        mStallRecoverTurnRateCmd = STALL_RECOVER_TURNRATE;
      else
        mStallRecoverTurnRateCmd = -STALL_RECOVER_TURNRATE;
    }
    mTranslationalSpeedCmd = mStallRecoverSpeedCmd;
    mRotationalSpeedCmd = mStallRecoverTurnRateCmd;
    snprintf ( statusStr, 20, "%0.1f %0.1f", mTranslationalSpeedCmd,
               mRotationalSpeedCmd );
  }

  // add ourselfs to the current task again
  if ( ( mFgWorking ) && ( mCurrentWorkTask ) )
    mCurrentWorkTask->subscribe ( this );

  //*****************************************
  // FSM stuff
  if ( mPrevTimestepState != mState ) {
    mElapsedStateTime = 0;
    mProgressTimer = 0;
    mFgStateChanged = true;
    mPrevState = mPrevTimestepState;
    //rprintf("state %s -> %s \n", mFsmText[mPrevState], mFsmText[mState]);
  }
  else {
    mFgStateChanged = false;
  }

  if ( mFgWorking && mCurrentWorkTask != NULL )
    mTaskId = mCurrentWorkTask->getTaskId();
  else if ( ( mState == GOTO_CHARGER ) || ( mState == CHARGE ) )
    mTaskId = -1;
  else if ( ( mState == GOTO_DEPOT ) || ( mState == UNALLOCATED ) )
    mTaskId = -2;

  mPosition->SetXSpeed ( mTranslationalSpeedCmd );
  mPosition->SetTurnSpeed ( mRotationalSpeedCmd );
  mPosition->Say ( statusStr );
  mSimTime = mSimTime + mSimInterval;
  mTimer = mTimer + mSimInterval;
  mProgressTimer =  mProgressTimer + mSimInterval;
  mElapsedStateTime = mElapsedStateTime + mSimInterval;
  mPrevTimestepState = mState;

  mLogWriter->print ( mSimTime, 1.0 );
}
//---------------------------------------------------------------------------
