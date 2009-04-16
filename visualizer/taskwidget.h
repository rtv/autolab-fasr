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
 * $Log: taskwidget.h,v $
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
 **************************************************************************/
#ifndef CTASKWIDGET_H
#define CTASKWIDGET_H

#include <QtGui>
#include "dataline.h"
#include "robotvisprotocol.h"

/**
 * This widget displays information about a single task
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CTaskWidget : public QGroupBox
{
  public:
    /**
     * Default constructor
     * @param parent widget
     */
    CTaskWidget ( QWidget* parent = NULL );
    /** Default destructor */
    ~CTaskWidget();
    /**
     * Update the widget with new data
     * @param msg new data
     */
    void updateData ( tTaskMsg* msg );

  private:
    /** Displays the number of robots servicing this task */
    CDataLine* mNumRobotDisplay;
    /** Displays the filtered reward rate */
    CDataLine* mFiltRateDisplay;
    /** Displays the reward */
    CDataLine* mRewardDisplay;
    /** Displays the distance between source and sink */
    CDataLine* mSrcSinkDistDisplay;
    /** Displays the production rate */
    CDataLine* mProductionRateDisplay;

};

#endif
