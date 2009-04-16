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
 * $Log: visserver.cpp,v $
 * Revision 1.10  2009-03-31 23:52:59  jwawerla
 * Moved cell index from float to int math
 *
 * Revision 1.9  2009-03-31 04:27:33  jwawerla
 * Some bug fixing
 *
 * Revision 1.8  2009-03-31 01:42:01  jwawerla
 * Task definitions moved to task manager and stage world file
 *
 * Revision 1.7  2009-03-27 19:05:27  vaughan
 * running on OS X and using new controller parameter method
 *
 * Revision 1.6  2009-03-26 05:15:30  jwawerla
 * *** empty log message ***
 *
 * Revision 1.5  2009-03-26 04:09:45  jwawerla
 * Minor bug fixing
 *
 * Revision 1.4  2009-03-25 14:49:03  jwawerla
 * Task visualization works now
 *
 * Revision 1.3  2009-03-24 03:52:22  jwawerla
 * Pause button works now
 *
 * Revision 1.2  2009-03-24 01:04:46  jwawerla
 * switching cost robot added
 *
 * Revision 1.1  2009-03-23 06:24:21  jwawerla
 * RobotVis works now for data, maps still need more work
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "visserver.h"
#include "robotvisprotocol.h"
#include "error.h"
#include <signal.h>

/** Size of send buffer */
const int MAX_TX_BUFFER = 10000;
/** Size of receive buffer */
const int MAX_RX_BUFFER = 100;
/** Sleep of com thread [usec] */
const int COM_THEAD_USLEEP = 100000;

