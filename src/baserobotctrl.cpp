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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "baserobotctrl.h"
#include "stagegridmap.h"


/** Distance to the loading bay at which we wait for it to be clear of robot [m] */
const float LOADING_BAY_WAIT_DISTANCE = 1.5;
/** Distance to loading bay that is considered close enough [m] */
const float LOADING_BAY_DISTANCE = 0.3;
/** Fiducial id of a robot */
const int ROBOT_FIDUCIAL_ID = 1;
/** Fiducial id of a charging station */
const int CHARGER_FIDUCIAL_ID = 2;
/** Energy needed to travel 1 meter [Wh] */
const float ENERGY_PER_METER = 0.11;
/** Speed for precise navigation while docking [m/s] */
const float DOCK_SPEED = 0.1;
/** Distance to back off [m] */
const float BACK_OFF_DISTANCE = 0.7;
/** Speed used to back off from a charger [m/s] */
const float BACK_OFF_SPEED = -0.05;
/** Name for log file */
const std::string LOGFILENAME = "fasr.log";

//-----------------------------------------------------------------------------
ABaseRobotCtrl::ABaseRobotCtrl( ARobot* robot )
    : ARobotCtrl( robot )
{
  CLooseStageRobot* looseRobot;
  Stg::Model* stgModel;


  //************************************
  // Initialize variables
  mCargoBayCapacity = 1;
  mCounter = 0;
  mState = START;
  mPrevTimestepState = START;
  mPrevState = START;
  mFgStateChanged = false;
  mCurrentCharger = NULL;
  mCurrentTask = NULL;
  mCurrentTask = NULL;
  mAngle = 0.0f;
  mElapsedStateTime = 0.0;
  mAccumulatedReward = 0.0;
  mNumTrips = 0;
  mWorkTaskStartedTimeStamp = 0.0;
  mTaskCompletionTime = 0.0;
  mPickupTime = 0.0;
  mSlowedDownTime = 0.0;
  mSlowedDownTimer = 0.0;
  mSourceWaitingTime = 0.0;
  mSinkWaitingTime = 0.0;
  mRewardRate = 0.0;
  mCountStageCollisionDisabled = 0;

  //*************************************
  // Setup timers and rapi variables
  mTimer = new CTimer( mRobot );
  mProgressTimer = new CTimer( mRobot );
  mFgChargerDetected.setRobot( mRobot );

  //*************************************
  // Register monitored variables
  mRobot->mVariableMonitor.addVar( &mNumTrips, "mNumTrips" );
  mRobot->mVariableMonitor.addVar( &mAccumulatedReward, "mAccumulatedReward" );
  mRobot->mVariableMonitor.addVar( &mTaskCompletionTime, "mTaskCompletionTime" );
  mRobot->mVariableMonitor.addVar( &mSourceWaitingTime, "mSourceWaitingTime" );
  mRobot->mVariableMonitor.addVar( &mSinkWaitingTime, "mSinkWaitingTime" );
  mRobot->mVariableMonitor.addVar( &mPickupTime, "mPickupTime" );
  mRobot->mVariableMonitor.addVar( &mRewardRate, "mRewardRate" );
  mRobot->mVariableMonitor.addVar( &mSlowedDownTime, "mSlowedDownTime" );
  mRobot->mVariableMonitor.addVar( &mCountStageCollisionDisabled, "mCountStageCollisionDisabled" );

  // just debug stuff
  mRobot->mVariableMonitor.addVar( &mElapsedStateTime, "mElapsedStateTime" );
  mRobot->mVariableMonitor.addVar( &mFgRobotInFront, "mFgRobotInFront" );
  mRobot->mVariableMonitor.addVar( &mCurrentWaypoint, "mCurrentWaypoint" );
  mRobot->mVariableMonitor.addVar( &mNdStalled, "mNdStalled" );
  mRobot->mVariableMonitor.addVar( &mNdAtGoal, "mNdAtGoal" );
  mRobot->mVariableMonitor.addVar( &mFgLeftBox, "mFgLeftBox" );
  mRobot->mVariableMonitor.addVar( &mFgFrontBox, "mFgFrontBox" );
  mRobot->mVariableMonitor.addVar( &mFgBackBox, "mFgBackBox" );
  mRobot->mVariableMonitor.addVar( &mFgRightBox, "mFgRightBox" );


  //************************************
  // FSM
  strncpy( mFsmText[START], "start", 20 );
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
  strncpy( mFsmText[ROTATE90], "rotate90", 20 );
  strncpy( mFsmText[UNALLOCATED], "unallocated", 20 );
  strncpy( mFsmText[DOCK_FAILED], "dock failed", 20 );

  looseRobot = ( CLooseStageRobot* ) mRobot;
  looseRobot->findDevice( mFiducial, "model:0.fiducial:0" );
  looseRobot->findDevice( mLaser, "laser:0" );
  looseRobot->findDevice( mPowerPack, "powerpack:0" );
  looseRobot->findDevice( mTextDisplay, "textdisplay:0" );
  looseRobot->findDevice( mDrivetrain, "position:0" );
  mDrivetrain->setTranslationalAccelerationLimit( CLimit( -INFINITY, INFINITY ) );
  mDrivetrain->setRotationalAccelerationLimit( CLimit( -INFINITY, INFINITY ) );
  if ( rapiError->hasError() ) {
    rapiError->print();
    exit( -1 );
  }

  // enable logging
  mDataLogger = CDataLogger::getInstance( LOGFILENAME, OVERWRITE );
  mDataLogger->setInterval( 0.1 );
  mDrivetrain->startLogging( LOGFILENAME );
  mDataLogger->addVar(( int* )&mState, "state" );
  mDataLogger->addVar( &mNumTrips, "numTrips" );
  mDataLogger->addVar( &mAccumulatedReward, "accumulatedReward", 1 );
  mDataLogger->addVar( &mTaskCompletionTime, "taskCompletionTime", 1 );
  mDataLogger->addVar( &mSourceWaitingTime, "sourceWaitingTime", 1 );
  mDataLogger->addVar( &mSinkWaitingTime, "sinkWaitingTime", 1 );
  mDataLogger->addVar( &mPickupTime, "pickupTime", 1 );
  mDataLogger->addVar( &mRewardRate, "rewardRate", 3 );
  mDataLogger->addVar( &mSlowedDownTime, "slowedDownTime", 1 );
  mDataLogger->addVar( &mCountStageCollisionDisabled, "countStageCollisionDisabled" );


  // get robot pose
  mRobotPose = mDrivetrain->getOdometry()->getPose();

  // Add nearest distance obstacle avoidance to robot
  stgModel = mDrivetrain->getStageModel();
  assert( stgModel );
  mNd = new CStageNd( stgModel, 0.4, 0.3, 0.2, mRobot->getName() );
  mNd->addRangeFinder( mLaser );

  //*************************************
  // Create Wavefront planner
  mWaveFrontMap = new CStageWaveFrontMap( new CStageGridMap( stgModel ),
                                          mDrivetrain->getName().c_str() );
  assert( mWaveFrontMap );
  mWaveFrontMap->addRangeFinder( mLaser );
  mWaveFrontMap->setRobotPose( &mRobotPose );
  mWaveFrontMap->setObstacleGrowth( 0.4 );
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
bool ABaseRobotCtrl::checkAreaForRobots( CPose2d center, float width )
{
  float angle;
  CPoint2d point;
  tFiducialData fiducialData;

  for ( unsigned int i = 0; i < mFiducial->getNumReadings(); i++ ) {
    fiducialData = mFiducial->mFiducialData[i];
    if ( fiducialData.id == ROBOT_FIDUCIAL_ID ) {
      angle = normalizeAngle( mRobotPose.mYaw + fiducialData.bearing );
      point.mX = mRobotPose.mX + cos( angle ) * fiducialData.range;
      point.mY = mRobotPose.mY + sin( angle ) * fiducialData.range;


      if (( fabs( point.mX - center.mX ) < width / 2.0 ) &&
          ( fabs( point.mY - center.mY ) < width / 2.0 ) ) {
        //rprintf( "detected robot at %s \n", point.toStr().c_str() );
        return true; // found a robot in the area
      }
    }
  } // for

  return false;  // no robots detected
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::powerEnabled( bool enable )
{
  mDrivetrain->setEnabled( enable );
  mLaser->setEnabled( enable );
  mFiducial->setEnabled( enable );
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::fiducialUpdate()
{
  float minRange = INFINITY;
  tFiducialData fiducialData;
  bool chargerDetected = false;


  for ( unsigned int i = 0; i < mFiducial->getNumReadings(); i++ ) {
    fiducialData = mFiducial->mFiducialData[i];

    // check if this fiducial is a charging station
    if (( fiducialData.id == CHARGER_FIDUCIAL_ID ) &&
        ( fiducialData.range < minRange ) ) {
      minRange = fiducialData.range;
      // keep the information for later
      chargerDetected = true;
      mLastChargingStation.bearing = fiducialData.bearing;
      mLastChargingStation.distance = fiducialData.range;
      //mLastChargingStation.orientation = fiducialData->geom.a;
    }
  }
  mFgChargerDetected = chargerDetected;
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::transferWaypointToStage()
{
  CRgbColor color;
  CPose2d pose;
  int i = 0;
  std::list<CWaypoint2d>::iterator it;
  Stg::Waypoint* oldList = NULL;
  Stg::Waypoint* wpList = new Stg::Waypoint[mWaypointList.size() + 1 ];

  // add current waypoint
  pose =  mCurrentWaypoint.getPose();
  wpList[i].pose.x = pose.mX;
  wpList[i].pose.y = pose.mY;
  wpList[i].pose.a = pose.mYaw;
  wpList[i].color = Stg::Color( 0, 1, 0, 0 ); // green
  i++;

  for ( it = mWaypointList.begin(); it != mWaypointList.end(); it++ ) {
    pose = ( *it ).getPose();
    color = ( *it ).getColor();
    wpList[i].pose.x = pose.mX;
    wpList[i].pose.y = pose.mY;
    wpList[i].pose.a = pose.mYaw;

    wpList[i].color = Stg::Color( color.mRed / 255.0,
                                  color.mBlue / 255.0,
                                  color.mGreen / 255.0, 0 );
    i ++;
  }

  oldList = mDrivetrain->getStageModel()->SetWaypoints( wpList, i );

  if ( oldList != NULL )
    delete[] oldList;
}
//-----------------------------------------------------------------------------
bool ABaseRobotCtrl::isMakingProgress()
{
  if ( mFgStateChanged ) {
    mFgProgress = true;
    mRobotProgressPose = mRobotPose;
    mProgressTimer->start();
  }

  if ( mProgressTimer->getElapsedTime() > 60.0 ) {
    if ( mRobotPose.distance( mRobotProgressPose ) > 0.2 )
      mFgProgress = true;
    else
      mFgProgress = false;
    mRobotProgressPose = mRobotPose;
    mProgressTimer->start();
  }
  return mFgProgress;
}
//-----------------------------------------------------------------------------
int ABaseRobotCtrl::planPathTo( const CPose2d goal, tSpacing spacing )
{

  switch ( spacing ) {
    case NORMAL_SPACING:
      mWaveFrontMap->setWaypointDistance( 1.0, 2.5 );
      break;
    case CLOSE_SPACING:
      mWaveFrontMap->setWaypointDistance( 0.1, 0.3 );
      break;
  };

  //***********************************
  // try to plan a path using sensor data
  mWaveFrontMap->calculateWaveFront( goal, CWaveFrontMap::USE_SENSOR_DATA );

  if ( mWaveFrontMap->calculatePlanFrom( mRobotPose ) != -1 ) {
    mWaypointList.clear();
    mWaveFrontMap->getWaypointList( mWaypointList );

    // the first waypoint is our current location, so remove if there are more
    // points remaining
    if ( mWaypointList.size() > 1 )
      mWaypointList.pop_front();

    return 1; // success
  }

  //***********************************
  // we failed to plan a path while including sensor data
  // try to plan a path with just the map
  mWaveFrontMap->calculateWaveFront( goal, CWaveFrontMap::USE_MAP_ONLY );
  if ( mWaveFrontMap->calculatePlanFrom( mRobotPose ) != -1 ) {
    mWaypointList.clear();
    mWaveFrontMap->getWaypointList( mWaypointList );

    // the first waypoint is our current location, so remove if there are more
    // points remaining
    if ( mWaypointList.size() > 1 )
      mWaypointList.pop_front();

    return 1; // success
  }

  //***********************************
  // failed to plan after all
  rprintf( "Failed to plan path\n" );
  return 0; // failed to plan path

}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::setDepot( CDestination* depot )
{
  mDepotDestination = depot;
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::addTransportationTask( ITransportTaskInterface* task )
{
  mTaskVector.push_back( task );
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::addCharger( CCharger* charger )
{
  mChargerList.push_back( charger );
  mCurrentCharger = charger;
}
//-----------------------------------------------------------------------------
bool ABaseRobotCtrl::chargingPolicy( float dt )
{
  float energyRequired;
  float distFromSinkToCharger;
  float distFromSourceToSink;
  float distFromHereToSource;

  if ( mCurrentTask == NULL )
    return false;  // we dont have a task, which means we should go to the depot

  mWaveFrontMap->calculateWaveFront( mCurrentTask->getSource()->getLocation(),
                                     CWaveFrontMap::USE_MAP_ONLY );
  distFromHereToSource = mWaveFrontMap->calculatePlanFrom( mRobotPose );

  mWaveFrontMap->calculateWaveFront( mCurrentTask->getSink()->getLocation(),
                                     CWaveFrontMap::USE_MAP_ONLY );
  distFromSourceToSink = mWaveFrontMap->calculatePlanFrom(
                           mCurrentTask->getSource()->getLocation() );

  distFromSinkToCharger = mWaveFrontMap->calculatePlanFrom(
                            mCurrentTask->getSource()->getLocation() );


  energyRequired = ( distFromSinkToCharger + distFromSourceToSink +
                     distFromHereToSource ) * ENERGY_PER_METER;

  if ( mPowerPack->getBatteryCapacity() > energyRequired )
    return false; // charging not required

  return true;  // charging required
}
//-----------------------------------------------------------------------------
tActionResult ABaseRobotCtrl::actionDock()
{
  CPoint2d point;

  if ( mElapsedStateTime > 60.0 )
    return FAILED;

  if ( mFgChargerDetected == true ) {
    if ( mLastChargingStation.distance > 0.6 ) {
      point.fromPolar( mLastChargingStation.distance - 0.2,
                       normalizeAngle( mLastChargingStation.bearing + mRobotPose.mYaw ) );
      mCurrentWaypoint = mRobotPose + point;
      mNd->setGoal( mCurrentWaypoint.getPose() );
      mNd->update( mRobot->getCurrentTime(), mRobotPose, mRobotVelocity );
      mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );
    }
    else {
      // creep towards it
      // update nd to get new data for the avoid boxes
      mNd->update( mRobot->getCurrentTime(), mRobotPose, mRobotVelocity );
      mNd->reset(); // clear stall flags etc. we are only interested in the avoid boxes
      mDrivetrain->setVelocityCmd( DOCK_SPEED,
                                   normalizeAngle( mLastChargingStation.bearing ) );
      if ( mPowerPack->isCharging() == true ) {
        mDrivetrain->stop();
        return COMPLETED;   // successfull dock
      }

      if ( mDrivetrain->isStalled() ) // touching
        mDrivetrain->setVelocityCmd( -0.01, 0.0 );  // back off a bit
    }
  }
  else {
    // can't see the charger any more, better stop, and let the FSM take
    // care of it
    mDrivetrain->stop();
    if ( mFgChargerDetected.getElapsedTimeSinceChange() > 10.0 )
      return FAILED; // we haven't seen a charger for 10s - something is wrong
  }

  return IN_PROGRESS; // not docked yet
}
//-----------------------------------------------------------------------------
tActionResult ABaseRobotCtrl::actionUnDock()
{
  Stg::ModelGripper::config_t gripperData;

  if ( mFgChargerDetected == false )
    return COMPLETED;  // we can't see the charger any more, lets assume we are undocked

  // back up a bit
  if ( mLastChargingStation.distance < BACK_OFF_DISTANCE ) {
    // update nd to get new data for the avoid boxes
    mNd->update( mRobot->getCurrentTime(), mRobotPose, mRobotVelocity );
    mNd->reset(); // clear stall flags etc. we are only interested in the avoid boxes
    if ( mNd->mBackAvoidBox.fgObstacle )
      mDrivetrain->stop();
    else
      mDrivetrain->setVelocityCmd( BACK_OFF_SPEED, 0.0 );
    mHeading = normalizeAngle( mLastChargingStation.bearing - PI );
  }
  else {
    mDrivetrain->setVelocityCmd( 0.0, 0.4 );
  }

  if (( mLastChargingStation.distance > BACK_OFF_DISTANCE ) &&
      ( epsilonEqual( mRobotPose.mYaw, mHeading, D2R( 10.0 ) ) == true ) )
    return COMPLETED;  // done undocking everything good

  return IN_PROGRESS; // undock procedure not completed yet
}
//-----------------------------------------------------------------------------
tActionResult ABaseRobotCtrl::actionDeliverLoad()
{
  if ( mRobotPose.distance( mCurrentTask->getSink()->getLocation() ) <=
       LOADING_BAY_DISTANCE ) {
    mDrivetrain->stop();

    if ( mDrivetrain->getStageModel()->GetFlagCount() > 0 ) {
      // unload flag
      mCurrentTask->getSink()->pushFlag( mDrivetrain->getStageModel()->PopFlag() );
    }
    else {
      // we were already empty exit here and do not collect any reward
      return COMPLETED;
    }
  }
  else {
    mNd->setEpsilonDistance( 0.1 );
    mNd->setEpsilonAngle( D2R( 180.0 ) );
    mNd->setGoal( mCurrentTask->getSink()->getLocation() );
    mNd->update( mRobot->getCurrentTime(), mRobotPose, mRobotVelocity );
    mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );
  }

  if ( mDrivetrain->getStageModel()->GetFlagCount() == 0 ) {
    mCurrentTask->incrementTaskCompletedCount();
    mTaskCompletionTime =  mRobot->getCurrentTime() - mWorkTaskStartedTimeStamp;
    mWorkTaskStartedTimeStamp = mRobot->getCurrentTime();
    mAccumulatedReward += mCurrentTask->getReward();
    mRewardRate = mCurrentTask->getReward() / mTaskCompletionTime;
    mSlowedDownTime = mSlowedDownTimer;
    mSlowedDownTimer = 0.0;
    mNumTrips ++;
    return COMPLETED;  // success, we are empty
  }

  return IN_PROGRESS; // nop still stuff in the cargo bay;
}
//-----------------------------------------------------------------------------
tActionResult ABaseRobotCtrl::actionPickupLoad( float dt )
{
  if ( mRobotPose.distance( mCurrentTask->getSource()->getLocation() ) <=
       LOADING_BAY_DISTANCE ) {
    mDrivetrain->stop();
    mPickupTime += dt;
    if ( mDrivetrain->getStageModel()->GetFlagCount() < mCargoBayCapacity ) {
      // load flag
      mDrivetrain->getStageModel()->PushFlag(
        mCurrentTask->getSource()->popFlag() );
    }
  }
  else {
    mPickupTime = 0.0;
    mNd->setEpsilonDistance( 0.1 );
    mNd->setEpsilonAngle( D2R( 180.0 ) );
    mNd->setGoal( mCurrentTask->getSource()->getLocation() );
    mNd->update( mRobot->getCurrentTime(), mRobotPose, mRobotVelocity );
    mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );
  }

  if ( mDrivetrain->getStageModel()->GetFlagCount() >= mCargoBayCapacity ) {
    return COMPLETED; // success, we are fully loaded
  }

  return IN_PROGRESS; // nop still space for more cargo
}
//-----------------------------------------------------------------------------
tActionResult ABaseRobotCtrl::actionRotate90()
{
  // update ND to get the latest avoid box data
  mNd->update( mRobot->getCurrentTime(), mRobotPose, mRobotVelocity );
  mNd->reset();
  if ( mFgStateChanged ) {
    if ( not mNd->mRightAvoidBox.fgObstacle )
      mHeading = mRobotPose.mYaw - D2R( 90.0 );
    else if ( not mNd->mLeftAvoidBox.fgObstacle )
      mHeading = mRobotPose.mYaw + D2R( 90.0 );
    else if ( randNo( 0.0, 1.0 ) > 0.5 )
      mHeading = mRobotPose.mYaw + D2R( 90.0 );
    else
      mHeading = mRobotPose.mYaw - D2R( 90.0 );
  }
  mAngle = normalizeAngle( mHeading - mRobotPose.mYaw );
  mDrivetrain->setVelocityCmd( 0.0, sign( mAngle ) * 0.2 );

  if ( fabs( mAngle ) < D2R( 5.0 ) ) {
    return COMPLETED;
  }

  return IN_PROGRESS;
}
//-----------------------------------------------------------------------------
tActionResult ABaseRobotCtrl::actionFollowWaypointList()
{

  if ( mFgStateChanged ) {
    if ( not mWaypointList.empty() ) {
      mCurrentWaypoint = mWaypointList.front();
      mNd->setGoal( mCurrentWaypoint.getPose() );
      mNd->setEpsilonAngle( D2R( 180.0 ) );
      mNd->setEpsilonDistance( 1.0 );
    }
  }

  if ( mNd->isStalled() ) {
    mCounter ++;

    if ( mCounter > 40 ) {
      // we are stalled, so plan a new path
      planPathTo( mCurrentDestination->getLocation(), CLOSE_SPACING );
      // and set the new waypoint if we have one otherwise set final destination
      if ( not mWaypointList.empty() ) {
        mCurrentWaypoint = mWaypointList.front();
        mWaypointList.pop_front();
        mNd->setGoal( mCurrentWaypoint.getPose() );
        mNd->setEpsilonAngle( D2R( 180.0 ) );
        mNd->setEpsilonDistance( 1.0 );
      }
      else {
        mNd->setGoal( mCurrentWaypoint.getPose() );
        mNd->setEpsilonAngle( D2R( 180.0 ) );
        mNd->setEpsilonDistance( 1.0 );
      }
    }
  }
  else
    mCounter = 0;

  // check if we are the goal
  if (( mNd->atGoal() ) ||
      ( mNd->hasCrossedPathNormal() ) ) {
    if ( mWaypointList.empty() ) {
      mDrivetrain->stop();
      return COMPLETED;
    }
    else {
      mCurrentWaypoint = mWaypointList.front();
      mWaypointList.pop_front();
      mNd->setGoal( mCurrentWaypoint.getPose() );
      // did we just pop the last waypoint ?
      if ( mWaypointList.empty() ) {
        mNd->setEpsilonAngle( D2R( 25.0 ) );
        mNd->setEpsilonDistance( 0.5 );
      }
      else {
        mNd->setEpsilonAngle( D2R( 180.0 ) );
        mNd->setEpsilonDistance( 1.0 );
      }
    }
  }

  // update ND and transfer velocity commands to drivetrain
  mNd->update( mRobot->getCurrentTime(), mRobotPose, mRobotVelocity );
  mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );

  return IN_PROGRESS;
}
//-----------------------------------------------------------------------------
tActionResult ABaseRobotCtrl::actionWaitingInQueue()
{
  int rank;
  CPose2d waitingPose;

  rank = mCurrentDestination->getQueuePosition( this, waitingPose );
  snprintf( mStatusStr, 20, "%s (%d)", mFsmText[mState], rank );

  mCurrentWaypoint = waitingPose;

  if ( waitingPose != mNd->getGoal() ) {
    mTimer->start();
    mNd->setGoal( waitingPose );
    mNd->setEpsilonDistance( 0.3 );
    mNd->setEpsilonAngle( D2R( 20.0 ) );
  }

  if ( mNd->isStalled() ) {
    mNd->setGoal( waitingPose );
    mNd->setEpsilonDistance( 0.3 );
    mNd->setEpsilonAngle( D2R( 20.0 ) );
  }

  mNd->update( mRobot->getCurrentTime(), mRobotPose, mRobotVelocity );
  if ( mNd->atGoal() ) {
    mDrivetrain->stop();
    if ( rank == 1 )
      return COMPLETED;
  }
  else {
    mDrivetrain->setVelocityCmd( mNd->getRecommendedVelocity() );
    if ( mTimer->getElapsedTime() > 60.0 )
      return FAILED; // it took too long to get to the current position
    // in the queue
  }

  return IN_PROGRESS;
}
//-----------------------------------------------------------------------------
void ABaseRobotCtrl::updateData( float dt )
{
  //float angle;

  //****************************************
  // Refresh data
  mRobotPose = mDrivetrain->getOdometry()->getPose();
  mRobotVelocity = mDrivetrain->getVelocity();
  mWaveFrontMap->update( mRobot->getCurrentTime() );
  mElapsedStateTime += dt;
  fiducialUpdate();
  snprintf( mStatusStr, 20, "%s", mFsmText[mState] );

  //****************************************
  // FSM
  switch ( mState ) {
    case START:
      startPolicy();
      break;

    case GOTO_SOURCE:
      if ( mFgStateChanged ) {
        mCurrentDestination = mCurrentTask->getSource();
        if ( planPathTo( mCurrentDestination->getLocation() ) == 0 )
          mState = ROTATE90;
      }
      actionFollowWaypointList();
      if ( mCurrentTask->getSource()->isNearEofQueue( mRobotPose ) ) {
        mDrivetrain->stop();
        mWaypointList.clear();
        mState = WAITING_AT_SOURCE;
      }
      if ( not isMakingProgress() )
        mState = ROTATE90;
      break;

    case GOTO_SINK:
      if ( mFgStateChanged ) {
        mCurrentDestination = mCurrentTask->getSink();
        if ( planPathTo( mCurrentDestination->getLocation() ) == 0 )
          mState = ROTATE90;
      }
      actionFollowWaypointList();
      if ( mCurrentTask->getSink()->isNearEofQueue( mRobotPose ) ) {
        mDrivetrain->stop();
        mWaypointList.clear();
        mState = WAITING_AT_SINK;
      }

      if ( not isMakingProgress() )
        mState = ROTATE90;
      break;

    case GOTO_CHARGER:
      if ( mFgStateChanged ) {
        mCurrentDestination = mCurrentCharger;
        if ( planPathTo( mCurrentDestination->getLocation() ) == 0 )
          mState = ROTATE90;
      }
      actionFollowWaypointList();
      if ( mCurrentDestination->isNearEofQueue( mRobotPose ) ) {
        mDrivetrain->stop();
        mWaypointList.clear();
        mState = WAITING_AT_CHARGER;
      }
      if ( not isMakingProgress() )
        mState = ROTATE90;
      break;

    case GOTO_DEPOT:
      if ( mFgStateChanged ) {
        mCurrentDestination = mDepotDestination;
        if ( planPathTo( mCurrentDestination->getLocation() ) == 0 )
          mState = ROTATE90;
      }
      actionFollowWaypointList();
      if ( mCurrentDestination->isNearEofQueue( mRobotPose ) ) {
        mDrivetrain->stop();
        mWaypointList.clear();
        mState = UNALLOCATED;
      }
      if ( not isMakingProgress() )
        mState = ROTATE90;
      break;

    case DOCK:
      switch ( actionDock() ) {
        case IN_PROGRESS: mState = DOCK;    break;
        case COMPLETED:   mState = CHARGE;  break;
        case FAILED:
          mState = DOCK_FAILED;
          mCurrentDestination->leaveQueue( this );
          break;
      }
      if ( not mFgChargerDetected ) {
        mCounter ++;
        if ( mCounter > 10 ) {
          mCurrentDestination->leaveQueue( this );
          mState = GOTO_CHARGER;
        }
      }
      else {
        mCounter = 0;
      }
      break;

    case DOCK_FAILED:
      if ( actionUnDock() == COMPLETED )
        mState = GOTO_CHARGER;
      break;

    case UNDOCK:
      if ( actionUnDock() == COMPLETED ) {
        leaveChargerPolicy( dt );
      }
      break;

    case CHARGE:
      if ( mFgStateChanged ) {
        mCurrentDestination->leaveQueue( this );
        powerEnabled( false );
      }
      if (( not mPowerPack->isCharging() ) ||
          ( mPowerPack->getBatteryLevel() > 0.99 ) ) {
        powerEnabled( true );
        mState = UNDOCK;
      }
      break;

    case ROTATE90:
      if ( actionRotate90() == COMPLETED ) {
        mState = mPrevState;
      }
      break;

    case PICKUP_LOAD:
      if ( actionPickupLoad( dt ) == COMPLETED ) {
        mCurrentTask->getSource()->leaveQueue( this );
        pickupCompletedPolicy( dt );
        mState = GOTO_SINK;
      }
      else {
        waitingAtPickupPolicy( dt );
      }
      break;

    case DELIVER_LOAD:
      if ( actionDeliverLoad() == COMPLETED ) {
        mCurrentTask->getSink()->leaveQueue( this );
        deliveryCompletedPolicy( dt );

        if ( chargingPolicy( dt ) == true ) {
          mState = GOTO_CHARGER;
        }
      }
      break;

    case WAITING_AT_SOURCE:
      if ( mFgStateChanged ) {
        mCurrentTask->getSource()->enterQueue( this );
        mCounter = 0;
      }
      switch ( actionWaitingInQueue() ) {
        case COMPLETED:
          if ( not checkAreaForRobots( mCurrentTask->getSource()->getLocation(), 2.0 ) )
            mCounter++;
          else
            mCounter = 0;
          if ( mCounter > 10 )  {
            mSourceWaitingTime = mElapsedStateTime;
            mState = PICKUP_LOAD;
          }
          break;

        case IN_PROGRESS:
          mState = WAITING_AT_SOURCE;
          break;

        case FAILED:
          mCurrentTask->getSource()->leaveQueue( this );
          mState = GOTO_SOURCE;
          break;
      } // switch
      waitingAtSourcePolicy( dt );
      break;

    case WAITING_AT_SINK:
      if ( mFgStateChanged ) {
        mCurrentTask->getSink()->enterQueue( this );
        mCounter = 0;
      }
      switch ( actionWaitingInQueue() ) {
        case COMPLETED:
          if ( not checkAreaForRobots( mCurrentTask->getSink()->getLocation(), 2.0 ) )
            mCounter++;
          else
            mCounter = 0;
          if ( mCounter > 10 )  {
            mSinkWaitingTime = mElapsedStateTime;
            mState = DELIVER_LOAD;
          }
          break;

        case IN_PROGRESS:
          mState = WAITING_AT_SINK;
          break;

        case FAILED:
          mCurrentTask->getSink()->leaveQueue( this );
          mState = GOTO_SINK;
          break;
      } // switch
      break;

    case WAITING_AT_CHARGER:
      if ( mFgStateChanged ) {
        mCurrentDestination->enterQueue( this );
        mCounter = 0;
      }
      switch ( actionWaitingInQueue() ) {
        case COMPLETED:
          if ( mFgChargerDetected == true )
            mCounter++;
          else
            mCounter = 0;
          if ( mCounter > 20 )  {
            mState = DOCK;
          }
          break;

        case IN_PROGRESS:
          mState = WAITING_AT_CHARGER;
          break;

        case FAILED:
          mCurrentDestination->leaveQueue( this );
          mState = GOTO_CHARGER;
          break;
      } // switch
      break;

    case RECOVER:
      break;

    case UNALLOCATED:
      if ( mFgStateChanged ) {
        mCurrentDestination->enterQueue( this );
        mTaskCompletionTime = 0.0; // clear old completion time
        mCounter = 0;
      }
      if ( mNd->atGoal() )
        powerEnabled( false );
      else
        powerEnabled( true );

      switch ( actionWaitingInQueue() ) {
        case COMPLETED:
          unallocatedPolicy( dt );
          if ( mState != UNALLOCATED ) {
            powerEnabled( true );
            mCurrentDestination->leaveQueue( this );
          }
          break;

        case IN_PROGRESS:
          mState = UNALLOCATED;
          break;

        case FAILED:
          mCurrentDestination->leaveQueue( this );
          mState = GOTO_DEPOT;
          break;
      } // switch
      break;

    default:
      rprintf( "Unknown FSM state %d \n", mState );
      break;

  } // switch

  //mNdTurningInPlace =  mNd->isTurningInPlace();
  mNdStalled = mNd->isStalled();
  mNdAtGoal = mNd->atGoal();
  mNdAngle = R2D( mNd->mInfo.angle );
  mNdState = mNd->mState;

  mFgBackBox = mNd->mBackAvoidBox.fgObstacle;
  mFgRightBox = mNd->mRightAvoidBox.fgObstacle;
  mFgLeftBox = mNd->mLeftAvoidBox.fgObstacle;
  mFgFrontBox = mNd->mFrontAvoidBox.fgObstacle;

  if ( mDrivetrain->isStalled() || mNd->isStalled() ) {
    strncpy( mStatusStr, "recover", 20 );
    if ( not mNd->mFrontAvoidBox.fgObstacle )
      mDrivetrain->setVelocityCmd( 0.02, 0.0 );
    else if ( not mNd->mBackAvoidBox.fgObstacle )
      mDrivetrain->setVelocityCmd( -0.02, 0.0 );
    else if ( not mNd->mRightAvoidBox.fgObstacle )
      mDrivetrain->setVelocityCmd( 0.0, 0.02 );
    else if ( not mNd->mLeftAvoidBox.fgObstacle )
      mDrivetrain->setVelocityCmd( 0.0, -0.02 );
    else {
      rprintf( "I am stuck\n" );
      mDrivetrain->setVelocityCmd( 0.0, 0.0 );
    }

    if ( mDrivetrain->stalledSince() > 120.0 ) {
      // try with a higher velocity
      mDrivetrain->setVelocityCmd( mDrivetrain->getVelocityCmd().mXDot * 20.0,
                                   mDrivetrain->getVelocityCmd().mYawDot * 20.0 );
    }
    if ( mDrivetrain->stalledSince() > 180.0 ) {
      // last a resort, disable Stage's collision detection and hope we break loose
      if ( mDrivetrain->getStageModel()->vis.obstacle_return ) {
        rprintf( "Stage collision detection disabled\n" );
        mCountStageCollisionDisabled ++;
        mDrivetrain->getStageModel()->vis.obstacle_return = false;
      }
    }
  }
  else {
    // re-enable Stage's collision detection
    if ( mDrivetrain->getStageModel()->vis.obstacle_return ) {
      mDrivetrain->getStageModel()->vis.obstacle_return = true;
    }
  }
  //****************************************
  // Finish up
  if ( rapiError->hasError() )
    rapiError->print();

  // set text display
  mTextDisplay->setText( mStatusStr );

  // transfer waypoints
  transferWaypointToStage();

  // advance states for next time step
  if ( mPrevTimestepState != mState ) {
    mFgStateChanged = true;
    mElapsedStateTime = 0.0;
    mPrevState = mPrevTimestepState;
  }
  else {
    mFgStateChanged = false;
  }
  mPrevTimestepState = mState;

  if (( mState == GOTO_SOURCE ) ||
      ( mState == GOTO_SINK ) ||
      ( mState == GOTO_CHARGER ) ||
      ( mState == ROTATE90 ) ) {
    // if we are driving slowly, we need to increment the slowedDownTimer
    if ( mDrivetrain->getVelocityCmd().mXDot <
         0.2 * mDrivetrain->getUppererVelocityLimit().mXDot )
      mSlowedDownTimer += dt;
  }

  mNumWaypoints = mWaypointList.size();
  mDataLogger->write( mRobot->getCurrentTime() );
}
//-----------------------------------------------------------------------------
