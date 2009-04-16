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
 * $Log: robotvisinterface.h,v $
 * Revision 1.3  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.2  2009-03-24 01:04:46  jwawerla
 * switching cost robot added
 *
 * Revision 1.1  2009-03-23 06:24:21  jwawerla
 * RobotVis works now for data, maps still need more work
 *
 *
 **************************************************************************/

#ifndef ROBOT_VIS_INTERFACE_H
#define ROBOT_VIS_INTERFACE_H

#include "robotvisprotocol.h"

class IRobotVis
{
  public:
     /** Default destructor */
    virtual ~IRobotVis() {};
    /**
     * Get the data from a robot to send to robotVis
     * @param id of robot
     * @param buffer to fill with data
     * @param len of buffer
     */
    virtual void getRobotVisData( int id, char* buffer, int &len ) = 0;
    /**
     * Gets the map from a robot to send it to robotVis
     * @param msg general information about map
     * @param buffer with map data
     * @param size of buffer [bytes]
     */
    virtual void getRobotVisMapData ( tMapMsg* msg, char* buffer, int& size) = 0;
    /**
     * Sets a command message received from RobotVis
     * @param msg
     */
    virtual void setRobotVisCommandMsg( tCommandMsg msg) = 0;
    /**
     * Gets the type of the controller
     * @return controller type
     */
    virtual tControllerType getControllerType() { return DEFAULT_CTRL; };

  protected:
     /** Default constructor */
    IRobotVis() {};
};

#endif
