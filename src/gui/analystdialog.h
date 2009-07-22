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
#ifndef ANALYSTDIALOG_H
#define ANALYSTDIALOG_H

#include "customdialog.h"
#include "analyst.h"
#include "analysttaskwidget.h"
#include <QtGui>

using namespace Rapi;

/**
 * A dialog for analysing the FASR experiment
 * @author Jens Wawerla
 */
class CAnalystDialog : public CCustomDialog
{
  public:
    /**
     * Default constructor
     * @param parent qt widget
     */
    CAnalystDialog(QWidget* parent);
    /** Default destructor */
    ~CAnalystDialog();
    /**
     * Update the dialog
     */
    virtual void update();

  protected:
    /** Analyst to display */
    CAnalyst* mAnalyst;

  protected:
    /** Qt close event */
    void closeEvent( QCloseEvent* event );
    /** Group box of general information */
    QGroupBox* mGeneralBox;
    /** Reassignment */
    CDataLine* mReassignmentLine;
    /** Number of robot charging */
    CDataLine* mNumChargingLine;
    /** Number of robots in depot */
    CDataLine* mNumDepotLine;
    /** Total accumulated reward over all robot */
    CDataLine* mTotalRewardLine;
    /** Total number of flags transported over all robot */
    CDataLine* mTotalFlagLine;
    /** Total amount of energy dissipated over all robot */
    CDataLine* mTotalEnergyLine;
    /** List of task data widget */
    std::vector<CAnalystTaskWidget*> mTaskDataWidgetList;
    /** Layout */
    QVBoxLayout* mLayout;
};

#endif
