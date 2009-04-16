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
 * $Log: looppolicyrobot.h,v $
 * Revision 1.5  2009-04-03 16:21:30  jwawerla
 * Move wavefront map specification to world file
 *
 * Revision 1.4  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.3  2009-03-27 00:27:37  jwawerla
 * *** empty log message ***
 *
 * Revision 1.2  2009-03-20 03:05:23  jwawerla
 * dynamic obstacles added to wavefront
 *
 * Revision 1.1.1.1  2009-03-15 03:52:03  jwawerla
 * First commit
 *
 *
 **************************************************************************/

#ifndef LOOPPOLICYROBOT_H
#define LOOPPOLICYROBOT_H

#include "baserobot.h"

/**
 * This robot alternates the work tasks by looping over the list of available
 * task and executing each once and at the end of the list begin at the front
 * of the list
 * @author
*/
class CLoopPolicyRobot : public ABaseRobot
{
  public:
    /**
     * Default constructor
     * @param modPos stage position model
     */
    CLoopPolicyRobot( Stg::ModelPosition* modPos );
    /** Default destructor */
    ~CLoopPolicyRobot();
    /** Chooses the next work task according to some policy */
    virtual void selectWorkTaskPolicy();
    /** Policy for start up */
    virtual void startupPolicy();
    /** Policy for leaving a charger */
    virtual void leaveChargerPolicy();
    /** Policy for waiting at source */
    virtual void waitAtSourcePolicy();
    /** Policy for waiting at charger */
    virtual void waitAtChargerPolicy();
    /** Policy for the first unallocated robot */
    virtual void unallocatedPolicy();
    /** Policy for a successfull pick up */
    virtual void pickupCompletedPolicy();
    /** Policy for waiting at the pickup patch, note not in the queue */
    virtual void waitingAtPickupPolicy();
};

#endif
