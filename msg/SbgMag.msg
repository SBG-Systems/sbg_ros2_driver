# SBG Ellipse Messages
std_msgs/Header header

# Time since sensor is powered up (us)
uint32 time_stamp

# Magnetometer output
#
# SBG body frame convention (FRD - Forward-Right-Down), when use_enu is false:
#   x: X axis of the device frame
#   y: Y axis of the device frame
#   z: Z axis of the device frame
#
# ROS body frame convention (FLU - Forward-Left-Up, per REP-103), when use_enu is true:
#   x: X axis of the device frame
#   y: -Y axis of the device frame
#   z: -Z axis of the device frame
geometry_msgs/Vector3 mag

# Accelerometer output  [m/s^2]
#
# SBG body frame convention (FRD - Forward-Right-Down), when use_enu is false:
#   x: X axis of the device frame
#   y: Y axis of the device frame
#   z: Z axis of the device frame
#
# ROS body frame convention (FLU - Forward-Left-Up, per REP-103), when use_enu is true:
#   x: X axis of the device frame
#   y: -Y axis of the device frame
#   z: -Z axis of the device frame
geometry_msgs/Vector3 accel

# Status
SbgMagStatus status
