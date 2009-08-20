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
#include "taskdatawidget.h"
#include <QHBoxLayout>
#include <assert.h>

//-----------------------------------------------------------------------------
CTaskDataWidget::CTaskDataWidget( tTaskData* data, QWidget* parent )
    : QGroupBox( parent )
{
  assert( data );
  mTaskData = data;
  QHBoxLayout* layout = new QHBoxLayout( this );

  setTitle( mTaskData->task->getName().c_str() );
  mOptNumWorkerLine = new CDataLine( this, "Opt. # robots" );
  layout->addWidget( mOptNumWorkerLine );

  mRTTLine =  new CDataLine( this, "Round trip time[s]" );
  layout->addWidget( mRTTLine );

  mNumAssignedRobotLine = new CDataLine( this, "Assigned # robots" );
  layout->addWidget( mNumAssignedRobotLine );

  mProductioRateLine = new CDataLine( this, "Production rate" );
  mProductioRateLine->setFloatPrecision( 3 );
  layout->addWidget( mProductioRateLine );

  mRewardLine = new CDataLine( this, "Reward [$]" );
  layout->addWidget( mRewardLine );

  mCompletedLine = new CDataLine( this, "Flags" );
  layout->addWidget( mCompletedLine );

  setLayout( layout );
}
//-----------------------------------------------------------------------------
CTaskDataWidget::~CTaskDataWidget()
{
}
//-----------------------------------------------------------------------------
void CTaskDataWidget::updateData()
{
  mOptNumWorkerLine->setData( mTaskData->optNumWorkers );
  mRTTLine->setData( mTaskData->roundTripTime );
  mNumAssignedRobotLine->setData( (int)mTaskData->assignedRobotList.size() );
  mProductioRateLine->setData( mTaskData->task->getProductionRate() );
  mRewardLine->setData( mTaskData->task->getReward() );
  mCompletedLine->setData( mTaskData->task->getTaskCompletedCount() );
}
//-----------------------------------------------------------------------------
