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

#ifndef BASEROBOTCTRL_H
#define BASEROBOTCTRL_H

#include "RapiLooseStage"
#include "stagend.h"
#include "stagewavefrontmap.h"
#include "transporttaskinterface.h"
#include "charger.h"
#include <vector>

using namespace Rapi;

/** Type definition for action results */
typedef enum {COMPLETED, IN_PROGRESS, FAILED } tActionResult;
/** Type definition for FSM */
typedef enum {START, GOTO_SINK, GOTO_SOURCE, GOTO_CHARGER, DOCK,
              UNDOCK, CHARGE, PICKUP_LOAD, DELIVER_LOAD, WAITING_AT_SOURCE,
              WAITING_AT_SINK, WAITING_AT_CHARGER, RECOVER, GOTO_DEPOT,
              ROTATE90, UNALLOCATED, DOCK_FAILED, NUM_STATES
             } tFsmState;

/** Type definition for charger data structure */
typedef struct {
  /** Bearing of charging station [rad] */
  float bearing;
  /** Distance to charging station [m] */
  float distance;
  /** Orientation of charging station [rad] */
  float orientation;
} tChargingStation;

typedef enum {NORMAL_SPACING, CLOSE_SPACING} tSpacing;


/**
 * This class provides a generic FASR robot
 * @author Jens Wawerla
 */
class ABaseRobotCtrl : public ARobotCtrl
{
  friend class CAnalyst;

  public:
    /** Default destructor */
    ~ABaseRobotCtrl();
    /**
     * Adds a transportation task to the robot
     * @param task to add
     */
    virtual void addTransportationTask ( ITransportTaskInterface* task );
    /**
     * Adds a charger to the list of available chargers
     * @param charger to be added
     */
    void addCharger ( CCharger* charger );
    /**
     * Sets the depot, a place for unallocated robot to go to
     * @param depot
     */
    void setDepot ( CDestination* depot );

  protected:
    /**
     * Default constructor
     * @param robot to control
     */
    ABaseRobotCtrl ( ARobot* robot );
    /**
     * Make a decision about recharging, it only makes sense to go charging
     * if at the sink, therefore this method gets called after a puck delivery
     * @param dt length of time step [s]
     * @return return true if charging needed, false otherwise
     */
    virtual bool chargingPolicy ( float dt );
    /** Start up policy */
    virtual void startPolicy() = 0;
    /** 
     * Policy for delivery completed, this policy determines what to do next,
     * after the cargo was delivered to the sink
     */
    virtual void deliveryCompletedPolicy( float dt) = 0;
    /**
     * Policy for pickup completed, this policy allows the robot controller to
     * decide what to do after the load was picked up
     */
    virtual void pickupCompletedPolicy( float dt ) = 0;
    /**
     * Policy for leaving a charger, this must set mState to what ever we
     * should do next
     * @param dt length of time step [s]
     */
    virtual void leaveChargerPolicy ( float dt ) = 0;
    /**
     * Policy for robot on rank 1 in waiting queue at depot. This is the robot
     * that is allowed to leave the queue next, the policy determines when
     * the robot leave the depot and what it does next
     */
    virtual void unallocatedPolicy( float dt ) = 0;
    /**
     * Policy for waiting at the source
     */
    virtual void waitingAtSourcePolicy( float dt) = 0;
    /**
     * Policy for waiting at the pickup patch, note not in the queue
     * @param dt length of time step [s]
     */
    virtual void waitingAtPickupPolicy ( float dt ) = 0;

    /** List of transportation tasks */
    std::vector<ITransportTaskInterface*> mTaskVector;
    /** Current transportation task */
    ITransportTaskInterface* mCurrentTask;
    /** List of charger location */
    std::list<CCharger*> mChargerList;
    /** Current charger destination */
    CCharger* mCurrentCharger;
    /** State of FSM */
    tFsmState mState;
    /** State of FSM previous to last state change */
    tFsmState mPrevState;
    /** Flags if state has changed */
    bool mFgStateChanged;
    /** Pose of robot */
    CPose2d mRobotPose;
    /** Current velocity of robot */
    CVelocity2d mRobotVelocity;
    /** Stage position model */
    CLooseStageDrivetrain2dof* mDrivetrain;
    /** Stage laser model */
    CLooseStageLaser* mLaser;
    /** Stage power pack */
    CLooseStagePowerPack* mPowerPack;
    /** Stage fiducial model */
    CLooseStageFiducialFinder* mFiducial;
    /** Text display */
    CLooseStageTextDisplay* mTextDisplay;
    /** Nearness distance obstacle avoidance */
    CStageNd* mNd;
    /** Wavefront map for general purpose */
    CStageWaveFrontMap* mWaveFrontMap;
    /** Accumulated reward [$] */
    float mAccumulatedReward;
    /** Reward rate from last task [$/s] */
    float mRewardRate;
    /** Total number of delivery trips */
    float mNumTrips;
    /** Time stamp of start of task */
    float mWorkTaskStartedTimeStamp;
    /** Time it took to complete the last task [s] */
    float mTaskCompletionTime;
    /** Time spent waiting at the source during last pickup [s] */
    float mSourceWaitingTime;
    /** Time spent waiting at the sink during last drop off [s] */
    float mSinkWaitingTime;
    /** Time spend in the pick up zone, required pick up load [s] */
    float mPickupTime;
    /** Time we had to slow down during this trip, possible due to interference [s] */
    float mSlowedDownTime;
    /** Elapsed time since state was entered [s] */
    float mElapsedStateTime;
    /** Size of the cargo bay [flags] */
    int mCargoBayCapacity;
    /** String for status messages */
    char mStatusStr[20];

