# SBG Ellipse Messages

std_msgs/Header header

# Time since sensor is powered up [us]
uint32 time_stamp

# Velocity [m/s]
# In NED convention:
#   x: North
#   y: East
#   z: Down
# In ENU convention:
#   x: East
#   y: North
#   z: Up
geometry_msgs/Vector3 velocity

# Velocity accuracy (1 sigma) [m/s].
# In NED convention:
#   x: North
#   y: East
#   z: Vertical
# In ENU convention:
#   x: East
#   y: North
#   z: Vertical
geometry_msgs/Vector3 velocity_accuracy

#  Global solution status
SbgEkfStatus status
