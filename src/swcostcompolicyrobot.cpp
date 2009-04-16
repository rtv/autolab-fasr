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
 * $Log: swcostcompolicyrobot.cpp,v $
 * Revision 1.7  2009-04-03 16:21:30  jwawerla
 * Move wavefront map specification to world file
 *
 * Revision 1.6  2009-03-31 01:42:01  jwawerla
 * Task definitions moved to task manager and stage world file
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
 * Revision 1.2  2009-03-25 14:49:02  jwawerla
 * Task visualization works now
 *
 * Revision 1.1  2009-03-24 01:04:46  jwawerla
 * switching cost robot added
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "swcostcompolicyrobot.h"

/** Probability of exploring, basically epsilon-greedy */
const float EXPLORE_PROBABILITY = 0.2;
/** Probability of not exploiting but just being conservative and sticking
 * with the old task
 */
const float NOT_EXPLOIT_PROBABILTY = 0.0;
//-----------------------------------------------------------------------------
CSwCostComPolicyRobot::CSwCostComPolicyRobot ( Stg::ModelPosition* modPos )
    : ABaseRobot ( modPos )
{
  mBeta = 0.98;
  mCountTaskSwitch = 0;
}
//-----------------------------------------------------------------------------
CSwCostComPolicyRobot::~CSwCostComPolicyRobot()
{
}
//-----------------------------------------------------------------------------
void CSwCostComPolicyRobot::selectWorkTaskPolicy()
{
  double travelCostNewTask;
  double travelCostCurrentTask;
  double rewardRatio;
  double switchingCost;
  IWorkTaskRobotInterface* task;
  IWorkTaskRobotInterface* oldTask = mCurrentWorkTask;
  std::vector<IWorkTaskRobotInterface*>::iterator it;

  // should we exploit or be conservative ?
  if ( drand48() < 1 - NOT_EXPLOIT_PROBABILTY ) {
    for ( it = mWorkTaskVector.begin(); it != mWorkTaskVector.end(); it++ ) {
      task = ( IWorkTaskRobotInterface* ) * it;

      if ( ( drand48() < EXPLORE_PROBABILITY ) && ( task->getNumSubcribers() == 0 ) ) {
        mCurrentWorkTask = task;
        break;
      }
      if ( task->getExperiencedRewardRate()  != 0 ) {
        // figure out how far it is to the source of the current task
        mWaveFrontMap->calculateWaveFront ( mCurrentWorkTask->mSource->getLocation(), false );
        travelCostCurrentTask = mWaveFrontMap->calculatePlanFrom ( mRobotPose );

        // figure out how far it is to the source of the potential new task
        mWaveFrontMap->calculateWaveFront ( task->mSource->getLocation(), false );
        travelCostNewTask = mWaveFrontMap->calculatePlanFrom ( mRobotPose );

        switchingCost = travelCostNewTask - travelCostCurrentTask;
        rewardRatio = mCurrentWorkTask->getExperiencedRewardRate() /
                      task->getExperiencedRewardRate();
        if ( rewardRatio < pow ( mBeta, switchingCost ) ) {
          mCurrentWorkTask = task;
        }
      }
    } // for

  }
  if ( oldTask != mCurrentWorkTask ) {
    mCountTaskSwitch ++;
    PRT_MSG3 ( 1, "Switchting Task from %s to %s  (%2d)", oldTask->getName(),
               mCurrentWorkTask->getName(), mCountTaskSwitch );
  }
}
//-----------------------------------------------------------------------------
void CSwCostComPolicyRobot::startupPolicy()
{
  //int index;

  if ( mRobotId <= 2 )
    mCurrentWorkTask = mWorkTaskVector[0];
  else
    mCurrentWorkTask = mWorkTaskVector[1];

  /*
    // select a task at random with uniform probability
    index = floor ( drand48() * mWorkTaskVector.size() );
    mCurrentWorkTask = & ( mWorkTaskVector[index] );
  */
  // select first charger
  mCurrentChargerDestination = mChargerDestinationList.front();
  if ( mCargoLoad == 0 )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_SINK;
}
//-----------------------------------------------------------------------------
void CSwCostComPolicyRobot::leaveChargerPolicy()
{
  if ( mCargoLoad == 0 )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_SINK;
}
//-----------------------------------------------------------------------------
void CSwCostComPolicyRobot::waitAtSourcePolicy()
{
}
//-----------------------------------------------------------------------------
void CSwCostComPolicyRobot::waitAtChargerPolicy()
{
}
//-----------------------------------------------------------------------------
void CSwCostComPolicyRobot::unallocatedPolicy()
{
}
//-----------------------------------------------------------------------------
void CSwCostComPolicyRobot::pickupCompletedPolicy()
{
}
//-----------------------------------------------------------------------------
void CSwCostComPolicyRobot::waitingAtPickupPolicy()
{
}
//-----------------------------------------------------------------------------


