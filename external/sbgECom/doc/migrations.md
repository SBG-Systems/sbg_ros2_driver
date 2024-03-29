# Migrations
These pages help you easily migrate your code from previous sbgECom major versions.

## From sbgECom v3.x

sbgECom v4.x is a major improvement over sbgECom v3.x in term of features, code quality and organization.  
SBG Systems always tries to minimize breaking changes between sbgECom versions but is also committed to avoid technical debts. 

The macro SBG_ECOM_USE_DEPRECATED_MACROS can be set to maximize backward compatibility and reduce existing code base rework.

However, SBG Systems recommend to update existing code base and drop usage of legacy / deprecated definitions and methods.

### Unified code namespace

sbgECom namespace conventions were not aligned with SBG Systems quality standards and an evolution was needed. 

Most sbgECom binary logs struct, enum and function definitions have been updated to comply with strict namespace.  
The legacy names have been kept and indicated to be deprecated to avoid breaking changes.

Almost all the source files located in the `src/logs` directory have been reworked.

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

sbgECom version 2.x drop the legacy IG-500 support so the methods `sbgEComCmdOutputGetLegacyConf` and `sbgEComCmdOutputSetLegacyConf` are removed. Please update your code to use sbgECom protocol instead.
