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
 * $Log: robotwaitinterface.h,v $
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 **************************************************************************/

#ifndef ROBOT_WAIT_INTERFACE_H
#define ROBOT_WAIT_INTERFACE_H

#include "common.h"

class IRobotWaitInterface
{
  public:
    /** Default destructor */
    virtual ~IRobotWaitInterface() {};
    /**
     * Gets the name of the robot
     * @return name of robot
     */
    virtual char* getName() = 0;
    /**
     * Gets the curren position of the robot
     * @return robot position
     */
    virtual CPose getPosition() = 0;

  protected:
    /** Default constructor */
    IRobotWaitInterface() {};
};

#endif
