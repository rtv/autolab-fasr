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
 * $Log: taskwidget.cpp,v $
 * Revision 1.3  2009-03-31 01:42:01  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.2  2009-03-26 04:09:46  jwawerla
 * Minor bug fixing
 *
 * Revision 1.1  2009-03-25 14:50:26  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "taskwidget.h"

//-----------------------------------------------------------------------------
CTaskWidget::CTaskWidget ( QWidget* parent )
    : QGroupBox ( parent )
{
  QVBoxLayout* layout = new QVBoxLayout ( this );

  mNumRobotDisplay = new CDataLine ( this, "#Robots" );
  layout->addWidget ( mNumRobotDisplay );

  mFiltRateDisplay = new CDataLine ( this, "Reward rate [$/s]" );
  mFiltRateDisplay->setFloatPrecision ( 5 );
  layout->addWidget ( mFiltRateDisplay );

  mRewardDisplay = new CDataLine ( this, "Reward [$]" );
  layout->addWidget ( mRewardDisplay );

  mSrcSinkDistDisplay = new CDataLine ( this, "Distance [m]" );
  layout->addWidget ( mSrcSinkDistDisplay );

  mProductionRateDisplay = new CDataLine ( this, "Prod. rate [1/s]" );
  mProductionRateDisplay->setFloatPrecision ( 3 );
  layout->addWidget ( mProductionRateDisplay );

  setLayout ( layout );

}
//-----------------------------------------------------------------------------
CTaskWidget::~CTaskWidget()
{
}
//-----------------------------------------------------------------------------
void CTaskWidget::updateData ( tTaskMsg* msg )
{
  setTitle ( QString ( "Task: " ) + QString ( msg->name ) );
  mNumRobotDisplay->setData ( msg->numRobotsServing );
  mRewardDisplay->setData( msg->reward );
  mFiltRateDisplay->setData ( msg->experiencedRewardRate );
  mSrcSinkDistDisplay->setData ( msg->srcSinkDistance );
  mProductionRateDisplay->setData ( msg->productionRate );
}
//-----------------------------------------------------------------------------



