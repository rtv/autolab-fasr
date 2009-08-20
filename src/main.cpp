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

#include "RapiLooseStage"
#include "analyst.h"
#include "taskmanager.h"
#include "baserobotctrl.h"
#include "looppolicyrobotctrl.h"
#include "staticpolicyrobotctrl.h"
#include "replanpolicyrobotctrl.h"
#include "waitprobpolicyrobotctrl.h"
#include "fixedratiopolicyrobotctrl.h"
#include "epsilonsamplerrobotctrl.h"

#undef GUI

#ifdef GUI
#include "fasrgui.h"
#endif

#ifdef GUI
CFasrGui* gui = NULL;
#endif

//-----------------------------------------------------------------------------
static int shutDownCallback( Stg::Model* model, void* ptr )
{
  printf( "shutDownCallback\n" );
#ifdef GUI
  if ( gui )
    delete gui;
#endif
  return 0; // ok
}
//-----------------------------------------------------------------------------
extern "C" int Init( Stg::Model* mod )
{
  std::string value;
  float probBroadcast = 1.0;
  char* policy = new char[40];
  static bool init = false;
  Rapi::CLooseStageRobot* robot;
  CTaskManager* taskManager;
  CAnalyst* analyst;
  ABaseRobotCtrl* robotCtrl;

  static CCharger charger( mod->GetWorld()->GetModel( "charger" ) );
  static CDestination depotDestination( mod->GetWorld()->GetModel( "depot" ) );


  if ( not init ) {
    init = true;
    mod->AddShutdownCallback(( Stg::stg_model_callback_t )shutDownCallback, NULL );
    printf( "-----------------------------------\n" );
    printf( "FASR \n" );
    printf( "  build %s %s \n", __DATE__, __TIME__ );
    printf( "  compiled against RAPI version %s (%s) build %s\n", RAPI_VERSION(),
            RAPI_GIT_VERSION(), RAPI_BUILD() );
    printf( "\n" );
  }
  //************************************
  // init general stuff
  ErrorInit( 4, false );
  initRandomNumberGenerator();

  //************************************
  // create robot and its controller
  robot = new Rapi::CLooseStageRobot( mod );
  if ( ! mod->GetPropertyStr( "policy", &policy,  NULL ) ) {
    PRT_WARN0( "No policy specified " );
    exit( -1 );
  }
  else {
    if ( strcmp( policy, "fixed" ) == 0 )
      robotCtrl = new CFixedRatioPolicyRobotCtrl( robot );
    else if ( strcmp( policy, "replan" ) == 0 )
      robotCtrl = new CReplanPolicyRobotCtrl( robot );
    else if ( strcmp( policy, "static" ) == 0 )
      robotCtrl = new CStaticPolicyRobotCtrl( robot );
    else if ( strcmp( policy, "loop" ) == 0 )
      robotCtrl = new CLoopPolicyRobotCtrl( robot );
    else if ( strcmp ( policy, "epsilon" ) == 0 )
      robotCtrl = new CEpsilonSamplerRobotCtrl( robot );
    else if ( strcmp( policy, "wait" ) == 0 ) {
      for( unsigned int i=0; i< Stg::World::args.size(); i++ ) {
         if (Stg::World::args[i].compare(0, 3, "-pb") == 0) {
           value = Stg::World::args[i].substr(4, Stg::World::args[i].size()-4);
           probBroadcast = atof(value.c_str());
         }
      }
      printf("running with prob broadcast %f \n", probBroadcast);
      robotCtrl = new CWaitProbPolicyRobotCtrl( robot, probBroadcast);
    }
    else {
      PRT_WARN1( "Unknown policy: %s", policy );
      exit( -1 );
    }
  }

  //************************************
  // get tasks from the task manager and hand them to the robot
  taskManager = CTaskManager::getInstance(
                  mod->GetWorld()->GetModel( "taskmanager" ) );
  analyst = CAnalyst::getInstance( mod->GetWorld()->GetModel( "taskmanager" ) );

  taskManager->registerRobot( robotCtrl );
  analyst->registerRobot( robotCtrl );
  robotCtrl->addCharger( &charger );
  robotCtrl->setDepot( &depotDestination );

#ifdef GUI
  gui = CFasrGui::getInstance( 0, NULL );
  gui->registerRobot( robot );
#endif

  return 0; // ok

}
//-----------------------------------------------------------------------------
