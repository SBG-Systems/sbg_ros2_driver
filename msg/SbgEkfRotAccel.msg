# SBG Ellipse Messages
std_msgs/Header header

#  Time since sensor is powered up [us]
uint32 time_stamp

# INS rotation rate free from earth rotation and sensor bias/scale errors (rad.s^-1)
#
# On the sbg/ekf_rot_accel_body topic, expressed in the body frame:
#   FRD (Forward-Right-Down), when use_enu is false:
#     x: X axis of the device frame
#     y: Y axis of the device frame
#     z: Z axis of the device frame
#   FLU (Forward-Left-Up, per REP-103), when use_enu is true:
#     x: X axis of the device frame
#     y: -Y axis of the device frame
#     z: -Z axis of the device frame
#
# On the sbg/ekf_rot_accel_ned topic, expressed in the navigation frame:
#   NED, when use_enu is false:
#     x: North
#     y: East
#     z: Down
#   ENU, when use_enu is true:
#     x: East
#     y: North
#     z: Up
geometry_msgs/Vector3 rate

# INS accelerations and free from gravity and sensor bias/scale errors (m.s^-2)
#
# On the sbg/ekf_rot_accel_body topic, expressed in the body frame:
#   FRD (Forward-Right-Down), when use_enu is false:
#     x: X axis of the device frame
#     y: Y axis of the device frame
#     z: Z axis of the device frame
#   FLU (Forward-Left-Up, per REP-103), when use_enu is true:
#     x: X axis of the device frame
#     y: -Y axis of the device frame
#     z: -Z axis of the device frame
#
# On the sbg/ekf_rot_accel_ned topic, expressed in the navigation frame:
#   NED, when use_enu is false:
#     x: North
#     y: East
#     z: Down
#   ENU, when use_enu is true:
#     x: East
#     y: North
#     z: Up
geometry_msgs/Vector3 acceleration