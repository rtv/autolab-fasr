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
#ifndef ROBOTSCHEDULERDIALOG_H
#define ROBOTSCHEDULERDIALOG_H


#include "customdialog.h"
#include "robotscheduler.h"
#include "taskdatawidget.h"
#include <vector>
#include <QtGui>

using namespace Rapi;

/**
 * A visualization dialog for the robot scheduler
 * @author Jens Wawerla
 */
class CRobotSchedulerDialog : public CCustomDialog
{
  public:
    /**
     * Default constructor
     * @param parent qt widget
     */
    CRobotSchedulerDialog(QWidget* parent = NULL);
    /** Default destructor */
    ~CRobotSchedulerDialog();
    /**
     * Update the dialog
     */
    virtual void update();

  protected:
    /** Qt close event */
    void closeEvent( QCloseEvent* event );
    /** Group box of general information */
    QGroupBox* mGeneralBox;
    /** Reassignment */
    CDataLine* mReassignmentLine;
    /** Robot scheduler */
    CRobotScheduler* mScheduler;
    /** List of task data widget */
    std::vector<CTaskDataWidget*> mTaskDataWidgetList;
    /** Layout */
    QVBoxLayout* mLayout;

};

#endif
