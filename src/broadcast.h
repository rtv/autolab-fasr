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
#ifndef BROADCAST_H
#define BROADCAST_H

#include <list>
#include "transporttaskinterface.h"

/**
 * A very simplistic abstract broadcasting system
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
    void addMessage (ITransportTaskInterface* task, float timestamp );
    /**
     * Gets the first message and removes it from the list
     * @return message
     */
    ITransportTaskInterface* popMessage();
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
    CBroadCast( float ttl = 0.2);
    /**
     * Removes messages that are too old
     * @param timestamp current time [s]
     */
    void removeOldMessages( float timestamp );

  private:
    typedef struct {
      double timestamp;
      ITransportTaskInterface* workTask;
    } tBCMsg;
    /** Time to live for a message [s] */
    float mTimeToLive;
    /** List of requests for more workers for a task */
    std::list<tBCMsg> mWorkerRequestList;
};

#endif