//-----------------------------------------------------------------------------
void* comThreadFunc ( void* arg )
{
  CVisServer* visServer = ( CVisServer* ) ( arg );
  visServer->comThreadMain();

  return NULL;
}
//-----------------------------------------------------------------------------
void* listenThreadFunc ( void* arg )
{
  CVisServer* visServer = ( CVisServer* ) ( arg );
  visServer->listenThreadMain();

  return NULL;
}
//-----------------------------------------------------------------------------
CVisServer::CVisServer()
{
  mTxBuffer = new char[MAX_RX_BUFFER];
  mRxBuffer = new char[MAX_RX_BUFFER];
  mPort = 0;
  init ( 7000 );
}
//-----------------------------------------------------------------------------
CVisServer::~CVisServer()
{
  if ( mTxBuffer )
    delete[] mTxBuffer;
  if ( mRxBuffer )
    delete[] mRxBuffer;
}
//-----------------------------------------------------------------------------
CVisServer* CVisServer::getInstance()
{
  static CVisServer visServerInstance;

  return &visServerInstance;
}
//-----------------------------------------------------------------------------
int CVisServer::init ( int port )
{
  int i;
  if ( signal ( SIGPIPE, SIG_IGN ) == SIG_ERR ) {
    PRT_ERR1 ( "signal() failed: %s", strerror ( errno ) );
    return 0;
  }

  mPort = port;

  if ( ( mSocketFd = socket ( PF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
    PRT_ERR1 ( "Failed to create socket %s", strerror ( errno ) );
    return 0;
  }
  i = 1;
  setsockopt ( mSocketFd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof ( i ) );
  mAddress.sin_family = AF_INET;
  mAddress.sin_port = htons ( mPort );
  mAddress.sin_addr.s_addr = htonl ( INADDR_ANY );

  if ( bind ( mSocketFd, ( struct sockaddr* ) &mAddress, sizeof ( mAddress ) ) ) {
    PRT_ERR1 ( "Failed to bind to socket %s", strerror ( errno ) );
    return 0;
  }

  if ( listen ( mSocketFd, 10 ) ) {
    PRT_ERR1 ( "Failed to listen on socket %s", strerror ( errno ) );
    return 0;
  }

  PRT_MSG1 ( 4, "Listening on port %d", mPort );
  pthread_create ( &mListenThread, NULL, listenThreadFunc, this );
  pthread_create ( &mComThread, NULL, comThreadFunc, this );

  return 0;
}
//-----------------------------------------------------------------------------
int CVisServer::listenThreadMain()
{
  tClient* client;
  int connFd;
  size_t len = sizeof ( struct sockaddr_in );

  while ( 1 ) {

    // terminate thread ??
    pthread_testcancel();

    PRT_MSG0 ( 5, "Waiting for connection..." );
    connFd = accept ( mSocketFd, ( struct sockaddr* ) & mAddress, (socklen_t*)&len );
    if ( connFd < 0 ) {
      PRT_ERR1 ( "Failed to connect to socket %s", strerror ( errno ) );
      close ( connFd );
    }
    else {
      // make the socket non-blocking
      if ( fcntl ( connFd, F_SETFL, O_NONBLOCK ) == -1 ) {
        PRT_ERR1 ( "fcntl() failed: %s", strerror ( errno ) );
        close ( connFd );
      }
      else {
        PRT_MSG1 ( 5, "Connection accepted at %d", connFd );

        if ( sendConfiguration ( connFd ) == 1 ) {
          client = new tClient;
          client->connFd = connFd;
          mClientFdList.push_back ( client );
        }
      }
    }
  } // while
  return 1;
}
//-----------------------------------------------------------------------------
int CVisServer::comThreadMain()
{
  bool fgHaveHeader;
  int len = 0;
  int ret = 0;
  tClient* client;
  std::list<tClient*>::iterator it;
  tReqMsg reqMsg;
  tHeader header;
  tCommandMsg commandMsg;
  bool fgError = false;
  int r;

  while ( 1 ) {
    // terminate thread ??
    pthread_testcancel();

    if ( mClientFdList.size() != 0 ) {
      for ( it = mClientFdList.begin(); it != mClientFdList.end(); it++ ) {
        client = *it;
        fgError = false;
        fgHaveHeader = false;
        mBytesRead = 0;
        do {
          // non-blocking read
          r = read ( client->connFd, mRxBuffer + mBytesRead, MAX_RX_BUFFER - mBytesRead );
          if ( r == 0 ) {
            PRT_MSG1 ( 4, "Connection closed on socket %d", client->connFd );
            close ( client->connFd );
            it = mClientFdList.erase ( it );
            fgError = true;
            break;
          }
          else if ( r > 0 ) {
            mBytesRead = r;
          }
          if ( ( mBytesRead > sizeof ( tHeader ) ) && ( fgHaveHeader == false ) ) {
            for ( unsigned int i = 0; i < mBytesRead - 1; i++ ) {
              if ( ( mRxBuffer[i  ] == STARTBYTE_0 ) &&
                   ( mRxBuffer[i+1] == STARTBYTE_1 ) ) {
                // check if there is some garbage in the buffer BEFORE the
                // start bytes of this message, if so remove them
                if ( i != 0 ) {
                  printf ( "remove %d bytes \n", i );
                  memmove ( mRxBuffer, mRxBuffer + i , MAX_RX_BUFFER - i );
                  mBytesRead = mBytesRead - i;
                }
                // copy header
                memcpy ( ( char* ) &header, mRxBuffer, sizeof ( tHeader ) );
                len = sizeof ( tHeader );
                fgHaveHeader = true;
              }
            } // for
          }
        }
        while ( ( fgHaveHeader == false ) && ( mBytesRead < ( header.size + sizeof ( tHeader ) ) ) );

        if ( fgError == false ) {
          switch ( header.msgType ) {
            case CMD_MSG:
              memcpy ( ( char* ) &commandMsg, mRxBuffer + len, sizeof ( tCommandMsg ) );
              if ( ( header.robotId > 0 ) && ( header.robotId < mRobotVector.size() ) ) {
                mRobotVector[header.robotId]->setRobotVisCommandMsg ( commandMsg );
              }
              break;

            case REQ_MSG:
              memcpy ( ( char* ) &reqMsg, mRxBuffer + len, sizeof ( tReqMsg ) );
              switch ( reqMsg.reqType ) {
                case DATA_REQ:
                  ret = sendDataForRobot ( client->connFd, header.robotId );
                  break;

                case MAP_REQ:
                  ret = sendMapForRobot ( client->connFd, header.robotId );
                  break;

                case TASK_REQ:
                  ret = sendTaskData ( client->connFd );
                  break;

                default:
                  PRT_ERR1 ( "Unknown request %d ", reqMsg.reqType );
              } // switch
              if ( ret == -1 ) {
                PRT_MSG1 ( 4, "Connection closed on socket %d", client->connFd );
                close ( client->connFd );
                it = mClientFdList.erase ( it );
              }
              break;

            default:
              PRT_ERR1 ( "Unknown message type %d", header.msgType );
              break;
          } // switch
        }
      } // for client list
    } // if
    usleep ( COM_THEAD_USLEEP );
  } // while

  return 1;
}
//-----------------------------------------------------------------------------
int CVisServer::sendConfiguration ( int connFd )
{
  int len = 0;
  tConfigMsg configMsg;
  tHeader header;

  header.startByte0 = STARTBYTE_0;
  header.startByte1 = STARTBYTE_1;
  header.size = sizeof ( tConfigMsg );
  header.msgType = CONFIG_MSG;
  header.robotId = -1;  //this is a system message
  memcpy ( mTxBuffer, ( char* ) &header, sizeof ( tHeader ) );
  len = sizeof ( tHeader );

  configMsg.controllerType = mRobotVector[0]->getControllerType();
  configMsg.numRobots = mRobotVector.size();
  configMsg.numTasks = mTaskVector.size();

  memcpy ( mTxBuffer + len, ( char* ) &configMsg, sizeof ( tConfigMsg ) );
  len = len + sizeof ( tConfigMsg );

  return sendTxBuffer ( connFd, len );
}
//-----------------------------------------------------------------------------
int CVisServer::sendTaskData ( int connFd )
{
  int len = 0;
  tTaskMsg taskMsg;
  tHeader header;

  header.startByte0 = STARTBYTE_0;
  header.startByte1 = STARTBYTE_1;
  header.size = sizeof ( tTaskMsg ) * mTaskVector.size();
  header.msgType = TASK_MSG;
  header.robotId = -1;  //this is a system message
  memcpy ( mTxBuffer, ( char* ) &header, sizeof ( tHeader ) );
  len = sizeof ( tHeader );

  for ( unsigned int i = 0; i < mTaskVector.size(); i++ ) {
    if ( len < MAX_TX_BUFFER ) {
      strncpy ( taskMsg.name, mTaskVector[i]->getName(), 20 );
      taskMsg.id = i;
      taskMsg.numRobotsServing = mTaskVector[i]->getNumSubcribers();
      taskMsg.experiencedRewardRate = mTaskVector[i]->getExperiencedRewardRate();
      taskMsg.reward = mTaskVector[i]->getReward();
      taskMsg.srcSinkDistance = mTaskVector[i]->getSrcSinkDistance();
      taskMsg.productionRate = mTaskVector[i]->getProductionRate();
      memcpy ( mTxBuffer + len, ( char* ) &taskMsg, sizeof ( tTaskMsg ) );
      len = len + sizeof ( tTaskMsg );
    }
  }
  return sendTxBuffer ( connFd, len );
}
//-----------------------------------------------------------------------------
int CVisServer::sendMapForRobot ( int connFd, int id )
{
  int len = 0;
  tMapMsg mapMsg;
  tHeader header;
  int offset;
  int mapSize;

  if ( ( id >= ( int ) mRobotVector.size() ) || ( id < 0 ) ) {
    PRT_ERR2 ( "Request for robot %d but we only have %d robots", id,
               mRobotVector.size() );
    return 0;
  }

  offset = sizeof ( tHeader ) + sizeof ( tMapMsg );
  mapSize = MAX_TX_BUFFER - offset ;

  mRobotVector[id]->getRobotVisMapData ( &mapMsg, mTxBuffer + offset, mapSize );

  header.startByte0 = STARTBYTE_0;
  header.startByte1 = STARTBYTE_1;
  header.msgType = MAP_MSG;
  header.size = sizeof ( tMapMsg ) + mapSize;
  header.robotId = id;
  memcpy ( mTxBuffer, ( char* ) &header, sizeof ( tHeader ) );
  len = sizeof ( tHeader );

  memcpy ( mTxBuffer + len, ( char* ) &mapMsg, sizeof ( tMapMsg ) );
  len = len + sizeof ( tMapMsg );
  len = len + mapSize;

  return sendTxBuffer ( connFd, len );
}
//-----------------------------------------------------------------------------
int CVisServer::sendDataForRobot ( int connFd, int id )
{
  int msgSize;
  int len = 0;
  tHeader header;

  if ( ( id >= ( int ) mRobotVector.size() ) || ( id < 0 ) ) {
    PRT_ERR2 ( "Request for robot %d but we only have %d robots", id,
               mRobotVector.size() );
    return 0;
  }

  header.startByte0 = STARTBYTE_0;
  header.startByte1 = STARTBYTE_1;
  header.size = sizeof ( tDataMsg );
  header.msgType = DATA_MSG;
  header.robotId = id;
  memcpy ( mTxBuffer, ( char* ) &header, sizeof ( tHeader ) );
  len = sizeof ( tHeader );

  msgSize = MAX_TX_BUFFER - len;
  mRobotVector[id]->getRobotVisData (id, mTxBuffer + len, msgSize );

  len = len + msgSize;

  return sendTxBuffer ( connFd, len );
}
//-----------------------------------------------------------------------------
int CVisServer::sendTxBuffer ( int connFd, int len )
{
  int l;

  l = write ( connFd, mTxBuffer, len );

  if ( l < 0 ) {
    if ( ( errno == EPIPE ) || ( errno == ECONNRESET ) ) {
      return -1; // close socket
    }
    else {
      PRT_ERR2 ( "Error while sending data to socket %d: %s", connFd,
                 strerror ( errno ) );
      return 0; // error
    }
  }

  if ( l != len ) {
    PRT_ERR2 ( "Error while sending request, send %d of %d bytes", l, len );
    return 0;
  }

  return 1;
}
//-----------------------------------------------------------------------------
void CVisServer::registerRobot ( IRobotVis* robot )
{
  mRobotVector.push_back ( robot );
}
//-----------------------------------------------------------------------------
void CVisServer::registerTask ( CWorkTask* task )
{
  std::vector<CWorkTask*>::iterator it;

  if ( mTaskVector.size() > 0 ) {
    for ( it = mTaskVector.begin(); it != mTaskVector.end(); it++ ) {
      if ( *it == task )
        return;  // task already in vector
    }
  }

  mTaskVector.push_back ( task );
}
//-----------------------------------------------------------------------------
void CVisServer::registerTask ( IWorkTaskRobotInterface* task )
{
  registerTask ( (CWorkTask*) task );
}
//-----------------------------------------------------------------------------
