# README #

This is a ROS node for the kartech_actuator. It publishes motor data, and subscribes to commands.

# NOTE #

Set the positionReachError in 'ms' and it has to be a unit16 bit value. Inserting a value of 2000 to set_position_reach_error() will set the position reach error to 2000 ms. 
To move the actuator to a certain distance, the input value can be set on command line by publishing the value in inches on the /command/set_position topic. Setting a value of 2.5 on this topic will call the callback funtion, callback_set_position, which moves the actuator to 2.5 inches from the zero calibration position. 
