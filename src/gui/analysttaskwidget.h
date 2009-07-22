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
#ifndef ANALYSTTASKWIDGET_H
#define ANALYSTTASKWIDGET_H

#include <QGroupBox>
#include "dataline.h"
#include "analyst.h"

/**
 * A widget to display the task data from the analyst
 * @author Jens Wawerla
 */
class CAnalystTaskWidget : public QGroupBox
{
  public:
    /**
     * Default constructor
     */
    CAnalystTaskWidget(tAnalystTaskData* data, QWidget* parent = 0 );
    /** Default destructor */
    ~CAnalystTaskWidget();
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
    tAnalystTaskData* mTaskData;
};

#endif
