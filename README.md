# sbg_driver
ROS2 driver package for SBG Systems IMU, AHRS and INS.  
This driver package uses the [sbgECom binary protocol](https://github.com/SBG-Systems/sbgECom) to read data and configure SBG Systems devices.  

<i>Initial work has been done by [ENSTA Bretagne](https://github.com/ENSTABretagneRobotics).</i>

**Author: [SBG Systems](https://www.sbg-systems.com/)**  
**Maintainer: [SBG Systems](https://www.sbg-systems.com/)**  
**Contact:** support@sbg-systems.com

## Features
The driver supports all SBG systems sensors, IMUs, AHRS and INS, with a various degree of configurations available.

The driver supports the following features:
 - Parse IMU/AHRS/INS/GNSS using the sbgECom protocol
 - Publish standard ROS2 messages and more detailed specific SBG Systems topics
 - Subscribe and forward RTCM data to support DGPS/RTK mode with centimeters-level accuracy
 - Calibrate 2D/3D magnetic field using the on-board ELLIPSE algorithms
 - Configure the device with the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/), from a JSON settings file

> [!NOTE]
> Device settings are configured with the sbgInsRestApi, which covers the whole product range:
> ELLIPSE, PULSE, EKINOX, APOGEE and QUANTA. Devices that don't support it, such as ELLIPSE
> firmware v2 and before, are handled by a deprecated fallback described in
> [Configure the SBG device](#configure-the-sbg-device).

## Installation
### Installation from Packages
User can install the sbg_ros2_driver through the standard ROS2 installation system.
* Humble ```sudo apt-get install ros-humble-sbg-driver```
* Galactic ```sudo apt-get install ros-galactic-sbg-driver```
* Foxy ```sudo apt-get install ros-foxy-sbg-driver```

### Building from sources
#### Dependencies
* [Robot Operating System (ROS2)](https://docs.ros.org/)
* [sbgECom C Library](https://github.com/SBG-Systems/sbgECom) (embeds v4.0.1987-stable - compatible with ELLIPSE firmware 2.5 and above)

#### Building
1. Clone the repository (use a Release version)
2. Build using the ROS2 colcon build system

```
cd colcon_ws/src
git clone https://github.com/SBG-Systems/sbg_ros2_driver.git
cd sbg_ros2_driver
rosdep update
rosdep install --from-path .
cd ../..
colcon build
source install/setup.bash
```


## Usage
To run the default ROS2 node with the default configuration file
```
ros2 launch sbg_driver sbg_device_launch.py
```

To run the magnetic calibration node

```
ros2 launch sbg_driver sbg_device_mag_calibration_launch.py
```

## Config files
The yaml configuration files cover the ROS2 side of the driver: which interface to use, frame
conventions and IDs, time reference, odometry, RTCM and NMEA topics.

The device settings are not described here. They live in a
[sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) JSON document referenced by the
`ins.settingsFile` parameter — see [Configure the SBG device](#configure-the-sbg-device). Every
SBG log the device sends is published, so there is nothing to declare per topic.

### Default config files
Every configuration file is defined according to the same structure, and differs only by the
communication interface it sets up.

* **sbg_device_uart_default.yaml**
Default config file for a UART connection with the device.

* **sbg_device_udp_default.yaml**
Default config file for an UDP connection with the device.

* **sbg_device_file_default.yaml**
Default config file to replay SBG data from a log file.

None of them configures the device: `confWithRos` is `false`, so the device has to be configured
beforehand.

### Example config files
The examples differ by their ROS2 side settings (baudrate, odometry, ROS2 standard messages).
Product specific device settings are no longer part of them.

* **ellipse_A_default.yaml**
Example config file for an Ellipse-A.

* **ellipse_E_default.yaml**
Example config file for an Ellipse-E with an external NMEA GNSS.

* **ellipse_N_default.yaml**
Example config file for an Ellipse-N using internal GNSS.

* **ellipse_D_default.yaml**
Example config file for an Ellipse-D using internal GNSS. The only example that configures the
device, showing the `ins.settingsFile` workflow.

### Settings and legacy files
* **settings/ellipse_example.json**
Minimal sbgInsRestApi settings document, used by `ellipse_D_default.yaml`.

* **legacy/sbg_device_uart_legacy.yaml**
Reference for the deprecated yaml device configuration format, see
[Deprecated: configuring from yaml parameters](#deprecated-configuring-from-yaml-parameters).

## Launch files
### Default launch files
* **sbg_device_launch.py**
Launch the sbg_device node to handle the received data, and load the `sbg_device_uart_default.yaml` configuration.

* **sbg_device_mag_calibration_launch.py**
Launch the sbg_device_mag node to calibrate the magnetometers, and load the `ellipse_E_default.yaml` configuration.

## Nodes
### sbg_device node
The `sbg_device` node handles the communication with the connected device, publishes the SBG output to the ROS2 environment and subscribes to useful topics such as RTCM data streams.

#### Published Topics
##### SBG Systems specific topics
SBG Systems has defined proprietary ROS2 messages to report more detailed information from the AHRS/INS.  
These messages try to match as much as possible the sbgECom logs as they are output by the device.
>[!NOTE] 
> Please refer to the [firmware manual](https://support.sbg-systems.com/sc/dev/latest/firmware-documentation) to check the availability of the outputs per product.

* **`/sbg/status`** [sbg_driver/SbgStatus](http://docs.ros.org/api/sbg_driver/html/msg/SbgStatus.html)

  Provides information about the general status (Communication, Aiding, etc..).
  
* **`/sbg/utc_time`** [sbg_driver/SbgUtcTime](http://docs.ros.org/api/sbg_driver/html/msg/SbgUtcTime.html)

  Provides UTC time reference.

* **`/sbg/imu_data`** [sbg_driver/SbgImuData](http://docs.ros.org/api/sbg_driver/html/msg/SbgImuData.html)

  IMU status, and sensors values.
  
* **`/sbg/ekf_euler`** [sbg_driver/SbgEkfEuler](http://docs.ros.org/api/sbg_driver/html/msg/SbgEkfEuler.html)

  Computed orientation using Euler angles.
  
* **`/sbg/ekf_quat`** [sbg_driver/SbgEkfQuat](http://docs.ros.org/api/sbg_driver/html/msg/SbgEkfQuat.html)

  Computed orientation using Quaternion.
  
* **`/sbg/ekf_nav`** [sbg_driver/SbgEkfNav](http://docs.ros.org/api/sbg_driver/html/msg/SbgEkfNav.html)

  Computed navigation data.
  
* **`/sbg/ekf_vel_body`** [sbg_driver/SbgEkfVelBody](http://docs.ros.org/api/sbg_driver/html/msg/SbgEkfVelBody.html)

  Computed velocity expressed in the INS body/vehicle frame.
  
* **`/sbg/ekf_rot_accel_body`** [sbg_driver/SbgEkfRotAccel](http://docs.ros.org/api/sbg_driver/html/msg/SbgEkfRotAccel.html)

  Computed rotations rate and accelerations in the INS body/vehicle frame.
  
* **`/sbg/ekf_rot_accel_ned`** [sbg_driver/SbgEkfRotAccel](http://docs.ros.org/api/sbg_driver/html/msg/SbgEkfRotAccel.html)

  Computed rotations rate and accelerations in North, East, Down (NED) navigation frame.
  
* **`/sbg/mag`** [sbg_driver/SbgMag](http://docs.ros.org/api/sbg_driver/html/msg/SbgMag.html)

  Calibrated magnetic field measurement.
  
* **`/sbg/mag_calib`** [sbg_driver/SbgMagCalib](http://docs.ros.org/api/sbg_driver/html/msg/SbgMagCalib.html)

  Magnetometer calibration data.
  
* **`/sbg/ship_motion`** [sbg_driver/SbgShipMotion](http://docs.ros.org/api/sbg_driver/html/msg/SbgShipMotion.html)

  Heave, surge and sway data.
  
* **`/sbg/gps_vel`** [sbg_driver/SbgGpsVel](http://docs.ros.org/api/sbg_driver/html/msg/SbgGpsVel.html)

  GPS velocities from GPS receiver.
  
* **`/sbg/gps_pos`** [sbg_driver/SbgGpsPos](http://docs.ros.org/api/sbg_driver/html/msg/SbgGpsPos.html)

  GPS positions from GPS receiver.
  
* **`/sbg/gps_hdt`** [sbg_driver/SbgGpsHdt](http://docs.ros.org/api/sbg_driver/html/msg/SbgGpsHdt.html)

  GPS true heading from dual antenna system.
  
* **`/sbg/gps_raw`** [sbg_driver/SbgGpsRaw](http://docs.ros.org/api/sbg_driver/html/msg/SbgGpsRaw.html)

  GPS raw data for post processing.
  
* **`/sbg/odo_vel`** [sbg_driver/SbgOdoVel](http://docs.ros.org/api/sbg_driver/html/msg/SbgOdoVel.html)

  Odometer velocity.
  
* **`/sbg/event[ABCDE]`** [sbg_driver/SbgEvent](http://docs.ros.org/api/sbg_driver/html/msg/SbgEvent.html)

  Event on sync in the corresponding pin.
  
* **`/sbg/pressure`** [sbg_driver/SbgPressure](http://docs.ros.org/api/sbg_driver/html/msg/SbgPressure.html)

  Pressure data.

##### ROS2 standard topics
In order to define ROS2 standard topics, it requires sometimes several SBG messages, to be merged.
For each ROS2 standard, you have to activate the needed SBG outputs.

* **`/imu/data`** [sensor_msgs/Imu](http://docs.ros.org/melodic/api/sensor_msgs/html/msg/Imu.html)

  IMU data.
  Requires `/sbg/imu_data` or `/sbg/imu_short`.
  Optional (orientation) `/sbg/ekf_quat`.
  
* **`/imu/temp`** [sensor_msgs/Temperature](http://docs.ros.org/melodic/api/sensor_msgs/html/msg/Temperature.html)

  IMU temperature data.
  Requires `/sbg/imu_data`.
  
* **`/imu/velocity`** [geometry_msgs/TwistStamped](http://docs.ros.org/melodic/api/geometry_msgs/html/msg/TwistStamped.html)

  IMU velocity data.
  Requires `/sbg/imu_data`.
  
* **`/imu/mag`** [sensor_msgs/MagneticField](http://docs.ros.org/melodic/api/sensor_msgs/html/msg/MagneticField.html)

  IMU magnetic field.
  Requires `/sbg/mag`.
  
* **`/imu/pres`** [sensor_msgs/FluidPressure](http://docs.ros.org/melodic/api/sensor_msgs/html/msg/FluidPressure.html)

  IMU pressure data.
  Requires `/sbg/pressure`.
  
* **`/imu/pos_ecef`** [geometry_msgs/PointStamped](http://docs.ros.org/melodic/api/geometry_msgs/html/msg/PointStamped.html)

  Earth-Centered Earth-Fixed position.
  Requires `/sbg/ekf_nav`.
  
* **`/imu/utc_ref`** [sensor_msgs/TimeReference](http://docs.ros.org/melodic/api/sensor_msgs/html/msg/TimeReference.html)

  UTC time reference.
  Requires `/sbg/utc_time`.
  
* **`/imu/nav_sat_fix`** [sensor_msgs/NavSatFix](http://docs.ros.org/melodic/api/sensor_msgs/html/msg/NavSatFix.html)

  Navigation satellite fix for any Global Navigation Satellite System.
  Requires `/sbg/gps_pos`.
  
* **`/imu/odometry`** [nav_msgs/Odometry](http://docs.ros.org/en/melodic/api/nav_msgs/html/msg/Odometry.html)

  UTM projected position relative to the first valid INS position.
  Requires `/sbg/imu_data` and `/sbg/ekv_nav` and either `/sbg/ekf_euler` or `/sbg/ekf_quat`.
  Disabled by default, set `odometry.enable` in configuration file.

> [!NOTE]
> Please update the driver configuration to enable standard ROS2 messages publication. Also, the driver only publish standard ROS2 messages if the driver is setup to use ENU frame convention.

##### NMEA topics
The driver can publish NMEA GGA messages from the internal GNSS receiver. It can be used with third party [NTRIP client](https://github.com/LORD-MicroStrain/ntrip_client) modules to support VRS networks providers.

 Disabled by default, set `nmea.publish` to `true` in .yaml config file to use this feature.

* **`/ntrip_client/nmea`** [nmea_msgs/Sentence](http://docs.ros.org/en/api/nmea_msgs/html/msg/Sentence.html)  
  
  Data from `/sbg/gps_pos` serialized into NMEA GGA format. Requires `/sbg/gps_pos`.  
  Namespace `ntrip_client` and topic_name `nmea` can be customized in .yaml config files.

#### Subscribed Topics
##### RTCM topics
The `sbg_device` node can subscribe to RTCM topics published by third party ROS2 modules.  
Incoming RTCM data are forwarded to the INS internal GNSS receiver to enable DGPS/RTK solutions.

 Disabled by default, set `rtcm.subscribe` to `true` in .yaml config file to use this feature.

* **`/ntrip_client/rtcm`** [rtcm_msgs/Message](https://github.com/tilk/rtcm_msgs/blob/master/msg/Message.msg)

  RTCM data from `/ntrip_client/rtcm` will be forwarded to the internal INS GNSS receiver.  
  Namespace `ntrip_client` and topic_name `rtcm` can be customized in .yaml config files.

### sbg_device_mag node
The sbg_device_mag node is used to execute on board in-situ 2D or 3D magnetic field calibration.  
If you are planning to use magnetic based heading, it is mandatory to perform a magnetic field calibration in a clean magnetic environnement.

Only ELLIPSE products support magnetic based heading and feature the on-board magnetic field calibration process.

#### Services
* **`/sbg/mag_calibration`** [std_srvs/Trigger](http://docs.ros.org/api/std_srvs/html/srv/Trigger.html)

  Service to start/stop the magnetic calibration.

* **`/sbg/mag_calibration_save`** [std_srvs/Trigger](http://docs.ros.org/api/std_srvs/html/srv/Trigger.html)

  Service to save in FLASH memory the latest computed magnetic field calibration.

## HowTo
### Configure the SBG device
The SBG ROS2 driver can configure the device before starting data parsing. The device settings
are described by a [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) JSON
document, which the driver sends to the `api/v1/settings` endpoint at startup.

Enable the configuration and point the driver at the settings file:

```
# Configuration of the device with ROS2.
confWithRos: true

ins:
  # sbgInsRestApi JSON settings file to apply at startup.
  settingsFile: "/path/to/my_device_settings.json"
```

The easiest way to write that file is to configure the device once, then export its settings.
Use the device web interface, or the `sbgEComApi` command line tool shipped with sbgECom:

```
# Export the current settings of the device
sbgEComApi -s /dev/ttyUSB0 -r 115200 api/v1/settings -g > my_device_settings.json

# Only the settings that differ from the device defaults
sbgEComApi -s /dev/ttyUSB0 -r 115200 "api/v1/settings?delta=true&format=pretty" -g
```

A partial document is accepted, so the file may hold only the fields to update. See
[config/settings/ellipse_example.json](config/settings/ellipse_example.json) for a minimal
example, and the [sbgInsRestApi reference](https://developer.sbg-systems.com/sbgInsRestApi/)
for the settings available on your product and firmware version.

The driver reads the device reply to know whether the new settings require a reboot. When they
do, it saves them to the FLASH memory, reboots the device and reopens the communication
interface, all before it starts publishing.

> [!NOTE]
> `confWithRos` only impacts the configuration of the SBG device, not the configuration of the
> ROS2 driver itself. RTCM corrections, reference frames, published topics, etc. are still
> configured from the yaml file when `confWithRos` is false.

#### Deprecated: configuring from yaml parameters
Before this release the device settings were described by dedicated yaml parameters
(`sensorParameters`, `imuAlignementLeverArm`, `aidingAssignment`, `gnss`, `odom`, `output.log_*`,
...) and applied with sbgECom binary commands. Those commands are deprecated by sbgECom itself
and only cover the ELLIPSE family.

That path is still built by default and still works, so an existing configuration keeps running
after upgrading, but it now logs a deprecation warning. It is only selected when the connected
device does not support the sbgInsRestApi, which is the case for ELLIPSE firmware v2 and before.

[config/legacy/sbg_device_uart_legacy.yaml](config/legacy/sbg_device_uart_legacy.yaml) is kept as
a reference for that format. To build without the deprecated path at all:

```
colcon build --cmake-args -DSBG_USE_DEPRECATED_ECOM_CONFIG=OFF
```

The option will default to `OFF`, then be removed, in future releases. Migrate by exporting your
device settings to a JSON file as described above.

### Configure for RTK/DGPS
The `sbg_device` node can subscribe to [rtcm_msgs/Message](https://github.com/tilk/rtcm_msgs/blob/master/msg/Message.msg) topics to forward differential corrections to the INS internal GNSS receiver.

The RTCM data stream is sent through the serial/ethernet interface used by ROS2 to communicate with the INS.  
This enables simple and efficient RTK operations without requiring additional hardware or wiring.

When combined with a third party [NTRIP client](https://github.com/LORD-MicroStrain/ntrip_client), it offers a turnkey solution to access local VRS providers and get centimeter-level accuracy solutions.

The driver and the device should be properly setup:
 - Configure the INS to accept RTCM corrections on the interface used by the ROS2 driver:
   - For ELLIPSE, simply use the `sbgCenter` and in `Assignment panel`, `RTCM` should be set to `Port A`.
   - For High Performance INS, either use the configuration web interface or the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).
 - Install and configure a third party node that broadcast RTCM corrections such as a [NTRIP client](https://github.com/LORD-MicroStrain/ntrip_client)
 - Update the node config `yaml` file to set `rtcm.subscribe` and `nmea.publish` to `true`
 - If you use a different node to broadcast RTCM topics, you might have to update the config `yaml` file to update topics and namespaces.

### Calibrate the magnetometers
ELLIPSE products can use magnetometers to determine the heading. A calibration is then required to compensate for soft and hard iron distortions due to the vehicle the product is installed on. The magnetic calibration procedure should be held in a clean magnetic environnement (outside of buildings).

You can read more information about magnetic field calibration procedure from the SBG Systems [Support Center](https://support.sbg-systems.com/sc/kb/v3/inertial-sensors-installation/magnetic-calibration).

The ROS2 driver provides a dedicated node to easily use ELLIPSE on board magnetic field calibration algorithms.  
The ELLIPSE offers both a 2D and 3D magnetic field calibration mode.

The procedure runs over the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/). On
devices that don't support it, such as ELLIPSE firmware v2 and before, the driver falls back to
the deprecated sbgECom magnetic calibration commands, which requires a driver built with
`SBG_USE_DEPRECATED_ECOM_CONFIG` (the current default).

> [!NOTE]
> The `calibration.bandwidth` parameter has been removed from the configuration files. It has no
> effect on ELLIPSE firmware v3.0 and above and has no sbgInsRestApi equivalent. It is only read
> on the deprecated fallback path, see
> [config/legacy/sbg_device_uart_legacy.yaml](config/legacy/sbg_device_uart_legacy.yaml).

1) Make sure you have selected the desired 2D or 3D magnetic field calibration mode (`calibration.mode` in the configuration `yaml` file).
2) Start a new magnetic calibration session once you are ready to map the magnetic field:

```
ros2 launch sbg_driver sbg_device_mag_calibration_launch.py
ros2 service call /sbg/mag_calibration std_srvs/srv/Trigger
```

> response:
> std_srvs.srv.Trigger_Response(success=True, message='Magnetometer calibration process started.')

3) Rotate as much as possible the unit to map the surrounding magnetic field (ideally, perform a 360° with X then Y then Z axis pointing downward).
4) Once you believe you have covered enough orientations, compute a magnetic field calibration:

```
ros2 service call /sbg/mag_calibration std_srvs/srv/Trigger
```

> response:
> std_srvs.srv.Trigger_Response(success=True, message='Magnetometer calibration is finished. See the output console to get calibration information.')

5) If you are happy with the results (Quality, Confidence), apply and save the new magnetic calibration parameters.  
   If not, you can continue to rotate the product and try to perform a new computation (and repeat step 4)

```
ros2 service call /sbg/mag_calibration_save std_srvs/srv/Trigger
```

> response:
> std_srvs.srv.Trigger_Response(success=True, message='Magnetometer calibration has been uploaded to the device.')

6) Reset/Power Cycle the device and you should now get an accurate magnetic based heading.

### Enable communication with the SBG device
To be able to communicate with the device, be sure that your user is part of the dialout group.  
Once added, restart your machine to save and apply the changes.

```
sudo adduser $USER dialout
```

### Create udev rules
Udev rules can be defined for communication port, in order to avoid modifying the port in configuration if it has changed.
[Udev documentation](https://wiki.debian.org/udev)

A symlink can be configured and defined to uniquely identify the connected device.  
Once it is done, configuration file could be updated `portName: "/dev/sbg"`.

See the docs folder, to see an example of rules with the corresponding screenshot using the udev functions.

### Time source & reference
ROS2 uses an internal system time to time stamp messages. This time stamp is generally gathered when the message is processed and published.
As a result, the message is not time stamped accurately due to transmission and processing delays.

SBG Systems INS however provides a very accurate timing based on GNSS time if available. The following conditions have to be met to get
absolute accurate timing information:
* The ELLIPSE-N or D should have a connected GNSS antenna with internal GNSS enabled
* The ELLIPSE-E should be connected to an external GNSS receiver with a PPS signal
* A valid GNSS position has to be available to get UTC data
* The ELLIPSE internal clock should be aligned to PPS signal (clock status)
* The ELLIPSE should be setup to send SBG_ECOM_LOG_UTC message

You can select which time source to use with the parameter `time_reference` to time stamp messages published by this driver:
* `ros`: The header.stamp member contains the current ROS2 system time when the message has been processed.
* `ins_unix`: The header.stamp member contains an absolute and accurate time referenced to UNIX epoch (00:00:00 UTC on 1 January 1970)

Configuration example to use an absolute and accurate time reference to UNIX epoch:
```
# Time reference:
time_reference: "ins_unix"
```

## Frame parameters & conventions
### Frame ID
The frame_id of the header can be set with this parameter:
```
# Frame name
frame_id: "imu_link_ned"
```

> [!NOTE]
> This parameter has not impact on the configuration and will be inserted as-is within the header field. The recommended default parameters for ros are imu_link when use_enu is activated, and imu_link_ned when use_enu is inactive. You can also use another frame_id as needed.

### GPS Frame ID
GPS position, velocity and true heading logs are expressed at the GNSS antenna phase center and not at the IMU.
The frame_id of the header of these messages (including `sensor_msgs/NavSatFix`) can be set with this parameter:
```
# GPS antenna frame name
gps_frame_id: "gps_link"
```

> [!NOTE]
> If this parameter is not set, it defaults to frame_id to keep the previous driver behavior. Set it to the frame of your GNSS antenna so consumers such as robot_localization navsat_transform_node can correctly compensate the antenna lever arm.

### Frame convention
The frame convention can be set to NED or ENU:
* The NED convention is SBG Systems native convention so no transformation is applied
* The ENU convention follows ROS2 standard [REP-103](https://www.ros.org/reps/rep-0103.html#coordinate-frame-conventions)

Please read the SBG Systems [Support Center article](https://support.sbg-systems.com/sc/kb/latest/underlying-maths-conventions) for more details.

You can select the frame convention to use with the following parameter:
```
# Frame convention
use_enu: true
```

> [!NOTE]
> The driver only publish standard ROS2 messages if the driver is setup to use ENU frame convention.

#### Body/Vehicle Frame:
The X axis should point the vehicle **forward** direction for both NED and ENU frame conventions. 
The table below summarizes the body/vehicle axis frame definitions for each convention:

| NED Convention | ENU Convention |
| -------------- | -------------- |
| X Forward      | X Forward      |
| Y Right        | Y Left         |
| Z Downward     | Z Upward       |

#### Navigation Frame:

The navigation frame also referred by ROS2 as the cartesian representation is defined as follow:

| NED Convention | ENU Convention |
| -------------- | -------------- |
| X North        | X East         |
| Y East         | Y North        |
| Z Down         | Z Up           |

#### Heading Example:

Based on the definitions above, when using a NED frame, if the vehicle X axis is pointing North, the INS should return a zero heading. 
When using a ENU frame, the INS should return a zero heading when the vehicle X axis is pointing East.

## Sensor fusion with state estimation packages

The ROS2 standard outputs of the driver can feed any state estimation stack consuming standard `sensor_msgs/Imu` and `sensor_msgs/NavSatFix` messages, for example to fuse the INS with wheel odometry on ground robots.

### Driver configuration
Standard ROS2 messages are disabled by default and require the ENU frame convention. In the .yaml configuration file:

```
output:
  # Publish ROS2 standard messages
  ros_standard: true
  # ROS2 standard messages require the ENU convention (REP-103)
  use_enu: true
  # Frame of the IMU body (FLU) and frame of the GNSS antenna
  frame_id: "imu_link"
  gps_frame_id: "gps_link"
```

Each standard topic is built from several SBG logs, and is only published once the logs it needs
have been received. Make sure the device is configured to output them — `imuData` (or `imuShort`)
and `ekfQuat` for `/imu/data` and `/imu/velocity`, `gps1Pos` for `/imu/nav_sat_fix`, `utcTime`
for `/imu/utc_ref` — see [Configure the SBG device](#configure-the-sbg-device).

### Topics and frames

| Topic | Message | Content |
| ----- | ------- | ------- |
| `/imu/data` | sensor_msgs/Imu | Body-frame (FLU) rates and accelerations, orientation with respect to ENU |
| `/imu/nav_sat_fix` | sensor_msgs/NavSatFix | Raw GNSS position, expressed at the antenna phase center |
| `/imu/utc_ref` | sensor_msgs/TimeReference | INS UTC time reference |

State estimators rely on TF to compensate the GNSS antenna lever arm: publish static transforms from `base_link` to `frame_id` (IMU mounting) and from `base_link` to `gps_frame_id` (antenna mounting) matching the physical setup.

> [!NOTE]
> `/imu/nav_sat_fix` is not compensated for the antenna lever arm. The INS EKF fused position, expressed at the INS reference point, is available on `/sbg/ekf_nav` and `/imu/odometry`.

## Troubleshooting

If you experience higher latency than expected and have connected the IMU via an USB interface, you can enable the serial driver low latency mode:
```
/bin/setserial /dev/<device> low_latency
```

## Contributing
### Bugs and issues
Please report bugs and/or issues using the [Issue Tracker](https://github.com/SBG-Systems/sbg_ros2_driver/issues)

### Features requests or additions
In order to contribute to the code, please use Pull requests to the `devel` branch.  
If you have some feature requests, use the [Issue Tracker](https://github.com/SBG-Systems/sbg_ros2_driver/issues) as well.

## Known limitations
> Device information is not displayed for Pulse-40.
> Every SBG log received is published, so topics are advertised for logs the connected device may never send.
> The settings file is applied on every launch when `confWithRos` is true. If the device reports that the new settings require a reboot, this writes its FLASH memory and reboots it at each start.
> Baudrate, GNSS and RTCM port configuration are set through the `ins.settingsFile` document, they have no dedicated ROS2 parameter.  
