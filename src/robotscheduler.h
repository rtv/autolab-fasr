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
#ifndef ROBOTSCHEDULER_H
#define ROBOTSCHEDULER_H

#include "transportationtask.h"
#include "robotschedulerinterface.h"
#include <list>

/** Structure for task data used to do the planning */
typedef struct {
  /** Basic task information */
  CTransportationTask* task;
  /** Optimal number of robot serviceing this task */
  unsigned int optNumWorkers;
  /** Round trip time source sink [s] */
  float roundTripTime;
  /** List of assigned robots */
  std::list<IRobotSchedulerInterface*> assignedRobotList;
} tTaskData;


/**
 * This class provides a robot scheduler that plans an optimal task allocation
 * based on the available information, either the map data or if the robot
 * provide feedback from measurements.
 * @author Jens Wawerla
 */
class CRobotScheduler
{
  friend class CRobotSchedulerDialog;

  public:
    /** Default destructor */
    ~CRobotScheduler();
    /**
     * Gets the only instance of this class
     * @return instance of this class
     */
    static CRobotScheduler* getInstance();
    /**
     * Adds a task to the scheduler
     * @param task to be added
     */
    void addTransportationTask( ITransportTaskInterface* task );
    /**
     * Given a robot it returns the task the robot is scheduler to perform
     * @param robot requesting a task
     * @return task to be performed, or NULL if no task available
     */
    ITransportTaskInterface* getTask( IRobotSchedulerInterface* robot );
    /**
     * Gets the initial task randomly
     * @param robot requesting a task
     * @return task to be performed, or NULL if no task available
     */
    ITransportTaskInterface* getFirstTaskAtRandom (  IRobotSchedulerInterface* robot );
    /**
     * Register a robot with the scheduler
     * @param robot to register
     */
    void registerRobot( IRobotSchedulerInterface* robot );
    /**
     * Give a robot the chance to inform the scheduler about the experienced
     * task completion time
     * @param task that was completed
     * @param time [s]
     */
    void setExperiencedTaskCompletionTime( ITransportTaskInterface* task,
                                           float time );
    /**
     * Checks if a robot should keep waiting at the source of a given task
     * @param task to wait for
     * @param robot waiting
     * @return true keep waiting, false stop waiting
     */
    bool keepWaiting( ITransportTaskInterface* task, IRobotSchedulerInterface* robot );
    /**
     * Notifies the planner that a given robot is now charging
     * @param robot that is charging
     */
    void robotIsCharging ( IRobotSchedulerInterface* robot );

  protected:

    /**
     * Remove robot from the list of assigned robots of a task
     * @param task to remove robot from
     * @param robot to remove
     */
    void removeRobotFromTask( tTaskData* task, IRobotSchedulerInterface* robot );
    /**
     * Remove robot from the list of assigned robots of a task
     * @param task to remove robot from
     * @param robot to remove
     */
    void removeRobotFromTask( CTransportationTask* task, IRobotSchedulerInterface* robot );


  private:
    /** Default constructor */
    CRobotScheduler();
    /** Vector of tasks */
    std::vector<tTaskData> mTaskList;
    /** Vector of all robots */
    std::vector<IRobotSchedulerInterface*> mRobotList;
    /** List of all free (unassigned robots) */
    std::list<IRobotSchedulerInterface*> mFreeRobotList;
    /** Number of robots reassigned to different task */
    unsigned int mNumReassignments;
};

#endif
