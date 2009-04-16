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
 * $Log: replanpolicyrobotinterface.h,v $
 * Revision 1.3  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.2  2009-03-28 21:53:57  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1  2009-03-28 00:54:39  jwawerla
 * *** empty log message ***
 *
 *
 **************************************************************************/
#ifndef REPLANPOLICYROBOT_INTERFACE_H
#define REPLANPOLICYROBOT_INTERFACE_H

#include "baserobot.h"

/**
 * Implements a replanning policy, this robot replans the optimal task
 * allocation after completion of each task using the latest experience.
 * @author Jens Wawerla <jwawerla@sfu.ca>
*/
class IReplanPolicyRobotInterface
{
  public:
    /** Default destructor */
    virtual ~IReplanPolicyRobotInterface() {};
    /** Checks if this robot is working or not */
    virtual bool isWorking() = 0;
    /**
     * Gets the currently assigned task
     * @return task
     */
    virtual IWorkTaskRobotInterface* getTask() = 0;
    /** Checks if a robot is in state unallocated */
    virtual bool isUnAllocated() = 0;
    /** Gets the id of the robot */
    virtual int getRobotId() = 0;
    /**
     * Gets the size of the cargo bay
     * @return [flags]
     */
    virtual int getCargoBayCapacity() = 0;

  protected:
    /**
     * Default constructor
     * @param modPos stage position model
     * @param map wave front map of the world used for navigation
     */
    IReplanPolicyRobotInterface() {};


  private:


};

#endif
