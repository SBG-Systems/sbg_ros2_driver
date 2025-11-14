# Migrations
These pages help you easily migrate your code from previous sbgECom major versions.

## ELLIPSE Firmware v3

ELLIPSE firmware v3 is a complete rewrite from scratch over firmware v2.  
This new firmware brings massive performance and reliability improvements for VG, AHRS and INS operating modes as well as magnetic heading and heave accuracy.

This new firmware also improve the user experience with the new [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) configuration API already used by High Performance INS products.

Finally, the new firmware v3 is only available for ELLIPSE hardware generation 3 thanks to a more powerful CPU and more available RAM.
ELLIPSE v1 and v2 are not concerned by this new firmware with only maintenance support on firmware branch v2.x.

### New Features

The main new features or changes introduced by the firmware v3 are:
  - Latest generation INS filter algorithms
  - Latest generation heave filters
  - New magnetic heading aiding implementation
  - New on-board magnetic field hard/soft iron compensation routines
  - New [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) configuration API
  - New settings streaming over serial interface
  - Up to 1 kHz output rate for IMU data
  - Up to 2 Mbit/s serial interface baudrates
  - New configuration, operating modes and more

### Compatibility

The firmware v3 introduces the new [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) as the preferred method to configure the device.
However, the legacy sbgECom configuration commands are still supported to ease firmware migration.

For most users, existing sbgECom implementations should work with new ELLIPSE firmware v3 without requiring code modifications.
However minor compatibility issues might arose between the ELLIPSE firmware v3 and previous sbgECom versions (before v5).

When possible, ELLIPSE firmware v3 new features and options have been added to the legacy sbgECom configuration commands.  
It should help users benefits from the new features and improvements brought by the ELLIPSE firmware v3 with limited code updates.

> [!NOTE]
> Even if sbgECom legacy configuration commands have been extended to support some ELLIPSE v3 new features and options, SBG Systems recommends migrating to the new [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) configuration API.

### Serial Interface

The ELLIPSE firmware v3 allow up to 1 kHz IMU output on serial interface as well as settings streaming.
To better support high throughput, the maximum allowable baudrate has been increased from 921600 bps to 2 Mbit/s.

> [!WARNING]
> Make sure your hardware and cables are able to cope with such high baudrates.

### Precise Mechanical Installation

Starting with ELLIPSE firmware v3, all lever arms and alignment configurations are treated as precisely measured and entered.  
In previous firmware versions (v2), the system refined these parameters online, which came at the cost of reliability and accuracy.

This online estimation feature has been disabled in firmware v3 to ensure more repeatable and robust performance.  
The following parameters are affected:
- GNSS primary lever arm
- GNSS secondary lever arm (HDT)
- DVL lever arm

However, accurately measuring lever arms and alignment parameters during initial product setup can be challenging.  
To simplify this process, SBG Systems has developed a free software, the **Qinertia Lever Arm Estimation** tool.

