# SBG Ellipse Messages
std_msgs/Header header

# Time since sensor is powered up [us]
uint32 time_stamp

# IMU Status
SbgImuStatus imu_status

# Filtered Accelerometer [m/s^2]
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

# Filtered Gyroscope [rad/s]
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
geometry_msgs/Vector3 gyro

# Internal Temperature [°C]
float32 temp

# Sculling output [m/s2]
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
geometry_msgs/Vector3 delta_vel

# Coning output [rad/s]
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
