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
 * $Log: charger.h,v $
 * Revision 1.2  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.1  2009-03-27 01:32:58  jwawerla
 * wait prob controller improved
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 **************************************************************************/
#ifndef CCHARGER_H
#define CCHARGER_H

#include "destination.h"

/**
 * A class to handle the information about a charger
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CCharger : public CDestination
{
  public:
    /**
     * Default constructor
     * @param x coordinate [m]
     * @param y coordinate [m]
     * @param queueDir direction of queue [rad]
     * @param chargeRate rate at which this charger charges a robot
     * @param name of device
     */
    CCharger(float x, float y, float queueDir, float chargeRate,
             const char* name=NULL);
     /**
      * Constructor from a stage model
      * @param mod stage model
      */
    CCharger(Stg::Model* mod);
    /** Default destructor */
    ~CCharger();
    /**
     * Gets the charge rate
     * @return [J/s]
     */
    float getChargeRate() { return mChargeRate; };

  private:
    /** Rate at which to give energy [J/s] */
    float mChargeRate;

};

#endif
