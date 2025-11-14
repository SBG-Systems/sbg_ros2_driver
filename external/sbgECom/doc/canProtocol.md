# CAN Protocol Specifications

The CAN protocol in SBG Systems devices offers extensive support for nearly all the information provided by the `sbgECom` serial protocol.  
Although the CAN and serial protocols are not identical, they share many definitions and data structures to ensure consistency.

The CAN interface is primarily designed for outputting messages and can support external aiding equipment such as CAN-based odometers.  
However, the CAN interface cannot be used to configure the device. For configuration purposes, standard serial or Ethernet interfaces must be utilized.

## CAN Conventions & Definitions

### CAN 2.0 A/B Interface

The protocol described in this documentation is used to communicate with the AHRS/INS over a Controller Area Network (CAN) bus.  
The CAN bus is a message-based protocol originally designed for automotive applications but is now widely used across various industries.

SBG Systems' CAN implementation supports both CAN 2.0A and CAN 2.0B standards, ensuring maximum compatibility with third-party equipment and CAN software.  
The CAN bus implementation adheres to the CiA (CAN in Automation) DS-102 standard for quantum and timing definitions.

The following standard CAN bus bitrates are supported:
- 1 000 kBit/s
- 800 kBit/s
- 500 kBit/s
- 250 kBit/s
- 200 kBit/s
- 125 kBit/s
- 100 kBit/s
- 50 kBit/s
- 25 kBit/s
- 20 kBit/s
- 10 kBit/s

A maximum of 8 bytes per frame are transmitted, both standard (11 bits) and extended (29 bits) identifiers are supported.

> [!WARNING]
> SBG Systems' products do not include a 120 Ohm termination resistor; it is the user's responsibility to implement this component.

### Status and Enum Definitions

