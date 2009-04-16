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
 * $Log: individualswcostpolicyrobot.cpp,v $
 * Revision 1.5  2009-04-03 16:21:30  jwawerla
 * Move wavefront map specification to world file
 *
 * Revision 1.4  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.3  2009-03-27 01:32:58  jwawerla
 * wait prob controller improved
 *
 * Revision 1.2  2009-03-27 00:27:36  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1  2009-03-26 04:10:46  jwawerla
 * *** empty log message ***
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
#include "individualswcostpolicyrobot.h"

/** Probability of exploring, basically epsilon-greedy */
const float EXPLORE_PROBABILITY = 0.1;
/** Probability of not exploiting but just being conservative and sticking
 * with the old task
 */
const float NOT_EXPLOIT_PROBABILTY = 0.0;
//-----------------------------------------------------------------------------
CIndividualSwCostPolicyRobot::CIndividualSwCostPolicyRobot ( Stg::ModelPosition* modPos)
    : ABaseRobot ( modPos )
{
  mBeta = 0.98;
  mCountTaskSwitch = 0;
  mCountCurrentTask = 0;
  mExperienceVector.clear();
}
//-----------------------------------------------------------------------------
CIndividualSwCostPolicyRobot::~CIndividualSwCostPolicyRobot()
{
}
//-----------------------------------------------------------------------------
void CIndividualSwCostPolicyRobot::selectWorkTaskPolicy()
{
  double travelCostNewTask;
  double travelCostCurrentTask;
  double rewardRatio;
  double switchingCost;
  IWorkTaskRobotInterface* task;
  IWorkTaskRobotInterface* oldTask = mCurrentWorkTask;
  std::vector<IWorkTaskRobotInterface*>::iterator it;

  if ( mCountCurrentTask > 1 ) {
    // update our experiences
    mExperienceVector[mTaskIndex].filtRewardRate =
      mExperienceVector[mTaskIndex].filtRewardRate  + 0.3 * ( mRewardRate -
          mExperienceVector[mTaskIndex].filtRewardRate );

    // epsilon exploration
    if ( drand48() < EXPLORE_PROBABILITY ) {
      mTaskIndex = mTaskIndex + 1;
      if ( mTaskIndex >= mWorkTaskVector.size() )
        mTaskIndex = 0;
      mCurrentWorkTask = mWorkTaskVector[mTaskIndex];
    }
    else {
      for ( unsigned int i = 0; i < mWorkTaskVector.size(); i++ ) {
        task = mWorkTaskVector[i];

        rprintf("experience %d %f \n", i, mExperienceVector[i].filtRewardRate);
        if ( mExperienceVector[i].filtRewardRate  != 0 ) {
          // figure out how far it is to the source of the current task
          mWaveFrontMap->calculateWaveFront ( mCurrentWorkTask->mSource->getLocation(), false );
          travelCostCurrentTask = mWaveFrontMap->calculatePlanFrom ( mRobotPose );

          // figure out how far it is to the source of the potential new task
          mWaveFrontMap->calculateWaveFront ( task->mSource->getLocation(), false );
          travelCostNewTask = mWaveFrontMap->calculatePlanFrom ( mRobotPose );

          switchingCost = travelCostNewTask - travelCostCurrentTask;
          rewardRatio = mExperienceVector[mTaskIndex].filtRewardRate /
                        mExperienceVector[i].filtRewardRate;
          if ( rewardRatio < pow ( mBeta, switchingCost ) ) {
            mCurrentWorkTask = task;
            mTaskIndex = i;
            mCountCurrentTask = 0;
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
  mCountCurrentTask ++;
}
//-----------------------------------------------------------------------------
void CIndividualSwCostPolicyRobot::startupPolicy()
{
  tExperience experience;

  if ( mRobotId <= 2 )
    mTaskIndex = 0;
  else
    mTaskIndex = 1;

  mCurrentWorkTask = mWorkTaskVector[mTaskIndex];

  for ( unsigned int i = 0; i < mWorkTaskVector.size(); i++ ) {
    experience.filtRewardRate = 0.0;
    mExperienceVector.push_back( experience ) ;
  }

  // select first charger
  mCurrentChargerDestination = mChargerDestinationList.front();
  if ( mCargoLoad == 0 )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_SINK;
}
//-----------------------------------------------------------------------------
void CIndividualSwCostPolicyRobot::leaveChargerPolicy()
{
  if ( mCargoLoad == 0 )
    mState = GOTO_SOURCE;
  else
    mState = GOTO_SINK;
}
//-----------------------------------------------------------------------------
void CIndividualSwCostPolicyRobot::waitAtSourcePolicy()
{
}
//-----------------------------------------------------------------------------
void CIndividualSwCostPolicyRobot::waitAtChargerPolicy()
{
}
//-----------------------------------------------------------------------------
void CIndividualSwCostPolicyRobot::unallocatedPolicy()
{
}
//-----------------------------------------------------------------------------
void CIndividualSwCostPolicyRobot::pickupCompletedPolicy()
{
}
//-----------------------------------------------------------------------------
void CIndividualSwCostPolicyRobot::waitingAtPickupPolicy()
{
}
//-----------------------------------------------------------------------------

