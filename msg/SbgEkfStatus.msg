# SBG Ellipse Messages
# Submessage

# Defines the Kalman filter computation mode (see the table 4 below)
# 0 UNINITIALIZED	The Kalman filter is not initialized and the returned data are all invalid.
# 1 VERTICAL_GYRO	The Kalman filter only rely on a vertical reference to compute roll and pitch angles. Heading and navigation data drift freely.
# 2 AHRS			A heading reference is available, the Kalman filter provides full orientation but navigation data drift freely.
# 3 NAV_VELOCITY	The Kalman filter computes orientation and velocity. Position is freely integrated from velocity estimation.
# 4 NAV_POSITION	Nominal mode, the Kalman filter computes all parameters (attitude, velocity, position). Absolute position is provided. 
uint8 solution_mode 

# True if Attitude data is reliable (Roll/Pitch error < 0,5 deg)
bool attitude_valid

# True if Heading data is reliable (Heading error < 1 deg)
bool heading_valid

# True if Velocity data is reliable (velocity error < 1.5 m/s)
bool velocity_valid

# True if Position data is reliable (Position error < 10m)
bool position_valid

# True if vertical reference is used in solution (data used and valid since 3s)
bool vert_ref_used

# True if magnetometer is used in solution (data used and valid since 3s)
bool mag_ref_used

# True if GPS velocity is used in solution (data used and valid since 3s)
bool gps1_vel_used

# True if GPS Position is used in solution (data used and valid since 3s)
bool gps1_pos_used

# True if GPS True Heading is used in solution (data used and valid since 3s)
bool gps1_hdt_used

# True if GPS2 velocity is used in solution (data used and valid since 3s)
bool gps2_vel_used

# True if GPS2 Position is used in solution (data used and valid since 3s)
bool gps2_pos_used

# True if GPS2 True Heading is used in solution (data used and valid since 3s)
bool gps2_hdt_used

# True if Odometer is used in solution (data used and valid since 3s)
bool odo_used

# True if DVL Bottom Tracking is used in solution (data used and valid since 3s)
bool dvl_bt_used

# True if DVL Water Tracking is used in solution (data used and valid since 3s)
bool dvl_wt_used

# True if user Position is used in solution (data used and valid since 3s)
bool user_pos_used

# True if user Velocity is used in solution (data used and valid since 3s)
bool user_vel_used

# True if user Course is used in solution (data used and valid since 3s)
bool user_heading_used

# True if USBL / LBL is used in solution (data used and valid since 3s)
bool usbl_used

# True if AirData (altimeter and/or true airspeed) is used in solution (data used and valid since 3s)
bool air_data_used

# True if a ZUPT is used in solution (data used and valid since 3s)
bool zupt_used

# True if sensor alignment and calibration parameters are valid
bool align_valid

# True if Depth sensor (for sub-sea navigation) is used in solution (data used and valid since 3s)
bool depth_used