The Qinertia Lever Arm Estimation tool uses forward/backward post-processing to deliver highly accurate and repeatable estimations.  
For detailed instructions, please refer to the specific documentation available on the [Support Center](https://support.sbg-systems.com).

### Magnetometers

#### External Magnetometer

ELLIPSE firmware v3 adds a new option to accept external magnetometer modules.  
The command `SBG_ECOM_CMD_AIDING_ASSIGNMENT` has been updated accordingly and `SBG_ECOM_MAG_MODEL_ECOM_NORMAL` model has been added.

#### Error Model

The integration of magnetometers within the AHRS/INS solution and their rejection handling has been completely reworked.
As a result, the magnetic error mode `SBG_ECOM_MAG_MODEL_NOISY_MAG_TOLERANT` is no longer relevant.

The updated magnetometer interference rejection and mitigation algorithm provides improved accuracy, even in magnetically disturbed environments, without sacrificing heading robustness.

#### 2D/3D Magnetic Calibration

The new magnetometer implementation requires the AHRS/INS filter to know if the magnetic field has been calibrated using a 2D or 3D procedure.  
This new settings is available through the sbgInsRestApi at `api/v1/settings/aiding/magnetometers/calibrationMode` endpoint.

Legacy sbgECom configuration command `SBG_ECOM_CMD_SET_MAG_CALIB` has been extended to include the 2D/3D calibration mode information.  
The new method `sbgEComCmdMagSetCalibData2` has been introduced to provide the mode information.

## From sbgECom v4.x

sbgECom v5.x brings several modifications and improvements to support ELLIPSE Firmware v3.
This new version also includes several new outputs, fields as well as deprecates some enums or definitions.

The sbgECom library and protocol documentation has been also completely rewritten.
This guide should help you update your code base to latest sbgECom 5.x for the new ELLIPSE firmware v3.

> [!WARNING]
> The sbgECom v4.x supports latest ELLIPSE running firmware v3, HPINS and PULSE products.  
> For ELLIPSE running firmware v2 or earlier, it is recommended to use sbgECom v3.x.

### Documentation

This version introduce a new unified sbgECom protocol and library documentation.
This documentation supersede the previous [Firmware Reference Manual](https://support.sbg-systems.com/sc/dev/latest/firmware-documentation) PDF.

The new documentation focuses only on latest products and firmware and doesn't cover ELLIPSE v1/v2 anymore.  
You should use the previous Firmware Reference Manual for these products.

### New DVL model

ELLIPSE firmware v3 now supports a new DVL mode: `SBG_ECOM_DVL_MODEL_NORTEK`

### Output Event Triggers

ELLIPSE firmware v3 introduces new Sync Out trigger options to support up to 1 kHz output rates.  
To ease migration to firmware v3, the following new definitions have been added to legacy sbgECom configuration commands:
  - `SBG_ECOM_SYNC_OUT_MODE_DIV_5`: Trigger a Sync Out pulse at 40 Hz
  - `SBG_ECOM_SYNC_OUT_MODE_DIV_100`: Trigger a Sync Out pulse at 2 Hz
  - `SBG_ECOM_SYNC_OUT_MODE_1_MS`: Trigger a Sync Out pulse at 1000 Hz
  - `SBG_ECOM_SYNC_OUT_MODE_2_MS`: Trigger a Sync Out pulse at 500 Hz
  - `SBG_ECOM_SYNC_OUT_MODE_4_MS`: Trigger a Sync Out pulse at 250 Hz

> [!WARNING]
> These new trigger enum values are only supported starting with ELLIPSE firmware v3.

### Output Message Triggers

ELLIPSE firmware v3 introduces new Output Message trigger options to support up to 1 kHz output rates.  
To ease migration to firmware v3, the following new definitions have been added to legacy sbgECom configuration commands:
  - `SBG_ECOM_OUTPUT_MODE_DIV_100`: Output a message at 2 Hz
  - `SBG_ECOM_SYNC_OUT_MODE_1_MS`: Output a message at 1000 Hz
  - `SBG_ECOM_SYNC_OUT_MODE_2_MS`: Output a message at 500 Hz
  - `SBG_ECOM_SYNC_OUT_MODE_4_MS`: Output a message at 250 Hz

> [!WARNING]
> These new trigger enum values are only supported starting with ELLIPSE firmware v3.

### NMEA Output From GNSS

A new output message class `SBG_ECOM_CLASS_LOG_NMEA_GNSS` has been added to support NMEA messages from the internal GNSS receiver instead of from the INS filter.  
The following new messages are available in this class:
  - `SBG_ECOM_LOG_NMEA_GNSS_GGA`
  - `SBG_ECOM_LOG_NMEA_GNSS_RMC`
  - `SBG_ECOM_LOG_NMEA_GNSS_HDT`
  - `SBG_ECOM_LOG_NMEA_GNSS_VTG`
  - `SBG_ECOM_LOG_NMEA_GNSS_ZDA`
  - `SBG_ECOM_LOG_NMEA_GNSS_GSA`
  - `SBG_ECOM_LOG_NMEA_GNSS_GST`

These definitions have been added to support this new feature using the legacy sbgECom configuration commands.  
Of course, SBG Systems recommends using the sbgInsRestApi for new code implementations.

### Magnetometers Aiding

To support external magnetometers aiding, a reserved field has been used in the `SBG_ECOM_CMD_AIDING_ASSIGNMENT` command payload to define the new magnetometer module assignment.  
However, in sbgECom before 4.0 and before, this field was set to zero resulting in a Port A assignment instead of internal.

> [!WARNING]
> The command `SBG_ECOM_CMD_AIDING_ASSIGNMENT` might create compatibility issues when used with sbgECom 4.x or before.

### Magnetometer Models

The `SBG_ECOM_MAG_MODEL_NOISY_MAG_TOLERANT` error model is not relevant anymore and has been removed.  

The `SBG_ECOM_MAG_MODEL_NORMAL` model has been replaced by `SBG_ECOM_MAG_MODEL_INTERNAL_NORMAL`.
The `SBG_ECOM_MAG_MODEL_ECOM_NORMAL` has been added to support external magnetometers using the sbgECom protocol ([SBG_ECOM_LOG_MAG](#SBG_ECOM_LOG_MAG) message).

### Magnetometer Calibration

The hard and soft iron calibration module has been improved as well as how the magnetic field is fused in the AHRS/INS filter.  

The command `SBG_ECOM_CMD_SET_MAG_CALIB` has a new field to indicate if the magnetic calibration is a 2D or 3D one.  
This new field is available with the new `sbgEComCmdMagSetCalibData2` method that supersede `sbgEComCmdMagSetCalibData`.

The command `SBG_ECOM_CMD_START_MAG_CALIB` bandwidth parameter is not used anymore and has no effect.
The method `sbgEComCmdMagStartCalib` is updated accordingly to reflect this change.

### New Motion Profiles

Two new motion profiles have been added that tunes the INS filter:
  - `SBG_ECOM_MOTION_PROFILE_OFF_ROAD_VEHICLE`
  - `SBG_ECOM_MOTION_PROFILE_UNDERWATER`

### New Output Messages

#### SBG_ECOM_LOG_PTP_STATUS

The new [SBG_ECOM_LOG_PTP_STATUS](#SBG_ECOM_LOG_PTP_STATUS) log returns advanced Precise Time Protocol status information for HPINS products.
When the INS doesn't act as the GrandMaster Clock on the network, this message is used to track the delta time between the INS and an external Grand Master Clock.

#### SBG_ECOM_LOG_SESSION_INFO

Returns in small chunks and continuously the device current configuration and information.  
Used to retrieve the device configuration from output messages binary stream over any serial or Ethernet interface.

Only available on ELLIPSE firmware v3 and HPINS running firmware 5.3 and above.

The new class `SbgEComSessionInfoCtx` offers a convenient way to reassemble each [SBG_ECOM_LOG_SESSION_INFO](#SBG_ECOM_LOG_SESSION_INFO) chunk and to get the device information and settings as JSON format.

### Updated Output Messages

### SBG_ECOM_LOG_STATUS
Added new CPU usage in percent field

#### SBG_ECOM_LOG_EFK_EULER and SBG_ECOM_LOG_EFK_QUAT
Added magDeclination and magInclination information
New `sbgEComLogEkfEulerGetMagneticHeading` getter to return INS magnetic heading instead of geographic.

### SBG_ECOM_LOG_IMU_SHORT
Added a high range scale factor for gyroscopes to support rotation rates above 1833°/s
Transparent if you use the getter `sbgEComLogImuShortGetDeltaAngle` otherwise, please use it.

### SBG_ECOM_LOG_SHIP_MOTION and SBG_ECOM_LOG_SHIP_MOTION_HP

The Ship Motion status bitmask has been cleaned up and improved for clarity and consistency:
 - `SBG_ECOM_HEAVE_VALID` renamed to `SBG_ECOM_SHIP_MOTION_HEAVE_VALID`.
 - `SBG_ECOM_HEAVE_VEL_AIDED` renamed to `SBG_ECOM_SHIP_MOTION_VEL_AIDED`.
 - `SBG_ECOM_HEAVE_SURGE_SWAY_INCLUDED` renamed to `SBG_ECOM_SHIP_MOTION_SURGE_SWAY_VALID`.
 - `SBG_ECOM_HEAVE_PERIOD_INCLUDED` has been deprecated and removed. Please use `SBG_ECOM_SHIP_MOTION_HEAVE_PERIOD_VALID` exclusively.
 - `SBG_ECOM_HEAVE_PERIOD_VALID` renamed to `SBG_ECOM_SHIP_MOTION_HEAVE_PERIOD_VALID`.
 - `SBG_ECOM_HEAVE_SWELL_MODE` renamed to `SBG_ECOM_SHIP_MOTION_SWELL_MODE`.
 - `SBG_ECOM_SHIP_MOTION_ACCEL_VALID` has been added to indicate when `ACCEL_X`, `ACCEL_Y`, and `ACCEL_Z` accelerations are valid.

### Updated Output Triggers

The following messages have new output triggers options and are automatically migrated.

| sbgECom ID                | sbgECom Trigger | RestApi Trigger       |
|---------------------------|-----------------|-----------------------|
| SBG_ECOM_LOG_IMU_SHORT    | New Data        | Converted to 200 Hz   |
| SBG_ECOM_LOG_MAG_CALIB    | 1 Hz to 50 Hz   | Converted to New Data |

### Deprecations

The message [SBG_ECOM_LOG_IMU_DATA](#SBG_ECOM_LOG_IMU_DATA) is deprecated and [SBG_ECOM_LOG_IMU_SHORT](#SBG_ECOM_LOG_IMU_SHORT) should be used instead.
The message `SBG_ECOM_LOG_FAST_IMU_DATA` has been removed from ELLIPSE firmware v3 and [SBG_ECOM_LOG_IMU_SHORT](#SBG_ECOM_LOG_IMU_SHORT) should be used instead.

## From sbgECom v3.x

sbgECom v4.x is a major improvement over sbgECom v3.x in term of features, code quality and organization.  
SBG Systems always tries to minimize breaking changes between sbgECom versions but is also committed to avoid technical debts. 

The macro SBG_ECOM_USE_DEPRECATED_MACROS can be set to maximize backward compatibility and reduce existing code base rework.

However, SBG Systems recommend to update existing code base and drop usage of legacy / deprecated definitions and methods.

### Unified code namespace

sbgECom namespace conventions were not aligned with SBG Systems quality standards and an evolution was needed. 

Most sbgECom binary logs struct, enum and function definitions have been updated to comply with strict namespace.  
The legacy names have been kept and indicated to be deprecated to avoid breaking changes.

Almost all the source files located in the `sbgECom/src/logs` directory have been reworked.

Legacy sbgECom v3 style:

```c
SbgErrorCode sbgEComBinaryLogParseEkfEulerData(SbgStreamBuffer *pInputStream, SbgLogEkfEulerData *pOutputData);
```

New sbgECom v4 style:

```c
SbgErrorCode sbgEComLogEkfEulerReadFromStream(SbgEComLogEkfEuler *pLogData, SbgStreamBuffer *pStreamBuffer);
```

### Breaking change for SBG_ECOM_CMD_API_GET

The SBG_ECOM_CMD_API_GET command introduced in sbgECom v3 and only used for High Performance INS were using the ID **46**.  
However, the command SBG_ECOM_CMD_GNSS_1_INSTALLATION used for ELLIPSE was also using ID 46 resulting in a conflict.

To solve this issue, the command SBG_ECOM_CMD_API_GET ID has been updated to **48** and you must update your code.  
This new command ID is used by High Performance INS firmware 5.x and above.

> SBG_ECOM_CMD_API_GET command ID 46 changed to 48 is a breaking change that requires code update if used.

### SBG_ECOM_LOG_GPS#_SAT code
In sbgECom v3.x, the list of satellites and signals were dynamically allocated using malloc/free.  
This has been replaced by a static allocation as the SbgEComLogSatList struct is smaller than existing SbgEComLogUnion union size.   
The new static struct size is 1993 bytes and the current SbgEComLogUnion size is 4086 bytes.  

As a result, the `sbgEComLogSatListConstruct` and `sbgEComLogSatListAdd` methods have been updated.  
The `sbgEComLogSatListDestroy` has been removed as no more memory release is needed.

The `SbgEComLogSatList` struct has been updated:
 - `pSatData` has been replaced by `satData`
 - `satDataArraySize` has been removed

The `SbgEComLogSatEntry`struct has been updated:
 - `pSignalData` has been replaced by `signalData`
 - `signalDataArraySize` has been removed

Please update your code to reflect these changes. The protocol itself has not changed.

### Deprecated SBG_ECOM_LOG_IMU_DATA log

The log SBG_ECOM_LOG_IMU_DATA is now considered to be deprecated and will be removed in future firmware and sbgECom updates.  
Please use the log SBG_ECOM_LOG_IMU_SHORT to get high rate, unfiltered and straight IMU data.

Please also checkout SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY and SBG_ECOM_LOG_EKF_ROT_ACCEL_NED logs to get un-biased, earth rotation free 
and gravity free vehicle (body) or navigation measurements.

These two new logs are for now, only available for High Performance INS such as the EKinox Micro.

## From sbgECom v1.x
The sbgECom version 2.x change the C API even if the low level sbgECom protocol API remains backward compatible.  
In otherworld, a C code written with sbgECom version 1.x will not compile directly with sbgECom versions 2.x and higher.  
But your old C code using sbgECom versions 1.x will still be able to correctly setup and configure your ELLIPSE product.  

### GNSS module
The _SbgEComGnssModelsStdIds_ enum has been simplified and now ELLIPSE-N and ELLIPSE-D should only use _SBG_ECOM_GNSS_MODEL_INTERNAL_ to select the internal GNSS receiver.  
The only exception is for ELLIPSE-N hardware revision 1 & 2 as the ublox Max M8 has two modes of operations, either GPS+GLONASS or GPS+BeiDou.  
The default GPS+GLONASS mode is selected with the _SBG_ECOM_GNSS_MODEL_INTERNAL_ model and the GPS+BeiDou one with _SBG_ECOM_GNSS_MODEL_UBLOX_GPS_BEIDOU_.  
The _SBG_ECOM_GNSS_MODEL_UBLOX_GPS_BEIDOU_ model couldn't be used with ELLIPSE-N with hardware revision 3.x or ELLIPSE-D as the GNSS receiver is tracking GPS+GLONASS+BeiDou+Galileo concurrently.  

The table below helps you update your enums to sbgECom v2 correctly:
| sbgECom 1.x                             | sbgECom 2.x                             | Remarks                                                                        |
| --------------------------------------- | --------------------------------------- | ------------------------------------------------------------------------------ |
| SBG_ECOM_GNSS_MODEL_UBLOX_GPS_GLONASS   | SBG_ECOM_GNSS_MODEL_INTERNAL            | To use for ELLIPSE-N and ELLIPSE-D                                             |
| SBG_ECOM_GNSS_MODEL_NMEA                | SBG_ECOM_GNSS_MODEL_NMEA                | ELLIPSE-E, external NMEA receiver (listen only)                                |
| SBG_ECOM_GNSS_MODEL_UBLOX_GPS_BEIDOU    | SBG_ECOM_GNSS_MODEL_UBLOX_GPS_BEIDOU    | Can only be selected on ELLIPSE-N revision 1 & 2                               |
| SBG_ECOM_GNSS_MODEL_UBLOX_EXTERNAL      | SBG_ECOM_GNSS_MODEL_UBLOX_EXTERNAL      | ELLIPSE-E, external ublox receiver (listen only)                               |
| SBG_ECOM_GNSS_MODEL_UBLOX_HIGH_DYNAMICS | SBG_ECOM_GNSS_MODEL_INTERNAL            | For high dynamics applications, please selected the appropriate motion profile |
| SBG_ECOM_GNSS_MODEL_NOVATEL_EXTERNAL    | SBG_ECOM_GNSS_MODEL_NOVATEL_EXTERNAL    | ELLIPSE-E, external Novatel receiver (listen only)                             |
| SBG_ECOM_GNSS_MODEL_ELLIPSE_D_INTERNAL  | SBG_ECOM_GNSS_MODEL_INTERNAL            | Legacy ELLIPSE-D hardware 1 & 2, don't use it anymore                          |
| SBG_ECOM_GNSS_MODEL_UBLOX_HIGH_SPEED    | SBG_ECOM_GNSS_MODEL_INTERNAL            | For high speed applications, please selected the appropriate motion profile    |
| SBG_ECOM_GNSS_MODEL_SEPTENTRIO_EXTERNAL | SBG_ECOM_GNSS_MODEL_SEPTENTRIO_EXTERNAL | ELLIPSE-E, external Septentrio receiver (listen only)                          |
| SBG_ECOM_GNSS_MODEL_UBLOX_LOW_SPEED     | SBG_ECOM_GNSS_MODEL_INTERNAL            | For low dynamics applications, please selected the appropriate motion profile  |

Please also update your code according to the following recommendations and modifications:
- `sbgEComCmdGnss1GetModelInfo` method has been replaced by `sbgEComCmdGnss1GetModelId` with a simpler interface as the version field is no more used in ELLIPSE firmware.
- `sbgEComCmdGnss1GetLeverArmAlignment` and `sbgEComCmdGnss1SetLeverArmAlignment` methods have been suppressed by `sbgEComCmdGnss1InstallationGet` and `sbgEComCmdGnss1InstallationSet` methods.

### Magnetometers module

The method `sbgEComCmdMagGetModelInfo` has been replaced by `sbgEComCmdMagGetModelId` with a simpler interface as the version field is no more used in ELLIPSE firmware.

### Sensor module

The method `sbgEComCmdSensorGetMotionProfileInfo` has been replaced by `sbgEComCmdSensorGetMotionProfileId` with a simpler interface as the version field is no more used in ELLIPSE firmware.

### Legacy IG-500 sbgCom

sbgECom version 2.x drop the legacy IG-500 support so the methods `sbgEComCmdOutputGetLegacyConf` and `sbgEComCmdOutputSetLegacyConf` are removed.  
Please update your code to use sbgECom protocol instead.
