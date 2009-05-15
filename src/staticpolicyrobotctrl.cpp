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
 *
 *
 ***************************************************************************/
#include "staticpolicyrobotctrl.h"

/** Average speed of robot, to plan round trip tip [m/s] */
const float AVG_SPEED = 0.40;


//-----------------------------------------------------------------------------
CStaticPolicyRobotCtrl::CStaticPolicyRobotCtrl ( ARobot* robot )
    : ABaseRobotCtrl ( robot )
{
  rprintf ( "Loading CStaticPolicyRobotCtrl\n" );
}
//-----------------------------------------------------------------------------
CStaticPolicyRobotCtrl::~CStaticPolicyRobotCtrl()
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::selectWorkTaskPolicy ( float dt )
{
  // static policy so we don't change anything
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::startupPolicy ( float dt )
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

  chargeTime = mPowerPack->getMaxBatteryCapacity() /
               mCurrentChargerDestination->getChargeRate();

  // calculate optimal number of robots serving each task
  for ( unsigned int i = 0; i < mWorkTaskVector.size(); i++ )
  {

    // normal source to sink round trip distance
    normalRoundTripDistance = 2.0 * mWorkTaskVector[i]->getSrcSinkDistance();

    // calculate distance from sink to charger
    mWaveFrontMap->calculateWaveFront ( mCurrentChargerDestination->getLocation(),\
                                        CWaveFrontMap::USE_MAP_ONLY );
    sinkToCharger = mWaveFrontMap->calculatePlanFrom ( mWorkTaskVector[i]->mSink->getLocation() );

    // calculcate distance from charger to source
    mWaveFrontMap->calculateWaveFront ( mWorkTaskVector[i]->mSource->getLocation(),
                                        CWaveFrontMap::USE_MAP_ONLY );
    chargerToSource = mWaveFrontMap->calculatePlanFrom ( mCurrentChargerDestination->getLocation() );

    chargeRoundTripDistance = mWorkTaskVector[i]->getSrcSinkDistance() +
                              sinkToCharger + chargerToSource;

    tripsPerCharge = ( mPowerPack->getMaxBatteryCapacity() -
                       chargeRoundTripDistance * ENERGY_PER_METER ) /
                     ( normalRoundTripDistance * ENERGY_PER_METER );

    avgRoundTripTime = ( normalRoundTripDistance / AVG_SPEED * tripsPerCharge +
                         chargeTime + chargeRoundTripDistance / AVG_SPEED ) /
                       ( tripsPerCharge );
    optNumRobots[i] = ( int ) ceil ( avgRoundTripTime *
                                     mWorkTaskVector[i]->getProductionRate() /
                                     ( float ) mCargoBayCapacity );
    //printf ( "%d optNumRobots %d  trips %f \n", i, optNumRobots[i], tripsPerCharge );
    //printf ( "sinkToCharger %f, chargerToSource %f normal %f  \n", sinkToCharger,
    // chargerToSource, normalRoundTripDistance );
  }

  mCurrentWorkTask = NULL;
  for ( unsigned int i = 0; i < mWorkTaskVector.size(); i++ )
  {
    if ( ( allocatedRobots < mRobotId ) && ( mRobotId <= allocatedRobots + optNumRobots[i] ) )
    {
      mCurrentWorkTask = mWorkTaskVector[i];
      rprintf ( "allocated to task: %s\n", mCurrentWorkTask->getName() );
      break;
    }
    allocatedRobots += optNumRobots[i];
  }

  if ( mCurrentWorkTask == NULL )
  {
    if ( mState != UNALLOCATED )
      mState = GOTO_DEPOT;
  }
  else
  {
    mState = GOTO_SOURCE;
  }
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::leaveChargerPolicy ( float dt )
{
  if ( mCargoLoad == 0 )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_SINK;
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::waitAtSourcePolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::waitAtChargerPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::unallocatedPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::pickupCompletedPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------
void CStaticPolicyRobotCtrl::waitingAtPickupPolicy ( float dt )
{
}
//-----------------------------------------------------------------------------



