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
 * $Log: visserver.h,v $
 * Revision 1.5  2009-03-31 23:52:59  jwawerla
 * Moved cell index from float to int math
 *
 * Revision 1.4  2009-03-31 04:27:33  jwawerla
 * Some bug fixing
 *
 * Revision 1.3  2009-03-25 14:49:03  jwawerla
 * Task visualization works now
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
 **************************************************************************/
#ifndef VISSERVER_H
#define VISSERVER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <list>
#include <pthread.h>
#include "worktask.h"
#include "robotvisinterface.h"


/**
 * An tcp server to connect to a visualization tool
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CVisServer
{
  public:
    /** Default destructor */
    ~CVisServer();
    /**
     * Gets the only instance of this class
     * @return object
     */
    static CVisServer* getInstance();
    /**
     * Initialize and start the server
     * @param port tcp port to listen for connections
     * @return 1 ok, 0 otherwise
     */
    int init ( int port = 7000 );
    /**
     * Registers a robot with the server
     * @param robot to register
     */
    void registerRobot ( IRobotVis* robot );
    /**
     * Registers a task with the server
     * @param task to register
     */
    void registerTask ( CWorkTask* task );
    /**
     * Registers a task with the server
     * @param task to register
     */
    void registerTask ( IWorkTaskRobotInterface* task );

  protected:
    /**
     * Default constructor
     */
    CVisServer();
    /**
     * Send the configuration to a client at a given socket
     * @param connFd socket fd
     * @return 1 if success, -1 if socket broken, 0 otherwise
     */
    int sendConfiguration ( int connFd );
    /**
     * Sends robot data to a client
     * @param connFd file descriptor of socket to send data to
     * @param id of robot, who's data to send
     * @return 1 if success, -1 if socket broken, 0 otherwise
     */
    int sendDataForRobot ( int connFd, int id );
    /**
     * Sends the map for a robot to a client
     * @param connFd file descriptor of socket to send data to
     * @param id of robot, who's data to send
     * @return 1 if success, -1 if socket broken, 0 otherwise
     */
    int sendMapForRobot ( int connFd, int id );
    /**
     * Sends the task data to a client
     * @param connFd file descriptor of socket to send data to
     * @return 1 if success, -1 if socket broken, 0 otherwise
     */
    int sendTaskData ( int connFd );
    /**
     * Sends the contents of the txBuffer
     * @param connFd socket to send to
     * @param len number of bytes to send
     * @return 1 if success, -1 if socket broken, 0 otherwise
     */
    int sendTxBuffer (  int connFd, int len );

    /** Declare the listen thread function as a friend to grant access to this class */
    friend void* listenThreadFunc ( void* arg );
    /** Declare the com thread function as a friend to grant access to this class */
    friend void* comThreadFunc ( void* arg );
    /** Definition of client data */
    typedef struct {
      int connFd;
    } tClient;

  private:
    /**
     * This loop runs in a thread and accepts new clients
     * @return 1 if successful, 0 otherwise
     */
    int listenThreadMain();
    /**
     * This loop runs in a thread and handles the tcp communication
     * @return 1 if successful, 0 otherwise
     */
    int comThreadMain();
    /** TCP port */
    int mPort;
    /** Server socket file descriptor */
    int mSocketFd;
    /** Socket address stuff */
    struct sockaddr_in mAddress;
    /** Thread the listen loop runs in */
    pthread_t mListenThread;
    /** Communication thread */
    pthread_t mComThread;
    /** List with client file descriptors */
    std::list<tClient*> mClientFdList;
    /** Vector of registered robots */
    std::vector<IRobotVis*> mRobotVector;
    /** Vector of registered tasks */
    std::vector<CWorkTask*> mTaskVector;
    /** Send buffer */
    char* mTxBuffer;
    /** Receive buffer */
    char* mRxBuffer;
    /** Number of bytes currently new in rx buffer */
    unsigned int mBytesRead;
};


#endif
