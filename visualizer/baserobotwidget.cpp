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
 * $Log: baserobotwidget.cpp,v $
 * Revision 1.5  2009-03-29 00:54:27  jwawerla
 * Replan ctrl seems to work now
 *
 * Revision 1.4  2009-03-25 14:49:03  jwawerla
 * Task visualization works now
 *
 * Revision 1.3  2009-03-24 03:52:22  jwawerla
 * Pause button works now
 *
 * Revision 1.2  2009-03-23 06:24:22  jwawerla
 * RobotVis works now for data, maps still need more work
 *
 * Revision 1.1  2009-03-16 14:27:19  jwawerla
 * robots still get stuck
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "baserobotwidget.h"
#include "utilities.h"
#include "unicodechar.h"

//-----------------------------------------------------------------------------
CBaseRobotWidget::CBaseRobotWidget (int robotId, QWidget* parent )
    : QWidget ( parent )
{
  mRobotId = robotId;

  QVBoxLayout* layout = new QVBoxLayout ( this );

  mFsmDisplay = new CDataLine ( this, "FSM" );
  layout->addWidget ( mFsmDisplay );

  //********************************************************
  // Task
  mTaskBox = new QGroupBox ( "Task", this );
  layout->addWidget ( mTaskBox );
  QHBoxLayout* rewardLayout = new QHBoxLayout ( mTaskBox );

  mRewardDisplay = new CDataLine ( mTaskBox, "Reward" );
  rewardLayout->addWidget ( mRewardDisplay );

  mTotalRewardDisplay = new CDataLine ( mTaskBox, "Sum Reward" );
  rewardLayout->addWidget ( mTotalRewardDisplay );

  mNumTasksCompletedDisplay = new CDataLine ( mTaskBox, "#Tasks" );
  rewardLayout->addWidget ( mNumTasksCompletedDisplay );

  mTaskCompletionTimeDisplay = new CDataLine ( mTaskBox, "Tasks time" );
  rewardLayout->addWidget ( mTaskCompletionTimeDisplay );

  mTaskNameDisplay = new CDataLine ( mTaskBox, "Task" );
  rewardLayout->addWidget ( mTaskNameDisplay );

  mTravelTimeDisplay = new CDataLine( mTaskBox, "Travel time [s]");
  rewardLayout->addWidget (mTravelTimeDisplay);

  mTaskBox->setLayout ( rewardLayout );

  //********************************************************
  // Drivetrain
  mDriveTrainBox = new QGroupBox ( "Drive train", this );
  layout->addWidget ( mDriveTrainBox );
  QHBoxLayout* drivetrainLayout = new QHBoxLayout ( mDriveTrainBox );

  mTranslationalSpeedCmdDisplay = new CDataLine ( mDriveTrainBox, "Speed [m/s]" );
  drivetrainLayout->addWidget ( mTranslationalSpeedCmdDisplay );

  mRotationalSpeedCmdDisplay = new CDataLine ( mDriveTrainBox, "Turnrate [" + Q_DEGREE + "/s]" );
  drivetrainLayout->addWidget ( mRotationalSpeedCmdDisplay );

  mBatteryLevelDisplay = new CDataLine ( mDriveTrainBox, "Battery [%]" );
  drivetrainLayout->addWidget ( mBatteryLevelDisplay );

  mPowerLed = new CDataLed ( mDriveTrainBox, "Power" );
  drivetrainLayout->addWidget ( mPowerLed );

  mChargingLed = new CDataLed ( mDriveTrainBox, "Charging" );
  drivetrainLayout->addWidget ( mChargingLed );

  mStalledLed = new CDataLed ( mDriveTrainBox, "Stalled" );
  drivetrainLayout->addWidget ( mStalledLed );

  mPauseButton = new QPushButton("Pause", mDriveTrainBox);
  mPauseButton->setCheckable(true);
  connect(mPauseButton, SIGNAL(toggled(bool)), this, SLOT(pauseButtonToggled(bool)) );
  drivetrainLayout->addWidget ( mPauseButton );

  //********************************************************
  // General purpose
  mGeneralPurposeBox = new QGroupBox ( "", this );
  layout->addWidget ( mGeneralPurposeBox );
  QHBoxLayout* generalPurposeLayout = new QHBoxLayout ( mGeneralPurposeBox );

  mCargoLoadDisplay = new CDataLine ( mGeneralPurposeBox, "Cargo [%]" );
  generalPurposeLayout->addWidget ( mCargoLoadDisplay );

  setLayout ( layout );
}
//-----------------------------------------------------------------------------
CBaseRobotWidget::~CBaseRobotWidget()
{
}
//-----------------------------------------------------------------------------
void CBaseRobotWidget::pauseButtonToggled(bool checked )
{
  emit pauseRobot( mRobotId, checked );
}
//-----------------------------------------------------------------------------
void CBaseRobotWidget::udateData ( tDataMsg* msg )
{
  mFsmDisplay->setData ( msg->fsmText );
  mRewardDisplay->setData ( msg->reward );
  mTotalRewardDisplay->setData ( msg->totalReward );
  mNumTasksCompletedDisplay->setData ( msg->numTasksCompleted );
  mTaskCompletionTimeDisplay->setData ( msg->taskCompletionTime );
  mTravelTimeDisplay->setData( msg->travelTime );
  mTaskNameDisplay->setData( msg->taskName );
  mTranslationalSpeedCmdDisplay->setData ( msg->translationalSpeedCmd );
  mRotationalSpeedCmdDisplay->setData ( R2D ( msg->rotationalSpeedCmd ) );
  mBatteryLevelDisplay->setData ( msg->batteryLevel * 100.0 );
  mCargoLoadDisplay->setData ( msg->cargoLoad * 100.0);
  if ( msg->fgCharging )
    mChargingLed->setData ( CDataLed::GREEN_ON );
  else
    mChargingLed->setData ( CDataLed::GREEN_OFF );

  if ( msg->fgPoweredUp )
    mPowerLed->setData ( CDataLed::GREEN_ON );
  else
    mPowerLed->setData ( CDataLed::GREEN_OFF );

  if ( msg->fgStalled )
    mStalledLed->setData ( CDataLed::RED_ON );
  else
    mStalledLed->setData ( CDataLed::RED_OFF );
}
//-----------------------------------------------------------------------------



