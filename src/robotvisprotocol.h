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
 * $Log: robotvisprotocol.h,v $
 * Revision 1.7  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.6  2009-03-29 00:54:27  jwawerla
 * Replan ctrl seems to work now
 *
 * Revision 1.5  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.4  2009-03-25 14:49:02  jwawerla
 * Task visualization works now
 *
 * Revision 1.3  2009-03-24 03:52:22  jwawerla
 * Pause button works now
 *
 * Revision 1.2  2009-03-24 01:04:46  jwawerla
 * switching cost robot added
 *
 * Revision 1.1  2009-03-23 06:24:21  jwawerla
 * RobotVis works now for data, maps still need more work
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 **************************************************************************/


#ifndef ROBOT_VIS_PROTOCOL_H
#define ROBOT_VIS_PROTOCOL_H

  const char STARTBYTE_0 = 'A';
  const char STARTBYTE_1 = 'A';

typedef enum {CONFIG_MSG, REQ_MSG, CMD_MSG, DATA_MSG, TASK_MSG, MAP_MSG, MAX_MSG_TYPE} tVisMessageType;
typedef enum {DATA_REQ, MAP_REQ, TASK_REQ} tReqType;
typedef enum {DEFAULT_CTRL, WAITPROB_CTRL} tControllerType;

typedef struct {
  char startByte0;
  char startByte1;
  tVisMessageType msgType;
  unsigned short robotId;
  unsigned short size;
} tHeader;

typedef struct {
  /** Type of controller */
  tControllerType controllerType;
  /** Number of robots available */
  unsigned short numRobots;
  /** Number of tasks available */
  unsigned short numTasks;
} tConfigMsg;

typedef struct {
  /** pauses robot */
  bool fgPause;
} tCommandMsg;

typedef struct {
  /** Type of request */
  tReqType reqType;
} tReqMsg;

typedef struct {
  unsigned short id;
  char fsmText[20];
  char taskName[20];
  float batteryLevel;
  float travelTime;
  bool fgCharging;
  bool fgStalled;
  bool fgPoweredUp;
  float cargoLoad;
  float rewardRate;
  float taskCompletionTime;
  float reward;
  float totalReward;
  unsigned short numTasksCompleted;
  float translationalSpeedCmd;
  float rotationalSpeedCmd;
} tDataMsg;

typedef struct {
  unsigned short id;
  char name[20];
  unsigned short numRobotsServing;
  float experiencedRewardRate;
  float reward;
  float productionRate;
  float srcSinkDistance;
} tTaskMsg;

typedef struct {
  unsigned short width;
  unsigned short height;
  unsigned short numWaypoints;
} tMapMsg;

#endif
