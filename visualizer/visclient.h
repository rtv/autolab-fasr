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
 * $Log: visclient.h,v $
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
 **************************************************************************/
#ifndef VISCLIENT_H
#define VISCLIENT_H

#include <QTime>
#include <QObject>
#include <QMessageBox>
#include <QTcpSocket>
#include <QString>
#include "robotvisprotocol.h"

/**
 * A tcp client for RobotVis
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CVisClient : public QObject
{
    Q_OBJECT

  public:
    /**
     * Default constructor
     * @param object parent Qt object
     * @param hostname of server
     * @param port for tcp connection
     */
    CVisClient ( QObject* parent = NULL, QString hostname = "localhost", int port = 7000 );
    /** Default destructor */
    virtual ~CVisClient();
    /** Latest config message */
    tConfigMsg mConfigMsg;
    /** Latest map message */
    tMapMsg mMapMsg;
    /**
     * Checks if the socket is open
     * @return true if open, false otherwise
     */
    bool isOpen();

  signals:
    /**
     * Signal emitted if an error occurred during communication
     * @param msg error message
     */
    void errorMessage ( QString msg );
    /** Signal emitted if a new configuration message was received */
    void newConfigMessage();
    /** Signal emitted if a new data message was received */
    void newRobotDataMessage ( tDataMsg dataMsg );
    /** Signal emitted if new task data was received */
    void newTaskDataMessage ( tTaskMsg msg );

  public slots:
    /**
     * Call this slot to request robot data
     * @param id of robot
     * @return 1 if success, 0 otherwise
     */
    int requestRobotData ( int id );
    /**
     * Call this slot to request a map from a robot
     * @param id of robot
     * @return 1 if success, 0 otherwise
     */
    int requestRobotMap ( int id );
    /**
     * Call this slot to request task data
     * @return 1 if success, 0 otherwise
     */
    int requestTaskData();
    /**
     * Pauses or unpauses a robot
     * @param id of robot
     * @param pause, true to pause, false to unpause
     * @return 1 if success, 0 otherwise
     */
    int pauseRobot ( int id, bool pause );

  protected slots:
    /** Slot to handle errors from the tcp socket */
    void errorHandler ( QAbstractSocket::SocketError socketError );
    /** Slot called by Qt if the host was found */
    void hostFound();
    /** Slot called by Qt if the connection was closed */
    void disconnected ();
    /** Slot called by Qt if the connection was established */
    void connected ();
    /** Slot called once data is available at the socket */
    void dataRecieved();

  private:
    /** TCP socket for communicating with the simulation */
    QTcpSocket* mTcpSocket;
    /** Host name */
    QString mHostname;
    /** TCP port */
    int mPort;
    /** Receive Buffer */
    char* mRxBuffer;
    /** Send buffer */
    char* mTxBuffer;
    /** Number of bytes read in the buffer */
    unsigned int mBytesRead;
    /** Flags if request is pending, so we don't flood the server */
    bool mFgRequestPending;
    /** Time of last request */
    QTime mTimeOfLastRequest;
    /** Flag if connected to server or not */
    bool mFgConnected;

};

#endif
