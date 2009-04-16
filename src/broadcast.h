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
 * $Log: broadcast.h,v $
 * Revision 1.3  2009-03-31 04:27:32  jwawerla
 * Some bug fixing
 *
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
 **************************************************************************/
#ifndef BROADCAST_H
#define BROADCAST_H

#include <list>
#include "worktaskrobotinterface.h"

/**
 * A very abstract broadcasting system
 * @author Jens Wawerla <jwawerla@sfu.ca>
 */
class CBroadCast {
  public:
    /** Default destructor */
    ~CBroadCast();
   /**
    * Gets the only instance of this class
    * @param ttl time to leve for messages [s]
    * @return only instance
    */
    static CBroadCast* getInstance( float ttl = 10.0 );
    /**
     * Checks if broadcast has messages
     * @param timestamp current time [s]
     * @return true if message available, false otherwise
     */
    bool hasMessage(float timestamp);
    /**
     * Adds a message to the broadcast system
     * @param task to add
     * @param timestamp current time [s]
     */
    void addMessage (IWorkTaskRobotInterface* task, float timestamp );
    /**
     * Gets the first message and removes it from the list
     * @return message
     */
    IWorkTaskRobotInterface* popMessage();
    /**
     * Gets the number of messages in the system
     * @param timestamp current time [s]
     */
    int getNumMessage(float timestamp);

  protected:
    /**
     * Default constructor
     * @param ttl time to live for messages [s]
     */
    CBroadCast( float ttl = 10.0);
    /**
     * Removes messages that are too old
     * @param timestamp current time [s]
     */
    void removeOldMessages( float timestamp );

  private:
    typedef struct {
      double timestamp;
      IWorkTaskRobotInterface* workTask;
    } tBCMsg;
    /** Time to live for a message [s] */
    float mTimeToLive;
    /** List of requests for more workers for a task */
    std::list<tBCMsg> mWorkerRequestList;
};

#endif
