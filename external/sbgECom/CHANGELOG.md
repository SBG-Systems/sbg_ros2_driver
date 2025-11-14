# Change Log
This change log lists all modifications for each sbgECom library release.

sbgECom C library change log issued on: 2025-06-23  
Copyright (C) 2007-2024, SBG Systems SAS. All rights reserved.

## Release Summary

The sbgECom 5.3.2276-stable is a major release that supports the following products:
 - **High Performance INS**: Firmware 5.x and above
 - **ELLIPSE v3**: Firmware 3.x and above
 - **ELLIPSE v1/v2**: Firmware 2.6 (legacy support)

This release introduces support for the **ELLIPSE v3** major firmware update.  
The ELLIPSE v3 firmware primarily relies on the [sbgRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) for product configuration.

All new features and capabilities introduced by the ELLIPSE firmware v3 are fully accessible through this [sbgECom](https://developer.sbg-systems.com/sbgECom/) release, alongside the latest version of the [sbgRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).

A significant effort has been dedicated to improving the sbgECom logs and protocol documentation.  
The previously used Firmware Reference Manual PDF has been replaced with an integrated, always up-to-date, Markdown/Doxygen-based documentation system.

Access this new and improved documentation here: [sbgECom Documentation](https://developer.sbg-systems.com/sbgECom/).

Backward compatibility has been maintained as much as possible; however, please review the **Migration.md** file for further details on transitioning to this version.

## Import Highlights for this release

Below are the key improvements and modifications in this release:
 - Support for the new ELLIPSE firmware v3
 - Support for output rates of up to 1 kHz
 - Added device info/settings streaming
 - Completely rewritten sbgECom protocol documentation
 - General code cleanup and improvements for better maintainability

## Release - 5.3.2276-stable

### New Features
 - SBGECOM-479 - Doc: Add GSA NMEA frame
 - SBGECOM-478 - Add GSA, ZDA, and GST NMEA frames in direct GNSS mode
 - SBGECOM-476 - Implement a generic pager to handle messages split across multiple chunks
 - SBGECOM-467 - Add generic 3D velocity aiding input/output log
 - SBGECOM-464 - Add new logs for vibration analysis

### Improvements
 - SBGECOM-465 - Add SBG_ECOM_HEAVE_ACCEL_VALID and improved ship motion doc and defines

### Bug Fixes
 - SBGECOM-473 - Doc: Fix SBG_ECOM_LOG_GPSX_POS message payload size
 - SBGECOM-472 - Doc: Fix SBG_ECOM_CMD_INFO message description

## Release - 5.2.590-stable

### New Features
 - SBGECOM-451 - Add support for new Septentrio Pro+ and CLAS receivers variants
 - SBGECOM-455 - Update PTP message with PTP over Ethernet
 - SBGECOM-457 - Doc: Add Magnetic declination and inclination fields in EKF Euler and Quaternion messages
 - SBGECOM-458 - Doc: Add missing documentation for output message SBG_ECOM_LOG_RTCM_RAW
 - SBGECOM-459 - Doc: Add SBG_ECOM_LOG_SESSION_INFO output message with device info and settings

### Bug Fixes
 - SBGECOM-452 - Doc: Fix several issues with anchors, titles and notes
 - SBGECOM-454 - Doc: SBG_ECOM_LOG_IMU_SHORT message ID is 22 instead of 44

## Release - 5.1.708-stable

### New Features
 - SBGECOM-433 - Added SBG_ECOM_FEATURE_IM / OSNMA to report GNSS capabilities
 - SBGECOM-432 - Doc: Add instruction for the magnetic factory calibration
 - SBGECOM-426 - Doc: Explain standard/high range scale factor for SBG_ECOM_LOG_IMU_SHORT output
 - SBGECOM-425 - Add SBG_ECOM_LOG_PTP_STATUS message to report PTP time offset and status
 - SBGECOM-423 - Returns the CPU usage in the SBG_ECOM_LOG_STATUS message
 - SBGECOM-422 - Logger: updated EKF euler output to use only degrees instead of radians
 - SBGECOM-420 - Output: Add magnetic heading/declination/inclination in EKF Euler/Quat logs
 - SBGECOM-419 - Add proper support for more than 200Hz output logs for ELLIPSE firmware v3
 - SBGECOM-414 - Add support for up to 1 kHz Sync Out events for ELLIPSE firmware v3
 - SBGECOM-413 - Add support for new straight GNSS NMEA outputs logs (GGA/RMC/VTG/HDT)
 - SBGECOM-411 - Add new motion profiles to better support new ELLIPSE firmware v3
 - SBGECOM-408 - Add a new sbgECom output log with advanced internal INS filter debug data
 - SBGECOM-407 - Add support for Nortek DVL
 - SBGECOM-406 - Add SBG_ECOM_SOL_ZARU_USED used in solution bitmask
 - SBGECOM-402 - Add a new SBG_ECOM_CLASS_LOG_ALL option to disabled/enable all outputs on an interface
 - SBGECOM-394 - Add SBG_ECOM_LOG_SESSION_INFO message to stream settings & device info
 - SBGECOM-393 - Added in SBG_ECOM_CMD_SET_MAG_CALIB a field to indicate if the magnetic calibration is 2D or 3D
 - SBGECOM-381 - Add a high range scale variant for IMU Short output logs

### Improvements
 - SBGECOM-448 - Improve public CMake build process and documentation
 - SBGECOM-431 - New sbgEComMinimal example that supports all Serial and Ethernet devices
 - SBGECOM-429 - Doc: Improved Ship Motion description about output monitoring points
 - SBGECOM-428 - Doc: Completly reworked documentation with built-in firmware reference manual
 - SBGECOM-427 - Improved overall doxygen code base documentation
 - SBGECOM-424 - Updated unix serial interface to report error with SBG_LOG_XXXXX macros
 - SBGECOM-404 - Doc: Add sbgECom verion to the online URL documentation
 - SBGECOM-399 - Add new error models for external sbgECom protocol magnetometers support
 - SBGECOM-305 - Improved documentation for extended sbgECom frame definition

### Bug Fixes
 - SBGECOM-439 - Fix typo for SGB_CALIB_MAG_NOT_ENOUGH_POINTS
 - SBGECOM-409 - sbgBasicLogger: Fixed erroneous output times for utcIso8601 mode
 - SBGECOM-405 - CAN: Fix depth and air data airspeed logs types from integers to float values
 - SBGECOM-398 - Removed unused SBG_ECOM_SYNC_OUT_MODE_EVENT_IN_# definitions
 - SBGECOM-391 - sbgBasicLogger: Fixed invalid GNSS#_HDT output for numSvTracked, numSvUsed fields

### Removed Features
 - SBGECOM-430 - Magnetometer: Remove robust magnetic model (not relevant anymore)
 - SBGECOM-410 - Doc: Indicate all 'preciseInstallation' fields to be deprecated

## Release - 4.0.1987-stable

### New Features
 - SBGECOM-346 - Add spoofing, jamming and OSNMA status in SBG_ECOM_LOG_GPS#_POS
 - SBGECOM-347 - Add in SBG_ECOM_LOG_GPS#_HDT number of SV tracked and used
 - SBGECOM-364 - Add an option to change UDP connected/unconnected behavior
 - SBGECOM-237 - Add callback to retrieve complete raw sbgECom frames on reception
 - SBGECOM-330 - Add Ethernet Tx and Rx status flags in SBG_ECOM_LOG_STATUS message
 - SBGECOM-341 - sbgBasicLogger: Rewritten from scratch using C++ 14
 - SBGECOM-342 - sbgBasicLogger: Export RTCM raw stream to a binary file that can be used for post processing
 - SBGECOM-351 - Add numSvTracked in SBG_ECOM_LOG_GPS#_POS message
 - SBGECOM-352 - BasicLogger: Add new outputs on SBG_ECOM_LOG_GPS#_XXXX logs
 - SBGECOM-363 - sbgEComApi: Add Ethernet UDP interface support
 - SBGECOM-365 - Added to SBG_ECOM_LOG_UTC_TIME internal clock quality indicators
 - SBGECOM-368 - Add new SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY message
 - SBGECOM-369 - Add new SBG_ECOM_LOG_EKF_VELOCITY_BODY message
 - SBGECOM-370 - Add new SBG_ECOM_LOG_EKF_ROT_ACCEL_NED message

### Improvements
 - SBGECOM-308 - Add support for clang enhanced compiler
 - SBGECOM-337 - Improve on board magnetic calibration example to save to FLASH memory
 - SBGECOM-338 - Improved SBG_ECOM_LOG_GPS#_VEL velocity standard deviation documentation
 - SBGECOM-343 - sbgBasicLogger: Export GNSS raw stream to a binary file that can be used for post processing
 - SBGECOM-344 - sbgBasicLogger: Added new option to export logs using UTC time
 - SBGECOM-345 - sbgBasicLogger: Added a new option to skip logs until the INS has a valid UTC time
 - SBGECOM-350 - Report unavailable numSvUsed in SBG_ECOM_LOG_GPS#_POS with 0xFF value
 - SBGECOM-357 - Improved sbgECom log file naming and methods conventions
 - SBGECOM-358 - Updated migration documentation from sbgECom 3 to 4
 - SBGECOM-361 - Improved sbgEComLogUtc enums/definitions/setters/getters naming
 - SBGECOM-366 - sbgBasicLogger: Add an option to output status using decimal or hexadecimal representation
 - SBGECOM-380 - Reduce stack usage for some sbgECom commands
 - SBGECOM-367 - Improved  sbgEComCmdOutput##### documentation
 - SBGECOM-403 - Replaced __FUNCTION__ macro with standard C99 __func__ variable

### Bug Fixes
 - SBGECOM-383 - Updated CMD_API_GET ID (46) to (48) to fix conflict with CMD_GNSS_1_INSTALLATION (46)
 - SBGECOM-359 - Fixed incompatible deprecated typedef with opencv such as uint64
 - SBGECOM-401 - Fixed invalid sleep usage in sbgEComReceiveCmd2 method
 - SBGECOM-339 - Relaxed Cmake version requirements to 3.16 to support Ubuntu 20.04
 - SBGECOM-340 - Fixed SyncOut direct PPS comment in sbgEComCmdEvent.h

### Removed feature
 - SBGECOM-349 - Flagged SBG_ECOM_IMU_DATA as legacy and not to be used

## Release - 3.2.4011-stable

### New Features
 - SBGECOM-306 - Added built in CHANGELOG file in the repository
 - SBGECOM-307 - Add support for event markers in sbgBasicLogger
 - SBGECOM-309 - Add SBG_ECOM_LOG_GPS#_SAT message containing satellite information
 - SBGECOM-310 - Add NMEA message GPROT
 - SBGECOM-312 - sbgBasicLogger: handle sbgEComLogSat messages
 - SBGECOM-313 - Added a static motion profile for 27/7 operations on ELLIPSE
 - SBGECOM-314 - Added pedestrian motion profile that uses foot odometer
 - SBGECOM-317 - Added search capabilities for Doxygen documentation
 - SBGECOM-319 - Add support for the Teledyne Wayfinder DVL protocol
 - SBGECOM-323 - Add a new PSBGA NMEA message that output the INS attitude
 - SBGECOM-326 - Add NMEA GSV output message with satellite in view information
 - SBGECOM-327 - Added L-Band support for GNSS receiver channel reporting
 
### Improvements
 - SBGECOM-311 - Improve public CMakeLists
 - SBGECOM-315 - Enable sbgEComApi users to provide command execution parameters
 - SBGECOM-322 - Added Truck and Railway motion profiles
 - SBGECOM-325 - sbgBasicLogger: Improve project code and build script for code source public release
 - SBGECOM-328 - Improved doxygen documentation

### Bug Fixes
 - SBGECOM-316 - Fix ELLIPSE settings import that was not working
 - SBGECOM-324 - Fix invalid time-out errors when calling sbgEComReceiveCmd2

## Release - 3.1.2358-stable

### New Features
 - SBGECOM-225 - Added KMB binary sensor output log for High Performance INS
 - SBGECOM-226 - Add a new log SBG_ECOM_LOG_RTCM_STREAM
 - SBGECOM-228 - Add new commands to implement GET/POST over sbgECom protocol
 - SBGECOM-229 - Now ELLIPSE-N and D can accept RTCM corrections over Port A
 - SBGECOM-231 - Add support for NMEA PPS frames
 - SBGECOM-253 - Add specific NMEA like output log WASSP
 - SBGECOM-279 - Add in SBG_ECOM_CMD_ADVANCED_CONF a GNSS option bitmask
 - SBGECOM-281 - Add a new sbgBasicLogger command line tool
 - SBGECOM-282 - Add a sbgEComApi CLI tool to access sbgInsRestApi over sbgECom
 - SBGECOM-283 - Add a minimal sbgECom example for PULSE IMU
 - SBGECOM-285 - Add in SBG_ECOM_CMD_ADVANCED_CONF an option to always output time in NMEA messages
 - SBGECOM-286 - Add in SBG_ECOM_CMD_ADVANCED_CONF an option select standard or extended NMEA mode

### Improvements
 - SBGECOM-181 - Reduce stack usage and use new sbgEComReceiveCmd2
 - SBGECOM-220 - Updated license to MIT
 - SBGECOM-232 - Updated protocol to support large transfer using multiple pages
 - SBGECOM-287 - Updated sbgEComSetReceiveLogCallback to remove the unused SbgErrorCode return
 - SBGECOM-301 - Improved ELLIPSE onboard magnetic calibration code example
 - SBGECOM-302 - Added new sbgEComPurgeIncoming method to discard rx data
 - SBGECOM-303 - Improved examples, they now use simple CLI arguments

### Bug Fixes
 - SBGECOM-265 - Add missing 25ms period to output log message definitions
 - SBGECOM-267 - Add missing 40ms period to output log message definitions
 - SBGECOM-284 - Receive a command must not be blocking when timeout = 0
 - SBGECOM-289 - Fix SBG_ECAN_MSG_GPS#_ALT NUM_SV & DIFF_CORR DBC/DBF definitions

## Release - 2.0.4536-stable

### New Features
 - SBGECOM-180 - Added NMEA GGK message output
 - SBGECOM-191 - Added in SBG_ECOM_LOG_GPS#_POS status report for all GNSS constellations
 - SBGECOM-194 - Add specific SBG_ECOM_THIRD_PARTY_ADA_01 output log
 - SBGECOM-208 - Added a README.md file with migration guidelines
 - SBGECOM-216 - Add Cobham SBG_ECOM_THIRD_PARTY_AT_ITINS output log support
 - SBGECOM-219 - Added CAN dbc and BusMaster definitions in sbgECom project
### Improvements
 - SBGECOM-188 - Updated SBG_ECAN_MSG_ODO_VELOCITY from float field to integers
 - SBGECOM-196 - Added status field for SBG_ECAN_MSG_AUTO_TRACK_SLIP_CURV (0x220) message
 - SBGECOM-199 - Simplified SbgEComGnssModelsStdIds enum for ELLIPSE-N and ELLIPSE-D
 - SBGECOM-204 - Renamed SBG_ECOM_GNSS_MODEL_UBLOX_GPS_GLONASS to SBG_ECOM_GNSS_MODEL_INTERNAL
 - SBGECOM-207 - Reworked motion profile / aiding equipments errors models set/get API (removed SbgEComModelInfo)
 - SBGECOM-209 - Reworked and simplified GNSS model ids to comply with sbgECom 2.x
 - SBGECOM-212 - Added in SBG_ECOM_CMD_FEATURES gnss firmware version field
### Removed Features
 - SBGECOM-200 - Removed SBG_ECOM_GNSS_MODEL_UBLOX_HIGH_DYNAMICS, use SBG_ECOM_GNSS_MODEL_INTERNAL instead
 - SBGECOM-201 - Removed SBG_ECOM_GNSS_MODEL_ELLIPSE_D_INTERNAL, use SBG_ECOM_GNSS_MODEL_INTERNAL instead
 - SBGECOM-202 - Removed SBG_ECOM_GNSS_MODEL_UBLOX_HIGH_SPEED, use SBG_ECOM_GNSS_MODEL_INTERNAL instead
 - SBGECOM-203 - Removed SBG_ECOM_GNSS_MODEL_UBLOX_LOW_SPEED, use SBG_ECOM_GNSS_MODEL_INTERNAL instead
 - SBGECOM-206 - Removed deprecated methods sbgEComCmdGnss1GetLeverArmAlignment & sbgEComCmdGnss1SetLeverArmAlignment
 - SBGECOM-211 - Removed legacy IG-500 protocol support

## Release - 1.11.920-stable

### New Features
 - SBGECOM-123 - Implement DVL aiding configuration SBG_ECOM_CMD_DVL_####
 - SBGECOM-126 - Implement AirData aiding configuration SBG_ECOM_CMD_AIRDATA_####
 - SBGECOM-135 - Add configuration commands for CAN odometer support
 - SBGECOM-136 - Add sbgECom Log Event Output useful for virtual odometer
 - SBGECOM-137 - Add SBG_ECOM_LOG_DEPTH / SBG_ECAN_MSG_DEPTH_INFO/ALTITUDE output logs
 - SBGECOM-140 - Updated SBG_ECOM_CMD_AIDING_ASSIGNMENT command for AirData support
 - SBGECOM-141 - Add airDataInput demo project to show external sbgECom AirData aiding
 - SBGECOM-142 - Updated SBG_ECOM_LOG_PRESSURE to SBG_ECOM_LOG_AIR_DATA with airspeed
 - SBGECOM-143 - Updated SBG_ECAN_MSG_PRESSURE to SBG_ECAN_MSG_AIR_DATA with airspeed
 - SBGECOM-156 - Add baseline length field in SBG_ECOM_LOG_GPS#_HDT log
 - SBGECOM-162 - Add SBG_ECOM_LOG_DIAG message to send text
 - SBGECOM-163 - Add CAN output message with vehicle body velocity
 - SBGECOM-164 - Add CAN output message with track, slip and curvature indications
 - SBGECOM-168 - Add INDYN NMEA like message output for marine applications
 - SBGECOM-170 - Add SBG_ECOM_CMD_GNSS_1_INSTALLATION command to set/get GNSS lever arm
 - SBGECOM-173 - Add compatibility support Crossbow AHRS500 series
 - SBGECOM-174 - Basic Logger: Add support for error log messages
 - SBGECOM-175 - Basic Logger: Add support for UDP interfaces

### Improvements
 - SBGECOM-132 - Updated SBG_ECOM_CMD_AIDING_ASSIGNMENT command for DVL support
 - SBGECOM-133 - Renamed DVL standard deviation as quality indicator in DVL log structure
 - SBGECOM-144 - Converted SBG_ECOM_AIDING_EM_LOG_RECV to SBG_ECOM_AIDING_DEPTH_RECV
 - SBGECOM-145 - Renamed SBG_ECOM_AIDING_PRESSURE_RECV to SBG_ECOM_AIDING_AIR_DATA_RECV
 - SBGECOM-146 - Renamed SBG_ECOM_SOL_PRESSURE_USED to SBG_ECOM_SOL_AIR_DATA_USED
 - SBGECOM-147 - Added SBG_ECOM_SOL_DEPTH_USED in EKF solution status
 - SBGECOM-165 - Updated CAN log default IDs for Ship Motion HP from 0x210 to 0x14A
 - SBGECOM-169 - Updated SBG_ECOM_THIRD_PARTY_IXBLUE_XXX logs to comply with naming conventions
 - SBGECOM-176 - Minor code cleanup and removed useless defines
 - SBGECOM-177 - Updated all C types to use standard ones uint32 -> uint32_t
 - SBGECOM-178 - Updated common lib code with improved organization

### Removed Features
 - SBGECOM-100 - Removed deprecated sbgEComSetReceiveCallback method
 - SBGECOM-101 - Removed deprecated sbgEComCmdGnss1SetModel method
 - SBGECOM-102 - Removed deprecated sbgEComCmdSensorSetMotionProfile method
 - SBGECOM-103 - Removed deprecated sbgEComCmdMagSetModel method
 - SBGECOM-171 - SBG_ECOM_CMD_GNSS_1_LEVER_ARM_ALIGNMENT deprecated and replaced by SBG_ECOM_CMD_GNSS_1_INSTALLATION

## Release - 1.10.3692-stable

### New Features
 - SBGECOM-115 - Add Septentrio Internal GNSS model for new Ellipse D
 - SBGECOM-117 - Added getters for delta a angle / velocity / temperature for IMU Short log
 - SBGECOM-119 - Added new Swell Mode Ship Motion status flags

### Improvements
 - SBGECOM-118 - Improved sbgEComStartFrameGeneration & sbgEComFinalizeFrameGeneration methods
 - SBGECOM-122 - Increased time out for sbgEComCmdLicenseApply to support new ELLIPSE-D internal GNSS

## Release - 1.9.706-stable

### New Features
 - SBGECOM-110 - Added DOLOG HRP proprietary message
 - SBGECOM-111 - Added a new short IMU log message also used for post processing
 - SBGECOM-112 - Add a heavy machinery motion profile definition for Ellipse series

### Bug Fixes
 - SBGECOM-113 - Fixed invalid SBG_ECOM_CAN_RX/TX_OK comments

## Release - 1.8.2916-stable

### New Features
 - SBGECOM-95 - Added GPS number of SV used and diff corrections details in sbgECan protocol
 - SBGECOM-105 - Add compatibility with SBG_ECOM_CMD_VALIDITY_THRESHOLDS command
 - SBGECOM-108 - Added support for aiding assignment on Port E for ELLIPSE-E and N

### Improvements
 - SBGECOM-91 - Added sbgEComSetCmdTrialsAndTimeOut to setup the number of trials and default time out for commands

## Release - 1.7.235-stable

### New Features
 - SBGECOM-89 - Implement Kongsberg Binary 26 message output
 - SBGECOM-87 - Add an uptime indication in SBG_ECOM_LOG_STATUS
 - SBGECOM-86 - Added the command SBG_ECOM_CMD_ETHERNET_INFO to current device IP address
 - SBGECOM-85 - Added command SBG_ECOM_CMD_ETHERNET_CONF to define / retrieve the Ethernet configuration
 - SBGECOM-77 - Add new output log class for NMEA proprietary messages
 - SBGECOM-75 - Added UAV motion profile definition (for low dynamic rotary wing UAV applications)

### Improvements
 - SBGECOM-83 - Better use of size_t type instead of uint32 to comply with C standard and 64 bit platforms
 - SBGECOM-84 - Updated sbgCommonLib to latest revision

### Removed Features
 - SBGECOM-79 - Removed deprecated "course" from the GNSS configurable aiding sources

## Release - 1.5.209-stable

### New Features
 - SBGECOM-72 - Added proprietary NMEA message PASHR for roll, pitch, heading, heave
 - SBGECOM-70 - Added SBG Proprietary NMEA message with acceleration and angular rate
 - SBGECOM-68 - Added SBG_ECOM_OUTPUT_MODE_DIV_5 flag for 40 Hz output
 - SBGECOM-66 - Added SBG_ECOM_GENERAL_CPU_OK status flag
 - SBGECOM-65 - Added sbgEComHandleOneLog method to return even if more logs are available
 - SBGECOM-64 - Added sbgEComSendAck method
 - SBGECOM-62 - Added sbgEComStartFrameGeneration and sbgEComFinalizeFrameGeneration methods
 - SBGECOM-59 - Added sbgECom log generation code
 - SBGECOM-57 - Added SBG_ECOM_LOG_FAST_IMU_DATA message definition
 - SBGECOM-40 - Added KVH third party output format id (SBG_ECOM_THIRD_PARTY_KVH)

### Improvements
 - SBGECOM-74 - Switched unix projects to CMake
 - SBGECOM-73 - Added examples into the sbgECom
 - SBGECOM-63 - Updated sbgCommonLib to latest revision
 - SBGECOM-61 - Use stream buffer instead of basic buffer for sbgECom log parsing
 - SBGECOM-51 - Added SBF (Septentrio) protocol support on Ellipse-E
 - SBGECOM-50 - Added direct PPS from internal GNSS Sync Output (SBG_ECOM_CMD_SYNC_OUT_CONF)
 - SBGECOM-47 - Made the sbgECom 64 bit compatible
 - SBGECOM-46 - Switched project files to Visual Studio 2013
 - SBGECOM-45 - Added a new callback method (sbgEComSetReceiveLogCallback) and deprecated the old one (sbgEComSetReceiveCallback)
 - SBGECOM-42 - Improved handling of MSG and CLASS fields in low level protocol functions

### Bug Fixes
 - SBGECOM-71 - Fixed sbgEComCmdGetInfo incorrect error code return when an invalid payload is received
 - SBGECOM-44 - Fixed Various incompatibilities in Big Endian platforms
 - SBGECOM-43 - Added output of NACK reasons in sbgECom configuration commands using the return code

### Removed Features
 - SBGECOM-60 - Removed Ship Motion 1,2,3 and Ship Motion HP 1,2,3 due to new deported heave concepts

## Release - 1.4.3239-stable

### New Features
 - SBGECOM-28 - Added differential correction age, diff base id and num sv to the SBG_ECOM_LOG_GPS#_POS 
 - SBGECOM-29 - Added GNSS raw data log for the second GNSS receiver 
 - SBGECOM-30 - Added official support for Ellipse additional output interfaces PORT C and PORT E 
 - SBGECOM-33 - Added big/little endian support for stream buffer 
 - SBGECOM-34 - Added sbgPlatform.h file to setup platform specific configuration such as endianness 

### Improvements
 - SBGECOM-7 - Added support for both little and big endian platforms
 - SBGECOM-32 - Improved stream buffer error handling
 - SBGECOM-36 - Improved File naming and overall library organization
 - SBGECOM-37 - Modified firmware and software version numbering scheme
 - SBGECOM-38 - Increased raw GPS data buffer size from 2048 to 4096 bytes 

### Bug Fixes
 - SBGECOM-21 - Fixed SBG_ECOM_ETH#_RX_OK and SBG_ECOM_ETH#_TX_OK status definitions
 - SBGECOM-27 - Changed sbgEComHandle behavior so the error returned by receive call back is taken into account
 - SBGECOM-35 - Fixed improper comments in some configuration structures

## Release - 1.3

### New Features
 - SBGECOM-10 - Added sbgInterfaceChangeBaudrate for both windows and unix platforms 
 - SBGECOM-19 - Added SBG_ECOM_LOG_PRESSURE log for depth sensors and altimeters 
 - SBGECOM-25 - Added support for Ellipse series 
 - SBGECOM-26 - Added SBG_ECOM_LOG_USBL log for USBL aiding equipments (beta)

### Improvements
 - SBGECOM-18 - Fixed Typos in GPS pos, Vel and Hdt Fix Status 
 - SBGECOM-20 - Better error checking for sbgStreamBuffer with new method sbgStreamBufferGetLastError 
 - SBGECOM-22 - Added UTC & Clock status to the binary log SbgLogUtcData 
 - SBGECOM-23 - Added Solution status to the binary log SbgLogEkfEuler, SbgLogEkfQuat, SbgLogEkfNav 
 - SBGECOM-24 - Added timestamp to the log SBG_ECOM_LOG_MAG_CALIB

## Release - 1.2

### New Features
 - SBGECOM-14 - Added SBG_ECOM_LOG_SHIP_MOTION_HP logs for delayed heave output
 - SBGECOM-15 - Added sbgInterfaceSerialChangeBaudrate method to change the serial interface baud rate
 - SBGECOM-17 - Added SBG_ECOM_POS_FIXED / SBG_ECAN_POS_FIXED position type for GPS

### Improvements
 - SBGECOM-13 - Updated SBG_ECOM_LOG_SHIP_MOTION_XXX logs to add velocity and status data
 - SBGECOM-16 - Changed GPS OmniStar solution type to PPP ones for better compatibility with third party GPS

### Removed Features
 - SBGECOM-11 - Removed heave status field from SBG_ECOM_LOG_STATUS log

## Release - 1.1

### New Features
 - SBGECOM-1 - Added output log for DVL support 
 - SBGECOM-3 - Added output for GPS 1 raw data in order to support post processing 
 - SBGECOM-4 - Added event markers logs support 
 - SBGECOM-6 - Added Unix support and build script 
 - SBGECOM-8 - Added sbgEComReceiveAnyCmd method that return any received command that is not an output log 
 - SBGECOM-9 - Added settings import and export command

### Improvements
 - SBGECOM-2 - Added pitch information in the SbgLogGpsHdt GPS true heading log 
 - SBGECOM-5 - Now sbgEComProtocolReceive method returns the received command even if the CRC is not valid
