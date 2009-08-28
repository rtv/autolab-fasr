# FASR common include file
# Authors: Richard Vaughan, Jens Wawerla
# $Id: fasr_cost.world,v 1.20 2009-04-25 16:08:26 jwawerla Exp $

include "pioneer.inc"
include "map.inc"
include "sick.inc"

speedup 100
paused 0			 
		 
# stop Stage after x seconds, 0 = run forever
quit_time 7200.0

# resolution of items in the world [m]
resolution 0.02

#threads 8

# configure the GUI window
window
(
  size [ 853.000 752.000 ]

  center [ -3.856 -0.868 ]
  rotate [ 0 0 ]
  #scale 35.648
  scale 20.746

  pcam_loc [ 0 -4.000 2.000 ]
  pcam_angle [ 70.000 0 ]
  pcam_on 0

  show_data 1
  show_flags 1

  interval 50
)

# load an environment bitmap
floorplan
(
  name "background"
  size [32.000 32.000 0.600]
  pose [0 0 0 0]
  bitmap "bitmaps/cave.png"
)


###############################################################################
# Taskmanager, Depot
model
(
  pose [ 19.000 19.000 0 0]
  name "taskmanager"
  ctrl "taskmanager"
  db_count 3
  db [ "scriptname<string>tasks.txt" "mapmodel<string>background" "wavefrontcellsize<float>0.4" ]
)

model
(
  name "depot"
  pose [-14.000 0 0 0]
  size [0 0 0]
  gui_move 1

  # insensible to collision and range sensors
  obstacle_return 0
  laser_return 0
  ranger_return 0

  db_count 1
  db [ "queuedir<float>-90.0"]
)

model
(
  name "charger"
  pose [13.000 -6.000 0 0]
  size [0 0 0]

  # insensible to collision and range sensors
  obstacle_return 0
  laser_return 0
  ranger_return 0

  db_count 2
  db [ "chargerate<float>5000.0" "queuedir<float>-150.0"]
)

###############################################################################
# Sources and sinks

zone
(
  color "green"
  db_count 2
  db [ "scriptname<string>tasks.txt" "queuedir<float>25" ]

  pose [ -15.000 -15.000 0 0 ]
  name "sourceA"
  ctrl "fasrsource"
)

zone
(
  color "DarkGreen"

  db_count 1
  db [ "queuedir<float>165" ]

  pose [ 15.000 -15.000 0 0 ]
  name "sinkA"
  ctrl "sink"
)

zone
(
  color "blue"
  db_count 2
  db [ "scriptname<string>tasks.txt" "queuedir<float>-85" ]
  pose [ -15.000 15.000 0 0 ]
  name "sourceB"
  ctrl "fasrsource"
)

zone
(
  color "DarkBlue"
  db_count 1
  db [ "queuedir<float>-95" ]
  pose [ 15.000 15.000 0 0 ]
  name "sinkB"
  ctrl "sink"
)



###############################################################################
# Robot definition

define charging_bump model
(
  pose [0.240 0 -0.100 0 ]
  size [0.120 0.050 0.100]
  take_watts 1000.0
  color "orange"
  obstacle_return 0
  fiducial( range_max 5 range_max_id 5 fov 270 pose [ 0 0 -0.100 0 ] )
)

define fasr_pioneer2dx pioneer2dx
(
 origin [ 0 0 0 0 ]
 localization "gps"
 localization_origin [ 0 0 0 0 ]
 sicklaser( samples 64 range_max 5 laser_return 2 watts 30 fov 360 orgin [0.0 0 0 0] color "red" )
 ctrl "autolab-fasr"
 joules -1
 joules_capacity 400000
 fiducial_return 0
 charging_bump(  )
)


###############################################################################
# Charging station stuff

define charge_station model
(
  size [ 0.100 0.300 0.100 ]
  color "purple"

  gui_move 0

  # side blocks to restrict view angle
  model( color "purple" size [0.100 0.050 0.250] pose [ 0 0.100 0 0 ] )
  model( color "purple" size [0.100 0.050 0.250] pose [ 0 -0.100 0 0 ] )

  # the charging block
  model(
    pose [ 0.010 0 0 0 ]
    color "yellow"
    size [ 0.050 0.200 0.100 ]
    joules -1  # provides infinite energy
    give_watts 5000
    fiducial_return 2 # look for this in the fiducial sensor
   )

)

#charge_station(  pose [ 15.940 -4.500 0 0 ] )
#charge_station(  pose [ 15.940 -5.500 0 0 ] )
#charge_station(  pose [ 15.940 -6.500 0 0 ] )
#charge_station(  pose [ 15.940 -7.500 0 0 ] )