  private:
    /**
     * Updates the controller for one time step
     * @param dt time since last update [s]
     */
    void updateData ( float dt );
    /**
     * Enables or disables all robot devices
     * @param enable true power up, false power down
     */
    void powerEnabled(bool enable);
    /**
     * Plans a path to a given goal
     * @param goal to go to
     * @param spacing between waypoints
     * @return 1 if successfull, 0 otherwise
     */
    int planPathTo ( const CPose2d goal, tSpacing spacing = NORMAL_SPACING );
    /**
     * Checks if the robot is making progress, meaning moving over ground
     * @return true if making progress, false otherwise
     */
    bool isMakingProgress();
    /**
     * Updates the data gained forom the fiducial device
     */
    void fiducialUpdate ();
    /**
     * Checks a squred area for robot fiducials
     * @param center of square
     * @param width of square [m]
     * @return true if robot detected, false otherwise
     */
    bool checkAreaForRobots(CPose2d center, float width);
    /**
     * Transferes waypoints to Stage
     */
    void transferWaypointToStage();
    /**
     * Action makes the robot wait in queue
     * @return action result
     */
    tActionResult actionWaitingInQueue();
    /**
     * Action drives the robot along the waypoint list
     * @return action result
     */
    tActionResult actionFollowWaypointList();
    /**
     * Action that pickups flags till the cargo capacity is reached
     * @param dt step size [s]
     * @return action result
     */
    tActionResult actionPickupLoad(float dt);
    /**
     * Action that unloads all flags in the cargo bay
     * @return action result
     */
    tActionResult actionDeliverLoad();
    /**
     * An action that causes the robot to dock at a charging station
     * @return action result
     */
    tActionResult actionDock();
    /**
     * Action causes the robot to undock from a charging station
     * @return action result
     */
    tActionResult actionUnDock();
    /**
     * Action causes the robot to rotate 90 degrees towards the most free area
     * @return action results
     */
    tActionResult actionRotate90();
    /** Single word describtion of FSM states */
    char mFsmText[20][NUM_STATES];
    /** State from the previous time step */
    tFsmState mPrevTimestepState;
    /** List of way points */
    std::list<CWaypoint2d> mWaypointList;
    /** Current waypoint */
    CWaypoint2d mCurrentWaypoint;
    /** Current destination */
    CDestination* mCurrentDestination;
    /** Depot */
    CDestination* mDepotDestination;
    /** Flags if a charger was detected */
    CRapiVariable<bool> mFgChargerDetected;
    /** Information about last seen charging station */
    tChargingStation mLastChargingStation;
    /** General purpose counter */
    int mCounter;
    /** General purpose heading [rad] */
    float mHeading;
    /** Robot pose to measure progress */
    CPose2d mRobotProgressPose;
    /** Timer for measuring progress */
    CTimer* mProgressTimer;
    /** Flags if we make progress or not */
    bool mFgProgress;
    /** General purpose timer */
    CTimer* mTimer;
    /** Count the number of time we have to disable Stage's collision detection */
    int mCountStageCollisionDisabled;
    /** Timer to measure the time we are slowed down */
    float mSlowedDownTimer;
    /** Data logger */
    CDataLogger* mDataLogger;

// ALL THESE VARIABLES ARE DEBUG ONLY
    bool mFgRobotInFront;
    int mNumWaypoints;
    float mAngle;
    bool mNdStalled;
    bool mNdTurningInPlace;
    bool mNdAtGoal;
    bool mFgLeftBox;
    bool mFgRightBox;
    bool mFgBackBox;
    bool mFgFrontBox;

    float mNdAngle;
    int mNdState;

};


#endif
