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
 * $Log: baserobotwidget.h,v $
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
 **************************************************************************/
#ifndef BASEROBOTWIDGET_H
#define BASEROBOTWIDGET_H

#include <QtGui>
#include "robotvisprotocol.h"
#include "dataline.h"
#include "dataled.h"

/**
 * A visualization widget for a baserobot be be displayed by RobotVis
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CBaseRobotWidget : public QWidget
{
  Q_OBJECT

  public:
    /**
     * Default constructor
     * @param robotId id of robot
     * @param parent widget
     */
    CBaseRobotWidget ( int robotId,  QWidget* parent = NULL );
    /** Default destructor */
    ~CBaseRobotWidget();
    /**
     * Update the widget with data from msg
     * @param msg data to update the widget
     */
    void udateData ( tDataMsg* msg );

  signals:
    /**
     * Signal to pauses or unpauses a robot
     * @param id of robot
     * @param pause, true to pause, false to unpause
     */
    void pauseRobot(int id, bool pause);

  private slots:
    /** Slot for the pause button */
    void pauseButtonToggled(bool checked );

  private:
    /** ID of robot */
    int mRobotId;
    /** Group box for reward related information */
    QGroupBox* mTaskBox;
    /** Group box for drive train related information */
    QGroupBox* mDriveTrainBox;
    /** Group box for general purpose information */
    QGroupBox* mGeneralPurposeBox;
    /** Displays the fsm state */
    CDataLine* mFsmDisplay;
    /** Button to pause the robot */
    QPushButton* mPauseButton;
    CDataLine* mRewardDisplay;
    CDataLine* mTotalRewardDisplay;
    CDataLine* mNumTasksCompletedDisplay;
    CDataLine* mTaskCompletionTimeDisplay;
    CDataLine* mTaskNameDisplay;
    CDataLine* mTravelTimeDisplay;
    CDataLine* mTranslationalSpeedCmdDisplay;
    CDataLine* mRotationalSpeedCmdDisplay;
    CDataLine* mBatteryLevelDisplay;
    CDataLine* mCargoLoadDisplay;
    CDataLed* mChargingLed;
    CDataLed* mStalledLed;
    CDataLed* mPowerLed;
};

#endif
