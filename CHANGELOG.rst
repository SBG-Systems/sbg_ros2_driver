^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package sbg_driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

3.2.0 (2024-10-17)
------------------
* Update README with ROS2 commands to launch magnetic calibration
* Fix segfault when running magnetic calibration
* Removed unused flags from SbgEkfStatus message
* Added new flags to SbgShipMotionStatus message
* Improved SbgStatusAiding message with new flags
* Improved SbgPosStatus message with new flags
* Improved SbgEkfStatus message with new flags
* Improved SbgStatusGeneral message with datalogger and cpu flags
* Updated documentation for sbgGpsPos message
* Updated documentation for sbgGpsHdt message
* Improved SbgUtcTime message with internal clock quality indicators
* Improved SbgStatusCom message with ethernet tx and rx status
* Improved SbgGpsHdt message with number of SV tracked and used
* Improved SbgGpsPos message with numSvTracked
* Improved SbgGpsPosStatus message with spoofing, jamming and OSNMA status
* Updated config files with new messages
* Removed obsolete documentation
* Updated published topic list
* Added missing topic names
* Updated dependencies requirement
* Added settings log_ekf_rot_accel_body / log_ekf_rot_accel_ned / log_ekf_vel_body
* Fixed functions description
* Added SbgEkfRotAccel body and NED messages
* Added SbgEkfVelBody message
* Fixing compiling issues
* Updated sbgECom lib with version 4.0-1987-stable
* Fixed typos about lever arm
* Fixed config applier for IMU Alignment / Aiding / Odometer lever arms
* Remove boost dependency
  (cherry picked from commit ab54c33f1e442c3737dd8e1c09a8b6f36c2c1afa)
* Cleanup
* Moved LLAtoECEF into a helper
* Variable naming
* WIP code cleanup
* Class documentation
* Code indentation
* Utm as class
* Indentation fix
* Updated sbg_utm documentation and function prototype
* Added documentation on Utm structure
* Added documentation on Position class
* Added Position class
* Factory for UTM data
* Fixed space / tabluations issues
* Code indentation
* SbgUtm documentation
* Using fma for computation
* Added constexpr to some variables
* Using pow instead of multiply
* Reworked createRosPointStampedMessage computations
* Removed sendTransform in fillTransform method
* Moved functions into helper namespace
* Moved UTM initialization into its own class
* Removed catkin reference in CMakeLists.txt
* Updated naming convention
* Improved ENU/NED documentation
* Disabling ROS standard message when in NED frame convention
* Reverted NED to ENU quaternion conversion
* Quaternion: cleaner version for NED to ENU conversion
* Reworked odometry message
* Quaternion: NED to ENU conversion rework
* Updated readme about ENU frame convention
* Reworked angle wrapping functions
* Added range for Euler angle measurement.
* More explicit naming for quaternions
* Reverted NED to ENU array conversion
* Documentation update
* odom->base_link is now correct in NED mode
* Renamed function.
* Fixed variable inversion.
* odom->base_link is now correct in NED mode
* Refactored NED to ENU array conversion
* Updated ROS messages documentation
* Fixed Euler / Quaternion orientation in ENU mode
* Fixed nmea output condition
* Compilation fixes
* NTRIP: GGA generation Work In Progress with the following fixex:
  - GPS to UTC time correctly apply leap second offset
  - GGA only sent if a valid position is available
  - GGA is sent at 1 Hz only
  - Minor improvements
  Code is not yet tested nor build
* Compilation fix
* Improved GGA generation and code cleanup
* Improved RTCM and NMEA parameters naming
* Code cleanup - removed (void) as it is not recommended in C++
* Reworked and improved main project README and small fixed in yaml examples
* Added documentation about RTCM messages and device configuration.
* Removed MessageSubscriber class
* Switched dependency from mavros_msgs to rtcm_msgs
* Updated documentation
* Removed SbgInterface as class member
* Removed threaded subscription.
* Namespace related coding style fix
* Fixes in GGA serialization
* Realigned members.
* Code documentation
* Improved NMEA GGA message
* Added rtcm / nmea parameters in config files
* Fixed deprecated header warning
* Added publisher for nmea msg
* Added subscription to RTCM msg
* remove build status
* fix build on Windows
* time_reference parameter fix
* Fix deprecated use of rosidl_target_interfaces
  The use of rosidl_target_interfaces is deprecated (see [Humble release notes](http://docs.ros.org.ros.informatik.uni-freiburg.de/en/humble/Releases/Release-Humble-Hawksbill.html#deprecation-of-rosidl-target-interfaces). Here that actually causes an issue with CMake not setting the right include directory paths, breaking `colcon build` on humble.
  This applies the documented update, making the driver build under Humble
* Contributors: Michael Zemb, Raphael Siryani, Samuel Toledano, SanderVanDijk-StreetDrone, Timon Mentink, VladimirL, cledant, rsiryani

3.1.0 (2021-10-18)
------------------
* Add imu/odometry publisher
  * Fix dependencies
  * Fix wrong SbgGpsHdt description
  * Update doc
  * Add missing MIT licences
  * Based on release 3.1 of ros1 driver
* Add ENU/NED option, rework frame IDs, time stamps and driver frequency.
  * Add parameters to set frame ID and ENU convention
  * Add a parameter to select header stamp source and read ROS time when publishing the message
  * Remove node ros::Rate period auto computation and only read it from a node parameter
  * Update documentation and messages definitions
  * Fix timeStamp value initializing in SbgEkfNavMessage
  * Based on release 3.0.0 of ros1 driver
* update maintainer
* print interface details at startup
* fix configuration files
* Contributors: Michael Zemb, Raphael Siryani

1.0.1 (2020-07-09)
------------------
* Update Licenses
* First version
