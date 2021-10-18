^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package sbg_driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
