

#include <stdio.h>
#include <stdlib.h>
#include "baserobotctrl.h"
#include <math.h>



/** Static variable to count the number of robots using this controller */
static int robotNumber = 0;

//-----------------------------------------------------------------------------
ABaseRobotCtrl::ABaseRobotCtrl( ARobot* robot )
    : ARobotCtrl( robot ), IRobotWaitInterface()
{
  CLooseStageRobot* looseRobot;
  Stg::Model* stgModel;
  Stg::Model* bg;


  //***********************************
  // User parameters
  mCargoBayCapacity = 1;

  //***********************************
  // Member variables, not subject to user configuration
  mQueueStallRecoverTimer = new CTimer( mRobot );
  robotNumber = robotNumber + 1;
  mRobotId = robotNumber;
  mFgChargerDetected = false;
  mWaitingQueueTimer = 0.0;
  mLastStalledTimestamp = 0.0;
  mAvoidCount = 0;
  mState = START;
  mPrevState = START;
  mPrevTimestepState = START;
  mTotalReward = 0.0;
  mFgStateChanged = true;
  mWayPointList.clear();
  mTimer = 0.0;
  mSimTime = 0.0;
  mProgressTimer = 0.0;
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
  mWaitDestination = NULL;
  mPowerPack = NULL;
  mNd = NULL;
  mFgPoweredUp = true;
  mFgPublishTaskInformation = true;
  mLogWriter = CLogWriter::getInstance();

  //******************************************
  // Log writing
  mLogWriter->registerVariable( &mTotalReward, CLogWriter::FLOAT,
                                "SumReward", "" );
  mLogWriter->registerVariable( &mCountTasksCompleted, CLogWriter::INT,
                                "Trips", "" );
  mLogWriter->registerVariable( &mTaskId, CLogWriter::INT, "task", "" );
  mLogWriter->registerVariable( &mTotalEnergyDissipated, CLogWriter::FLOAT,
                                "dissipated", "" );
  //mLogWriter->registerVariable ( &mBatteryEnergyRemaining, CLogWriter::FLOAT,
  //                              "battery", "" );
  //************************************
  // FSM
  strncpy( mFsmText[START], "start", 20 );
  strncpy( mFsmText[PAUSE], "pause", 20 );
  strncpy( mFsmText[GOTO_SINK], "sink", 20 );
  strncpy( mFsmText[GOTO_SOURCE], "source", 20 );
  strncpy( mFsmText[GOTO_CHARGER], "charger", 20 );
  strncpy( mFsmText[DOCK], "dock", 20 );
  strncpy( mFsmText[UNDOCK], "undock", 20 );
  strncpy( mFsmText[CHARGE], "refuel", 20 );
  strncpy( mFsmText[PICKUP_LOAD], "pickup", 20 );
  strncpy( mFsmText[DELIVER_LOAD], "deliver", 20 );
  strncpy( mFsmText[WAITING_AT_SOURCE], "waiting", 20 );
  strncpy( mFsmText[WAITING_AT_SINK], "waiting", 20 );
  strncpy( mFsmText[WAITING_AT_CHARGER], "waiting", 20 );
  strncpy( mFsmText[RECOVER], "recover", 20 );
  strncpy( mFsmText[GOTO_DEPOT], "depot", 20 );
  strncpy( mFsmText[UNALLOCATED], "unallocated", 20 );

  looseRobot = ( CLooseStageRobot* ) mRobot;
  looseRobot->findDevice( mFiducial, "model:0.fiducial:0" );
  looseRobot->findDevice( mLaser, "laser:0" );
  looseRobot->findDevice( mPowerPack, "powerpack:0" );
  looseRobot->findDevice( mTextDisplay, "textdisplay:0" );
  looseRobot->findDevice( mDrivetrain, "position:0" );
  if ( rapiError->hasError() ) {
    rapiError->print();
    exit( -1 );
  }

  // get robot pose
  mRobotPose = mDrivetrain->getOdometry()->getPose();

  // Add nearest distance obstacle avoidance to robot
  mNd = new CNd( 0.4, 0.3, 0.2, mRobot->getName().c_str() );
  mNd->addRangeFinder( mLaser );

  //*************************************
  // Create Wavefront planner
  stgModel = mDrivetrain->getStageModel();
  assert( stgModel );
  mWaveFrontMap = new CWaveFrontMap( new CStageGridMap( stgModel ),
                                     mDrivetrain->getName().c_str() );
  assert( mWaveFrontMap );
  mWaveFrontMap->addRangeFinder( mLaser );
  mWaveFrontMap->setRobotPose( &mRobotPose );
  mWaveFrontMap->setWaypointDistance( 1.0, 5.0 );
  mWaveFrontMap->setObstacleGrowth( 0.4 );
  // place a pointer to the map in the model where others (e.g. the MapVis object)
  // can find it
  stgModel->SetProperty( "wavefront_map", static_cast<void*>( mWaveFrontMap ) );

  // Add the Wavefront map visualizer, disabled by default
  bg = stgModel->GetWorld()->GetModel( "background" );
  assert( bg );
  bg->AddVisualizer( &stgMapVis, false );

  mRobot->mVariableMonitor.addVar( &mFgChargerDetected, "mFgChargerDetected" );
  mRobot->mVariableMonitor.addVar( &mWaitingQueueTimer, "mWaitingQueueTimer" );
}
//-----------------------------------------------------------------------------
ABaseRobotCtrl::~ABaseRobotCtrl()
{
  if ( mWaveFrontMap )
    delete mWaveFrontMap;

  if ( mNd )
    delete mNd;
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::removeWorkTask( IWorkTaskRobotInterface* task )
{
  std::vector<IWorkTaskRobotInterface*>::iterator it;

  for ( it = mWorkTaskVector.begin(); it != mWorkTaskVector.end(); it++ ) {
    if ( task == *it )
      it = mWorkTaskVector.erase( it );
  }
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::addWorkTask( IWorkTaskRobotInterface* newTask )
{
  IWorkTaskRobotInterface* task;
  std::vector<IWorkTaskRobotInterface*>::iterator it;
  float rewardRate;

  rewardRate = newTask->getReward() / newTask->getSrcSinkDistance();

  if ( mWorkTaskVector.size() == 0 ) {
    mWorkTaskVector.push_back( newTask );
    return;
  }

  for ( it = mWorkTaskVector.begin(); it != mWorkTaskVector.end(); it++ ) {
    task = *it;
    if ( rewardRate > task->getReward() / task->getSrcSinkDistance() ) {
      it = mWorkTaskVector.insert( it, newTask );
      return;
    }
  }

  mWorkTaskVector.push_back( newTask );
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::addCharger( CCharger* charger )
{
  mChargerDestinationList.push_back( charger );
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::setDepot( CDestination* depot )
{
  mDepotDestination = depot;
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::setBroadCast( CBroadCast* broadCast )
{
  mBroadCast = broadCast;
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::printTaskStatistics()
{
  IWorkTaskRobotInterface* task;
  std::vector<IWorkTaskRobotInterface*>::iterator it;

  for ( it = mWorkTaskVector.begin(); it != mWorkTaskVector.end(); it++ ) {
    task = ( IWorkTaskRobotInterface* ) * it;
    printf( "Task %s: #robots %2d reward rate %f\n", task->getName(),
            task->getNumSubcribers(),
            task->getExperiencedRewardRate() );
  }
}
//-----------------------------------------------------------------------------
char* ABaseRobotCtrl::getName()
{
  return ( char* ) mDrivetrain->getName().c_str();
}
//-----------------------------------------------------------------------------
bool ABaseRobotCtrl::noProgress( float timeout )
{
  if ( isModAboutZero( mElapsedStateTime, 2.0 ) ) {
    mProgressPose = mRobotPose;
  }

  if ( mRobotPose.distance( mProgressPose ) < 0.2 ) {
    if ( mProgressTimer > timeout )
      return true;  // no progress :-(
  }
  else {
    mProgressTimer = 0.0;
    mProgressPose = mRobotPose;
  }
  return false;  // nop, we are still making some progress
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::transferWaypointToStage()
{
  CRgbColor color;
  CPose2d pose;
  int i = 0;
  std::list<CWaypoint2d>::iterator it;
  Stg::Waypoint* oldList = NULL;
  Stg::Waypoint* wpList = new Stg::Waypoint[mWayPointList.size() + 1 ];

  // add current waypoint
  pose =  mCurrentWaypoint.getPose();
  wpList[i].pose.x = pose.mX;
  wpList[i].pose.y = pose.mY;
  wpList[i].pose.a = pose.mYaw;
  wpList[i].color = Stg::stg_color_pack( 0, 1, 0, 0 );
  i++;

  for ( it = mWayPointList.begin(); it != mWayPointList.end(); it++ ) {
    pose = ( *it ).getPose();
    color = ( *it ).getColor();
    wpList[i].pose.x = pose.mX;
    wpList[i].pose.y = pose.mY;
    wpList[i].pose.a = pose.mYaw;

    wpList[i].color = Stg::stg_color_pack( color.mRed/255.0, color.mBlue/255.0,
                                           color.mGreen/255.0, 0 );
    i ++;
  }

  oldList = mDrivetrain->getStageModel()->SetWaypoints(
              wpList, i );

  if ( oldList != NULL )
    delete[] oldList;
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::powerUp( bool on )
{
  if ( mFgPoweredUp != on ) {

    if ( on ) {
      //mDrivetrain->setEnabled ( true );
      mLaser->setEnabled( true );
      mFiducial->setEnabled( true );
    }
    else {
      //mDrivetrain->setEnabled ( false );();  // cannot disable the position
      //  model, or we won't get updated
      mLaser->setEnabled( false );
      mFiducial->setEnabled( false );
    }
    mFgPoweredUp = on;
  }
}
//-----------------------------------------------------------------------------
int ABaseRobotCtrl::fiducialUpdate()
{
  float minRange = INFINITY;
  tFiducialData fiducialData;

  mFgChargerDetected = false;
  mFgRobotDetected = false;

  for ( unsigned int i = 0; i < mFiducial->getNumReadings(); i++ ) {
    fiducialData = mFiducial->mFiducialData[i];

    // check if this fiducial is a charging station
    if (( fiducialData.id == 2 ) && ( fiducialData.range < minRange ) ) {
      minRange = fiducialData.range;
      // keep the information for later
      mFgChargerDetected = true;
      mLastChargingStation.bearing = fiducialData.bearing;
      mLastChargingStation.distance = fiducialData.range;
      //mLastChargingStation.orientation = fiducialData->geom.a;
    }

    if ( fiducialData.id == 1 )
      mFgRobotDetected = true;
  }

  return 1; // everything is fine
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::replan( const CPoint2d goal, const bool force )
{
  float localUpdate = false;
  float angleResolution;
  float halfConeAngle;
  int k;
  int n;

  // check if replanning is required
  if ( force == false ) {
    angleResolution = mLaser->getFov() / ( float ) mLaser->getNumSamples();

    halfConeAngle = sin(( REPLANNING_BOX_WIDTH / 2.0 ) / REPLANNING_BOX_LENGTH );

    k = ( int )(( mLaser->getNumSamples() / 2 ) - ( halfConeAngle / angleResolution ) );
    n = ( int )( 2 * ceil( halfConeAngle / angleResolution ) );

    for ( int i = k; i < k + n; i++ ) {
      if ( mLaser->mRangeData[i].range < REPLANNING_BOX_LENGTH ) {
        localUpdate = true;
        break;
      }
    }
    if ( localUpdate ) {
      //mWaveFrontMap->calculateLocalWaveFront ( true );
      mWaveFrontMap->calculateWaveFront( goal, CWaveFrontMap::USE_SENSOR_DATA );
    }
    else {
      return; // no replanning required - we are out of here
    }
  }
  else {
    mWaveFrontMap->calculateWaveFront( goal, CWaveFrontMap::USE_SENSOR_DATA );
  }

  if ( mWaveFrontMap->calculatePlanFrom( mRobotPose ) != -1 ) {
    mWayPointList.clear();
    mFgWayPointListChanged = true;
    mWaveFrontMap->getWaypointList( mWayPointList );
    mWayPointList.pop_front();
  }
  else {
    //rprintf ( "Failed to plan path \n" );
  }
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::leaveCurrentWaitingQueue()
{
  if ( mWaitDestination != NULL ) {
    mWaitDestination->leaveWaitingQueue( this );
    mWaitDestination = NULL;
  }
}
//-----------------------------------------------------------------------------
bool ABaseRobotCtrl::chargingPolicy( float dt )
{
  float energyRequired;
  float distFromSinkToCharger;
  float distFromSourceToSink;
  float distFromHereToSource;

  if ( mCurrentWorkTask == NULL )
    return false;  // we dont have a task, which means we should go to the depot

  mWaveFrontMap->calculateWaveFront( mCurrentWorkTask->mSource->getLocation(),
                                     CWaveFrontMap::USE_MAP_ONLY );
  distFromHereToSource = mWaveFrontMap->calculatePlanFrom( mRobotPose );

  mWaveFrontMap->calculateWaveFront( mCurrentWorkTask->mSink->getLocation(),
                                     CWaveFrontMap::USE_MAP_ONLY );
  distFromSourceToSink = mWaveFrontMap->calculatePlanFrom(
                           mCurrentWorkTask->mSource->getLocation() );

  distFromSinkToCharger = mWaveFrontMap->calculatePlanFrom(
                            mCurrentWorkTask->mSource->getLocation() );


  energyRequired = ( distFromSinkToCharger + distFromSourceToSink +
                     distFromHereToSource ) * ENERGY_PER_METER;

  if ( mPowerPack->getBatteryCapacity() > energyRequired )
    return false; // charging not required

  return true;  // charging required
}
//-----------------------------------------------------------------------------
bool ABaseRobotCtrl::actionDock()
{
  CPoint2d point;

  if ( mFgChargerDetected == true ) {
    if ( mLastChargingStation.distance > 0.6 ) {
      point.fromPolar( mLastChargingStation.distance,
                       NORMALIZE_ANGLE( mLastChargingStation.bearing + mRobotPose.mYaw ) );
      mCurrentWaypoint = mRobotPose + point;
      mNd->setGoal( mCurrentWaypoint.getPose() );
      mNd->update( mSimTime, mRobotPose, mRobotVelocity );
      mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );
    }
    else {
      // creep towards it
      mDrivetrain->setVelocityCmd( DOCK_SPEED,
                                   NORMALIZE_ANGLE( mLastChargingStation.bearing ) );
      if ( mPowerPack->isCharging() == true ) {
        mDrivetrain->stop();
        return true;   // successfull dock
      }

      if ( mDrivetrain->isStalled() ) // touching
        mDrivetrain->setVelocityCmd( -0.01, 0.0 );  // back off a bit
    }
  }
  else {
    // can't see the charger any more, better stop, and let the FSM take
    // care of it
    mDrivetrain->stop();
  }

  return false; // not docked yet
}
//-----------------------------------------------------------------------------
bool ABaseRobotCtrl::actionUnDock()
{
  Stg::ModelGripper::config_t gripperData;

  if ( mFgChargerDetected == false )
    return true;  // we can't see the charger any more, lets assume we are undocked

  // back up a bit
  if ( mLastChargingStation.distance < BACK_OFF_DISTANCE ) {
    mDrivetrain->setVelocityCmd( BACK_OFF_SPEED, 0.0 );
    mHeading = NORMALIZE_ANGLE( mLastChargingStation.bearing - PI );
  }
  else {
    mDrivetrain->setVelocityCmd( 0.0, 0.5 );
  }

  if (( mLastChargingStation.distance > BACK_OFF_DISTANCE ) &&
      ( epsilonEqual( mRobotPose.mYaw, mHeading, D2R( 10.0 ) ) == true ) )
    return true;  // done undocking everything good

  return false; // undock procedure not completed yet
}
//---------------------------------------------------------------------------
int ABaseRobotCtrl::actionAvoidObstacle()
{
  bool obstruction = false;
  bool stop = false;

  // find the closest distance to the left and right and check if
  // there's anything in front
  float minleft = 1e6;
  float minright = 1e6;


  for ( uint32_t i = 0; i <  mLaser->getNumSamples(); i++ ) {

    if (( mLaser->mRelativeBeamPose[i].mYaw > D2R( -45.0 ) ) &&
        ( mLaser->mRelativeBeamPose[i].mYaw < D2R( 45.0 ) ) &&
        ( mLaser->mRangeData[i].range < MIN_FRONT_DISTANCE ) ) {
      obstruction = true;
    }

    if (( mLaser->mRelativeBeamPose[i].mYaw > D2R( -60.0 ) ) &&
        ( mLaser->mRelativeBeamPose[i].mYaw < D2R( 60.0 ) ) ) {
      if ( mLaser->mRangeData[i].range < STOP_DISTANCE ) {
        stop = true;
      }
    }

    if (( mLaser->mRelativeBeamPose[i].mYaw > D2R( -90.0 ) ) &&
        ( mLaser->mRelativeBeamPose[i].mYaw < D2R( 90.0 ) ) ) {

      if ( mLaser->mRangeData[i].range < STOP_DISTANCE ) {
        stop = true;
      }
      if ( i > mLaser->getNumSamples() / 2 )
        minleft = MIN( minleft, mLaser->mRangeData[i].range );
      else
        minright = MIN( minright, mLaser->mRangeData[i].range );
    }
  }

  if ( obstruction || stop || ( mAvoidCount > 0 ) ) {

    mDrivetrain->setVelocityCmd( stop ? 0.0 : AVOID_SPEED, 0.0 );

    /* once we start avoiding, select a turn direction and stick
       with it for a few iterations */
    if ( mAvoidCount < 1 ) {
      mAvoidCount = ( int ) randNo( AVOID_DURATION , 2 * AVOID_DURATION );

      if ( minleft < minright ) {
        mDrivetrain->setRotationalSpeedCmd( -AVOID_TURN );
      }
      else {
        mDrivetrain->setRotationalSpeedCmd( AVOID_TURN );
      }
    }
    mAvoidCount--;

    return 1; // busy avoding obstacles
  }

  return 0; // didn't have to avoid anything
}
//-----------------------------------------------------------------------------
bool ABaseRobotCtrl::actionPickupLoad()
{
  if ( mCurrentWorkTask->mSource->getDistance( mRobotPose ) <=
       LOADING_BAY_DISTANCE ) {
    mDrivetrain->stop();

    // protect access to source
    mCurrentWorkTask->mStgSource->Lock();
    if ( mDrivetrain->getStageModel()->GetFlagCount() < mCargoBayCapacity ) {
      // load flag
      mDrivetrain->getStageModel()->PushFlag( mCurrentWorkTask->mStgSource->PopFlag() );
    }
    // unlock source
    mCurrentWorkTask->mStgSource->Unlock();
  }
  else {
    mNd->setEpsilonDistance( 0.3 );
    mNd->setEpsilonAngle( D2R( 180.0 ) );
    mNd->setGoal( mCurrentWorkTask->mSource->getPose() );
    mNd->update( mSimTime, mRobotPose, mRobotVelocity );
    mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );
  }

  if ( mDrivetrain->getStageModel()->GetFlagCount() >= mCargoBayCapacity ) {
    return true; // success, we are fully loaded
  }

  return false; // nop still space for more cargo
}
//---------------------------------------------------------------------------
bool ABaseRobotCtrl::actionDeliverLoad()
{
  if ( mCurrentWorkTask->mSink->getDistance( mRobotPose ) <=
       LOADING_BAY_DISTANCE ) {
    mDrivetrain->stop();

    if ( mDrivetrain->getStageModel()->GetFlagCount() > 0 ) {
      // protect access to sink
      mCurrentWorkTask->mStgSink->Lock();
      // unload flag
      mCurrentWorkTask->mStgSink->PushFlag( mDrivetrain->getStageModel()->PopFlag() );
      // unlock sink
      mCurrentWorkTask->mStgSink->Unlock();
    }
    else {
      return true;  // we were already empty exit here and do not collect
      // any reward
    }
  }
  else {
    mNd->setEpsilonDistance( 0.3 );
    mNd->setEpsilonAngle( D2R( 180.0 ) );
    mNd->setGoal( mCurrentWorkTask->mSink->getPose() );
    mNd->update( mSimTime, mRobotPose, mRobotVelocity );
    mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );
  }

  if ( mDrivetrain->getStageModel()->GetFlagCount() == 0 ) {
    mReward = mCurrentWorkTask->getReward();
    mTotalReward += mReward;
    mTaskCompletionTime = mSimTime - mWorkTaskStartedTimeStamp;
    mRewardRate = mReward / mTaskCompletionTime;
    mWorkTaskStartedTimeStamp = mSimTime;
    mCountTasksCompleted = mCountTasksCompleted + 1;
    // make experienced work rate available to others
    if ( mFgPublishTaskInformation == true ) {
      mCurrentWorkTask->setExperiencedRewardRate( mRewardRate );
      mCurrentWorkTask->setExperiencedTaskDuration( mTaskCompletionTime );
      mCurrentWorkTask->setExperiencedTravelDuration( mTravelDuration );
    }

    mFgPublishTaskInformation = true;

    return true;  // success, we are empty
  }

  return false; // nop still stuff in the cargo bay;
}
//---------------------------------------------------------------------------
bool ABaseRobotCtrl::actionRecover()
{
  if ( mFgStateChanged ) {
    mHeading = randNo( -PI, PI );
    mRecoveryTimeout = randNo( 10.0, 20.0 );
  }
  if ( !actionAvoidObstacle() ) {
    mDrivetrain->setVelocityCmd( 0.2,
                                 SIGN( NORMALIZE_ANGLE( mRobotPose.mYaw
                                                        - mHeading ) ) * 0.5 );
  }

  if ( mElapsedStateTime > mRecoveryTimeout )
    return true;

  return false;
}
//---------------------------------------------------------------------------
bool ABaseRobotCtrl::actionFollowWayPointList()
{
  if ( mNd->atGoal() == true  || mFgWayPointListChanged == true ) {
    mFgWayPointListChanged = false;
    if ( mWayPointList.empty() == false ) {
      mCurrentWaypoint = mWayPointList.front();
      mWayPointList.pop_front();

      mNd->setGoal( mCurrentWaypoint.getPose() );
      // did we just pop the last waypoint ?
      if ( mWayPointList.empty() ) {
        mNd->setEpsilonAngle( D2R( 25.0 ) );
        mNd->setEpsilonDistance( 0.5 );
      }
      else {
        mNd->setEpsilonAngle( D2R( 180.0 ) );
        mNd->setEpsilonDistance( 0.75 );
      }
    }
    else {
      mDrivetrain->stop();
      return true; // way point list is empty, this must mean we are at the goal
    }
  }
  if ( mNd->hasActiveGoal() == false ) {
    mNd->setGoal( mCurrentWaypoint.getPose() );
  }

  if ( mNd->isStalled() == true ) {
    mDrivetrain->stop();
    if ( mTimer > ND_STALL_TIMEOUT ) {
      mDrivetrain->setVelocityCmd( BACK_OFF_SPEED, 0.0 );

      if ( mTimer > ND_STALL_TIMEOUT + ( BACK_OFF_DISTANCE / fabs( BACK_OFF_SPEED ) ) ) {
        mNd->setGoal( mCurrentWaypoint.getPose() );
        mTimer = 0.0;
      }
    }
    return false;  // we are still in line and waiting
  }

  mNd->update( mSimTime, mRobotPose, mRobotVelocity );
  mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );


  return false; // not at goal yet
}
//---------------------------------------------------------------------------
bool ABaseRobotCtrl::actionWaitInQueue( float dt,
                                        bool fgEnforceWaitingPosition )
{
  CPoint2d point;
  CPose2d waitingPose;

  if ( mWaitDestination == NULL ) {
    rprintf( "actionWaitInQueue has no waiting destination in state %s\n",
             mFsmText[mState] );
    return false;
  }

  mWayPointList.clear();
  mQueueRank = mWaitDestination->getWaitingPosition( this,  waitingPose );
  mWayPointList.push_back( waitingPose );

  /*
  if ( mNd->isStalled() ) {
    if ( mQueueStallRecoverTimer->isRunning() == false ) {
      mQueueStallRecoverTimer->start ( 10.0 );
      mCurrentWaypoint = mRobotPose + CPoint2d(randNo(-2.0, 2.0), randNo(2.0, 2.0));
      mNd->setGoal(mCurrentWaypoint.getPose());
      mNd->setEpsilonDistance ( 0.75 );
      mNd->setEpsilonAngle ( D2R ( 180.0 ) );
    }
  }
  */
  //if ( mQueueStallRecoverTimer->isRunning() == false ) {
  if (( mNd->isStalled() == true ) ||
      ( waitingPose != mCurrentWaypoint.getPose() ) ) {
    if ( waitingPose != mCurrentWaypoint.getPose() )
      mWaitingQueueTimer = 0.0;

    mCurrentWaypoint = waitingPose;
    point.mX = waitingPose.mX;
    point.mY = waitingPose.mY;
    replan( point, true );
    mNd->setGoal( waitingPose );
    mNd->setEpsilonDistance( 0.3 );
    mNd->setEpsilonAngle( D2R( 20.0 ) );
  }
  //}


  if ( mNd->atGoal() == false ) {
    mWaitingQueueTimer += dt;
    if ( mWaitingQueueTimer > 20.0 )
      leaveCurrentWaitingQueue();

    mNd->update( mSimTime, mRobotPose, mRobotVelocity );
    mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );
  }
  else {
    mDrivetrain->stop();
  }

  if ( mQueueRank == 1 ) {
    if ( mNd->atGoal() || fgEnforceWaitingPosition == false )
      return true;  // we made it to the front of the queue
  }

  return false; // still waiting in line
}
//---------------------------------------------------------------------------
void ABaseRobotCtrl::updateData( float dt )
{
  char statusStr[20];
  float r;

  // get latest position
  mRobotPose = mDrivetrain->getOdometry()->getPose();
  mRobotVelocity = mDrivetrain->getVelocity();
  mCargoLoad = mDrivetrain->getStageModel()->GetFlagCount();
  mSimTime = mRobot->getCurrentTime();
  fiducialUpdate();

  snprintf( statusStr, 20, "%s", mFsmText[mState] );
  mWaveFrontMap->update( mSimTime );
  // remove ourselfs from the current task counter
  if ( mCurrentWorkTask )
    mCurrentWorkTask->unsubscribe( this );

  // assume we are not working, till proven otherwise
  mFgWorking = false;

  switch ( mState ) {

    case START:
      startupPolicy( dt );
      break;

    case PAUSE:
      mDrivetrain->stop();
      break;

    case WAITING_AT_SOURCE:
      mFgWorking = true;
      snprintf( statusStr, 20, "%s (%d)", mFsmText[mState], mQueueRank );
      if ( mWaitDestination == NULL ) {
        mState = GOTO_SOURCE;
      }
      else if ( actionWaitInQueue( dt ) == true ) {
        mState = PICKUP_LOAD;
      }
      waitAtSourcePolicy( dt );
      break;

    case WAITING_AT_SINK:
      mFgWorking = true;
      snprintf( statusStr, 20, "%s (%d)", mFsmText[mState], mQueueRank );
      if ( mWaitDestination == NULL ) {
        mState = GOTO_SINK;
      }
      else if ( actionWaitInQueue( dt ) == true ) {
        mState = DELIVER_LOAD;
      }
      break;

    case WAITING_AT_CHARGER:
      snprintf( statusStr, 20, "%s (%d)", mFsmText[mState], mQueueRank );

      if ( mWaitDestination == NULL ) {
        mState = GOTO_CHARGER;
      }
      else if ( actionWaitInQueue( dt ) == true ) {
        if (( mFgRobotDetected == true ) && ( mFgChargerDetected == false ) )
          mState = WAITING_AT_CHARGER;
        if ( mFgChargerDetected == true ) {
          mState = DOCK;
        }
        if (( mFgRobotDetected == false ) && ( mFgChargerDetected == false ) )
          mState = GOTO_CHARGER;
      }
      waitAtChargerPolicy( dt );
      break;

    case DELIVER_LOAD:
      mFgWorking = true;
      if ( actionDeliverLoad() == true ) {
        mState = GOTO_SOURCE;
        selectWorkTaskPolicy( dt );
        //printTaskStatistics();
        mTravelStartTimeStamp = mSimTime;
        if ( chargingPolicy( dt ) == true ) {
          mState = GOTO_CHARGER;
        }
      }
      break;

    case PICKUP_LOAD:
      mPickupDuration = mPickupDuration + dt;
      mFgWorking = true;
      if ( mCurrentWorkTask->mSource->getDistance( mRobotPose ) >
           LOADINGZONE_DISTANCE_LIMIT ) {
        mPickupDuration = 0.0;
        if ( mElapsedStateTime > LOADINGZONE_APPROACH_TIMEOUT ) {
          mState = GOTO_SOURCE;
        }
      }

      if ( actionPickupLoad() == true ) {
        mState = GOTO_SINK;
        mTravelStartTimeStamp = mSimTime;
        pickupCompletedPolicy( dt );
      }
      else {
        waitingAtPickupPolicy( dt );
      }
      break;

    case GOTO_SOURCE:
      mFgWorking = true;
      if ( mFgStateChanged ) {
        replan( mCurrentWorkTask->mSource->getLocation(), true );
      }
      replan( mCurrentWorkTask->mSource->getLocation() );

      // are we far enought from the SINK to safely leave the queue ?
      if ( mCurrentWorkTask->mSink->getDistance( mRobotPose ) > 0.5 ) {
        leaveCurrentWaitingQueue();
      }
      // close enough to SOURCE ?
      if (( mCurrentWorkTask->mSource->isNear( mRobotPose ) == true ) ||
          ( actionFollowWayPointList() == true ) ) {
        mCurrentWorkTask->mSource->enterWaitingQueue( this );
        mWaitDestination = mCurrentWorkTask->mSource;
        mState = WAITING_AT_SOURCE;
        mTravelDuration = mSimTime - mTravelStartTimeStamp;
      }

      if ( noProgress( NO_PROGRESS_TIMEOUT ) == true ) {
        mState = RECOVER;
      }
      break;

    case GOTO_SINK:
      mFgWorking = true;
      if ( mFgStateChanged ) {
        replan( mCurrentWorkTask->mSink->getLocation(), true );
      }
      replan( mCurrentWorkTask->mSink->getLocation() );

      // are we far enought from the SOURCE to safely leave the queue ?
      if ( mCurrentWorkTask->mSource->getDistance( mRobotPose ) > 0.5 ) {
        leaveCurrentWaitingQueue();
      }

      if ( noProgress( NO_PROGRESS_TIMEOUT ) == true ) {
        mState = RECOVER;
      }

      if (( mCurrentWorkTask->mSink->isNear( mRobotPose ) == true ) ||
          ( actionFollowWayPointList() == true ) ) {
        mCurrentWorkTask->mSink->enterWaitingQueue( this );
        mWaitDestination = mCurrentWorkTask->mSink;
        mTravelDuration = mSimTime - mTravelStartTimeStamp;
        mState = WAITING_AT_SINK;
      }
      break;


    case GOTO_CHARGER:
      if ( mFgStateChanged ) {
        replan( mCurrentChargerDestination->getLocation(), true );
      }
      leaveCurrentWaitingQueue();
      actionFollowWayPointList();
      if ( noProgress( NO_PROGRESS_TIMEOUT ) == true ) {
        mState = RECOVER;
      }

      if ( mCurrentChargerDestination->isNear( mRobotPose ) == true ) {
        mCurrentChargerDestination->enterWaitingQueue( this );
        mWaitDestination = mCurrentChargerDestination;
        mState = WAITING_AT_CHARGER;
      }
      break;

    case GOTO_DEPOT:
      if ( mFgStateChanged ) {
        replan( mDepotDestination->getLocation(), true );
        leaveCurrentWaitingQueue();
      }
      actionFollowWayPointList();
      if ( noProgress( NO_PROGRESS_TIMEOUT ) == true ) {
        mState = RECOVER;
      }

      if ( mDepotDestination->isNear( mRobotPose ) == true ) {
        mDepotDestination->enterWaitingQueue( this );
        mWaitDestination = mDepotDestination;
        mState = UNALLOCATED;
      }
      break;

    case CHARGE:
      mDrivetrain->stop();
      leaveCurrentWaitingQueue();
      powerUp( false );
      if (( mPowerPack->getBatteryLevel() > 0.99 ) ||
          ( mPowerPack->isCharging() == false ) ) {
        mState = UNDOCK;
        powerUp( true );
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
        leaveChargerPolicy( dt );
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
      snprintf( statusStr, 20, "%s (%d)", mFsmText[mState], mQueueRank );

      if ( mWaitDestination == NULL ) {
        powerUp( true );
        mState = GOTO_DEPOT;
      }
      else {
        if ( mNd->atGoal() )
          powerUp( false );
        else
          powerUp( true );

        if ( actionWaitInQueue( false ) == true ) {
          unallocatedPolicy( dt );
          if ( mState != UNALLOCATED )
            powerUp( true );
        }
      }
      break;

    default:
      PRT_ERR1( "unknown FSM state %d \n", mState );
      mState = START;
      break;
  } // switch


  transferWaypointToStage();
  //*****************************************
  // Emergency routines
  if ( mDrivetrain->isStalled()  || ( mSimTime - mLastStalledTimestamp < 5.0 ) ) {
    if ( mDrivetrain->isStalled() )
      mLastStalledTimestamp = mSimTime;

    leaveCurrentWaitingQueue();
    if ( isModAboutZero( mSimTime, 3.0f ) == true ) {
      r = drand48();
      if ( r <= 0.33 )
        mStallRecoverSpeedCmd.mVX = 0.0;
      else if ( r <= 0.66 )
        mStallRecoverSpeedCmd.mVX = STALL_RECOVER_SPEED;
      else
        mStallRecoverSpeedCmd.mVX = -STALL_RECOVER_SPEED;

      r = drand48();
      if ( r <= 0.33 )
        mStallRecoverSpeedCmd.mYawDot = 0.0;
      else if ( r <= 0.66 )
        mStallRecoverSpeedCmd.mYawDot = STALL_RECOVER_TURNRATE;
      else
        mStallRecoverSpeedCmd.mYawDot = -STALL_RECOVER_TURNRATE;
    }
    mDrivetrain->setVelocityCmd( mStallRecoverSpeedCmd );
    snprintf( statusStr, 20, "%0.1f %0.1f", mStallRecoverSpeedCmd.mVX,
              mStallRecoverSpeedCmd.mYawDot );
  }

  // add ourselfs to the current task again
  if (( mFgWorking ) && ( mCurrentWorkTask ) )
    mCurrentWorkTask->subscribe( this );

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
  else if (( mState == GOTO_CHARGER ) || ( mState == CHARGE ) )
    mTaskId = -1;
  else if (( mState == GOTO_DEPOT ) || ( mState == UNALLOCATED ) )
    mTaskId = -2;

  mTextDisplay->setText( statusStr );
  mTimer = mTimer + dt;
  mProgressTimer =  mProgressTimer + dt;
  mElapsedStateTime = mElapsedStateTime + dt;
  mPrevTimestepState = mState;

  mLogWriter->print( mSimTime, 1.0 );

  if ( rapiError->hasError() )
    rapiError->print();
}
//---------------------------------------------------------------------------
