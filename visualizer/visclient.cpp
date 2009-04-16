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
 * $Log: visclient.cpp,v $
 * Revision 1.4  2009-03-26 04:09:46  jwawerla
 * Minor bug fixing
 *
 * Revision 1.3  2009-03-25 14:49:03  jwawerla
 * Task visualization works now
 *
 * Revision 1.2  2009-03-24 03:52:22  jwawerla
 * Pause button works now
 *
 * Revision 1.1  2009-03-24 02:07:44  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2009-03-15 03:52:02  jwawerla
 * First commit
 *
 *
 ***************************************************************************/
#include "visclient.h"
#include "error.h"

/** Maximum receive buffer size */
const unsigned int MAX_RX_BUFFER = 10000;
/** Maximum send buffer size */
const unsigned int MAX_TX_BUFFER = 100;
/** Timeout for a request message [ms] */
const int REQUEST_TIMEOUT = 500;

//-----------------------------------------------------------------------------
CVisClient::CVisClient ( QObject* parent, QString hostname, int port )
    : QObject ( parent )
{
  mBytesRead = 0;
  mFgConnected = false;

  if ( MAX_RX_BUFFER <= sizeof ( tDataMsg ) )
    PRT_ERR2 ( "Buffer to small is %d should be %d bytes", MAX_RX_BUFFER,
               sizeof ( tDataMsg ) );
  mHostname = hostname;
  mPort = port;
  mRxBuffer = new char[MAX_RX_BUFFER];
  mTxBuffer = new char[MAX_TX_BUFFER];
  mFgRequestPending = false;
  mTimeOfLastRequest.start();

  mTcpSocket = new QTcpSocket ( this );
  connect ( mTcpSocket, SIGNAL ( error ( QAbstractSocket::SocketError ) ),
            this, SLOT ( errorHandler ( QAbstractSocket::SocketError ) ) );

  connect ( mTcpSocket, SIGNAL ( hostFound() ), this, SLOT ( hostFound() ) );
  connect ( mTcpSocket, SIGNAL ( disconnected() ), this, SLOT ( disconnected() ) );
  connect ( mTcpSocket, SIGNAL ( connected() ), this, SLOT ( connected() ) );
  connect ( mTcpSocket, SIGNAL ( readyRead() ), this, SLOT ( dataRecieved() ) );

  mTcpSocket->connectToHost ( mHostname, mPort );
}
//-----------------------------------------------------------------------------
CVisClient::~CVisClient()
{
  if ( mRxBuffer )
    delete[] mRxBuffer;

  if ( mTxBuffer )
    delete[] mTxBuffer;

  mTcpSocket->close();
}
//-----------------------------------------------------------------------------
bool CVisClient::isOpen()
{
  if ( !mTcpSocket )
    return false;

  return mTcpSocket->isOpen();
}
//-----------------------------------------------------------------------------
void CVisClient::errorHandler ( QAbstractSocket::SocketError socketError )
{
  QString portStr;
  QString msgStr;

  mFgConnected = false;

  portStr = portStr.setNum ( mPort );

  msgStr = "The following error occurred while communicating with <b>" + mHostname +
           ":" + portStr + "</b> : <br>" + mTcpSocket->errorString();

  emit errorMessage ( msgStr );
}
//-----------------------------------------------------------------------------
void CVisClient::hostFound()
{
}
//-----------------------------------------------------------------------------
void CVisClient::connected()
{
  mFgConnected = true;
}
//-----------------------------------------------------------------------------
void CVisClient::disconnected()
{
  if ( mFgConnected == true ) {
    mFgConnected = false;
    emit errorMessage ( "Connection to was closed" );
  }
}
//-----------------------------------------------------------------------------
int CVisClient::pauseRobot ( int id, bool pause )
{
  int len = 0;
  int l;
  tCommandMsg commandMsg;
  tHeader header;

  if ( mFgRequestPending ) {
    if ( mTimeOfLastRequest.elapsed() < REQUEST_TIMEOUT )
      return 0;
  }

  header.startByte0 = STARTBYTE_0;
  header.startByte1 = STARTBYTE_1;
  header.msgType = CMD_MSG;
  header.size = sizeof ( tCommandMsg );
  header.robotId = id;
  memcpy ( mTxBuffer + len, ( char* ) &header, sizeof ( tHeader ) );
  len = sizeof ( tHeader );

  commandMsg.fgPause = pause;
  memcpy ( mTxBuffer + len, ( char* ) &commandMsg, sizeof ( tCommandMsg ) );
  len = len + sizeof ( tCommandMsg );

  if ( mFgConnected == false )
    return 0;

  l = mTcpSocket->write ( mTxBuffer, len );

  if ( len != l ) {
    PRT_ERR2 ( "Error while sending request, send %d of %d bytes", l, len );
    return 0;
  }

  return 1;

}
//-----------------------------------------------------------------------------
int CVisClient::requestRobotMap ( int id )
{
  int len = 0;
  int l;
  tReqMsg reqMsg;
  tHeader header;

  if ( mFgRequestPending ) {
    if ( mTimeOfLastRequest.elapsed() < REQUEST_TIMEOUT )
      return 0;
  }

  header.startByte0 = STARTBYTE_0;
  header.startByte1 = STARTBYTE_1;
  header.size = sizeof ( tReqMsg );
  header.msgType = REQ_MSG;
  header.robotId = id;
  memcpy ( mTxBuffer + len, ( char* ) &header, sizeof ( tHeader ) );
  len = sizeof ( tHeader );

  reqMsg.reqType = MAP_REQ;
  memcpy ( mTxBuffer + len, ( char* ) &reqMsg, sizeof ( tReqMsg ) );
  len = len + sizeof ( tReqMsg );

  if ( mFgConnected == false )
    return 0;

  l = mTcpSocket->write ( mTxBuffer, len );

  if ( len != l ) {
    PRT_ERR2 ( "Error while sending request, send %d of %d bytes", l, len );
    return 0;
  }

  mFgRequestPending = true;
  mTimeOfLastRequest.start();
  return 1;
}
//-----------------------------------------------------------------------------
int CVisClient::requestRobotData ( int id )
{
  int len = 0;
  int l;
  tReqMsg reqMsg;
  tHeader header;


  if ( mFgRequestPending ) {
    if ( mTimeOfLastRequest.elapsed() < REQUEST_TIMEOUT )
      return 0;
  }

  header.startByte0 = STARTBYTE_0;
  header.startByte1 = STARTBYTE_1;
  header.size = sizeof ( tReqMsg );
  header.msgType = REQ_MSG;
  header.robotId = id;
  memcpy ( mTxBuffer + len, ( char* ) &header, sizeof ( tHeader ) );
  len = sizeof ( tHeader );

  reqMsg.reqType = DATA_REQ;
  memcpy ( mTxBuffer + len, ( char* ) &reqMsg, sizeof ( tReqMsg ) );
  len = len + sizeof ( tReqMsg );

  if ( mFgConnected == false )
    return 0;

  l = mTcpSocket->write ( mTxBuffer, len );

  if ( len != l ) {
    PRT_ERR2 ( "Error while sending request, send %d of %d bytes", l, len );
    return 0;
  }

  mFgRequestPending = true;
  mTimeOfLastRequest.start();
  return 1;
}
//-----------------------------------------------------------------------------
int CVisClient::requestTaskData ( )
{
  int len = 0;
  int l;
  tReqMsg reqMsg;
  tHeader header;

  if (mFgConnected == false) {
    return 0;
  }

  if ( mFgRequestPending ) {
    if ( mTimeOfLastRequest.elapsed() < REQUEST_TIMEOUT )
      return 0;
  }

  header.startByte0 = STARTBYTE_0;
  header.startByte1 = STARTBYTE_1;
  header.size = sizeof ( tReqMsg );
  header.msgType = REQ_MSG;
  header.robotId = -1;
  memcpy ( mTxBuffer + len, ( char* ) &header, sizeof ( tHeader ) );
  len = sizeof ( tHeader );

  reqMsg.reqType = TASK_REQ;
  memcpy ( mTxBuffer + len, ( char* ) &reqMsg, sizeof ( tReqMsg ) );
  len = len + sizeof ( tReqMsg );

  l = mTcpSocket->write ( mTxBuffer, len );

  if ( len != l ) {
    PRT_ERR2 ( "Error while sending request, send %d of %d bytes", l, len );
    return 0;
  }

  mFgRequestPending = true;
  mTimeOfLastRequest.start();
  return 1;
}
//-----------------------------------------------------------------------------
void CVisClient::dataRecieved()
{
  int numTasks;
  tDataMsg dataMsg;
  tHeader header;
  tTaskMsg taskMsg;
  unsigned int msgLen;
  int len = 0;


  if (mFgConnected == false)
    return;

  mBytesRead += mTcpSocket->readLine ( mRxBuffer + mBytesRead, MAX_RX_BUFFER - mBytesRead );

  if ( mBytesRead < sizeof ( tHeader ) )
    return; // not enough bytes in buffer to contain message


  // check for start bytes and then read header
  for ( unsigned int i = 0; i < mBytesRead - 1; i++ ) {
    if ( ( mRxBuffer[i  ] == STARTBYTE_0 ) &&
         ( mRxBuffer[i+1] == STARTBYTE_1 ) ) {
      // check if there is some garbage in the buffer BEFORE the
      // start bytes of this message, if so remove them
      if ( i != 0 ) {
        memmove ( mRxBuffer, mRxBuffer + i , MAX_RX_BUFFER - i );
        mBytesRead = mBytesRead - i;
      }
      // copy header
      memcpy ( ( char* ) &header, mRxBuffer, sizeof ( tHeader ) );
      len = sizeof ( tHeader );
      // see if this is a valid device type, if not remove message
      // from buffer
      if ( header.msgType >= MAX_MSG_TYPE ) {
        memmove ( mRxBuffer, mRxBuffer + len,
                  MAX_RX_BUFFER - len );
        mBytesRead = mBytesRead - len;
        PRT_MSG1 ( 3, "Unknown device type %d", header.msgType );
        return;
      }
      break; // that's it leave the loop
    }
  }

  if ( ( mBytesRead - len ) >= header.size ) {
    // how long is the message we deal with right now
    msgLen = header.size + sizeof ( tHeader );

    if ( mBytesRead < msgLen ) {
      PRT_MSG2 ( 5, "Not enough bytes, got %d but expected %d", mBytesRead,
                 msgLen );
      return;
    }

    switch ( header.msgType ) {
      case CONFIG_MSG:
        memcpy ( ( char* ) &mConfigMsg, mRxBuffer + len, sizeof ( tConfigMsg ) );
        emit newConfigMessage();
        mFgRequestPending = false;
        break;

      case DATA_MSG:
        memcpy ( ( char* ) &dataMsg, mRxBuffer + len, sizeof ( tDataMsg ) );
        len = len + sizeof ( tDataMsg );
        // check if we have controller specific information
        if ( header.size > sizeof(tDataMsg) ) {
          printf("controller information \n");
        }
        emit newRobotDataMessage ( dataMsg );
        mFgRequestPending = false;
        break;

      case MAP_MSG:
        memcpy ( ( char* ) &mMapMsg, mRxBuffer + len, sizeof ( tMapMsg ) );
        len = len + sizeof ( tMapMsg );
        //emit newRobotDataMessage ( header.robotId );;
        mFgRequestPending = false;
        break;

      case TASK_MSG:
        numTasks = header.size / sizeof( tTaskMsg);
        for (int i = 0; i < numTasks; i++ ) {
          memcpy ( ( char* ) &taskMsg, mRxBuffer + len, sizeof ( tTaskMsg ) );
          len = len + sizeof ( tTaskMsg );
          emit newTaskDataMessage( taskMsg );
        }
        break;

      default:
        PRT_ERR1 ( "Unimplemented protocol %d", header.msgType );
        break;
    } // switch

    // remove message from buffer by moving the rest of the buffer
    // forward
    memmove ( mRxBuffer, mRxBuffer + msgLen , MAX_RX_BUFFER - msgLen );
    mBytesRead = mBytesRead - msgLen;
  }

  return;
}
//-----------------------------------------------------------------------------
