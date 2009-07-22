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
#include "analystdialog.h"

//-----------------------------------------------------------------------------
CAnalystDialog::CAnalystDialog( QWidget* parent )
    : CCustomDialog( parent )
{
  QSettings settings;

  QHBoxLayout* layout = new QHBoxLayout( this );
  mAnalyst = CAnalyst::getInstance();
  mLayout = new QVBoxLayout( this );

  mGeneralBox = new QGroupBox( "General", this );
  mGeneralBox->setLayout( layout );
  mLayout->addWidget( mGeneralBox );

  mReassignmentLine = new CDataLine( mGeneralBox, "Reassignments" );
  layout->addWidget( mReassignmentLine );

  mNumDepotLine = new CDataLine( mGeneralBox, "Unallocated" );
  layout->addWidget( mNumDepotLine );

  mNumChargingLine = new CDataLine( mGeneralBox, "Charging" );
  layout->addWidget( mNumChargingLine );

  mTotalRewardLine = new CDataLine( mGeneralBox, "Reward [$]" );
  layout->addWidget( mTotalRewardLine );

  mTotalFlagLine = new CDataLine( mGeneralBox, "Flags" );
  layout->addWidget( mTotalFlagLine );

  mTotalEnergyLine = new CDataLine( mGeneralBox, "Energy [Wh]" );
  layout->addWidget( mTotalEnergyLine );

  setLayout( mLayout );
  show();
  setWindowTitle( "Analyst" );

  resize( settings.value( "analystDialog/size" ).toSize() );
  move( settings.value( "analystDialog/pos" ).toPoint() );
}
//-----------------------------------------------------------------------------
CAnalystDialog::~CAnalystDialog()
{
}
//-----------------------------------------------------------------------------
void CAnalystDialog::closeEvent( QCloseEvent* event )
{
  QSettings settings;

  event->accept();

  settings.setValue( "robotSchedulerDialog/size", size() );
  settings.setValue( "robotSchedulerDialog/pos", pos() );
}
//-----------------------------------------------------------------------------
void CAnalystDialog::update()
{
  CAnalystTaskWidget* widget;

  if ( mTaskDataWidgetList.size() != mAnalyst->mTaskList.size() ) {
    for ( unsigned int i = 0; i < mTaskDataWidgetList.size(); i++ ) {
      delete mTaskDataWidgetList[i];
    }
    mTaskDataWidgetList.clear();

    for ( unsigned int i = 0; i < mAnalyst->mTaskList.size(); i++ ) {
      widget = new CAnalystTaskWidget( mAnalyst->mTaskList[i], this );
      mTaskDataWidgetList.push_back( widget );
      mLayout->addWidget( widget );
    }
  }

  for ( unsigned int i = 0; i < mTaskDataWidgetList.size(); i++ ) {
    mTaskDataWidgetList[i]->updateData();
  }

  //mReassignmentLine->setData(mScheduler->mNumReassignments);
  mNumChargingLine->setData( mAnalyst->mNumRobotCharging );
  mNumDepotLine->setData( mAnalyst->mNumRobotsInDepot );
  mTotalRewardLine->setData( mAnalyst->mTotalReward );
  mTotalFlagLine->setData( mAnalyst->mTotalNumFlags );
  mTotalEnergyLine->setData( mAnalyst->mTotalEnergy );
}
//-----------------------------------------------------------------------------

