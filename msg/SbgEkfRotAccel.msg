# SBG Ellipse Messages
std_msgs/Header header

#  Time since sensor is powered up [us]
uint32 time_stamp

# INS rotation rate free from earth rotation and sensor bias/scale errors (rad.s^-1)
# In NED convention:
#   X: North
#   Y: East
#   Z: Down
# In ENU convention:
#   X: East
#   Y: North
#   Z: Up
geometry_msgs/Vector3 rate

# INS accelerations and free from gravity and sensor bias/scale errors (m.s^-2)
# In NED convention:
#   X: North
#   Y: East
#   Z: Down
# In ENU convention:
#   X: East
#   Y: North
#   Z: Up
geometry_msgs/Vector3 acceleration