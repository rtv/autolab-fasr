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
#ifndef DESTINATION_H
#define DESTINATION_H

#include "stage.hh"
#include "pose2d.h"
#include "robotctrl.h"
#include <list>
#include <string>

/** Spacing between robots in the waiting queue [m] */
const float WAITING_SPACING = 1.0;
/** Threshold for closeness to the destination [m] */
const float CLOSENESS_THRESHOLD = 1.5;
/** Upper limit on the distance to call it "near" [m] */
const float MAX_CLOSENESS_DISTANCE = 4.0;

using namespace Rapi;
/**
 * A class for sink or source destinations
 * @author Jens Wawerla
 */
class CDestination {

  public:
    /** Default constructor */
    CDestination(Stg::Model* model);
    /** Default destructor */
    ~CDestination();
    /**
     * Location of destination
     * @return location
     */
    CPose2d getLocation();
    /**
     * Makes the robot enter the waiting queue if it is not already in the queue
     * @param robotCtrl to enter queue
     */
    void enterQueue(ARobotCtrl* robotCtrl);
    /**
     * Removes a robot controller from the queue
     * @param robotCtrl to leave the queue
     */
    void leaveQueue ( ARobotCtrl* robotCtrl );
    /**
     * Gets the geographical position in the waiting queue
     * @param robot that requests its position
     * @return waiting location and ran
     */
    int getQueuePosition(ARobotCtrl* robotCtrl, CPose2d &pose);
    /**
     * Pushes a flag
     * @param flag
     */
    void pushFlag(Stg::Flag* flag);
    /**
     * Pops a flag 
     * @return flag
     */
    Stg::Flag* popFlag();
    /**
     * Checks if flags are available
     * @return true if avaible, false otherwise
     */
    bool hasNumFlags();
    /**
     * Gets the name of the destination
     * @return name
     */
    std::string getName() { return mName; };
    /**
     * Checks if the pose is near the end of the queue
     * @return true if is near
     */
    bool isNearEofQueue ( CPose2d pose );

  protected:
    /** Name of destination */
    std::string mName;
    /** Stage model */
    Stg::Model* mStgModel;
    /** Location of destination */
    CPose2d mLocation;
    /** Waiting queue */
    std::list<ARobotCtrl*> mWaitingQueue;
    /** Direction of waiting queue [rad] */
    float mQueueDirection;
};

#endif
