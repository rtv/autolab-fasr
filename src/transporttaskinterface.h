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
#ifndef TRANSPORTTASKINTERFACE_H
#define TRANSPORTTASKINTERFACE_H

#include "pose2d.h"
#include "stage.hh"
#include "destination.h"

/**
 * Interface to a transportation task, this is what the robot actually knows
 * about the task
 * @author Jens Wawerla
 */
class ITransportTaskInterface
{
  public:
    /** Default destructor */
    virtual ~ITransportTaskInterface() {};
    /**
     * Gets the name of a task
     * @return task name
     */
    virtual std::string getName() = 0;
    /**
     * Gets the source destination
     * @return source destination
     */
    virtual CDestination* getSource() = 0;
    /**
     * Gets the sink destination
     * @return sink destination
     */
    virtual CDestination* getSink() = 0;
    /**
     * Gets the reward for the transported goods
     * @return [$]
     */
    virtual float getReward() const = 0;
    /**
     * Increments the task complete counter, this is used for statistical
     * purposes only
     */
    virtual void incrementTaskCompletedCount() = 0;

  protected:
    /** Default constructor */
    ITransportTaskInterface() {};
};

#endif
