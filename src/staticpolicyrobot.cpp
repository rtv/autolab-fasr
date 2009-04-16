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
 * $Log: staticpolicyrobot.cpp,v $
 * Revision 1.9  2009-04-03 16:21:30  jwawerla
 * Move wavefront map specification to world file
 *
 * Revision 1.8  2009-03-31 23:52:59  jwawerla
 * Moved cell index from float to int math
 *
 * Revision 1.7  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.6  2009-03-28 00:54:39  jwawerla
 * *** empty log message ***
 *
 * Revision 1.5  2009-03-27 01:32:58  jwawerla
 * wait prob controller improved
 *
 * Revision 1.4  2009-03-27 00:27:37  jwawerla
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.2  2009-03-24 01:04:46  jwawerla
 * switching cost robot added
 *
 * Revision 1.1  2009-03-21 03:37:03  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "staticpolicyrobot.h"

/** Average speed of robot, to plan round trip tip [m/s] */
const float AVG_SPEED = 0.40;


//-----------------------------------------------------------------------------
CStaticPolicyRobot::CStaticPolicyRobot ( Stg::ModelPosition* modPos )
    : ABaseRobot ( modPos )
{
  rprintf("Loading CStaticPolicyRobot\n");
}
//-----------------------------------------------------------------------------
CStaticPolicyRobot::~CStaticPolicyRobot()
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobot::selectWorkTaskPolicy()
{
  // static policy so we don't change anything
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobot::startupPolicy()
{
  int allocatedRobots = 0;
  float chargeTime;
  float tripsPerCharge;
  float sinkToCharger;
  float chargerToSource;
  float normalRoundTripDistance;
  float chargeRoundTripDistance;
  float avgRoundTripTime;
  int optNumRobots[mWorkTaskVector.size() ];


  // select charger
  mCurrentChargerDestination = mChargerDestinationList.front();

  chargeTime = mBatteryCapacity / mCurrentChargerDestination->getChargeRate();

  // calculate optimal number of robots serving each task
  for ( unsigned int i = 0; i < mWorkTaskVector.size(); i++ ) {

    // normal source to sink round trip distance
    normalRoundTripDistance = 2.0 * mWorkTaskVector[i]->getSrcSinkDistance();

    // calculate distance from sink to charger
    mWaveFrontMap->calculateWaveFront ( mCurrentChargerDestination->getLocation(), false );
    sinkToCharger = mWaveFrontMap->calculatePlanFrom ( mWorkTaskVector[i]->mSink->getLocation() );

    // calculcate distance from charger to source
    mWaveFrontMap->calculateWaveFront ( mWorkTaskVector[i]->mSource->getLocation(), false );
    chargerToSource = mWaveFrontMap->calculatePlanFrom ( mCurrentChargerDestination->getLocation() );

    chargeRoundTripDistance = mWorkTaskVector[i]->getSrcSinkDistance() +
                              sinkToCharger + chargerToSource;

    tripsPerCharge = ( mBatteryCapacity -  chargeRoundTripDistance * ENERGY_PER_METER ) /
                     ( normalRoundTripDistance * ENERGY_PER_METER );

    avgRoundTripTime = ( normalRoundTripDistance / AVG_SPEED * tripsPerCharge +
                         chargeTime + chargeRoundTripDistance / AVG_SPEED ) /
                       ( tripsPerCharge );
    optNumRobots[i] = ( int ) ceil ( avgRoundTripTime *
                                     mWorkTaskVector[i]->getProductionRate() /
                                     (float)mCargoBayCapacity );
    //printf("%d optNumRobots %d  trips %f \n", i, optNumRobots[i], tripsPerCharge);
    //printf("sinkToCharger %f, chargerToSource %f normal %f  \n", sinkToCharger, chargerToSource, normalRoundTripDistance);
  }

  mCurrentWorkTask = NULL;
  for ( unsigned int i = 0; i < mWorkTaskVector.size(); i++ ) {
    if ( ( allocatedRobots < mRobotId ) && ( mRobotId <= allocatedRobots + optNumRobots[i] ) ) {
      mCurrentWorkTask = mWorkTaskVector[i];
      rprintf ( "allocated to task: %s\n", mCurrentWorkTask->getName() );
      break;
    }
    allocatedRobots += optNumRobots[i];
  }

  if ( mCurrentWorkTask == NULL ) {
    if ( mState != UNALLOCATED )
      mState = GOTO_DEPOT;
  }
  else {
    mState = GOTO_SOURCE;
  }


}
//-----------------------------------------------------------------------------
void CStaticPolicyRobot::leaveChargerPolicy()
{
  if ( mCargoLoad == 0 )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_SINK;
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobot::waitAtSourcePolicy()
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobot::waitAtChargerPolicy()
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobot::unallocatedPolicy()
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobot::pickupCompletedPolicy()
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobot::waitingAtPickupPolicy()
{
}
//-----------------------------------------------------------------------------



