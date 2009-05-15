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
 * $Log: destination.h,v $
 * Revision 1.8  2009-03-31 23:52:59  jwawerla
 * Moved cell index from float to int math
 *
 * Revision 1.7  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.6  2009-03-24 01:04:46  jwawerla
 * switching cost robot added
 *
 * Revision 1.5  2009-03-21 02:31:50  jwawerla
 * *** empty log message ***
 *
 * Revision 1.4  2009-03-20 03:05:23  jwawerla
 * dynamic obstacles added to wavefront
 *
 * Revision 1.3  2009-03-17 03:11:30  jwawerla
 * Another day no glory
 *
 * Revision 1.2  2009-03-16 14:27:18  jwawerla
 * robots still get stuck
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 **************************************************************************/
#ifndef CDESTINATION_H
#define CDESTINATION_H

#include <glib.h>
#include <assert.h>
#include <list>
#include "RapiStage"
#include "robotwaitinterface.h"


/** Spacing between robots in the waiting queue [m] */
const float WAITING_SPACING = 1.0;
/** Threshold for closeness to the destination [m] */
const float CLOSENESS_THRESHOLD = 1.5;
/** Upper limit on the distance to call it "near" [m] */
const float MAX_CLOSENESS_DISTANCE = 4.0;

using namespace Rapi;

/**
 * A destination like a source, sink etc.
 */
class CDestination {
  public:
     /**
      * Default constructor
      * @param x coordinate
      * @param y coordinate
      * @param queueDirection direction of waiting queue
      * @param name of destination
      */
    CDestination(double x, double y, double queueDirection,
                 const char* name=NULL);
     /**
      * Constructor from a stage model
      * @param mod stage model
      */
    CDestination(Stg::Model* mod);
     /**
      * constructor
      * @param name of destination
      */
    CDestination( const char* name=NULL);
    /**
     * Default destructor
     */
    ~CDestination();
    /**
     * Gets the coordinates of location
     * @return point of desination
     */
    CPoint2d getLocation();
    /**
     * Set location
     *  @param point location [m][m]
     */
    void setLocation(CPoint2d point);
    /**
     * Set location
     * @param x [m]
     * @param y [m]
     */
    void setLocation(float x, float y);
    /** Locks the mutex */
    void lock();
    /** unlocks the mutex */
    void unlock();
    /**
     * Adds a robot to the waiting queue for this destination
     * @param robot to be added
     */
    void enterWaitingQueue(IRobotWaitInterface* robot);
    /**
     * Gets the geographical position in the waiting queue
     * @param robot that requests its position
     * @return waiting location and rank
     */
    int getWaitingPosition(IRobotWaitInterface* robot, CPose2d &pos);
    /**
     * Checks if the robot is near the destination
     * @param pose to check
     * @return true if near, false otherwise
     */
    bool isNear( CPose2d pose );
    /**
     * Gets the distance to the location from a given position
     * @param pose to get distance to
     * @return distance [m]
     */
    float getDistance( CPose2d pose );
    /**
     * Causes a robot to leave the waiting queue by removing the robot from
     * waiting queue
     * @param robot to remove
     */
    void leaveWaitingQueue(IRobotWaitInterface* robot);
    /**
     * Gets the pose of the destination
     * @return pose
     */
    CPose2d getPose();
    /**
     * Gets the name of the destination
     * @return name
     */
    char* getName();
    /**
     * Sets the direction of the waiting queue
     * @param dir [rad]
     */
    void setQueueDirection(float dir );
    /**
     * Gets the direction of the waiting queue
     * @return [rad]
     */
    float getQueueDirection() { return mQueueDirection; };

  protected:
    /** Stage model */
    Stg::Model* mStgModel;

  private:
    /** Name of destination */
    char mName[20];
    /** Location */
    CPoint2d mLocation;
    /** Access mutex */
    GMutex* mAccessMutex;
    /** Waiting queue for robots */
    std::list<IRobotWaitInterface*> mWaitingQueue;
    /** Direction of waiting queue [rad] */
    float mQueueDirection;
};

#endif
