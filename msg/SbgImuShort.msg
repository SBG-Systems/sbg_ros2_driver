# SBG Ellipse Messages
std_msgs/Header header

# Time since sensor is powered up us 
uint32 time_stamp

# IMU Status
SbgImuStatus imu_status

# X, Y, Z delta velocity. Unit is 1048576 LSB for 1 m.s^-2.
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
geometry_msgs/Vector3 delta_velocity

# X, Y, Z delta angle. Unit is 67108864 LSB for 1 rad.s^-1.
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
geometry_msgs/Vector3 delta_angle

# IMU average temperature. Unit is 256 LSB for 1°C.
int16 temperature
