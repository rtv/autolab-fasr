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
#ifndef IROBOTSCHEDULERINTERFACE_H
#define IROBOTSCHEDULERINTERFACE_H

/**
 * Interface for a robot to register with and access the robot scheduler
 * @author Jens Wawerla
*/
class IRobotSchedulerInterface
{
  public:
    /** Default destructor */
    virtual ~IRobotSchedulerInterface() {};
    /**
     * Gets the ID of the robot
     * @return robot id
     */
    virtual unsigned int getRobotId() const  = 0;
    /**
     * Gets the capacity of the cargo bay
     * return [flags]
     */
    virtual unsigned int getCargoBayCapacity() const = 0;
    /**
     * Gets the current task of the robot
     * @return task
     */
    virtual ITransportTaskInterface* getTask() const = 0;

  protected:
    /** Default constructor */
    IRobotSchedulerInterface() {};

};

#endif
