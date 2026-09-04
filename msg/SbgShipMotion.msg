# SBG Ellipse Messages
std_msgs/Header header

# Time since sensor is powered up us
uint32 time_stamp

# Main heave period in seconds. s float 4 4
uint16 heave_period

# [Surge, Sway, Heave] position at main location (in m)
# Note : Surge & Sway are not fulfilled
#
# SBG body frame convention (FRD - Forward-Right-Down), when use_enu is false:
#   x: Surge
#   y: Sway
#   z: Heave, positive down
#
# ROS body frame convention (FLU - Forward-Left-Up, per REP-103), when use_enu is true:
#   x: Surge
#   y: -Sway
#   z: -Heave, positive up
geometry_msgs/Vector3 ship_motion

# [Longitudinal, Lateral, Vertical] acceleration (in m/s2)
#
# SBG body frame convention (FRD - Forward-Right-Down), when use_enu is false:
#   x: Longitudinal
#   y: Lateral
#   z: Vertical, positive down
#
# ROS body frame convention (FLU - Forward-Left-Up, per REP-103), when use_enu is true:
#   x: Longitudinal
#   y: -Lateral
#   z: -Vertical, positive up
geometry_msgs/Vector3 acceleration

# [Longitudinal, Lateral, Vertical] velocity (in m/s)
#
# SBG body frame convention (FRD - Forward-Right-Down), when use_enu is false:
#   x: Longitudinal
#   y: Lateral
#   z: Vertical, positive down
#
# ROS body frame convention (FLU - Forward-Left-Up, per REP-103), when use_enu is true:
#   x: Longitudinal
#   y: -Lateral
#   z: -Vertical, positive up
geometry_msgs/Vector3 velocity

# Ship motion output status
SbgShipMotionStatus status
