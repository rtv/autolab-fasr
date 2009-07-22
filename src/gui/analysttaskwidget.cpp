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
#include "analysttaskwidget.h"
#include <assert.h>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
CAnalystTaskWidget::CAnalystTaskWidget(tAnalystTaskData* data, QWidget* parent)
 : QGroupBox(parent)
{
  assert(data);
  mTaskData = data;

  QHBoxLayout* layout = new QHBoxLayout( this );

  mNumAssignedRobotLine = new CDataLine( this, "Assigned # robots" );
  layout->addWidget( mNumAssignedRobotLine );

  setTitle( mTaskData->task->getName().c_str() );
  mOptNumWorkerLine = new CDataLine( this, "Opt. # robots" );
  layout->addWidget( mOptNumWorkerLine );

  mRTTLine =  new CDataLine( this, "Round trip time[s]" );
  layout->addWidget( mRTTLine );

  mProductioRateLine = new CDataLine(this, "Production rate");
  mProductioRateLine->setFloatPrecision(3);
  layout->addWidget(mProductioRateLine);

  mRewardLine =  new CDataLine(this, "Reward [$]");
  layout->addWidget(mRewardLine);

  mCompletedLine = new CDataLine( this, "Flags" );
  layout->addWidget( mCompletedLine );

  setLayout( layout );
}
//-----------------------------------------------------------------------------
CAnalystTaskWidget::~CAnalystTaskWidget()
{
}
//-----------------------------------------------------------------------------
void CAnalystTaskWidget::updateData()
{
  mOptNumWorkerLine->setData( mTaskData->optNumWorkers );
  mRTTLine->setData( mTaskData->roundTripTime );
  mNumAssignedRobotLine->setData( mTaskData->numRobots );
  mProductioRateLine->setData( mTaskData->task->getProductionRate() );
  mRewardLine->setData( mTaskData->task->getReward() );
  mCompletedLine->setData( mTaskData->task->getTaskCompletedCount() );
}
//-----------------------------------------------------------------------------



