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
#include "robotschedulerdialog.h"
#include <assert.h>
#include <QSettings>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
CRobotSchedulerDialog::CRobotSchedulerDialog(QWidget* parent)
  :  CCustomDialog(parent)
{
  QSettings settings;

  QHBoxLayout* layout = new QHBoxLayout(this);
  mScheduler = CRobotScheduler::getInstance();
  mLayout = new QVBoxLayout( this );

  mGeneralBox = new QGroupBox("General", this);
  mGeneralBox->setLayout( layout );
  mLayout->addWidget( mGeneralBox );

  mReassignmentLine = new CDataLine(mGeneralBox, "Reassignments");
  layout->addWidget(mReassignmentLine);

  setLayout( mLayout );
  show();
  setWindowTitle( "Robot Scheduler" );

  resize( settings.value( "robotSchedulerDialog/size" ).toSize() );
  move( settings.value( "robotSchedulerDialog/pos" ).toPoint() );
}
//-----------------------------------------------------------------------------
CRobotSchedulerDialog::~CRobotSchedulerDialog()
{
}
//-----------------------------------------------------------------------------
void CRobotSchedulerDialog::closeEvent( QCloseEvent* event )
{
  QSettings settings;

  event->accept();

  settings.setValue( "robotSchedulerDialog/size", size() );
  settings.setValue( "robotSchedulerDialog/pos", pos() );
}
//-----------------------------------------------------------------------------
void CRobotSchedulerDialog::update()
{
  CTaskDataWidget* widget;


  if ( mTaskDataWidgetList.size() != mScheduler->mTaskList.size() ) {
    for ( unsigned int i = 0; i < mTaskDataWidgetList.size(); i++ ) {
      delete mTaskDataWidgetList[i];
    }
    mTaskDataWidgetList.clear();

    for ( unsigned int i = 0; i < mScheduler->mTaskList.size(); i++ ) {
      widget = new CTaskDataWidget( &( mScheduler->mTaskList[i] ), this );
      mTaskDataWidgetList.push_back( widget );
      mLayout->addWidget( widget );
    }
  }

  for ( unsigned int i = 0; i < mTaskDataWidgetList.size(); i++ ) {
    mTaskDataWidgetList[i]->updateData();
  }

  mReassignmentLine->setData(mScheduler->mNumReassignments);
}
//-----------------------------------------------------------------------------


