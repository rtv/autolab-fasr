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
#ifndef TASKDATAWIDGET_H
#define TASKDATAWIDGET_H

#include <QGroupBox>
#include "dataline.h"
#include "robotscheduler.h"

/**
 * Widget to display task data
 * @author Jens Wawerla
 */
class CTaskDataWidget : public QGroupBox
{
  public:
    /**
     * Default constructor
     * @param data to display
     * @param parent qt object
     */
    CTaskDataWidget( tTaskData* data, QWidget* parent = 0 );
    /** Default destructor */
    ~CTaskDataWidget();
    /** Updates the widget */
    void updateData();

  private:
    /** Optimal number of workers */
    CDataLine* mOptNumWorkerLine;
    /** Round trip time */
    CDataLine* mRTTLine;
    /** Production rate */
    CDataLine* mProductioRateLine;
    /** Number of assigend robots */
    CDataLine* mNumAssignedRobotLine;
    /** Reward */
    CDataLine* mRewardLine;
    /** Number of completed tasks */
    CDataLine* mCompletedLine;
    /** Task data to display */
    tTaskData* mTaskData;
};

#endif
