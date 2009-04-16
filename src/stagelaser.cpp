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
 * $Log: stagelaser.cpp,v $
 * Revision 1.3  2009-04-03 15:10:02  jwawerla
 * *** empty log message ***
 *
 * Revision 1.2  2009-03-20 03:05:23  jwawerla
 * dynamic obstacles added to wavefront
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "stagelaser.h"
#include "utilities.h"
#include "error.h"

//-----------------------------------------------------------------------------
CStageLaser::CStageLaser ( Stg::ModelLaser* stgLaser )
    : ARangeFinder ( 0 )
{
  mStgLaser = stgLaser;
}
//-----------------------------------------------------------------------------
CStageLaser::~CStageLaser()
{
}
//-----------------------------------------------------------------------------
int CStageLaser::init()

{
  float beamBearing;
  float beamIncrement;
  Stg::stg_laser_cfg_t cfg;

  cfg = mStgLaser->GetConfig();
  mMaxRange = cfg.range_bounds.max;
  mMinRange = cfg.range_bounds.min;
  mNumSamples = cfg.sample_count;
  mRangeResolution = cfg.resolution;
  mFov = cfg.fov;

  // initialize data structures
  mRelativeBeamPose = new CPose[mNumSamples];
  mRangeData = new tRangeData[mNumSamples];

  mBeamConeAngle = 0; // set to zero to indicate it is a laser

  beamBearing = -mFov / 2.0;
  beamIncrement = mFov / float( mNumSamples - 1);

  for ( unsigned int i = 0; i < mNumSamples; i++ ) {
    mRelativeBeamPose[i].mX = 0;
    mRelativeBeamPose[i].mY = 0;
    //mRelativeBeamPose[i].mYaw = -1 * ( ( mFov / 2 ) - i * ( mFov / mNumSamples ) );
    mRelativeBeamPose[i].mYaw =  beamBearing;
    beamBearing += beamIncrement;
  }

  return 1; // success
}
//-----------------------------------------------------------------------------
void CStageLaser::update()
{
  uint32_t sampleCount;

  mRangeData = ( tRangeData* ) mStgLaser->GetSamples ( &sampleCount );

  if ( sampleCount != mNumSamples )
    PRT_ERR2("Got wrong number of laser readings from Stage, expected %d but "\
             "got %d",mNumSamples, sampleCount);
}
//-----------------------------------------------------------------------------


