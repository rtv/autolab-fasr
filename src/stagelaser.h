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
 * $Log: stagelaser.h,v $
 * Revision 1.2  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.1.1.1  2009-03-15 03:52:03  jwawerla
 * First commit
 *
 *
 **************************************************************************/
#ifndef STAGELASER_H
#define STAGELASER_H

#include <stage.hh>
#include "rangefinder.h"

/**
 * Wrapper class for a stage laser model to make it work with ND
 * @author Jens Wawerla (jwawerla@sfu.ca)
 */
class CStageLaser : public ARangeFinder
{
  public:
    /**
     * Default constructor
     * @param stgLaser stage model of laser
     */
    CStageLaser(Stg::ModelLaser* stgLaser);
    /** Default destructor */
    virtual ~CStageLaser();
    /**
     * This method gets called by the framework every step to update
     * the sensor data
     */
    virtual void update();
   /**
     * Initializes the device
     * @param return 1 if success 0 otherwise
     */
    virtual int init();
  private:
    /** Stage laser model */
    Stg::ModelLaser* mStgLaser;
};

#endif
