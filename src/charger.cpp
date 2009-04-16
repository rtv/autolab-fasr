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
 * $Log: charger.cpp,v $
 * Revision 1.2  2009-03-31 01:42:00  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.1  2009-03-27 01:32:58  jwawerla
 * wait prob controller improved
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "charger.h"

//-----------------------------------------------------------------------------
CCharger::CCharger ( float x, float y, float queueDir, float chargeRate,
                     const char* name )
    : CDestination ( x, y, queueDir, name )
{
  mChargeRate = chargeRate;
}
//-----------------------------------------------------------------------------
CCharger::CCharger ( Stg::Model* mod ) : CDestination ( mod )
{
  if ( ! mStgModel->GetPropertyFloat ( "chargerate", &mChargeRate, 0.0 ) )
    PRT_WARN1 ( "chargerate not specified for sink %s\n",
                mStgModel->Token() );
}
//-----------------------------------------------------------------------------
CCharger::~CCharger()
{
}
//-----------------------------------------------------------------------------