All status indicators, enumerations, and bitmasks used in the CAN messages are consistent with those in the [sbgECom Binary Messages](#binaryMessages) definitions.  
To ensure simplicity and maintainability, please refer to the [sbgECom Binary Messages](#binaryMessages) for detailed definitions of status flags, bitmasks, and enumerations.

### Value Scaling and Units

To maximize compatibility with third-party CAN software, each parameter in the CAN messages can be adjusted using a simple offset and scale factor.  
For example, if a parameter has a scale factor of $10^{-2}$, the value returned by the device should be multiplied by $10^{-2}$ (or divided by 100) to convert it to the correct unit specified in the message.

### Message Identification

Each CAN message is assigned a unique identifier, encoded using 11 bits for standard CAN 2.0A messages or 29 bits for extended CAN 2.0B messages.
- **Standard CAN messages**: Message IDs range from `0x000` to `0x7FF`, allowing for 2,048 unique identifiers.
- **Extended CAN messages**: IDs can go up to `0x1FFFFFF`, offering over 536 million unique identifiers.

By default, all CAN message IDs are disabled to prevent conflicts with other devices on the same network.  
When configuring CAN message outputs, it is crucial to carefully select IDs to avoid collisions with other devices' IDs.

## CAN Messages Overview

The following section provides an overview of all available CAN messages with a brief description and default CAN identifier.

Because CAN payloads are limited to 8 bytes only, consistent data might be spread across several CAN messages.  
For example, the two CAN messages `SBG_ECAN_MSG_UTC_0` and `SBG_ECAN_MSG_UTC_1` form a consistent dataset that should be interpreted together.

| Name (Log ID)                                                                   | Description                                                                 |
|---------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| [SBG_ECAN_MSG_STATUS_01 (0x100)](#SBG_ECAN_MSG_STATUS_01)                       | General device status and information - part 1/3.                           |
| [SBG_ECAN_MSG_STATUS_02 (0x101)](#SBG_ECAN_MSG_STATUS_02)                       | General device status and information - part 2/3.                           |
| [SBG_ECAN_MSG_STATUS_03 (0x102)](#SBG_ECAN_MSG_STATUS_03)                       | General device status and information - part 3/3.                           |
| [SBG_ECAN_MSG_UTC_0 (0x110)](#SBG_ECAN_MSG_UTC_0)                               | INS clock and UTC time reference - part 1/2.                                |
| [SBG_ECAN_MSG_UTC_1 (0x111)](#SBG_ECAN_MSG_UTC_1)                               | INS clock and UTC time reference - part 2/2.                                |
| [SBG_ECAN_MSG_IMU_INFO (0x120)](#SBG_ECAN_MSG_IMU_INFO)                         | IMU timestamp, status and temperature.                                      |
| [SBG_ECAN_MSG_IMU_ACCEL (0x121)](#SBG_ECAN_MSG_IMU_ACCEL)                       | IMU body X,Y,Z accelerometers.                                              |
| [SBG_ECAN_MSG_IMU_GYRO (0x122)](#SBG_ECAN_MSG_IMU_GYRO)                         | IMU body X,Y,Z rotation rates.                                              |
| [SBG_ECAN_MSG_IMU_DELTA_VEL (0x123)](#SBG_ECAN_MSG_IMU_ACCEL)                   | Deprecated - same as `SBG_ECAN_MSG_IMU_ACCEL`.                              |
| [SBG_ECAN_MSG_IMU_DELTA_ANGLE (0x124)](#SBG_ECAN_MSG_IMU_GYRO)                  | Deprecated - same as `SBG_ECAN_MSG_IMU_GYRO`.                               |
| [SBG_ECAN_MSG_EKF_INFO (0x130)](#SBG_ECAN_MSG_EKF_INFO)                         | AHRS/INS solution timestamp.                                                |
| [SBG_ECAN_MSG_EKF_QUAT (0x131)](#SBG_ECAN_MSG_EKF_QUAT)                         | AHRS/INS attitude using quaternion representation.                          |
| [SBG_ECAN_MSG_EKF_EULER (0x132)](#SBG_ECAN_MSG_EKF_EULER)                       | AHRS/INS attitude using Euler angles representation.                        |
| [SBG_ECAN_MSG_EKF_ORIENTATION_ACC (0x133)](#SBG_ECAN_MSG_EKF_ORIENTATION_ACC)   | AHRS/INS attitude standard deviation accuracy.                              |
| [SBG_ECAN_MSG_EKF_POS (0x134)](#SBG_ECAN_MSG_EKF_POS)                           | INS latitude/longitude position.                                            |
| [SBG_ECAN_MSG_EKF_ALTITUDE (0x135)](#SBG_ECAN_MSG_EKF_ALTITUDE)                 | INS altitude above MSL and undulation.                                      |
| [SBG_ECAN_MSG_EKF_POS_ACC (0x136)](#SBG_ECAN_MSG_EKF_POS_ACC)                   | INS position standard deviation accuracy.                                   |
| [SBG_ECAN_MSG_EKF_VEL_NED (0x137)](#SBG_ECAN_MSG_EKF_VEL_NED)                   | INS velocity in North, East, Down navigation frame.                         |
| [SBG_ECAN_MSG_EKF_VEL_NED_ACC (0x138)](#SBG_ECAN_MSG_EKF_VEL_NED_ACC)           | INS velocity standard deviation accuracy in the NED frame.                  |
| [SBG_ECAN_MSG_EKF_VEL_BODY (0x139)](#SBG_ECAN_MSG_EKF_VEL_BODY)                 | INS velocity X, Y, Z body frame.                                            |
| [SBG_ECAN_MSG_AUTO_TRACK_SLIP_CURV (0x220)](#SBG_ECAN_MSG_AUTO_TRACK_SLIP_CURV) | Automotive-specific track angle, slip angle, and curvature radius.          |
| [SBG_ECAN_MSG_SHIP_MOTION_INFO (0x140)](#SBG_ECAN_MSG_SHIP_MOTION_INFO)         | Marine-specific heave, surge, sway timestamp, and status.                   |
| [SBG_ECAN_MSG_SHIP_MOTION_0 (0x141)](#SBG_ECAN_MSG_SHIP_MOTION_0)               | Surge, sway, and heave measurements.                                        |
| [SBG_ECAN_MSG_SHIP_MOTION_1 (0x145)](#SBG_ECAN_MSG_SHIP_MOTION_1)               | Surge, sway, and heave acceleration measurements.                           |
| [SBG_ECAN_MSG_SHIP_MOTION_2 (0x149)](#SBG_ECAN_MSG_SHIP_MOTION_2)               | Surge, sway, and heave velocity measurements.                               |
| [SBG_ECAN_MSG_SHIP_MOTION_HP_INFO (0x14A)](#SBG_ECAN_MSG_SHIP_MOTION_HP_INFO)   | Delayed heave timestamp and status.                                         |
| [SBG_ECAN_MSG_SHIP_MOTION_HP_0 (0x14B)](#SBG_ECAN_MSG_SHIP_MOTION_HP_0)         | Delayed heave measurement (surge and sway are not computed).                |
| [SBG_ECAN_MSG_SHIP_MOTION_HP_1 (0x14C)](#SBG_ECAN_MSG_SHIP_MOTION_HP_1)         | Vessel's acceleration used to compute delayed heave.                        |
| [SBG_ECAN_MSG_SHIP_MOTION_HP_2 (0x14D)](#SBG_ECAN_MSG_SHIP_MOTION_HP_2)         | Delayed heave velocity (surge and sway are not computed).                   |
| [SBG_ECAN_MSG_MAG_0 (0x150)](#SBG_ECAN_MSG_MAG_0)                               | Magnetometer aiding timestamp and status.                                   |
| [SBG_ECAN_MSG_MAG_1 (0x151)](#SBG_ECAN_MSG_MAG_1)                               | Normalized X, Y, Z magnetic field values.                                   |
| [SBG_ECAN_MSG_MAG_2 (0x152)](#SBG_ECAN_MSG_MAG_2)                               | Accelerometers X, Y, Z aligned with magnetometer frame.                     |
| [SBG_ECAN_MSG_ODO_INFO (0x160)](#SBG_ECAN_MSG_ODO_INFO)                         | Wheel speed odometer velocity timestamp and status.                         |
| [SBG_ECAN_MSG_ODO_VEL (0x161)](#SBG_ECAN_MSG_ODO_VEL)                           | Wheel speed odometer velocity measurement.                                  |
| [SBG_ECAN_MSG_AIR_DATA_INFO (0x162)](#SBG_ECAN_MSG_AIR_DATA_INFO)               | AirData aiding timestamp, status and air temperature.                       |
| [SBG_ECAN_MSG_AIR_DATA_ALTITUDE (0x163)](#SBG_ECAN_MSG_AIR_DATA_ALTITUDE)       | AirData static pressure and barometric altitude.                            |
| [SBG_ECAN_MSG_AIR_DATA_AIRSPEED (0x164)](#SBG_ECAN_MSG_AIR_DATA_AIRSPEED)       | AirData dynamic pressure and true airspeed.                                 |
| [SBG_ECAN_MSG_DEPTH_INFO (0x166)](#SBG_ECAN_MSG_DEPTH_INFO)                     | Subsea depth pressure sensor timestamp and status.                          |
| [SBG_ECAN_MSG_DEPTH_ALTITUDE (0x167)](#SBG_ECAN_MSG_DEPTH_ALTITUDE)             | Subsea water pressure value and depth value.                                |
| [SBG_ECAN_MSG_GPS1_VEL_INFO (0x170)](#SBG_ECAN_MSG_GPSX_VEL_INFO)               | GNSS module 1 velocity timestamp and status.                                |
| [SBG_ECAN_MSG_GPS1_VEL (0x171)](#SBG_ECAN_MSG_GPSX_VEL)                         | GNSS module 1 velocity North, East, Down values.                            |
| [SBG_ECAN_MSG_GPS1_VEL_ACC (0x172)](#SBG_ECAN_MSG_GPSX_VEL_ACC)                 | GNSS module 1 velocity NED standard deviation accuracies.                   |
| [SBG_ECAN_MSG_GPS1_COURSE (0x173)](#SBG_ECAN_MSG_GPSX_COURSE)                   | GNSS module 1 course over ground and accuracy.                              |
| [SBG_ECAN_MSG_GPS1_POS_INFO (0x174)](#SBG_ECAN_MSG_GPSX_POS_INFO)               | GNSS module 1 position timestamp and status.                                |
| [SBG_ECAN_MSG_GPS1_POS (0x175)](#SBG_ECAN_MSG_GPSX_POS)                         | GNSS module 1 position latitude and longitude.                              |
| [SBG_ECAN_MSG_GPS1_POS_ALT (0x176)](#SBG_ECAN_MSG_GPSX_POS_ALT)                 | GNSS module 1 altitude MSL, undulation and information.                     |
| [SBG_ECAN_MSG_GPS1_POS_ACC (0x177)](#SBG_ECAN_MSG_GPSX_POS_ACC)                 | GNSS module 1 position accuracy and information.                            |
| [SBG_ECAN_MSG_GPS1_HDT_INFO (0x178)](#SBG_ECAN_MSG_GPSX_HDT_INFO)               | GNSS module 1 dual antenna timestamp and status.                            |
| [SBG_ECAN_MSG_GPS1_HDT (0x179)](#SBG_ECAN_MSG_GPSX_HDT)                         | GNSS module 1 dual antenna true heading, pitch angle, and accuracies.       |
| [SBG_ECAN_MSG_GPS2_VEL_INFO (0x180)](#SBG_ECAN_MSG_GPSX_VEL_INFO)               | GNSS module 2 velocity timestamp and status.                                |
| [SBG_ECAN_MSG_GPS2_VEL (0x181)](#SBG_ECAN_MSG_GPSX_VEL)                         | GNSS module 2 velocity North, East, Down values.                            |
| [SBG_ECAN_MSG_GPS2_VEL_ACC (0x182)](#SBG_ECAN_MSG_GPSX_VEL_ACC)                 | GNSS module 2 velocity NED standard deviation accuracies.                   |
| [SBG_ECAN_MSG_GPS2_COURSE (0x183)](#SBG_ECAN_MSG_GPSX_COURSE)                   | GNSS module 2 course over ground and accuracy.                              |
| [SBG_ECAN_MSG_GPS2_POS_INFO (0x184)](#SBG_ECAN_MSG_GPSX_POS_INFO)               | GNSS module 2 position timestamp and status.                                |
| [SBG_ECAN_MSG_GPS2_POS (0x185)](#SBG_ECAN_MSG_GPSX_POS)                         | GNSS module 2 position latitude and longitude.                              |
| [SBG_ECAN_MSG_GPS2_POS_ALT (0x186)](#SBG_ECAN_MSG_GPSX_POS_ALT)                 | GNSS module 2 altitude MSL, undulation and information.                     |
| [SBG_ECAN_MSG_GPS2_POS_ACC (0x187)](#SBG_ECAN_MSG_GPSX_POS_ACC)                 | GNSS module 2 position accuracy and information.                            |
| [SBG_ECAN_MSG_GPS2_HDT_INFO (0x188)](#SBG_ECAN_MSG_GPSX_HDT_INFO)               | GNSS module 2 dual antenna timestamp and status.                            |
| [SBG_ECAN_MSG_GPS2_HDT (0x189)](#SBG_ECAN_MSG_GPSX_HDT)                         | GNSS module 2 dual antenna true heading, pitch angle, and accuracies.       |
| [SBG_ECAN_MSG_EVENT_INFO_A (0x200)](#SBG_ECAN_MSG_EVENT_INFO_X)                 | Sync In A event marker timestamp and status.                                |
| [SBG_ECAN_MSG_EVENT_TIME_A (0x201)](#SBG_ECAN_MSG_EVENT_TIME_X)                 | Additional Sync In A event marker offset from primary timestamp.            |
| [SBG_ECAN_MSG_EVENT_INFO_B (0x202)](#SBG_ECAN_MSG_EVENT_INFO_X)                 | Sync In B event marker timestamp and status.                                |
| [SBG_ECAN_MSG_EVENT_TIME_B (0x203)](#SBG_ECAN_MSG_EVENT_TIME_X)                 | Additional Sync In B event marker offset from primary timestamp.            |
| [SBG_ECAN_MSG_EVENT_INFO_C (0x204)](#SBG_ECAN_MSG_EVENT_INFO_X)                 | Sync In C event marker timestamp and status.                                |
| [SBG_ECAN_MSG_EVENT_TIME_C (0x205)](#SBG_ECAN_MSG_EVENT_TIME_X)                 | Additional Sync In C event marker offset from primary timestamp.            |
| [SBG_ECAN_MSG_EVENT_INFO_D (0x206)](#SBG_ECAN_MSG_EVENT_INFO_X)                 | Sync In D event marker timestamp and status.                                |
| [SBG_ECAN_MSG_EVENT_TIME_D (0x207)](#SBG_ECAN_MSG_EVENT_TIME_X)                 | Additional Sync In D event marker offset from primary timestamp.            |
| [SBG_ECAN_MSG_EVENT_INFO_E (0x208)](#SBG_ECAN_MSG_EVENT_INFO_X)                 | Sync In E event marker timestamp and status.                                |
| [SBG_ECAN_MSG_EVENT_TIME_E (0x209)](#SBG_ECAN_MSG_EVENT_TIME_X)                 | Additional Sync In E event marker offset from primary timestamp.            |
## General Status Messages

These outputs aggregate system status data into six categories: General, Clock, Communications, Aiding, Solution, and Heave.  
To obtain a comprehensive status overview, the CAN messages `SBG_ECAN_MSG_STATUS_01`, `SBG_ECAN_MSG_STATUS_02`, and `SBG_ECAN_MSG_STATUS_03` should be used in combination.

### SBG_ECAN_MSG_STATUS_01 (0x100) {#SBG_ECAN_MSG_STATUS_01}

The `SBG_ECAN_MSG_STATUS_01` message provides general status information about the device.

- **Message Name (ID):** `SBG_ECAN_MSG_STATUS_01 (0x100)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                                    | Unit | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------------|------|--------|------|--------|
| TIME_STAMP       | Time since the sensor was powered on                                           | µs   | uint32 | 4    | 0      |
| GENERAL_STATUS   | General status (see [STATUS_GENERAL_STATUS](#STATUS_GENERAL_STATUS))           | -    | uint16 | 2    | 4      |
| CLOCK_STATUS     | UTC time and clock sync status (see [TIME_STATUS](#TIME_STATUS))               | -    | uint16 | 2    | 6      |

### SBG_ECAN_MSG_STATUS_02 (0x101) {#SBG_ECAN_MSG_STATUS_02}

The `SBG_ECAN_MSG_STATUS_02` message provides communication and aiding equipment status.

- **Message Name (ID):** `SBG_ECAN_MSG_STATUS_02 (0x101)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                                    | Unit | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------------|------|--------|------|--------|
| COM_STATUS       | Communication status (see [STATUS_COM_STATUS](#STATUS_COM_STATUS))             | -    | uint32 | 4    | 0      |
| AIDING_STATUS    | Status of aiding equipment (see [STATUS_AIDING_STATUS](#STATUS_AIDING_STATUS)) | -    | uint32 | 4    | 4      |

### SBG_ECAN_MSG_STATUS_03 (0x102) {#SBG_ECAN_MSG_STATUS_03}

The `SBG_ECAN_MSG_STATUS_03` message provides the status of the INS solution and ship motion outputs.

- **Message Name (ID):** `SBG_ECAN_MSG_STATUS_03 (0x102)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                                    | Unit | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------------|------|--------|------|--------|
| SOLUTION_STATUS  | INS solution status (see [SOLUTION_STATUS](#SOLUTION_STATUS))                  | -    | uint32 | 4    | 0      |
| HEAVE_STATUS     | Ship motion output status (see [SHIP_MOTION_STATUS](#SHIP_MOTION_STATUS))      | -    | uint16 | 2    | 4      |

> [!NOTE]
> All enumerations and bitmasks are consistent with those used in standard sbgECom output logs.

## UTC Time Output Messages

These messages provide a reference to UTC time, offering a time correlation between the device's `TIME_STAMP` value and the actual UTC time.  
These frames are essential if you need to timestamp all data with an absolute UTC or GPS time reference.

### SBG_ECAN_MSG_UTC_0 (0x110) {#SBG_ECAN_MSG_UTC_0}

The `SBG_ECAN_MSG_UTC_0` message provides the time since the sensor was powered up and the GPS Time of Week (TOW).

- **Message Name (ID):** `SBG_ECAN_MSG_UTC_0 (0x110)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field        | Description                           | Scaling | Unit | Format | Size | Offset |
|--------------|---------------------------------------|---------|------|--------|------|--------|
| TIME_STAMP   | Time since the sensor was powered on  | 1       | µs   | uint32 | 4    | 0      |
| GPS_TOW      | GPS Time of Week                      | 1       | ms   | uint32 | 4    | 4      |

### SBG_ECAN_MSG_UTC_1 (0x111) {#SBG_ECAN_MSG_UTC_1}

The `SBG_ECAN_MSG_UTC_1` message provides the detailed UTC date and time information.

- **Message Name (ID):** `SBG_ECAN_MSG_UTC_1 (0x111)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field       | Description                                                    | Scaling | Unit | Format | Size | Offset |
|-------------|----------------------------------------------------------------|---------|------|--------|------|--------|
| YEAR        | Year within the century (e.g., '10' for 2010)                  | 1       | -    | uint8  | 1    | 0      |
| MONTH       | Month in Year [1 ... 12]                                       | 1       | -    | uint8  | 1    | 1      |
| DAY         | Day in Month [1 ... 31]                                        | 1       | -    | uint8  | 1    | 2      |
| HOUR        | Hour in Day [0 ... 23]                                         | 1       | -    | uint8  | 1    | 3      |
| MIN         | Minute in Hour [0 ... 59]                                      | 1       | -    | uint8  | 1    | 4      |
| SEC         | Second in Minute [0 ... 60] (Note: 60 indicates a leap second) | 1       | s    | uint8  | 1    | 5      |
| MICRO_SEC   | Microseconds within the current second                         | 100     | µs   | uint16 | 2    | 6      |
## Inertial Data Output Messages

These CAN logs output the IMU calibrated accelerometer and gyroscope values expressed in the X,Y,Z body frame.

> [!NOTE]
> It is generally not recommended to use the CAN bus for high-frequency data output, particularly for IMU data.  
> When multiple nodes are present on the bus, there is an increased risk of message delays, duplication, or loss.

### SBG_ECAN_MSG_IMU_INFO (0x120) {#SBG_ECAN_MSG_IMU_INFO}

The `SBG_ECAN_MSG_IMU_INFO` message provides basic IMU information including the timestamp, IMU status, and temperature.

- **Message Name (ID):** `SBG_ECAN_MSG_IMU_INFO (0x120)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field        | Description                           | Scaling | Unit  | Format | Size | Offset |
|--------------|---------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP   | Time since the sensor was powered on  | 1       | µs    | uint32 | 4    | 0      |
| IMU_STATUS   | IMU status bitmask                    | -       | -     | uint16 | 2    | 4      |
| TEMPERATURE  | IMU temperature                       | 10^-2   | °C    | int16  | 2    | 6      |

> [!NOTE]
> Please refer to the sbgECom serial log IMU_STATUS definition for more details on the status bitmask.

### SBG_ECAN_MSG_IMU_ACCEL (0x121) {#SBG_ECAN_MSG_IMU_ACCEL}

The `SBG_ECAN_MSG_IMU_ACCEL` message provides the calibrated accelerometer readings for the X, Y, and Z axes.

- **Message Name (ID):** `SBG_ECAN_MSG_IMU_ACCEL (0x121)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field   | Description       | Scaling | Unit    | Format | Size | Offset |
|---------|-------------------|---------|---------|--------|------|--------|
| ACCEL_X | Acceleration X    | 10^-2   | m/s²    | int16  | 2    | 0      |
| ACCEL_Y | Acceleration Y    | 10^-2   | m/s²    | int16  | 2    | 2      |
| ACCEL_Z | Acceleration Z    | 10^-2   | m/s²    | int16  | 2    | 4      |

### SBG_ECAN_MSG_IMU_GYRO (0x122) {#SBG_ECAN_MSG_IMU_GYRO}

The `SBG_ECAN_MSG_IMU_GYRO` message provides the calibrated gyroscope readings for the X, Y, and Z axes.

- **Message Name (ID):** `SBG_ECAN_MSG_IMU_GYRO (0x122)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field   | Description      | Scaling | Unit      | Format | Size | Offset |
|---------|------------------|---------|-----------|--------|------|--------|
| GYRO_X  | Rate of turn X    | 10^-3   | rad/s     | int16  | 2    | 0      |
| GYRO_Y  | Rate of turn Y    | 10^-3   | rad/s     | int16  | 2    | 2      |
| GYRO_Z  | Rate of turn Z    | 10^-3   | rad/s     | int16  | 2    | 4      |
## EKF Output Messages

These CAN logs output all parameters computed by the device's Extended Kalman Filter (EKF), such as the 3D attitude, velocity, and position.

### SBG_ECAN_MSG_EKF_INFO (0x130) {#SBG_ECAN_MSG_EKF_INFO}

The `SBG_ECAN_MSG_EKF_INFO` message provides basic EKF information, including the timestamp of the EKF solution.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_INFO (0x130)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 4 bytes

| Field       | Description                          | Scaling | Unit  | Format | Size | Offset |
|-------------|--------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP  | Time since the sensor was powered on | 1       | µs    | uint32 | 4    | 0      |

### SBG_ECAN_MSG_EKF_QUAT (0x131) {#SBG_ECAN_MSG_EKF_QUAT}

The `SBG_ECAN_MSG_EKF_QUAT` message provides the orientation of the device using a quaternion representation.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_QUAT (0x131)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field  | Description                          | Scaling | Unit | Format | Size | Offset |
|--------|--------------------------------------|---------|------|--------|------|--------|
| Q0     | Orientation quaternion, q0 component | 1/32767 | -    | int16  | 2    | 0      |
| Q1     | Orientation quaternion, q1 component | 1/32767 | -    | int16  | 2    | 2      |
| Q2     | Orientation quaternion, q2 component | 1/32767 | -    | int16  | 2    | 4      |
| Q3     | Orientation quaternion, q3 component | 1/32767 | -    | int16  | 2    | 6      |

### SBG_ECAN_MSG_EKF_EULER (0x132) {#SBG_ECAN_MSG_EKF_EULER}

The `SBG_ECAN_MSG_EKF_EULER` message provides the orientation of the device using Euler angles representation.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_EULER (0x132)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field | Description                    | Scaling | Unit | Format | Size | Offset |
|-------|--------------------------------|---------|------|--------|------|--------|
| ROLL  | Roll angle [-π to +π]          | 10^-4   | rad  | int16  | 2    | 0      |
| PITCH | Pitch angle [-π/2 to +π/2]     | 10^-4   | rad  | int16  | 2    | 2      |
| YAW   | Yaw angle [-π to +π]           | 10^-4   | rad  | int16  | 2    | 4      |

### SBG_ECAN_MSG_EKF_ORIENTATION_ACC (0x133) {#SBG_ECAN_MSG_EKF_ORIENTATION_ACC}

The `SBG_ECAN_MSG_EKF_ORIENTATION_ACC` message provides the standard deviation accuracy of the orientation measurements.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_ORIENTATION_ACC (0x133)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field      | Description                         | Scaling | Unit | Format | Size | Offset |
|------------|-------------------------------------|---------|------|--------|------|--------|
| ROLL_ACC   | 1σ Roll angle accuracy [0 to +π]    | 10^-4   | rad  | uint16 | 2    | 0      |
| PITCH_ACC  | 1σ Pitch angle accuracy [0 to +π/2] | 10^-4   | rad  | uint16 | 2    | 2      |
| YAW_ACC    | 1σ Yaw angle accuracy [0 to +π]     | 10^-4   | rad  | uint16 | 2    | 4      |

### SBG_ECAN_MSG_EKF_POS (0x134) {#SBG_ECAN_MSG_EKF_POS}

The `SBG_ECAN_MSG_EKF_POS` message provides the INS position data, consisting of latitude and longitude expressed in the WGS84 datum.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_POS (0x134)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field     | Description                    | Scaling | Unit | Format | Size | Offset |
|-----------|--------------------------------|---------|------|--------|------|--------|
| LATITUDE  | Latitude angle, positive north | 10^-7   | °    | int32  | 4    | 0      |
| LONGITUDE | Longitude angle, positive east | 10^-7   | °    | int32  | 4    | 4      |

### SBG_ECAN_MSG_EKF_ALTITUDE (0x135) {#SBG_ECAN_MSG_EKF_ALTITUDE}

The `SBG_ECAN_MSG_EKF_ALTITUDE` message provides the altitude above Mean Sea Level (MSL) and the undulation.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_ALTITUDE (0x135)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field      | Description                                     | Scaling | Unit | Format | Size | Offset |
|------------|-------------------------------------------------|---------|------|--------|------|--------|
| ALTITUDE   | Altitude above Mean Sea Level                   | 10^-3   | m    | int32  | 4    | 0      |
| UNDULATION | Altitude difference between geoid and ellipsoid | 0.005   | m    | int16  | 2    | 4      |

### SBG_ECAN_MSG_EKF_POS_ACC (0x136) {#SBG_ECAN_MSG_EKF_POS_ACC}

The `SBG_ECAN_MSG_EKF_POS_ACC` message provides the standard deviation accuracy of the position data.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_POS_ACC (0x136)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field         | Description                        | Scaling | Unit | Format | Size | Offset |
|---------------|------------------------------------|---------|------|--------|------|--------|
| LATITUDE_ACC  | 1σ Latitude accuracy               | 10^-2   | m    | uint16 | 2    | 0      |
| LONGITUDE_ACC | 1σ Longitude accuracy              | 10^-2   | m    | uint16 | 2    | 2      |
| ALTITUDE_ACC  | 1σ Vertical Position accuracy      | 10^-2   | m    | uint16 | 2    | 4      |

### SBG_ECAN_MSG_EKF_VEL_NED (0x137) {#SBG_ECAN_MSG_EKF_VEL_NED}

The `SBG_ECAN_MSG_EKF_VEL_NED` message provides INS velocity data in the North, East, and Down directions.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_VEL_NED (0x137)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field         | Description                    | Scaling | Unit   | Format | Size | Offset |
|---------------|--------------------------------|---------|--------|--------|------|--------|
| VELOCITY_N    | Velocity in North direction    | 10^-2   | m/s    | int16  | 2    | 0      |
| VELOCITY_E    | Velocity in East direction     | 10^-2   | m/s    | int16  | 2    | 2      |
| VELOCITY_D    | Velocity in Down direction     | 10^-2   | m/s    | int16  | 2    | 4      |

### SBG_ECAN_MSG_EKF_VEL_NED_ACC (0x138) {#SBG_ECAN_MSG_EKF_VEL_NED_ACC}

The `SBG_ECAN_MSG_EKF_VEL_NED_ACC` message provides the 1σ accuracy of velocity data in the North, East, and Down directions.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_VEL_NED_ACC (0x138)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field           | Description                             | Scaling | Unit   | Format | Size | Offset |
|-----------------|-----------------------------------------|---------|--------|--------|------|--------|
| VELOCITY_ACC_N  | 1σ Velocity in North direction accuracy | 10^-2   | m/s    | uint16 | 2    | 0      |
| VELOCITY_ACC_E  | 1σ Velocity in East direction accuracy  | 10^-2   | m/s    | uint16 | 2    | 2      |
| VELOCITY_ACC_D  | 1σ Velocity in Down direction accuracy  | 10^-2   | m/s    | uint16 | 2    | 4      |

### SBG_ECAN_MSG_EKF_VEL_BODY (0x139) {#SBG_ECAN_MSG_EKF_VEL_BODY}

The `SBG_ECAN_MSG_EKF_VEL_BODY` message provides INS velocity data in the body X, Y, and Z frame.

- **Message Name (ID):** `SBG_ECAN_MSG_EKF_VEL_BODY (0x139)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field         | Description                      | Scaling | Unit   | Format | Size | Offset |
|---------------|----------------------------------|---------|--------|--------|------|--------|
| VELOCITY_X    | Velocity in body/INS X direction | 10^-2   | m/s    | int16  | 2    | 0      |
| VELOCITY_Y    | Velocity in body/INS Y direction | 10^-2   | m/s    | int16  | 2    | 2      |
| VELOCITY_Z    | Velocity in body/INS Z direction | 10^-2   | m/s    | int16  | 2    | 4      |
## Automotive Outputs

All messages in this section are specifically designed for automotive applications.

### SBG_ECAN_MSG_AUTO_TRACK_SLIP_CURV (0x220) {#SBG_ECAN_MSG_AUTO_TRACK_SLIP_CURV}

The `SBG_ECAN_MSG_AUTO_TRACK_SLIP_CURV` message provides critical automotive parameters such as track angle, slip angle, and curvature radius.

- **Message Name (ID):** `SBG_ECAN_MSG_AUTO_TRACK_SLIP_CURV (0x220)`
- **Compatibility:** Automotive applications
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 7 bytes

| Field            | Description                                              | Scaling | Unit   | Format | Size | Offset |
|------------------|----------------------------------------------------------|---------|--------|--------|------|--------|
| ANGLE_TRACK      | Track course angle/direction of travel [-π to +π]        | 10^-4   | rad    | int16  | 2    | 0      |
| ANGLE_SLIP       | Vehicle slip angle [-π to +π]                            | 10^-4   | rad    | int16  | 2    | 2      |
| CURVATURE_RADIUS | Curvature radius in meters based on down rotation rate   | 10^-2   | m      | uint16 | 2    | 4      |
| AUTO_STATUS      | Status bitmask as defined in [AUTO_STATUS](#AUTO_STATUS) | -       | -      | uint8  | 1    | 6      |

#### AUTO_STATUS Definition {#AUTO_STATUS}

This field indicates the validity of the automotive data.

| Bit | Name                          | Description                               |
|-----|-------------------------------|-------------------------------------------|
| 0   | SBG_ECOM_AUTO_TRACK_VALID     | Set to 1 if the track angle is valid      |
| 1   | SBG_ECOM_AUTO_SLIP_VALID      | Set to 1 if the slip angle is valid       |
| 2   | SBG_ECOM_AUTO_CURVATURE_VALID | Set to 1 if the curvature radius is valid |
## Ship Motion Data

Ship motion data represents the 3D displacement, velocities, and accelerations of a vessel.  
The device can output heave, surge, and sway measurements, along with the main heave period.

Some devices support both real-time and delayed heave values, which share the same message format.

### SBG_ECAN_MSG_SHIP_MOTION_INFO (0x140) {#SBG_ECAN_MSG_SHIP_MOTION_INFO}

The `SBG_ECAN_MSG_SHIP_MOTION_INFO` message provide information about the ship's motion, including the main heave period and the ship's motion status.

- **Message Name (ID):** `SBG_ECAN_MSG_SHIP_MOTION_INFO (0x140)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field         | Description                                                        | Scaling | Unit  | Format | Size | Offset |
|---------------|--------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP    | Time since sensor is powered up                                    | 1       | µs    | uint32 | 4    | 0      |
| PERIOD        | Main heave period                                                  | 10^-2   | s     | uint16 | 2    | 4      |
| HEAVE_STATUS  | Ship motion status (see [SHIP_MOTION_STATUS](#SHIP_MOTION_STATUS)) | -       | -     | uint16 | 2    | 6      |

### SBG_ECAN_MSG_SHIP_MOTION_0 (0x141) {#SBG_ECAN_MSG_SHIP_MOTION_0}

The `SBG_ECAN_MSG_SHIP_MOTION_0` message reports surge, sway, and heave motion measurements.

- **Message Name (ID):** `SBG_ECAN_MSG_SHIP_MOTION_0 (0x141)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field         | Description                                                        | Scaling | Unit  | Format | Size | Offset |
|---------------|--------------------------------------------------------------------|---------|-------|--------|------|--------|
| SURGE         | Surge motion (positive forward)                                    | 10^-3   | m     | int16  | 2    | 0      |
| SWAY          | Sway motion (positive right)                                       | 10^-3   | m     | int16  | 2    | 2      |
| HEAVE         | Heave motion (positive down)                                       | 10^-3   | m     | int16  | 2    | 4      |

### SBG_ECAN_MSG_SHIP_MOTION_1 (0x145) {#SBG_ECAN_MSG_SHIP_MOTION_1}

The `SBG_ECAN_MSG_SHIP_MOTION_1` message reports longitudinal, lateral, and vertical accelerations.

- **Message Name (ID):** `SBG_ECAN_MSG_SHIP_MOTION_1 (0x145)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field         | Description                                                        | Scaling | Unit  | Format | Size | Offset |
|---------------|--------------------------------------------------------------------|---------|-------|--------|------|--------|
| ACCEL_X       | Longitudinal acceleration (forward)                                | 10^-2   | m/s²  | int16  | 2    | 0      |
| ACCEL_Y       | Lateral acceleration (right)                                       | 10^-2   | m/s²  | int16  | 2    | 2      |
| ACCEL_Z       | Vertical acceleration (down)                                       | 10^-2   | m/s²  | int16  | 2    | 4      |

### SBG_ECAN_MSG_SHIP_MOTION_2 (0x149) {#SBG_ECAN_MSG_SHIP_MOTION_2}

The `SBG_ECAN_MSG_SHIP_MOTION_2` message reports the velocities in the longitudinal, lateral, and vertical directions.

- **Message Name (ID):** `SBG_ECAN_MSG_SHIP_MOTION_2 (0x149)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field         | Description                                                        | Scaling | Unit  | Format | Size | Offset |
|---------------|--------------------------------------------------------------------|---------|-------|--------|------|--------|
| VEL_X         | Longitudinal velocity (positive forward)                           | 10^-2   | m/s   | int16  | 2    | 0      |
| VEL_Y         | Lateral velocity (positive right)                                  | 10^-2   | m/s   | int16  | 2    | 2      |
| VEL_Z         | Vertical velocity (positive down)                                  | 10^-2   | m/s   | int16  | 2    | 4      |

### SBG_ECAN_MSG_SHIP_MOTION_HP_INFO (0x14A) {#SBG_ECAN_MSG_SHIP_MOTION_HP_INFO}

The SBG_ECAN_MSG_SHIP_MOTION_HP_INFO` message provide information about the delayed heave timestamp, period and status.

The timestamp reported in the message reflects the actual time of data validity.  
The data corresponds to conditions roughly 150 seconds earlier than other logs transmitted at the same time.

- **Message Name (ID):** `SBG_ECAN_MSG_SHIP_MOTION_HP_INFO (0x14A)`
- **Compatibility:** HPINS with hydro option
- **Firmware:** ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field         | Description                                                        | Scaling | Unit  | Format | Size | Offset |
|---------------|--------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP    | Time since sensor is powered up                                    | 1       | µs    | uint32 | 4    | 0      |
| PERIOD        | Main heave period                                                  | 10^-2   | s     | uint16 | 2    | 4      |
| HEAVE_STATUS  | Ship motion status (see [SHIP_MOTION_STATUS](#SHIP_MOTION_STATUS)) | -       | -     | uint16 | 2    | 6      |

### SBG_ECAN_MSG_SHIP_MOTION_HP_0 (0x14B) {#SBG_ECAN_MSG_SHIP_MOTION_HP_0}

The `SBG_ECAN_MSG_SHIP_MOTION_HP_0` message reports delayed heave measurement.  
Surge and sway quantities are not computed by the delayed heave algorithm.

- **Message Name (ID):** `SBG_ECAN_MSG_SHIP_MOTION_HP_0 (0x14B)`
- **Compatibility:** HPINS with hydro option
- **Firmware:** ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field         | Description                                                        | Scaling | Unit  | Format | Size | Offset |
|---------------|--------------------------------------------------------------------|---------|-------|--------|------|--------|
| SURGE         | Not available for delayed heave.                                   | 10^-3   | m     | int16  | 2    | 0      |
| SWAY          | Not available for delayed heave.                                   | 10^-3   | m     | int16  | 2    | 2      |
| HEAVE         | Delayed heave motion (positive down)                               | 10^-3   | m     | int16  | 2    | 4      |

### SBG_ECAN_MSG_SHIP_MOTION_HP_1 (0x14C) {#SBG_ECAN_MSG_SHIP_MOTION_HP_1}

The `SBG_ECAN_MSG_SHIP_MOTION_HP_1` message reports longitudinal, lateral, and vertical accelerations.

- **Message Name (ID):** `SBG_ECAN_MSG_SHIP_MOTION_HP_1 (0x14C)`
- **Compatibility:** HPINS with hydro option
- **Firmware:** ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field         | Description                                                        | Scaling | Unit  | Format | Size | Offset |
|---------------|--------------------------------------------------------------------|---------|-------|--------|------|--------|
| ACCEL_X       | Longitudinal acceleration (forward)                                | 10^-2   | m/s²  | int16  | 2    | 0      |
| ACCEL_Y       | Lateral acceleration (right)                                       | 10^-2   | m/s²  | int16  | 2    | 2      |
| ACCEL_Z       | Vertical acceleration (down)                                       | 10^-2   | m/s²  | int16  | 2    | 4      |

### SBG_ECAN_MSG_SHIP_MOTION_HP_2 (0x14D) {#SBG_ECAN_MSG_SHIP_MOTION_HP_2}

The `SBG_ECAN_MSG_SHIP_MOTION_HP_2` message reports the delayed heave velocity (vertical).  
Surge and sway velocities are not computed by the delayed heave algorithm.

- **Message Name (ID):** `SBG_ECAN_MSG_SHIP_MOTION_HP_2 (0x14D)`
- **Compatibility:** Heave capable products
- **Firmware:** ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field         | Description                                                        | Scaling | Unit  | Format | Size | Offset |
|---------------|--------------------------------------------------------------------|---------|-------|--------|------|--------|
| VEL_X         | Longitudinal velocity (positive forward)                           | 10^-2   | m/s   | int16  | 2    | 0      |
| VEL_Y         | Lateral velocity (positive right)                                  | 10^-2   | m/s   | int16  | 2    | 2      |
| VEL_Z         | Vertical velocity (positive down)                                  | 10^-2   | m/s   | int16  | 2    | 4      |
## Magnetometer Output

These CAN logs provide fully calibrated and normalized magnetometer values in arbitrary units (a.u.), along with associated accelerometer measurements.   
If properly calibrated, the norm of the magnetic vector should equal 1.

### SBG_ECAN_MSG_MAG_0 (0x150) {#SBG_ECAN_MSG_MAG_0}

The `SBG_ECAN_MSG_MAG_0` message provides the timestamp and status of the magnetometer.

- **Message Name (ID):** `SBG_ECAN_MSG_MAG_0 (0x150)`
- **Compatibility:** All products with magnetometers
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP       | Time since sensor is powered up                                          | 1       | µs    | uint32 | 4    | 0      |
| MAG_STATUS       | Magnetometer status bit-mask (see [MAG_STATUS](#MAG_STATUS))             | -       | -     | uint16 | 2    | 4      |

### SBG_ECAN_MSG_MAG_1 (0x151) {#SBG_ECAN_MSG_MAG_1}

The `SBG_ECAN_MSG_MAG_1` message reports the magnetometer outputs on the X, Y, and Z axes.

- **Message Name (ID):** `SBG_ECAN_MSG_MAG_1 (0x151)`
- **Compatibility:** All products with magnetometers
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| MAG_X            | Magnetometer output, X axis                                              | 10^-3   | a.u.  | int16  | 2    | 0      |
| MAG_Y            | Magnetometer output, Y axis                                              | 10^-3   | a.u.  | int16  | 2    | 2      |
| MAG_Z            | Magnetometer output, Z axis                                              | 10^-3   | a.u.  | int16  | 2    | 4      |

### SBG_ECAN_MSG_MAG_2 (0x152) {#SBG_ECAN_MSG_MAG_2}

The `SBG_ECAN_MSG_MAG_2` message provides the accelerometer measurements aligned with the magnetometer frame.

- **Message Name (ID):** `SBG_ECAN_MSG_MAG_2 (0x152)`
- **Compatibility:** All products with magnetometers
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| ACC_X            | Accelerometer output, X axis                                             | 10^-2   | m/s²  | int16  | 2    | 0      |
| ACC_Y            | Accelerometer output, Y axis                                             | 10^-2   | m/s²  | int16  | 2    | 2      |
| ACC_Z            | Accelerometer output, Z axis                                             | 10^-2   | m/s²  | int16  | 2    | 4      |

## Odometer Output

These CAN logs provide information on odometer status, time since reset, and raw velocity using detected odometer pulses.  
The `TIME_STAMP` field in these messages is not necessarily aligned with the main loop as it dates the actual odometer data.

### SBG_ECAN_MSG_ODO_INFO (0x160) {#SBG_ECAN_MSG_ODO_INFO}

The `SBG_ECAN_MSG_ODO_INFO` message provides odometer status and timestamp information.

- **Message Name (ID):** `SBG_ECAN_MSG_ODO_INFO (0x160)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP       | Time since sensor is powered up                                          | 1       | µs    | uint32 | 4    | 0      |
| ODO_STATUS       | Odometer velocity status bit-mask (see [ODO_STATUS](#ODO_STATUS))        | -       | -     | uint16 | 2    | 4      |

### SBG_ECAN_MSG_ODO_VEL (0x161) {#SBG_ECAN_MSG_ODO_VEL}

The `SBG_ECAN_MSG_ODO_VEL` message reports the velocity in the odometer direction.

- **Message Name (ID):** `SBG_ECAN_MSG_ODO_VEL (0x161)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 2 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| VELOCITY         | Velocity in odometer direction                                           | 10^-2   | m/s   | int16  | 2    | 0      |

## AirData Output

These CAN logs provide information related to the AirData module, including barometric pressure, altitude, and true airspeed measurements.

### SBG_ECAN_MSG_AIR_DATA_INFO (0x162) {#SBG_ECAN_MSG_AIR_DATA_INFO}

The `SBG_ECAN_MSG_AIR_DATA_INFO` message provides general status information for the AirData module, including the outside air temperature.

- **Message Name (ID):** `SBG_ECAN_MSG_AIR_DATA_INFO (0x162)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 7 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP       | Time since sensor is powered up                                          | 1       | µs    | uint32 | 4    | 0      |
| AIR_DATA_STATUS  | AirData module and sensor status (see [AIRDATA_STATUS](#AIRDATA_STATUS)) | -       | -     | uint8  | 1    | 4      |
| AIR_TEMPERATURE  | Outside air temperature                                                  | 10^-2   | °C    | int16  | 2    | 5      |

### SBG_ECAN_MSG_AIR_DATA_ALTITUDE (0x163) {#SBG_ECAN_MSG_AIR_DATA_ALTITUDE}

The `SBG_ECAN_MSG_AIR_DATA_ALTITUDE` message provides barometric pressure and altitude information.

- **Message Name (ID):** `SBG_ECAN_MSG_AIR_DATA_ALTITUDE (0x163)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| PRESSURE_ABS     | Absolute barometric pressure                                             | 10^-3   | Pa    | uint32 | 4    | 0      |
| ALTITUDE         | Barometric altitude (positive upward)                                    | 10^-3   | m     | int32  | 4    | 4      |

### SBG_ECAN_MSG_AIR_DATA_AIRSPEED (0x164) {#SBG_ECAN_MSG_AIR_DATA_AIRSPEED}

The `SBG_ECAN_MSG_AIR_DATA_AIRSPEED` message provides information about the differential pressure measured by a pitot tube and the true airspeed.

- **Message Name (ID):** `SBG_ECAN_MSG_AIR_DATA_AIRSPEED (0x164)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| PRESSURE_DIFF    | Differential pressure measured by a pitot tube                           | 10^-3   | Pa    | int32  | 4    | 0      |
| AIRSPEED         | True airspeed (positive forward)                                         | 10^-2   | m/s   | int16  | 2    | 4      |

Here is the conversion of the Depth Sensor CAN messages using the standard template:

## Depth Sensor Output

These CAN logs provide information related to the subsea depth sensor module, including absolute water pressure and depth measurements.

### SBG_ECAN_MSG_DEPTH_INFO (0x166) {#SBG_ECAN_MSG_DEPTH_INFO}

The `SBG_ECAN_MSG_DEPTH_INFO` message provides general status information for the depth sensor module.

- **Message Name (ID):** `SBG_ECAN_MSG_DEPTH_INFO (0x166)`
- **Compatibility:** INS capable products
- **Firmware:** Not Supported - Please contact SBG Systems
- **Payload Size:** 5 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP       | Time since sensor is powered up                                          | 1       | µs    | uint32 | 4    | 0      |
| DEPTH_STATUS     | Depth module and sensor status (see [DEPTH_STATUS](#DEPTH_STATUS))       | -       | -     | uint8  | 1    | 4      |

### SBG_ECAN_MSG_DEPTH_ALTITUDE (0x167) {#SBG_ECAN_MSG_DEPTH_ALTITUDE}

The `SBG_ECAN_MSG_DEPTH_ALTITUDE` message provides information about the absolute water pressure and the altitude measured by the depth sensor.

- **Message Name (ID):** `SBG_ECAN_MSG_DEPTH_ALTITUDE (0x167)`
- **Compatibility:** INS capable products
- **Firmware:** Not Supported - Please contact SBG Systems
- **Payload Size:** 8 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| PRESSURE_ABS     | Absolute water pressure                                                  | 10^-1   | Pa    | uint32 | 4    | 0      |
| DEPTH            | Altitude from depth sensor (positive upward)                             | 10^-3   | m     | int32  | 4    | 4      |

## GNSS Velocity Outputs

These logs provide the GNSS velocity, velocity accuracy, course, and status as computed by the primary or secondary GNSS receiver.  
The `TIME_STAMP` in these messages indicates the exact time of the GNSS velocity data and may not align with the main loop.

### SBG_ECAN_MSG_GPSX_VEL_INFO (0x170 - 0x180) {#SBG_ECAN_MSG_GPSX_VEL_INFO}

The `SBG_ECAN_MSG_GPSX_VEL_INFO` message provides information about the GNSS velocity status and timestamp for either the primary (GPS1) or secondary (GPS2) GNSS receiver.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_VEL_INFO (0x170)`, `SBG_ECAN_MSG_GPS2_VEL_INFO (0x180)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP       | Time since sensor is powered up                                                        | 1       | µs    | uint32 | 4    | 0      |
| GPS_VEL_STATUS   | GPS velocity fix and status bit-mask (see [GPS_VEL_STATUS_TYPE](#GPS_VEL_STATUS_TYPE)) | -       | -     | uint32 | 4    | 4      |

### SBG_ECAN_MSG_GPSX_VEL (0x171 - 0x181) {#SBG_ECAN_MSG_GPSX_VEL}

The `SBG_ECAN_MSG_GPSX_VEL` message provides the GNSS velocity in the North, East, and Down directions.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_VEL (0x171)`, `SBG_ECAN_MSG_GPS2_VEL (0x181)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| VEL_N            | Velocity in North direction                                                            | 10^-2   | m/s   | int16  | 2    | 0      |
| VEL_E            | Velocity in East direction                                                             | 10^-2   | m/s   | int16  | 2    | 2      |
| VEL_D            | Velocity in Down direction                                                             | 10^-2   | m/s   | int16  | 2    | 4      |

### SBG_ECAN_MSG_GPSX_VEL_ACC (0x172 - 0x182) {#SBG_ECAN_MSG_GPSX_VEL_ACC}

The `SBG_ECAN_MSG_GPSX_VEL_ACC` message provides the 1σ accuracy of the GNSS velocity in the North, East, and Down directions.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_VEL_ACC (0x172)`, `SBG_ECAN_MSG_GPS2_VEL_ACC (0x182)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| VEL_ACC_N        | 1σ Accuracy in North direction                                                         | 10^-2   | m/s   | uint16 | 2    | 0      |
| VEL_ACC_E        | 1σ Accuracy in East direction                                                          | 10^-2   | m/s   | uint16 | 2    | 2      |
| VEL_ACC_D        | 1σ Accuracy in Down direction                                                          | 10^-2   | m/s   | uint16 | 2    | 4      |

### SBG_ECAN_MSG_GPSX_COURSE (0x173 - 0x183) {#SBG_ECAN_MSG_GPSX_COURSE}

The `SBG_ECAN_MSG_GPSX_COURSE` message provides the true direction of motion over the ground and its 1σ accuracy.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_COURSE (0x173)`, `SBG_ECAN_MSG_GPS2_COURSE (0x183)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 4 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| COURSE           | True direction of motion over ground (0 to 360°)                                       | 10^-2   | °     | uint16 | 2    | 0      |
| COURSE_ACC       | 1σ Course accuracy                                                                     | 10^-2   | °     | uint16 | 2    | 2      |

## GNSS Position Outputs

These logs provide the GNSS position, position accuracy, and status as computed by the primary or secondary GNSS receiver.  
The `TIME_STAMP` in these messages indicates the exact time of the GNSS positioning data and may not align with the main loop.

### SBG_ECAN_MSG_GPSX_POS_INFO (0x174 - 0x184) {#SBG_ECAN_MSG_GPSX_POS_INFO}

The `SBG_ECAN_MSG_GPSX_POS_INFO` message provides information about the GNSS position status and timestamp for either the primary (GPS1) or secondary (GPS2) GNSS receiver.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_POS_INFO (0x174)`, `SBG_ECAN_MSG_GPS2_POS_INFO (0x184)`
- **Compatibility:** All products with GNSS capability
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP       | Time since sensor is powered up                                                        | 1       | µs    | uint32 | 4    | 0      |
| GPS_POS_STATUS   | GPS position fix and status bit-mask (see [GPS_POS_STATUS_TYPE](#GPS_POS_STATUS_TYPE)) | -       | -     | uint32 | 4    | 4      |

### SBG_ECAN_MSG_GPSX_POS (0x175 - 0x185) {#SBG_ECAN_MSG_GPSX_POS}

The `SBG_ECAN_MSG_GPSX_POS` message provides the GNSS latitude and longitude.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_POS (0x175)`, `SBG_ECAN_MSG_GPS2_POS (0x185)`
- **Compatibility:** All products with GNSS capability
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| LATITUDE         | Latitude, positive North                                                               | 10^-7   | °     | int32  | 4    | 0      |
| LONGITUDE        | Longitude, positive East                                                               | 10^-7   | °     | int32  | 4    | 4      |

### SBG_ECAN_MSG_GPSX_POS_ALT (0x176 - 0x186) {#SBG_ECAN_MSG_GPSX_POS_ALT}

The `SBG_ECAN_MSG_GPSX_POS_ALT` message provides the GNSS altitude, undulation, number of satellites in view, and the age of differential corrections.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_POS_ALT (0x176)`, `SBG_ECAN_MSG_GPS2_POS_ALT (0x186)`
- **Compatibility:** All products with GNSS capability
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| ALTITUDE         | Altitude Above Mean Sea Level                                                          | 10^-3   | m     | int32  | 4    | 0      |
| UNDULATION       | Altitude difference between the geoid and the ellipsoid                                | 0.005   | m     | int16  | 2    | 4      |
| NUM_SV           | Number of space vehicles used in solution                                              | -       | -     | uint8  | 1    | 6      |
| DIFF_CORR_AGE    | Age of differential corrections (set to 0xFF if not available)                         | 1       | s     | uint8  | 1    | 7      |

### SBG_ECAN_MSG_GPSX_POS_ACC (0x177 - 0x187) {#SBG_ECAN_MSG_GPSX_POS_ACC}

The `SBG_ECAN_MSG_GPSX_POS_ACC` message provides the 1σ accuracy of the GNSS latitude, longitude, and altitude.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_POS_ACC (0x177)`, `SBG_ECAN_MSG_GPS2_POS_ACC (0x187)`
- **Compatibility:** All products with GNSS capability
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| LAT_ACC         | 1σ Latitude Accuracy                                                                    | 10^-2   | m     | uint16 | 2    | 0      |
| LONG_ACC        | 1σ Longitude Accuracy                                                                   | 10^-2   | m     | uint16 | 2    | 2      |
| ALT_ACC         | 1σ Altitude Accuracy                                                                    | 10^-2   | m     | uint16 | 2    | 4      |
| BASE_STATION_ID | ID of the base station used (set to 0xFFFF if not available)                             | -      | -     | uint16 | 2    | 6      |

### GNSS True Heading Outputs

These logs provide the GNSS true heading and pitch solutions along with their respective accuracy and status as computed by the dual antenna GNSS receiver.  
The `TIME_STAMP` in these messages indicates the exact time of the GNSS true heading data and may not align with the main loop.

### SBG_ECAN_MSG_GPSX_HDT_INFO (0x178 - 0x188) {#SBG_ECAN_MSG_GPSX_HDT_INFO}

The `SBG_ECAN_MSG_GPSX_HDT_INFO` message provides information about the GNSS true heading status and timestamp for either the primary (GPS1) or secondary (GPS2) GNSS receiver.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_HDT_INFO (0x178)`, `SBG_ECAN_MSG_GPS2_HDT_INFO (0x188)`
- **Compatibility:** All products with dual antenna GNSS capability
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP       | Time since sensor is powered up                                                        | 1       | µs    | uint32 | 4    | 0      |
| GPS_HDT_STATUS   | GPS True Heading status bit-mask (see [GPS_HDT_STATUS_BIT](#GPS_HDT_STATUS_BIT))       | -       | -     | uint16 | 2    | 4      |

### SBG_ECAN_MSG_GPSX_HDT (0x179 - 0x189) {#SBG_ECAN_MSG_GPSX_HDT}

The `SBG_ECAN_MSG_GPSX_HDT` message provides the GNSS true heading, pitch angle, and their respective accuracies.

- **Message Name (ID):** `SBG_ECAN_MSG_GPS1_HDT (0x179)`, `SBG_ECAN_MSG_GPS2_HDT (0x189)`
- **Compatibility:** All products with dual antenna GNSS capability
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                                            | Scaling | Unit  | Format | Size | Offset |
|------------------|----------------------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TRUE_HEADING     | True heading angle (0 to 360°)                                                         | 10^-2   | °     | uint16 | 2    | 0      |
| TRUE_HEADING_ACC | 1σ True heading estimated accuracy (0 to 360°)                                         | 10^-2   | °     | uint16 | 2    | 2      |
| PITCH            | Pitch angle from the master to the rover                                               | 10^-2   | °     | int16  | 2    | 4      |
| PITCH_ACC        | 1σ Pitch estimated accuracy                                                            | 10^-2   | °     | uint16 | 2    | 6      |
## Event Markers

SBG Systems Inertial Systems can detect event markers at up to 1 kHz on synchronization input signals, such as Sync A, Sync B, Sync C, Sync D, and Sync E.  
For each input synchronization signal, the device outputs a binary log that captures the timestamp of each received event within the last 5 milliseconds.  
The maximum output rate for these logs is 200 Hz.

The `TIME_STAMP` field records the time of the first event detected within the 5 ms window.  
Subsequent events within this period are captured using `TIME_OFFSET` fields, which store the time difference between the `TIME_STAMP` and each subsequent event.  
This approach optimizes log size by reducing redundant timestamp data.

**Example:**

If three events are received within the last 5 ms:
- The first event's time is recorded directly in `TIME_STAMP`.
- The second event's time is recorded as `TIME_STAMP + TIME_OFFSET_0`.
- The third event's time is recorded as `TIME_STAMP + TIME_OFFSET_1`.

If there are fewer events, the remaining `TIME_OFFSET` fields are set to 0. The `EVENT_STATUS` flag indicates which `TIME_OFFSET` fields are valid.

### SBG_ECAN_MSG_EVENT_INFO_X (0x200 - 0x207) {#SBG_ECAN_MSG_EVENT_INFO_X}

This message provides the timestamp and status of event markers.

- **Message Name (ID):** `SBG_ECAN_MSG_EVENT_INFO_A (0x200)`, `SBG_ECAN_MSG_EVENT_INFO_B (0x202)`, `SBG_ECAN_MSG_EVENT_INFO_C (0x204)`, `SBG_ECAN_MSG_EVENT_INFO_D (0x206)`, `SBG_ECAN_MSG_EVENT_INFO_E (0x207)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 6 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_STAMP       | Time since the sensor is powered up                                      | 1       | µs    | uint32 | 4    | 0      |
| EVENT_STATUS     | Status bitmask (see [EVENT_STATUS](#EVENT_STATUS))                       | -       | -     | uint16 | 2    | 4      |

### SBG_ECAN_MSG_EVENT_TIME_X (0x201 - 0x209) {#SBG_ECAN_MSG_EVENT_TIME_X}

This message logs the time offsets for events received after the initial event within a 5 ms window.

- **Message Name (ID):** `SBG_ECAN_MSG_EVENT_TIME_A (0x201)`, `SBG_ECAN_MSG_EVENT_TIME_B (0x203)`, `SBG_ECAN_MSG_EVENT_TIME_C (0x205)`, `SBG_ECAN_MSG_EVENT_TIME_D (0x207)`, `SBG_ECAN_MSG_EVENT_TIME_E (0x209)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue)
- **Payload Size:** 8 bytes

| Field            | Description                                                              | Scaling | Unit  | Format | Size | Offset |
|------------------|--------------------------------------------------------------------------|---------|-------|--------|------|--------|
| TIME_OFFSET_0    | Time offset for the second received event                                | 1       | µs    | uint16 | 2    | 0      |
| TIME_OFFSET_1    | Time offset for the third received event                                 | 1       | µs    | uint16 | 2    | 2      |
| TIME_OFFSET_2    | Time offset for the fourth received event                                | 1       | µs    | uint16 | 2    | 4      |
| TIME_OFFSET_3    | Time offset for the fifth received event                                 | 1       | µs    | uint16 | 2    | 6      |

> [!NOTE]
> The system can detect event markers at rates up to 1 kHz. However, excessive event rates may overload the internal CPU, leading to decreased performance and reliability.

> [!WARNING]
> Never leave an activated Sync In signal unconnected, as noise on the line may trigger spurious events at very high rates, potentially causing issues with system performance.
