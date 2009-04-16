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
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"
#include "broadcast.h"
#include "charger.h"

#include "looppolicyrobot.h"
#include "staticpolicyrobot.h"
#include "swcostcompolicyrobot.h"
#include "individualswcostpolicyrobot.h"
#include "replanpolicyrobot.h"
#include "waitprobpolicyrobot.h"
#include "error.h"
#include "baserobot.h"
#include "stage.hh"
// #include "wavefrontmapvis.h"
#include "taskmanager.h"

extern "C" int Init ( Stg::Model* mod )
{
  ABaseRobot* robot = NULL;
  char* policy = NULL;

  CTaskManager* taskManager = CTaskManager::getInstance (
                                mod->GetWorld()->GetModel ( "taskmanager" ) );
  CBroadCast* broadCast = CBroadCast::getInstance ( 10.0 );

  //static MapVis stgMapvis;

  static CCharger charger ( mod->GetWorld()->GetModel ( "charger" ) );
  static CDestination depotDestination ( mod->GetWorld()->GetModel ( "depot" ) );

  ErrorInit ( 4, false );
  initRandomNumberGenerator();

  if ( ! mod->GetPropertyStr ( "policy", &policy,  NULL ) )
    PRT_WARN1 ( "robot %s has no control policy specified in worldfile.",
                mod->Token() );

  Stg::ModelPosition* pos = ( Stg::ModelPosition* ) mod;

  if ( strcmp ( policy, "static" ) == 0 )
    robot = new CStaticPolicyRobot ( pos ) ;
  else if ( strcmp ( policy, "waitprob" ) == 0 )
    robot = new CWaitProbPolicyRobot ( pos ) ;
  else if ( strcmp ( policy, "replan" ) == 0 )
    robot = new CReplanPolicyRobot ( ( Stg::ModelPosition* ) mod ) ;


  // Individual reward maximizing policies
  else if ( strcmp ( policy, "loop" ) == 0 )
    robot = new CLoopPolicyRobot ( pos ) ;
  else if ( strcmp ( policy, "indswcost" ) == 0 )
    robot = new CIndividualSwCostPolicyRobot ( pos ) ;
  else if ( strcmp ( policy, "costcom" ) == 0 )
    robot = new CSwCostComPolicyRobot ( pos ) ;
  else
    printf ( "[FASR] Error: control policy \"%s\" not recognized.\n", policy );

  assert ( robot );

  taskManager->registerRobot ( robot );
  robot->addCharger ( &charger );

  robot->setDepot ( &depotDestination );
  robot->setBroadCast ( broadCast );

  // Add the Wavefront map visualizer, disabled by default
  Stg::Model* bg = mod->GetWorld()->GetModel ( "background" );
  assert ( bg );

  //bg->AddVisualizer ( &stgMapvis, false );

  return 0; // ok
}
