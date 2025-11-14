# sbgECom Binary Messages {#binaryMessages}

The `SBG_ECOM_CLASS_LOG_ECOM_0 (0x01)` message class encompasses all binary messages that can be output by the device.  
In addition to outputting data, some messages can also be used to input aiding information, such as the [SBG_ECOM_LOG_AIR_DATA (36)](#SBG_ECOM_LOG_AIR_DATA).

The sbgECom binary logs are designed for high data rates with minimal payload size.  
This protocol is ideal for both real-time stabilization/navigation and post-processing applications. 

Each message encapsulates a coherent set of data and status information that can be fully interpreted without requiring additional context.
Users can configure the output rate for each message based on their specific requirements, optimizing the management of transmission bandwidth.

## Message Timestamping

Each log includes a monotonic timestamp in microseconds, representing the time since the sensor was powered up.  
This timestamp can be converted to GPS Time of Week (ToW) or UTC time using the  [SBG_ECOM_LOG_UTC_TIME](#SBG_ECOM_LOG_UTC_TIME) message.

When valid GPS time is available from a GNSS receiver, the `SBG_ECOM_LOG_UTC_TIME` message provides both the timestamp and the corresponding GPS/UTC time.  
This allows for straightforward conversion of any message timestamp to GPS/UTC time.

The following code sample demonstrates how to convert a message timestamp (`msgTimeStampUs`) to absolute GPS Time of Week in milliseconds.    
The variable `lastUtcMsg` holds the latest valid `SBG_ECOM_LOG_UTC_TIME` information received.

```C
  const uint32_t  gpsTimeOfWeekMaxMs  = 60ul*60ul*24ul*7ul*1000ul;
  
  int32_t         deltaTimeUs         = newMsg.timeStamp - lastUtcMsg.timeStamp;
  int32_t         newMsgTowMs         = lastUtcMsg.gpsTimeOfWeek + deltaTimeUs/1000;
  
  if (newMsgTowMs < 0)
  {
    newMsgTowMs += gpsTimeOfWeekMaxMs;
  }
  else if (newMsgTowMs >= gpsTimeOfWeekMaxMs)
  {
    newMsgTowMs -= gpsTimeOfWeekMaxMs;
  }
```

> [!NOTE]
> Timestamp to UTC time conversion is implemented in the `sbgBasicLogger` CLI tool.  
> For reference, you can review the `CLoggerContext` C++ implementation.

## Messages Overview

The following list, provides a quick overview of all available logs for this message class.  
It briefly describe which parameters are contained in each output log.

| Name (Msg ID)                                                            | Description                                                                            |
|--------------------------------------------------------------------------|----------------------------------------------------------------------------------------|
| [SBG_ECOM_LOG_STATUS (01)](#SBG_ECOM_LOG_STATUS)                         | Status general, clock, com aiding, solution, heave                                     |
| [SBG_ECOM_LOG_UTC_TIME (02)](#SBG_ECOM_LOG_UTC_TIME)                     | Provides UTC time reference                                                            |
| [SBG_ECOM_LOG_IMU_DATA (03)](#SBG_ECOM_LOG_IMU_DATA)                     | DEPRECATED: Synchronous body IMU rates and accelerations                               |
| [SBG_ECOM_LOG_IMU_SHORT (44)](#SBG_ECOM_LOG_IMU_SHORT)                   | Body IMU rates and accelerations. Synchronous or asynchronous depending of the INS.    |
| [SBG_ECOM_LOG_EKF_EULER (06)](#SBG_ECOM_LOG_EKF_EULER)                   | Includes roll, pitch, yaw and their accuracies on each axis                            |
| [SBG_ECOM_LOG_EKF_QUAT (07)](#SBG_ECOM_LOG_EKF_QUAT)                     | Includes the 4 quaternions values                                                      |
| [SBG_ECOM_LOG_EKF_NAV (08)](#SBG_ECOM_LOG_EKF_NAV)                       | Position and velocities in NED coordinates with the accuracies on each axis            |
| [SBG_ECOM_LOG_EKF_VEL_BODY (54)](#SBG_ECOM_LOG_EKF_VEL_BODY)             | INS velocity expressed in body/vehicle frame.                                          |
| [SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY (52)](#SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY) | Corrected Earth rate and gravity free rates and accelerations in body/vehicle frame.   |
| [SBG_ECOM_LOG_EKF_ROT_ACCEL_NED (53)](#SBG_ECOM_LOG_EKF_ROT_ACCEL_NED)   | Corrected Earth rate and gravity free rates and accelerations in NED navigation frame. |
| [SBG_ECOM_LOG_SHIP_MOTION (09)](#SBG_ECOM_LOG_SHIP_MOTION)               | Real time heave, surge, sway, accelerations and velocity                               |
| [SBG_ECOM_LOG_SHIP_MOTION_HP (32)](#SBG_ECOM_LOG_SHIP_MOTION_HP)         | Delayed heave, surge, sway, accelerations and velocity                                 |
| [SBG_ECOM_LOG_MAG (04)](#SBG_ECOM_LOG_MAG)                               | Magnetic data with associated accelerometer on each axis                               |
| [SBG_ECOM_LOG_MAG_CALIB (05)](#SBG_ECOM_LOG_MAG_CALIB)                   | Magnetometer calibration data (raw buffer)                                             |
| [SBG_ECOM_LOG_GPS1_VEL (13)](#SBG_ECOM_LOG_GPSX_VEL)                     | GNSS velocity from primary receiver                                                    |
| [SBG_ECOM_LOG_GPS1_POS (14)](#SBG_ECOM_LOG_GPSX_POS)                     | GNSS position from primary receiver                                                    |
| [SBG_ECOM_LOG_GPS1_HDT (15)](#SBG_ECOM_LOG_GPSX_HDT)                     | GNSS true heading from primary receiver                                                |
| [SBG_ECOM_LOG_GPS1_RAW (31)](#SBG_ECOM_LOG_GPSX_RAW)                     | GNSS raw data from primary receiver                                                    |
| [SBG_ECOM_LOG_GPS1_SAT (50)](#SBG_ECOM_LOG_GPSX_SAT)                     | GNSS satellite in view information from primary receiver                               |
| [SBG_ECOM_LOG_GPS2_VEL (16)](#SBG_ECOM_LOG_GPSX_VEL)                     | GNSS velocity from secondary receiver                                                  |
| [SBG_ECOM_LOG_GPS2_POS (17)](#SBG_ECOM_LOG_GPSX_POS)                     | GNSS position from secondary receiver                                                  |
| [SBG_ECOM_LOG_GPS2_HDT (18)](#SBG_ECOM_LOG_GPSX_HDT)                     | GNSS true heading from secondary receiver                                              |
| [SBG_ECOM_LOG_GPS2_RAW (38)](#SBG_ECOM_LOG_GPSX_RAW)                     | GNSS raw data from secondary receiver                                                  |
| [SBG_ECOM_LOG_GPS2_SAT (51)](#SBG_ECOM_LOG_GPSX_SAT)                     | GNSS satellite in view information from secondary receiver                             |
| [SBG_ECOM_LOG_ODO_VEL (19)](#SBG_ECOM_LOG_ODO_VEL)                       | Provides odometer velocity measured by the device                                      |
| [SBG_ECOM_LOG_AIR_DATA (36)](#SBG_ECOM_LOG_AIR_DATA)                     | Barometric altimeter input/output for airdata support                                  |
| [SBG_ECOM_LOG_DEPTH (47)](#SBG_ECOM_LOG_DEPTH)                           | Depth sensor measurement log used for subsea navigation                                |
| [SBG_ECOM_LOG_DVL_BOTTOM_TRACK (29)](#SBG_ECOM_LOG_DVL_XXXX)             | Doppler Velocity Log for bottom tracking data                                          |
| [SBG_ECOM_LOG_DVL_WATER_TRACK (30)](#SBG_ECOM_LOG_DVL_XXXX)              | Doppler Velocity log for water layer data                                              |
| [SBG_ECOM_LOG_USBL (37)](#SBG_ECOM_LOG_USBL)                             | Raw USBL position data for subsea navigation                                           |
| [SBG_ECOM_LOG_EVENT_A (24)](#SBG_ECOM_LOG_EVENT_XXXX)                    | Event marker sent when a signal is detected on Sync In A pin                           |
| [SBG_ECOM_LOG_EVENT_B (25)](#SBG_ECOM_LOG_EVENT_XXXX)                    | Event marker sent when a signal is detected on Sync In B pin                           |
| [SBG_ECOM_LOG_EVENT_C (26)](#SBG_ECOM_LOG_EVENT_XXXX)                    | Event marker sent when a signal is detected on Sync In C pin                           |
| [SBG_ECOM_LOG_EVENT_D (27)](#SBG_ECOM_LOG_EVENT_XXXX)                    | Event marker sent when a signal is detected on Sync In D pin                           |
| [SBG_ECOM_LOG_EVENT_E (28)](#SBG_ECOM_LOG_EVENT_XXXX)                    | Event marker sent when a signal is detected on Sync In E pin                           |
| [SBG_ECOM_LOG_EVENT_OUT_A (45)](#SBG_ECOM_LOG_EVENT_XXXX)                | Event marker used to timestamp each generated Sync Out A signal                        |
| [SBG_ECOM_LOG_EVENT_OUT_B (46)](#SBG_ECOM_LOG_EVENT_XXXX)                | Event marker used to timestamp each generated Sync Out B signal                        |
| [SBG_ECOM_LOG_DIAG (48)](#SBG_ECOM_LOG_DIAG)                             | Returns error, warning, info and debug messages                                        |
| [SBG_ECOM_LOG_RTCM_RAW (49)](#SBG_ECOM_LOG_RTCM_RAW)                     | RTCM/NTRIP RAW data stream that can be used in post-processing                         |
| [SBG_ECOM_LOG_SESSION_INFO (55)](#SBG_ECOM_LOG_SESSION_INFO)             | Session information, including device information and current settings.                |
| [SBG_ECOM_LOG_PTP_STATUS (57)](#SBG_ECOM_LOG_PTP_STATUS)                 | Precise Time Protocol (PTP) status and metric values.                                  |
| [SBG_ECOM_LOG_VELOCITY_1 (58)](#SBG_ECOM_LOG_VELOCITY_X)                 | Generic 1D, 2D or 3D velocity aiding measurement.                                      |
| [SBG_ECOM_LOG_VIB_MON_FFT (59)](#SBG_ECOM_LOG_VIB_MON_FFT)               | Vibration monitoring FFT spectrum magnitudes.                                          |
| [SBG_ECOM_LOG_VIB_MON_REPORT (60)](#SBG_ECOM_LOG_VIB_MON_REPORT)         | Vibration monitoring summary report with computed metrics.                             |

## Messages Availability

In each message definition, you'll find details about the availability of the message for specific products and the firmware version in which it was introduced.  
Some messages are specific to certain product capabilities, such as INS-only features or marine options.

For devices running up-to-date firmware, the new [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi) can be used to easily query the list of supported output logs.  
This can be done using a GET request to the following endpoint:

```sh
sbgEComApi -s COM1 -r 115200 -g /api/v1/messages/std
```

## Messages Versions

SBG Systems continuously improves its products by adding new features and outputs.  
This includes introducing new sbgECom binary messages and updating existing ones with additional fields and data.

### Payload versions

Each message payload definition specifies whether new fields have been added in various sbgECom releases.  
It is essential to determine the sbgECom version implemented by your device, as described [here](#SBG_ECOM_VERSION_DEF).

For example, the [SBG_ECOM_LOG_STATUS](#SBG_ECOM_LOG_STATUS) output log has been updated with two new fields over time, as shown below:

| Field                       | Description                                                                       | Unit | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|------|--------|------|--------|
| TIME_STAMP                  | Time since sensor is powered up                                                   | µs   | uint32 | 4    | 0      |
| GENERAL_STATUS              | General device status (see [STATUS_GENERAL_STATUS](#STATUS_GENERAL_STATUS))       | -    | uint16 | 2    | 4      |
| COM_STATUS_2                | Additional communication status (see [STATUS_COM_STATUS_2](#STATUS_COM_STATUS_2)) | -    | uint16 | 2    | 6      |
| COM_STATUS                  | Communication status (see [STATUS_COM_STATUS](#STATUS_COM_STATUS))                | -    | uint32 | 4    | 8      |
| AIDING_STATUS               | Aiding equipment status (see [STATUS_AIDING_STATUS](#STATUS_AIDING_STATUS))       | -    | uint32 | 4    | 12     |
| RESERVED_2                  | Reserved status field for future use                                              | -    | uint32 | 4    | 16     |
| RESERVED_3                  | Reserved field for future use                                                     | -    | uint16 | 2    | 20     |
| UP_TIME                     | System up time since the power on - 0 if N/A (added v1.7)                         | s    | uint32 | 4    | 22     |
| CPU_USAGE                   | Main CPU usage in percent - 0xFF if N/A (added v5.0)                              | %    | uint8  | 1    | 23     |

Each new field addition corresponds to a specific sbgECom version, allowing developers to understand the evolution of the message and implement backward compatibility when necessary.

### Backward/Upward Compatibility

New fields may be added to the end of a message payload over time, but fields are never removed to maintain backward binary compatibility.  
If a field becomes obsolete, it will be marked as reserved or deprecated.

If you use an older sbgECom implementation to parse messages from a firmware with a newer sbgECom protocol, the parsing should still function correctly.  
However, you will not have access to any new fields or features introduced in the latest firmware.

The sbgECom protocol implementation ensures compatibility by checking if a message payload meets the minimum required size.  
Any additional fields beyond this size are parsed optionally, based on the remaining payload data.

For reference on this mechanism, you can review the `SBG_ECOM_LOG_STATUS` message parsing implementation in the sbgECom C library.

> [!WARNING]
> If you implement your own sbgECom protocol and message parsing, avoid checking for exact equality in the received message payload size.  
> Instead, follow the sbgECom message payload parsing C implementation to ensure upward compatibility.
## Additional Messages

This sections covers output logs used to report device information, settings and for troubleshooting.

### SBG_ECOM_LOG_DIAG (48) {#SBG_ECOM_LOG_DIAG}

This log is sent by the device whenever an internal error, warning, or informational message is generated.

- **Message Name (ID):** `SBG_ECOM_LOG_DIAG (48)`
- **Compatibility:** AHRS/INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-3.0-blue) ![PULSE](https://img.shields.io/badge/PULSE-1.0-blue)
- **Payload Size:** Variable (6 bytes + message length)

| Field       | Description                                                    | Unit  | Format | Size       | Offset |
|-------------|----------------------------------------------------------------|-------|--------|------------|--------|
| TIME_STAMP  | Time since the sensor was powered up                           | µs    | uint32 | 4          | 0      |
| TYPE        | Type of message reported (see [TYPE](#DIAG_TYPE))              | -     | uint8  | 1          | 4      |
| ERROR_CODE  | Error code, if applicable (see [SbgErrorCode](#SbgErrorCode))  | -     | uint8  | 1          | 5      |
| MESSAGE     | Detailed message as a NULL-terminated C string                 | -     | char[] | [0-4080]   | 6      |

#### TYPE Definition {#DIAG_TYPE}

| Value | Name                      | Description                                                                    |
|-------|---------------------------|--------------------------------------------------------------------------------|
| 0     | SBG_DEBUG_LOG_TYPE_ERROR  | Indicates an error; should not occur during normal operation                   |
| 1     | SBG_DEBUG_LOG_TYPE_WARNING| Indicates a warning; may occur during normal operation                         |
| 2     | SBG_DEBUG_LOG_TYPE_INFO   | Provides informational content, useful for troubleshooting                     |
| 3     | SBG_DEBUG_LOG_TYPE_DEBUG  | Debug information, available only with specific firmware                       |

### SBG_ECOM_LOG_RTCM_RAW (49) {#SBG_ECOM_LOG_RTCM_RAW}

This log stores the RTCM raw data stream received by the INS/GNSS module for RTK/DGPS positioning modes.  
The RTCM data stream may originate from a serial/Ethernet interface or be retrieved via the built-in NTRIP client.

The data is stored as received, without modification, allowing Qinertia to use it for post-processing workflows and debugging.  
Each message can contain up to 4086 bytes of data to fit within a standard sbgECom protocol frame.   
The RTCM messages may be fragmented across multiple log entries depending on their length.

- **Message Name (ID):** `SBG_ECOM_LOG_RTCM_RAW (49)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.1-blue)
- **Payload Size:** Variable (up to 4086)

| Field       | Description                                              | Unit | Format | Size       | Offset |
|-------------|----------------------------------------------------------|------|--------|------------|--------|
| RAW_BUFFER  | Bytes containing raw RTCM data received by the INS.      | -    | void   | [0-4086]   | 0      |

### SBG_ECOM_LOG_SESSION_INFO (55) {#SBG_ECOM_LOG_SESSION_INFO}

This log periodically streams device information and settings as a JSON string.

Since the JSON string can be large, it is split into smaller chunks to prevent delays in data transmission and avoid serial port saturation.  
Each chunk is sent using the frame structure below, and the host is responsible for reassembling the chunks to reconstruct the full JSON message.

A new session info message is transmitted every minute, allowing the host to retrieve and log the latest device information and settings at any time.

- **Message Name (ID):** `SBG_ECOM_LOG_SESSION_INFO (55)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-5.4-blue)
- **Payload Size:** Variable (up to 70)

| Field       | Description                                              | Unit | Format | Size       | Offset |
|-------------|----------------------------------------------------------|------|--------|------------|--------|
| PAGE_INDEX  | Zero-based index of the current chunk.                   | -    | uint16 | 2          | 0      |
| PAGE_COUNT  | Total number of chunks in the current transmission.      | -    | uint16 | 2          | 2      |
| DATA_SIZE   | Size in bytes for the DATA field.                        | -    | uint16 | 2          | 4      |
| DATA        | JSON data chunk, part of the full JSON message.          | -    | char[] | [0-64]     | 6      |

## Status and Time

In this section you can find all sbgECom logs related to status reporting.

### SBG_ECOM_LOG_STATUS (01) {#SBG_ECOM_LOG_STATUS}

This output combines all system status data, divided into several categories: General, Communications, Aiding.

This log is useful for advanced status information.

- **Message Name (ID):** `SBG_ECOM_LOG_STATUS (01)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue) ![PULSE](https://img.shields.io/badge/PULSE-1.0-blue)
- **Payload Size:** 27 bytes

| Field                       | Description                                                                       | Unit | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|------|--------|------|--------|
| TIME_STAMP                  | Time since sensor is powered up                                                   | µs   | uint32 | 4    | 0      |
| GENERAL_STATUS              | General device status (see [STATUS_GENERAL_STATUS](#STATUS_GENERAL_STATUS))       | -    | uint16 | 2    | 4      |
| COM_STATUS_2                | Additional communication status (see [STATUS_COM_STATUS_2](#STATUS_COM_STATUS_2)) | -    | uint16 | 2    | 6      |
| COM_STATUS                  | Communication status (see [STATUS_COM_STATUS](#STATUS_COM_STATUS))                | -    | uint32 | 4    | 8      |
| AIDING_STATUS               | Aiding equipment status (see [STATUS_AIDING_STATUS](#STATUS_AIDING_STATUS))       | -    | uint32 | 4    | 12     |
| RESERVED_2                  | Reserved status field for future use                                              | -    | uint32 | 4    | 16     |
| RESERVED_3                  | Reserved field for future use                                                     | -    | uint16 | 2    | 20     |
| UP_TIME                     | System up time since the power on - 0 if N/A (added v1.7)                         | s    | uint32 | 4    | 22     |
| CPU_USAGE                   | Main CPU usage in percent - 0xFF if N/A (added v5.0)                              | %    | uint8  | 1    | 23     |

> [!WARNING]
> The `COM_STATUS_2` field has been introduced in sbgECom version 4.0. In previous versions, this field was reserved and not used.  
> Please ensure that you only utilize the additional communication status flags with products running firmware that supports sbgECom version 4.0 or later.

#### General device status (GENERAL_STATUS field) {#STATUS_GENERAL_STATUS}

Provides general device status and information such as the power supplies (main, IMU, GNSS), settings, temperature and data-logger.

| Bit | Name                            | Type | Description                                           |
|-----|---------------------------------|------|-------------------------------------------------------|
| 0   | SBG_ECOM_GENERAL_MAIN_POWER_OK  | Mask | Set if main power supply is OK.                       |
| 1   | SBG_ECOM_GENERAL_IMU_POWER_OK   | Mask | Set if IMU power supply is OK.                        |
| 2   | SBG_ECOM_GENERAL_GPS_POWER_OK   | Mask | Set if GPS power supply is OK.                        |
| 3   | SBG_ECOM_GENERAL_SETTINGS_OK    | Mask | Set if settings are correctly loaded.                 |
| 4   | SBG_ECOM_GENERAL_TEMPERATURE_OK | Mask | Set if temperature is within specified limits.        |
| 5   | SBG_ECOM_GENERAL_DATALOGGER_OK  | Mask | Set if the data-logger is working correctly.          |
| 6   | SBG_ECOM_GENERAL_CPU_OK         | Mask | Set if the CPU headroom is correct.                   |

#### Communication status (COM_STATUS field) {#STATUS_COM_STATUS}

Provide information on ports, tells is they are valid or saturated.

| Bit  | Name                     | Type | Description                                                   |
|------|--------------------------|------|---------------------------------------------------------------|
| 0    | SBG_ECOM_PORTA_VALID     | Mask | Set to 0 in case of low level communication error.            |
| 1    | SBG_ECOM_PORTB_VALID     | Mask | Set to 0 in case of low level communication error.            |
| 2    | SBG_ECOM_PORTC_VALID     | Mask | Set to 0 in case of low level communication error.            |
| 3    | SBG_ECOM_PORTD_VALID     | Mask | Set to 0 in case of low level communication error.            |
| 4    | SBG_ECOM_PORTE_VALID     | Mask | Set to 0 in case of low level communication error.            |
| 5    | SBG_ECOM_PORTA_RX_OK     | Mask | Set to 0 in case of saturation on PORT A input.               |
| 6    | SBG_ECOM_PORTA_TX_OK     | Mask | Set to 0 in case of saturation on PORT A output.              |
| 7    | SBG_ECOM_PORTB_RX_OK     | Mask | Set to 0 in case of saturation on PORT B input.               |
| 8    | SBG_ECOM_PORTB_TX_OK     | Mask | Set to 0 in case of saturation on PORT B output.              |
| 9    | SBG_ECOM_PORTC_RX_OK     | Mask | Set to 0 in case of saturation on PORT C input.               |
| 10   | SBG_ECOM_PORTC_TX_OK     | Mask | Set to 0 in case of saturation on PORT C output.              |
| 11   | SBG_ECOM_PORTD_RX_OK     | Mask | Set to 0 in case of saturation on PORT D input.               |
| 12   | SBG_ECOM_PORTD_TX_OK     | Mask | Set to 0 in case of saturation on PORT D output.              |
| 13   | SBG_ECOM_PORTE_RX_OK     | Mask | Set to 0 in case of saturation on PORT E input.               |
| 14   | SBG_ECOM_PORTE_TX_OK     | Mask | Set to 0 in case of saturation on PORT E output.              |
| 15   | SBG_ECOM_ETH0_VALID      | Mask | Set to 0 if the ETH 0 interface is either closed or in error. |
| 16   | SBG_ECOM_ETH1_VALID      | Mask | Set to 0 if the ETH 1 interface is either closed or in error. |
| 17   | SBG_ECOM_ETH2_VALID      | Mask | Set to 0 if the ETH 2 interface is either closed or in error. |
| 18   | SBG_ECOM_ETH3_VALID      | Mask | Set to 0 if the ETH 3 interface is either closed or in error. |
| 19   | SBG_ECOM_ETH4_VALID      | Mask | Set to 0 if the ETH 4 interface is either closed or in error. |
| 25   | SBG_ECOM_CAN_VALID       | Mask | Set to 0 in case of low level communication error.            |
| 26   | SBG_ECOM_CAN_RX_OK       | Mask | Set to 0 in case of saturation on CAN Bus input buffer.       |
| 27   | SBG_ECOM_CAN_TX_OK       | Mask | Set to 0 in case of saturation on CAN Bus output buffer.      |
| 28-30| SBG_ECOM_CAN_BUS         | Enum | CAN Bus status (see [SBG_ECOM_CAN_BUS](#SBG_ECOM_CAN_BUS)).   |

##### CAN Bus States {#SBG_ECOM_CAN_BUS}

Reports CAN bus status:

| Value | Name                       | Description                                       |
|-------|----------------------------|---------------------------------------------------|
| 0     | SBG_ECOM_CAN_BUS_OFF       | Bus OFF operation due to too many errors.         |
| 1     | SBG_ECOM_CAN_BUS_TX_RX_ERR | Transmit or receive error.                        |
| 2     | SBG_ECOM_CAN_BUS_OK        | The CAN bus is working correctly.                 |
| 3     | SBG_ECOM_CAN_BUS_ERROR     | A general error has occurred on the CAN bus.      |

#### Communication status (COM_STATUS_2 field) {#STATUS_COM_STATUS_2}

Provide additional status on interfaces such as Ethernet.

| Bit | Name                    | Type | Description                                                  |
|-----|-------------------------|------|--------------------------------------------------------------|
| 0   | SBG_ECOM_COM2_ETH0_RX_OK| Mask | Set to 0 in case of error on ETH0 input such as saturation.  |
| 1   | SBG_ECOM_COM2_ETH0_TX_OK| Mask | Set to 0 in case of error on ETH0 output such as saturation. |
| 2   | SBG_ECOM_COM2_ETH1_RX_OK| Mask | Set to 0 in case of error on ETH1 input such as saturation.  |
| 3   | SBG_ECOM_COM2_ETH1_TX_OK| Mask | Set to 0 in case of error on ETH1 output such as saturation. |
| 4   | SBG_ECOM_COM2_ETH2_RX_OK| Mask | Set to 0 in case of error on ETH2 input such as saturation.  |
| 5   | SBG_ECOM_COM2_ETH2_TX_OK| Mask | Set to 0 in case of error on ETH2 output such as saturation. |
| 6   | SBG_ECOM_COM2_ETH3_RX_OK| Mask | Set to 0 in case of error on ETH3 input such as saturation.  |
| 7   | SBG_ECOM_COM2_ETH3_TX_OK| Mask | Set to 0 in case of error on ETH3 output such as saturation. |
| 8   | SBG_ECOM_COM2_ETH4_RX_OK| Mask | Set to 0 in case of error on ETH4 input such as saturation.  |
| 9   | SBG_ECOM_COM2_ETH4_TX_OK| Mask | Set to 0 in case of error on ETH4 output such as saturation. |

#### INS aiding status (AIDING_STATUS field) {#STATUS_AIDING_STATUS}

Tells which aiding data is received.

| Bit | Name                          | Type | Description                                              |
| --- | ----------------------------- | ---- | -------------------------------------------------------- |
| 0   | SBG_ECOM_AIDING_GPS1_POS_RECV | Mask | Set to 1 when valid GPS 1 position data is received      |
| 1   | SBG_ECOM_AIDING_GPS1_VEL_RECV | Mask | Set to 1 when valid GPS 1 velocity data is received      |
| 2   | SBG_ECOM_AIDING_GPS1_HDT_RECV | Mask | Set to 1 when valid GPS 1 true heading data is received  |
| 3   | SBG_ECOM_AIDING_GPS1_UTC_RECV | Mask | Set to 1 when valid GPS 1 UTC time data is received      |
| 4   | SBG_ECOM_AIDING_GPS2_POS_RECV | Mask | Set to 1 when valid GPS 2 position data is received      |
| 5   | SBG_ECOM_AIDING_GPS2_VEL_RECV | Mask | Set to 1 when valid GPS 2 velocity data is received      |
| 6   | SBG_ECOM_AIDING_GPS2_HDT_RECV | Mask | Set to 1 when valid GPS 2 true heading data is received  |
| 7   | SBG_ECOM_AIDING_GPS2_UTC_RECV | Mask | Set to 1 when valid GPS 2 UTC time data is received      |
| 8   | SBG_ECOM_AIDING_MAG_RECV      | Mask | Set to 1 when valid Magnetometer data is received        |
| 9   | SBG_ECOM_AIDING_ODO_RECV      | Mask | Set to 1 when Odometer pulse is received                 |
| 10  | SBG_ECOM_AIDING_DVL_RECV      | Mask | Set to 1 when valid DVL data is received                 |
| 11  | SBG_ECOM_AIDING_USBL_RECV     | Mask | Set to 1 when valid USBL data is received                |
| 12  | SBG_ECOM_AIDING_DEPTH_RECV    | Mask | Set to 1 when valid Depth sensor data is received        |
| 13  | SBG_ECOM_AIDING_AIR_DATA_RECV | Mask | Set to 1 when valid altitude and/or airspeed is received |
| 14  | SBG_ECOM_AIDING_VEL1_RECV     | Mask | Set to 1 when valid generic velocity 1 data is received  |

### SBG_ECOM_LOG_UTC_TIME (02) {#SBG_ECOM_LOG_UTC_TIME}

Provides UTC time reference and internal clock estimation metrics.

This message provides a time correspondence between the device `TIME_STAMP` value and the actual UTC Time.  
You should use this information to timestamp data to an absolute UTC or GPS time reference.

- **Message Name (ID):** `SBG_ECOM_LOG_UTC_TIME (02)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue) ![PULSE](https://img.shields.io/badge/PULSE-1.0-blue)
- **Payload Size:** 33 bytes

| Field                       | Description                                                                       | Unit | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|------|--------|------|--------|
| TIME_STAMP                  | Time since sensor is powered up                                                   | µs   | uint32 | 4    | 0      |
| TIME_STATUS                 | UTC time and clock sync status (see [TIME_STATUS](#TIME_STATUS))                  | -    | uint16 | 2    | 4      |
| YEAR                        | Year                                                                              | year | uint16 | 2    | 6      |
| MONTH                       | Month in Year [1 ... 12]                                                          | month| uint8  | 1    | 8      |
| DAY                         | Day in Month [1 … 31]                                                             | d    | uint8  | 1    | 9      |
| HOUR                        | Hour in day [0 … 23]                                                              | h    | uint8  | 1    | 10     |
| MIN                         | Minute in hour [0 … 59]                                                           | min  | uint8  | 1    | 11     |
| SEC                         | Second in minute [0 … 60]                                                         | s    | uint8  | 1    | 12     |
| NANOSEC                     | Nanosecond of second                                                              | ns   | uint32 | 4    | 13     |
| GPS_TOW                     | GPS Time of week                                                                  | ms   | uint32 | 4    | 17     |
| CLK_BIAS_STD                | Estimated clock bias 1-σ standard deviation – Added in v4.0                       | s    | float  | 4    | 21     |
| CLK_SF_ERROR_STD            | Estimated scale factor 1-σ standard deviation – Added in v4.0                     | %    | float  | 4    | 25     |
| CLK_RESIDUAL_ERR            | Measured error between GNSS PPS and internal clock – Added in v4.0                | s    | float  | 4    | 29     |

> [!NOTE]
> The `SEC` field can report 60 seconds only when a leap second is inserted.

> [!WARNING]
> The UTC time status has changed in latest protocol versions (sbgECom 4.0 or later).  
> Once the UTC time is initialized and known it stays in this state.

#### Time and Clock status (TIME_STATUS field) {#TIME_STATUS}

Provide information on the internal clock estimation and UTC status.

| Bit  | Name                             | Type | Description                                                                   |
|------|----------------------------------|------|-------------------------------------------------------------------------------|
| 0    | HAS_CLOCK_INPUT                  | Mask | Set if a valid input clock signal (PPS) is detected.                          |
| [1-4]| CLOCK_STATE                      | Enum | Define the internal clock estimation status (see [CLOCK_STATE](#CLOCK_STATE)) |
| 5    | UTC_IS_ACCURATE                  | Mask | Set if the reported UTC time is accurate (i.e., aligned with PPS).            |
| [6-9]| UTC_STATUS                       | Enum | Define the UTC validity status (see [UTC_STATUS](#UTC_STATUS))                |

##### Clock State (CLOCK_STATE field) {#CLOCK_STATE}

List internal clock alignment and correction state values:

| Value | Name                              | Description                                                                          |
|-------|-----------------------------------|--------------------------------------------------------------------------------------|
| 0     | SBG_ECOM_CLOCK_STATE_ERROR        | An error has occurred on the clock estimation.                                       |
| 1     | SBG_ECOM_CLOCK_STATE_FREE_RUNNING | The clock is based on the internal crystal using latest estimated clock bias/sf.     |
| 2     | SBG_ECOM_CLOCK_STATE_STEERING     | A PPS has been detected and the clock is converging to it.                           |
| 3     | SBG_ECOM_CLOCK_STATE_VALID        | The clock has converged to the PPS or is still considered to be accurate.            |

##### UTC Status (UTC_STATUS field) {#UTC_STATUS}

List available UTC enum status:

| Value | Name                              | Description                                                                          |
|-------|-----------------------------------|--------------------------------------------------------------------------------------|
| 0     | SBG_ECOM_UTC_STATUS_INVALID       | The UTC time is not known and invalid. The firmware initial date time is used.       |
| 1     | SBG_ECOM_UTC_STATUS_NO_LEAP_SEC   | UTC time initialized but leap second is not known. Firmware leap second is used.     |
| 2     | SBG_ECOM_UTC_STATUS_INITIALIZED   | UTC time initialized with a fully resolved leap second information.                  |

### SBG_ECOM_LOG_PTP_STATUS (57) {#SBG_ECOM_LOG_PTP_STATUS}

Message that provides status and metrics regarding the embedded Precise Time Protocol (PTP) service.  
It can report the time difference between the local INS time and an external GrandMaster clock.

The PTP service can operate in two different modes:
- `Master`: If the INS is the best available clock, it will broadcast PTP messages and act as a Grandmaster clock.
- `Passive`: Only listen for a PTP Grandmaster clock and reports the time delta between internal INS clock and PTP Grandmaster clock.

The message reporting the PTP status and accuracy is detailed below:

- **Message Name (ID):** `SBG_ECOM_LOG_PTP_STATUS (57)`
- **Compatibility:** High Performance INS
- **Firmware:** ![HPINS](https://img.shields.io/badge/HPINS-5.3-blue)
- **Payload Size:** 82 bytes

| Field                      | Description                                                                        | Unit | Format | Size | Offset |
|----------------------------|------------------------------------------------------------------------------------|------|--------|------|--------|
| TIME_STAMP                 | Time since sensor is powered up.                                                   | µs   | uint32 | 4    | 0      |
| STATUS                     | State, transport and time scale status (see [PTP_STATUS](#PTP_STATUS)).            | -    | uint16 | 2    | 4      |
| TIME_SCALE_OFFSET          | Internal time scale offset, in seconds.                                            | s    | double | 8    | 6      |
| LOCAL_CLOCK_IDENTITY       | Local clock identity, UINT64_MAX if invalid.                                       | -    | uint64 | 4    | 14     |
| LOCAL_CLOCK_PRIORITY1      | Local clock priority1 attribute.                                                   | -    | uint8  | 4    | 22     |
| LOCAL_CLOCK_PRIORITY2      | Local clock priority2 attribute.                                                   | -    | uint8  | 4    | 23     |
| LOCAL_CLOCK_CLASS          | Local clock class attribute.                                                       | -    | uint8  | 4    | 24     |
| LOCAL_CLOCK_ACCURACY       | Local clock accuracy.                                                              | -    | uint8  | 4    | 25     |
| LOCAL_CLOCK_LOG2_VARIANCE  | Local clock variance expressed as an exponential (base 2).                         | -    | uint16 | 4    | 26     |
| LOCAL_CLOCK_TIME_SOURCE    | Local clock time source.                                                           | -    | uint8  | 4    | 28     |
| MASTER_CLOCK_IDENTITY      | Master clock identity, UINT64_MAX if invalid.                                      | -    | uint64 | 4    | 29     |
| MASTER_CLOCK_PRIORITY1     | Master clock priority1 attribute.                                                  | -    | uint8  | 4    | 37     |
| MASTER_CLOCK_PRIORITY2     | Master clock priority2 attribute.                                                  | -    | uint8  | 4    | 38     |
| MASTER_CLOCK_CLASS         | Master clock class attribute.                                                      | -    | uint8  | 4    | 39     |
| MASTER_CLOCK_ACCURACY      | Master clock accuracy.                                                             | -    | uint8  | 4    | 40     |
| MASTER_CLOCK_LOG2_VARIANCE | Master clock variance expressed as an exponential (base 2).                        | -    | uint16 | 4    | 41     |
| MASTER_CLOCK_TIME_SOURCE   | Master clock time source.                                                          | -    | uint8  | 4    | 43     |
| MASTER_IP_ADDRESS          | Master clock IP address, UINT32_MAX if invalid.                                    | -    | uint32 | 4    | 44     |
| MEAN_PATH_DELAY            | Mean path delay to/from the master clock, NaN if not available.                    | s    | float  | 4    | 48     |
| MEAN_PATH_DELAY_STD_DEV    | Mean path delay standard deviation, NaN if not available.                          | s    | float  | 4    | 52     |
| CLOCK_OFFSET               | Offset between the local and master clocks, NaN if not available.                  | s    | double | 8    | 56     |
| CLOCK_OFFSET_STD_DEV       | Master clock offset standard deviation, NaN if not available.                      | s    | float  | 4    | 64     |
| CLOCK_FREQ_OFFSET          | Offset between the frequency of the local and master clocks, NaN if not available. | Hz   | float  | 4    | 68     |
| CLOCK_FREQ_OFFSET_STD_DEV  | Frequency offset standard deviation, NaN if not available.                         | Hz   | float  | 4    | 72     |
| MASTER_MAC_ADDRESS         | Master clock MAC address, all fields to UINT32_MAX if invalid (added in v5.2).     | -    | uint8  | 6    | 76     |

> [!NOTE]
> The local clock members are valid if and only if the local clock identity is valid.
> Similarly, the master clock members are valid if and only if the master clock identity is valid.

#### PTP_STATUS Definition {#PTP_STATUS}

The `PTP_STATUS` field provides status details, including the main PTP mode, transport and internal time scale.  
The field encodes several enumerations as described below:

| Bits    | Type  | Name                          | Description                                                           |
|---------|-------|-------------------------------|-----------------------------------------------------------------------|
| [0-2]   | Enum  | SBG_ECOM_LOG_PTP_STATE        | Main PTP state. (see [PTP_STATE](#PTP_STATE)).                        |
| [3-5]   | Enum  | SBG_ECOM_LOG_PTP_TRANSPORT    | PTP transport (added in v5.2). (see [PTP_TRANSPORT](#PTP_TRANSPORT)). |
| [6-7]   |       | Reserved                      | Reserved for future use.                                              |
| [8-10]  | Enum  | SBG_ECOM_LOG_PTP_TIME_SCALE   | Internal time scale. (see [PTP_TIME_SCALE](#PTP_TIME_SCALE)).         |
| [11-15] |       | Reserved                      | Reserved for future use.                                              |

##### PTP State {#PTP_STATE}

Main PTP operating mode and status.

| Value | Name                                | Description                        |
|-------|-------------------------------------|------------------------------------|
| 0     | SBG_ECOM_LOG_PTP_STATE_DISABLED     | PTP is disabled.                   |
| 1     | SBG_ECOM_LOG_PTP_STATE_FAULTY       | The device is in the faulty state. |
| 2     | SBG_ECOM_LOG_PTP_STATE_MASTER       | The device is the domain master.   |
| 3     | SBG_ECOM_LOG_PTP_STATE_PASSIVE      | The device is passive.             |

##### PTP Transport {#PTP_TRANSPORT}

The transport defines how PTP messages are sent between devices. The main options are:  
- UDP (User Datagram Protocol): PTP messages are sent over IP networks (IPv4/IPv6), allowing synchronization across switches and routers.  
- Ethernet: PTP messages are sent directly over Ethernet frames, providing lower latency and more precise synchronization in local networks.

| Value | Name                                | Description                         |
|-------|-------------------------------------|-------------------------------------|
| 0     | SBG_ECOM_LOG_PTP_TRANSPORT_UDP      | Transport is UDP.                   |
| 1     | SBG_ECOM_LOG_PTP_TRANSPORT_ETHERNET | Transport is Ethernet (IEEE 802.3). |

##### PTP Time Scale {#PTP_TIME_SCALE}

This enumeration specifies the time scale used as the reference for broadcasting PTP time.
The reference time can also include a positive or negative offset value specified by `TIME_SCALE_OFFSET`.

The IEEE 1588 standard specifies TAI (International Atomic Time) as the standard time scale for PTP (Precision Time Protocol).

| Value | Name                                | Description                                             |
|-------|-------------------------------------|---------------------------------------------------------|
| 0     | SBG_ECOM_LOG_PTP_TIME_SCALE_TAI     | Reference time is TAI (International Atomic Time).      |
| 1     | SBG_ECOM_LOG_PTP_TIME_SCALE_UTC     | Reference time is UTC (Coordinated Universal Time).     |
| 2     | SBG_ECOM_LOG_PTP_TIME_SCALE_GPS     | Reference time is GPS (Global Positioning System Time). |
## IMU Measurements

This section details the messages used to report Inertial Measurement Unit (IMU) data as measured by the device's accelerometers and gyroscopes.  
IMU measurements represent physical quantities, specifically accelerations in (m/s²) and angular rates in (rad/s) expressed in the device's body frame (X, Y, Z axes).

All IMU measurements are factory-calibrated to compensate for thermal drift, alignment errors, scale factors, and non-linearity.  
The IMU data is ready for use in an Inertial Navigation System (INS) filter to compute the device's orientation and navigation solution.

Additionally, if any sensor frame alignment adjustments are made (such as rotating the device within a vehicle's frame), these will affect the reported IMU data.

> [!NOTE]
> For vehicle rotation rates and accelerations compensated for gravity, earth rotation rate and residual errors, please check [SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY](#SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY) message.

### IMU Data Synchronization Methods

Inertial Measurement Unit (IMU) data can be processed either synchronously or asynchronously, depending on the product series.  
The synchronization method determines how IMU data is collected, processed, and output in relation to the main INS computation loop.

#### Synchronous IMU

For products like the ELLIPSE series and IMUs such as the PULSE-40, IMU data is synchronized with the main computation loop of the device.  
Users can configure the output rate down to 1 Hz, with the device automatically downsampling the data using a moving average filter.

For instance, if the main sampling loop runs at 1 kHz and the output is set to 200 Hz, the device averages 5 samples to produce each output value.  
Consequently, if 200 Hz is requested for both IMU and Extended Kalman Filter (EKF) data, exactly 200 messages per second for both IMU and EKF will be received.

#### Asynchronous IMU

High Performance INS products, such as the Navsight, Quanta, Ekinox, and Apogee series, operate with asynchronous IMU data.  
In these systems, the IMU and INS do not share a common clock.  
The IMU runs on its own clock, typically sampling at 200 Hz, while the INS operates its main processing loop also at 200 Hz but aligned with the GNSS time.

During each INS processing cycle, the latest IMU data messages are transmitted.  
Depending on the timing discrepancies between the IMU and INS clocks, there may be 0, 1, or 2 IMU messages per INS processing cycle.

For example, if the system is set to output 200 Hz EKF data and IMU data triggered on a new data event, exactly 200 EKF messages per second will be received, while the number of IMU messages may slightly vary, influenced by the IMU clock's offset relative to the INS clock.

### SBG_ECOM_LOG_IMU_DATA (03) ![DEPRECATED](https://img.shields.io/badge/DEPRECATED-red) {#SBG_ECOM_LOG_IMU_DATA}

The message is **deprecated** and should not be used anymore.  
It returns calibrated IMU data expressed in body frame as well as the IMU temperature.

- **Message Name (ID):** `SBG_ECOM_LOG_IMU_DATA (03)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 58 bytes

| Field                       | Description                                                                       | Unit  | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP                  | Time since sensor is powered up                                                   |  µs   | uint32 | 4    |    0   |
| IMU_STATUS                  | IMU status (see [IMU_DATA_STATUS](#IMU_DATA_STATUS))                              |   -   | uint16 | 2    |    4   |
| ACCEL_0_X                   | Acceleration along the X axis in the body frame.                                  |  m/s² | float  | 4    |    6   |
| ACCEL_0_Y                   | Acceleration along the Y axis in the body frame                                   |  m/s² | float  | 4    |   10   |
| ACCEL_0_Z                   | Acceleration along the Z axis in the body frame                                   |  m/s² | float  | 4    |   14   |
| GYRO_0_X                    | Rotation rate along the X axis in the body frame                                  | rad/s | float  | 4    |   18   |
| GYRO_0_Y                    | Rotation rate along the Y axis in the body frame                                  | rad/s | float  | 4    |   22   |
| GYRO_0_Z                    | Rotation rate along the Z axis in the body frame                                  | rad/s | float  | 4    |   26   |
| TEMPERATURE                 | IMU internal average temperature.                                                 |  °C   | float  | 4    |   30   |
| ACCEL_1_X                   | Acceleration along the X axis in the body frame                                   |  m/s² | float  | 4    |   34   |
| ACCEL_1_Y                   | Acceleration along the Y axis in the body frame                                   |  m/s² | float  | 4    |   38   |
| ACCEL_1_Z                   | Acceleration along the Z axis in the body frame                                   |  m/s² | float  | 4    |   42   |
| GYRO_1_X                    | Rotation rate along the X axis in the body frame                                  | rad/s | float  | 4    |   46   |
| GYRO_1_Y                    | Rotation rate along the Y axis in the body frame                                  | rad/s | float  | 4    |   50   |
| GYRO_1_Z                    | Rotation rate along the Z axis in the body frame                                  | rad/s | float  | 4    |   54   |

> [!NOTE]
> The fields `ACCEL_0_*` and `ACCEL_1_*` as well as `GYRO_0_*` and `GYRO_1_*` provide identical values. They are maintained for backward compatibility purposes.

> [!WARNING]
> Do not use this log for post-processing with software like Qinertia or for computing an INS solution.  
> Instead, refer to the new [SBG_ECOM_LOG_IMU_SHORT](#SBG_ECOM_LOG_IMU_SHORT) message.

#### IMU Data Status {#IMU_DATA_STATUS}

The IMU_DATA_STATUS field is a bitmask that indicates the health and status of the IMU.  
Each bit represents a specific status flag, providing detailed information on various components of the IMU system.

| Bit     | Name                            | Description                                                                    |
|---------|---------------------------------|--------------------------------------------------------------------------------|
| 0 (LSB) | SBG_ECOM_IMU_COM_OK             | Set to 1 if communication with the IMU is working properly.                    |
| 1       | SBG_ECOM_IMU_STATUS_BIT         | Set to 1 if the IMU passes internal BIT, including calibration and CPU checks. |
| 2       | SBG_ECOM_IMU_ACCEL_X_BIT        | Set to 1 if the X-axis accelerometer passes PBIT and CBIT.                     |
| 3       | SBG_ECOM_IMU_ACCEL_Y_BIT        | Set to 1 if the Y-axis accelerometer passes PBIT and CBIT.                     |
| 4       | SBG_ECOM_IMU_ACCEL_Z_BIT        | Set to 1 if the Z-axis accelerometer passes PBIT and CBIT.                     |
| 5       | SBG_ECOM_IMU_GYRO_X_BIT         | Set to 1 if the X-axis gyroscope passes PBIT and CBIT.                         |
| 6       | SBG_ECOM_IMU_GYRO_Y_BIT         | Set to 1 if the Y-axis gyroscope passes PBIT and CBIT.                         |
| 7       | SBG_ECOM_IMU_GYRO_Z_BIT         | Set to 1 if the Z-axis gyroscope passes PBIT and CBIT.                         |
| 8       | SBG_ECOM_IMU_ACCELS_IN_RANGE    | Set to 1 if all accelerometers are operating within the specified range.       |
| 9       | SBG_ECOM_IMU_GYROS_IN_RANGE     | Set to 1 if all gyroscopes are operating within the specified range.           |

> [!NOTE]
> **PBIT (Power-On Built-In Tests):** These tests are conducted once when the IMU is powered on, ensuring that all essential systems are functioning correctly at startup.  
> **CBIT (Continuous Built-In Tests):** These tests are performed continuously during operation, monitoring the health and status of the IMU components in real-time.

### SBG_ECOM_LOG_IMU_SHORT (44) {#SBG_ECOM_LOG_IMU_SHORT}

The `SBG_ECOM_LOG_IMU_SHORT` message reports IMU data, including accelerations and rotation rates expressed in the IMU body frame, in a compact and efficient format.

For High Performance INS, this log is asynchronous and can only be triggered on a *New Data* event.  
For ELLIPSE v3 and IMUs like the PULSE-40, this log is synchronous ans can be output at any rate from 1 Hz up to the maximum rate supported by the device.  
Typically 1 kHz for ELLIPSE v3 and up to 2 kHz for IMU-only products like the PULSE-40.

To ensure the best dynamic range and LSB accuracy, measurements are provided using int32 integers with a fixed scaling.  
For high rotation rates above ~1833°/s, a different high range scaling is applied.

> [!NOTE]
> This message supersedes the deprecated [SBG_ECOM_LOG_IMU_DATA](#SBG_ECOM_LOG_IMU_DATA) message.

- **Message Name (ID):** `SBG_ECOM_LOG_IMU_SHORT (44)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-2.1-blue) ![PULSE](https://img.shields.io/badge/PULSE-1.0-blue)
- **Payload Size:** 32 bytes

| Field                       | Description                                                                       | Unit  | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP                  | Time since sensor is powered up                                                   |  µs   | uint32 | 4    |    0   |
| IMU_STATUS                  | IMU status (see [IMU_STATUS](#IMU_STATUS))                                        |   -   | uint16 | 2    |    4   |
| ACCELERATION_X              | Acceleration along the X axis in the body frame (in LSB).                         |  m/s² | int32  | 4    |    6   |
| ACCELERATION_Y              | Acceleration along the Y axis in the body frame (in LSB).                         |  m/s² | int32  | 4    |   10   |
| ACCELERATION_Z              | Acceleration along the Z axis in the body frame (in LSB).                         |  m/s² | int32  | 4    |   14   |
| RATE_X                      | Rotation rate along the X axis in the body frame (in LSB).                        | rad/s | int32  | 4    |   18   |
| RATE_Y                      | Rotation rate along the Y axis in the body frame (in LSB).                        | rad/s | int32  | 4    |   22   |
| RATE_Z                      | Rotation rate along the Z axis in the body frame (in LSB).                        | rad/s | int32  | 4    |   26   |
| TEMPERATURE                 | IMU internal average temperature (in LSB).                                        |  °C   | int16  | 2    |   30   |

#### Converting to Physical Quantities

##### Temperature
The temperature is using a fixed scaling of 256 LSB for 1°C: *temperature = temperatureLSB / 256*

##### Accelerations

The accelerations are using a fixed scaling of 1048576 LSB for 1 m/s²: *acceleration = accelerationLSB / 1048576*

##### Rotation Rates

The rotation rates are using two different scaling:
  - *Standard Scaling*:   67108864 LSB for 1 rad/s if `SBG_ECOM_IMU_GYROS_USE_HIGH_SCALE` flag is **not** set.
  - *High Range Scaling*: 12304174 LSB for 1 rad/s if `SBG_ECOM_IMU_GYROS_USE_HIGH_SCALE` flag **is** set.

The device automatically switches to high range scaling when any rotation rate component exceeds approximately 1833°/s.

The following code snippet illustrates how to convert rotation rates from LSB to rad/s:

```C
  if (status & SBG_ECOM_IMU_GYROS_USE_HIGH_SCALE)
  {
    rate = rateLSB / 12304174.0;
  }
  else
  {
    rate = rateLSB / 67108864.0;
  }
```

> [!NOTE]
> Most SBG Systems products, such as the ELLIPSE, have a rotation rate range below 1000°/s and thus only use the standard scaling.

#### IMU Status {#IMU_STATUS}

The IMU_STATUS field is a bitmask that indicates the health and status of the IMU.  
Each bit represents a specific status flag, providing detailed information on various components of the IMU system.

| Bit     | Name                              | Description                                                                   |
|---------|-----------------------------------|-------------------------------------------------------------------------------|
| 0 (LSB) | SBG_ECOM_IMU_COM_OK               | Set if communication with the IMU is working properly.                        |
| 1       | SBG_ECOM_IMU_STATUS_BIT           | Set if the IMU passes internal BIT, including calibration and CPU checks.     |
| 2       | SBG_ECOM_IMU_ACCEL_X_BIT          | Set if the X-axis accelerometer passes PBIT and CBIT.                         |
| 3       | SBG_ECOM_IMU_ACCEL_Y_BIT          | Set if the Y-axis accelerometer passes PBIT and CBIT.                         |
| 4       | SBG_ECOM_IMU_ACCEL_Z_BIT          | Set if the Z-axis accelerometer passes PBIT and CBIT.                         |
| 5       | SBG_ECOM_IMU_GYRO_X_BIT           | Set if the X-axis gyroscope passes PBIT and CBIT.                             |
| 6       | SBG_ECOM_IMU_GYRO_Y_BIT           | Set if the Y-axis gyroscope passes PBIT and CBIT.                             |
| 7       | SBG_ECOM_IMU_GYRO_Z_BIT           | Set if the Z-axis gyroscope passes PBIT and CBIT.                             |
| 8       | SBG_ECOM_IMU_ACCELS_IN_RANGE      | Set if all accelerometers are operating within the specified range.           |
| 9       | SBG_ECOM_IMU_GYROS_IN_RANGE       | Set if all gyroscopes are operating within the specified range.               |
| 10      | SBG_ECOM_IMU_GYROS_USE_HIGH_SCALE | Set if the high scale scale factor is being used for rotation rates.          |

> [!NOTE]
> **PBIT (Power-On Built-In Tests):** These tests are conducted once when the IMU is powered on, ensuring that all essential systems are functioning correctly at startup.  
> **CBIT (Continuous Built-In Tests):** These tests are performed continuously during operation, monitoring the health and status of the IMU components in real-time.
## Attitude, Velocity, Position

The following logs present the output of the navigation unit, vehicle attitude (represented by Euler angles and quaternions), velocity, and position data.  
These logs share common status flags and enumerations, which are detailed in the subsequent sections.

Additionally, this section includes logs that provide compensated vehicle accelerations and rotation rates.  
Unlike raw IMU data, these measurements have been processed by the INS Extended Kalman Filter to correct for sensor biases and scale factors.

Furthermore, the effects of Earth's gravity and rotation rate are also removed, providing data that accurately reflects the vehicle's true dynamics.

### SOLUTION_STATUS {#SOLUTION_STATUS}

This status provides information about the Extended Kalman Filter (EKF), indicating which aiding data is used and the current solution mode.

| Bit   | Name                        | Description                                                                        |
| ----- | --------------------------- | ---------------------------------------------------------------------------------- |
| [0-3] | SBG_ECOM_SOLUTION_MODE      | Indicates the Kalman filter computation mode (see [SOLUTION_MODE](#SOLUTION_MODE)) |
| 4     | SBG_ECOM_SOL_ATTITUDE_VALID | Set if attitude data is reliable (Roll/Pitch error within defined criteria).       |
| 5     | SBG_ECOM_SOL_HEADING_VALID  | Set if heading data is reliable (Heading error within defined criteria).           |
| 6     | SBG_ECOM_SOL_VELOCITY_VALID | Set if velocity data is reliable (Velocity error within defined criteria).         |
| 7     | SBG_ECOM_SOL_POSITION_VALID | Set if position data is reliable (Position error within defined criteria).         |
| 8     | SBG_ECOM_SOL_VERT_REF_USED  | Set if the vertical reference is used in the solution.                             |
| 9     | SBG_ECOM_SOL_MAG_REF_USED   | Set if magnetometer is used in the solution.                                       |
| 10    | SBG_ECOM_SOL_GPS1_VEL_USED  | Set if GNSS 1 velocity is used in the solution.                                    |
| 11    | SBG_ECOM_SOL_GPS1_POS_USED  | Set if GNSS 1 position is used in the solution.                                    |
| 13    | SBG_ECOM_SOL_GPS1_HDT_USED  | Set if GNSS 1 true heading is used in the solution.                                |
| 14    | SBG_ECOM_SOL_GPS2_VEL_USED  | Set if GNSS 2 velocity is used in the solution.                                    |
| 15    | SBG_ECOM_SOL_GPS2_POS_USED  | Set if GNSS 2 position is used in the solution.                                    |
| 17    | SBG_ECOM_SOL_GPS2_HDT_USED  | Set if GNSS 2 true heading is used in the solution.                                |
| 18    | SBG_ECOM_SOL_ODO_USED       | Set if odometer velocity is used in the solution.                                  |
| 19    | SBG_ECOM_SOL_DVL_BT_USED    | Set if DVL bottom tracking velocity is used in the solution.                       |
| 20    | SBG_ECOM_SOL_DVL_WT_USED    | Set if DVL water layer velocity is used in the solution.                           |
| 21    | SBG_ECOM_SOL_VEL1_USED      | Set if generic velocity 1 is used in the solution.                                 |
| 22    | Reserved                    | Reserved for future use.                                                           |
| 23    | Reserved                    | Reserved for future use.                                                           |
| 24    | SBG_ECOM_SOL_USBL_USED      | Set if USBL position is used in the solution.                                      |
| 25    | SBG_ECOM_SOL_AIR_DATA_USED  | Set if altitude or true airspeed is used in the solution.                          |
| 26    | SBG_ECOM_SOL_ZUPT_USED      | Set if a Zero Velocity Update (ZUPT) is is used in the solution.                   |
| 27    | SBG_ECOM_SOL_ALIGN_VALID    | Set if sensor alignment and residual sensors errors have fully converged.          |
| 28    | SBG_ECOM_SOL_DEPTH_USED     | Set if depth sensor is used in the solution (subsea navigation).                   |
| 29    | SBG_ECOM_SOL_ZARU_USED      | Set if a Zero Angular Rate Update (ZARU) is used in the solution.                  |

#### SOLUTION_MODE {#SOLUTION_MODE}

| Value | Name                            | Description                                                                 |
|-------|---------------------------------|-----------------------------------------------------------------------------|
| 0     | SBG_ECOM_SOL_MODE_UNINITIALIZED | Kalman filter is not initialized; data is invalid.                          |
| 1     | SBG_ECOM_SOL_MODE_VERTICAL_GYRO | Computes roll and pitch; heading and position drift freely.                 |
| 2     | SBG_ECOM_SOL_MODE_AHRS          | Full orientation with heading reference; navigation data may drift.         |
| 3     | SBG_ECOM_SOL_MODE_NAV_VELOCITY  | Deprecated. Not in use.                                                     |
| 4     | SBG_ECOM_SOL_MODE_NAV_POSITION  | Full computation of attitude, velocity, and position.                       |

> [!NOTE]
> The *Used in Solution* flags have a 3s timeout mechanism for easier interpretation.  
> For instance, the SBG_ECOM_SOL_ODO_USED flag remains valid for 3 seconds after the odometer was last used in the solution.

### SBG_ECOM_LOG_EKF_EULER (06) {#SBG_ECOM_LOG_EKF_EULER}

This log provides the Euler angles (roll, pitch, yaw) and their associated accuracies, along with the AHRS/INS solution status.

- **Message Name (ID):** `SBG_ECOM_LOG_EKF_EULER (06)`
- **Compatibility:** AHRS/INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 40 bytes

| Field           | Description                                                   | Unit  | Format | Size | Offset |
|-----------------|---------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP      | Time since the sensor was powered up                          | µs    | uint32 | 4    | 0      |
| ROLL            | Roll angle                                                    | rad   | float  | 4    | 4      |
| PITCH           | Pitch angle                                                   | rad   | float  | 4    | 8      |
| YAW             | Yaw angle (geographic heading)                                | rad   | float  | 4    | 12     |
| ROLL_ACC        | 1σ standard deviation of roll angle accuracy                  | rad   | float  | 4    | 16     |
| PITCH_ACC       | 1σ standard deviation of pitch angle accuracy                 | rad   | float  | 4    | 20     |
| YAW_ACC         | 1σ standard deviation of yaw angle accuracy                   | rad   | float  | 4    | 24     |
| SOLUTION_STATUS | INS solution status (see [SOLUTION_STATUS](#SOLUTION_STATUS)) | -     | uint32 | 4    | 28     |
| MAG_DECL        | Local magnetic declination, positive East.                    | rad   | float  | 4    | 32     |
| MAG_INCL        | Local magnetic inclination, positive Down.                    | rad   | float  | 4    | 36     |

### SBG_ECOM_LOG_EKF_QUAT (07) {#SBG_ECOM_LOG_EKF_QUAT}

This log provides the INS orientation using quaternions which avoid gimbal lock and are valid for all orientations.  
It also includes the standard deviation accuracies for roll, pitch, and yaw angles, along with the AHRS/INS solution status.

- **Message Name (ID):** `SBG_ECOM_LOG_EKF_QUAT (07)`
- **Compatibility:** AHRS/INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 44 bytes

| Field           | Description                                                   | Unit  | Format | Size | Offset |
|-----------------|---------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP      | Time since the sensor was powered up                          | µs    | uint32 | 4    | 0      |
| Q0              | First quaternion parameter (W component)                      | -     | float  | 4    | 4      |
| Q1              | Second  quaternion parameter (X component)                    | -     | float  | 4    | 8      |
| Q2              | Third quaternion parameter (Y component)                      | -     | float  | 4    | 12     |
| Q3              | Fourth quaternion parameter (Z component)                     | -     | float  | 4    | 16     |
| ROLL_ACC        | 1σ standard deviation of roll angle accuracy                  | rad   | float  | 4    | 20     |
| PITCH_ACC       | 1σ standard deviation of pitch angle accuracy                 | rad   | float  | 4    | 24     |
| YAW_ACC         | 1σ standard deviation of yaw angle accuracy                   | rad   | float  | 4    | 28     |
| SOLUTION_STATUS | INS solution status (see [SOLUTION_STATUS](#SOLUTION_STATUS)) | -     | uint32 | 4    | 32     |
| MAG_DECL        | Local magnetic declination, positive East.                    | rad   | float  | 4    | 36     |
| MAG_INCL        | Local magnetic inclination, positive Down.                    | rad   | float  | 4    | 40     |

### SBG_ECOM_LOG_EKF_NAV (08) {#SBG_ECOM_LOG_EKF_NAV}

This log provides the INS velocity in the North-East-Down (NED) coordinate system and position in Latitude, Longitude, and Altitude.  
It also includes accuracy parameters for each measurement along with the INS solution status.

- **Message Name (ID):** `SBG_ECOM_LOG_EKF_NAV (08)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 72 bytes

| Field           | Description                                                   | Unit  | Format | Size | Offset |
|-----------------|---------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP      | Time since the sensor was powered up                          | µs    | uint32 | 4    | 0      |
| VELOCITY_N      | Velocity in the North direction                               | m/s   | float  | 4    | 4      |
| VELOCITY_E      | Velocity in the East direction                                | m/s   | float  | 4    | 8      |
| VELOCITY_D      | Velocity in the Down direction                                | m/s   | float  | 4    | 12     |
| VELOCITY_N_ACC  | 1σ standard deviation of velocity accuracy in North direction | m/s   | float  | 4    | 16     |
| VELOCITY_E_ACC  | 1σ standard deviation of velocity accuracy in East direction  | m/s   | float  | 4    | 20     |
| VELOCITY_D_ACC  | 1σ standard deviation of velocity accuracy in Down direction  | m/s   | float  | 4    | 24     |
| LATITUDE        | Geographic latitude, positive North.                          | °     | double | 8    | 28     |
| LONGITUDE       | Geographic longitude, positive East.                          | °     | double | 8    | 36     |
| ALTITUDE        | Altitude above Mean Sea Level (MSL), positive upward.         | m     | double | 8    | 44     |
| UNDULATION      | Difference between geoid and ellipsoid altitude               | m     | float  | 4    | 52     |
| LATITUDE_ACC    | 1σ standard deviation of latitude accuracy                    | m     | float  | 4    | 56     |
| LONGITUDE_ACC   | 1σ standard deviation of longitude accuracy                   | m     | float  | 4    | 60     |
| ALTITUDE_ACC    | 1σ standard deviation of vertical position accuracy           | m     | float  | 4    | 64     |
| SOLUTION_STATUS | INS solution status (see [SOLUTION_STATUS](#SOLUTION_STATUS)) | -     | uint32 | 4    | 68     |

> [!NOTE]
> The undulation can be used to compute the height above ellipsoid from the MSL altitude.  
> Please use the following formula: *height = altitude + undulation*

### SBG_ECOM_LOG_EKF_VEL_BODY (54) {#SBG_ECOM_LOG_EKF_VEL_BODY}

This log provides the INS velocity expressed in the body/vehicle frame.

When the INS is correctly configured, the body frame aligns with the vehicle frame.  
For instance, in a car, the X-axis points forward, and the Y-axis points to the right.

- **Message Name (ID):** `SBG_ECOM_LOG_EKF_VEL_BODY (54)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-4.0-blue)
- **Payload Size:** 32 bytes

| Field           | Description                                                        | Unit  | Format | Size | Offset |
|-----------------|--------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP      | Time since the sensor was powered up                               | µs    | uint32 | 4    | 0      |
| SOLUTION_STATUS | INS solution status (see [SOLUTION_STATUS](#SOLUTION_STATUS))      | -     | uint32 | 4    | 4      |
| VELOCITY_X      | Velocity along the X-axis (forward direction in vehicle frame)     | m/s   | float  | 4    | 8      |
| VELOCITY_Y      | Velocity along the Y-axis (right direction in vehicle frame)       | m/s   | float  | 4    | 12     |
| VELOCITY_Z      | Velocity along the Z-axis (downward direction in vehicle frame)    | m/s   | float  | 4    | 16     |
| VELOCITY_X_ACC  | 1σ standard deviation of velocity accuracy along the X-axis        | m/s   | float  | 4    | 20     |
| VELOCITY_Y_ACC  | 1σ standard deviation of velocity accuracy along the Y-axis        | m/s   | float  | 4    | 24     |
| VELOCITY_Z_ACC  | 1σ standard deviation of velocity accuracy along the Z-axis        | m/s   | float  | 4    | 28     |

### SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY (52) {#SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY}

This log provides the corrected rotation rates and accelerations in the INS body/vehicle frame.  
The data is compensated by the Navigation Filter for sensor bias, scale factor, earth rotation, and gravity.

- **Message Name (ID):** `SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY (52)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-4.0-blue)
- **Payload Size:** 32 bytes

| Field           | Description                                                        | Unit  | Format | Size | Offset |
|-----------------|--------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP      | Time since the sensor was powered up                               | µs    | uint32 | 4    | 0      |
| SOLUTION_STATUS | INS solution status (see [SOLUTION_STATUS](#SOLUTION_STATUS))      | -     | uint32 | 4    | 4      |
| RATE_X          | Rotation rate around the X axis                                    | rad/s | float  | 4    | 8      |
| RATE_Y          | Rotation rate around the Y axis                                    | rad/s | float  | 4    | 12     |
| RATE_Z          | Rotation rate around the Z axis                                    | rad/s | float  | 4    | 16     |
| ACCELERATION_X  | Acceleration along the X axis                                      | m/s²  | float  | 4    | 20     |
| ACCELERATION_Y  | Acceleration along the Y axis                                      | m/s²  | float  | 4    | 24     |
| ACCELERATION_Z  | Acceleration along the Z axis                                      | m/s²  | float  | 4    | 28     |

> [!NOTE]
> This log is properly decimated based on the selected output rate to prevent aliasing issues.

### SBG_ECOM_LOG_EKF_ROT_ACCEL_NED (53) {#SBG_ECOM_LOG_EKF_ROT_ACCEL_NED}

This log provides the corrected rotation rates and accelerations in the North, East, Down (NED) navigation frame.  
The data is compensated by the Navigation Filter for sensor bias, scale factor, earth rotation, and gravity. 

This log is particularly useful for determining a vehicle's turn rate, such as the down rotation rate.

- **Message Name (ID):** `SBG_ECOM_LOG_EKF_ROT_ACCEL_NED (53)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-4.0-blue)
- **Payload Size:** 32 bytes

| Field           | Description                                                        | Unit  | Format | Size | Offset |
|-----------------|--------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP      | Time since the sensor was powered up                               | µs    | uint32 | 4    | 0      |
| SOLUTION_STATUS | INS solution status (see [SOLUTION_STATUS](#SOLUTION_STATUS))      | -     | uint32 | 4    | 4      |
| RATE_N          | Rotation rate around the North axis in the navigation frame        | rad/s | float  | 4    | 8      |
| RATE_E          | Rotation rate around the East axis in the navigation frame         | rad/s | float  | 4    | 12     |
| RATE_D          | Rotation rate around the Down axis in the navigation frame         | rad/s | float  | 4    | 16     |
| ACCELERATION_N  | Acceleration along the North axis in the navigation frame          | m/s²  | float  | 4    | 20     |
| ACCELERATION_E  | Acceleration along the East axis in the navigation frame           | m/s²  | float  | 4    | 24     |
| ACCELERATION_D  | Acceleration along the Down axis in the navigation frame           | m/s²  | float  | 4    | 28     |

> [!NOTE]
> This log is properly decimated based on the selected output rate to prevent aliasing issues.

> [!WARNING]
> Rotation rates around the North and East navigation axes are not commonly used.  
> In most cases, rotation rates in the body X and Y axes are preferable.
## Surge, Sway, Heave

This section covers output measurements specific to marine applications, including Surge, Sway, and Heave.

High-performance INS units with marine options support both real-time and delayed heave measurements.  
The ELLIPSE Series, however, offers real-time heave measurements only, without surge or sway channels.

Real-time heave delivers immediate data, whereas delayed heave offers higher accuracy with a 150-second time delay.

Real-time and delayed ship motion outputs share common conventions, message structures, status flags, and enumerations, as detailed below.

### Output frame and monitoring point

Ship motion measurements are defined in a vessel-specific coordinate frame:
- **Heave**: Vertical displacement, (positive downward)
- **Surge**: Longitudinal displacement (positive toward the bow/forward).
- **Sway**: Transverse displacement (positive toward the starboard side/right).

The heave measurement and vertical velocity (`HEAVE` and `VEL_Z` fields) are affected by the selected output monitoring point and will be re-located accordingly.  
However, surge/sway, horizontal velocities and accelerations values are **only valid** when output at the IMU physical measurement point (Bare IMU).

### Ship Motion Status {#SHIP_MOTION_STATUS}

The status field indicates the validity and availability of the ship motion output fields.  
It is crucial to check this status to determine which fields are active and if the data is valid.

| Bit | Name                                    | Description                                                                                                     |
|-----|-----------------------------------------|-----------------------------------------------------------------------------------------------------------------|
| 0   | SBG_ECOM_SHIP_MOTION_HEAVE_VALID        | Set if heave filter has stabilized (see [SBG_ECOM_SHIP_MOTION_HEAVE_VALID](#SBG_ECOM_SHIP_MOTION_HEAVE_VALID)). |
| 1   | SBG_ECOM_SHIP_MOTION_VEL_AIDED          | Set if the heave output is compensated for transient accelerations.                                             |
| 2   | SBG_ECOM_SHIP_MOTION_SURGE_SWAY_VALID   | Set if `SURGE`, `SWAY`, `VEL_X` and `VEL_Y` components are valid and reported in this output.                   |
| 4   | SBG_ECOM_SHIP_MOTION_HEAVE_PERIOD_VALID | Set if `HEAVE_PERIOD`, the main wave or swell period is available and valid.                                    |
| 5   | SBG_ECOM_SHIP_MOTION_SWELL_MODE         | Set if the heave filter is operating in swell computation mode.                                                 |
| 6   | SBG_ECOM_SHIP_MOTION_ACCEL_VALID        | Set if `ACCEL_X`, `ACCEL_Y`, and `ACCEL_Z` accelerations are valid and available.                               |

#### SBG_ECOM_SHIP_MOTION_HEAVE_VALID flag {#SBG_ECOM_SHIP_MOTION_HEAVE_VALID}

The `SBG_ECOM_SHIP_MOTION_HEAVE_VALID` flag indicates whether the heave measurement is valid and stable.  
When this flag is set, the `HEAVE` and `VEL_Z` outputs contain reliable data.

The heave estimation relies on a high-pass filter, which requires a convergence period and has a settling time.  
As with any complex filter, certain events can destabilize it and require re-convergence.

This flag is **cleared** under the following conditions:
  - At startup, or before the filter has converged
  - Following a vessel turn without velocity aiding
  - When heave measurements reach extreme or invalid values
  - After a sudden step change that requires the filter to re-converge
  - In the event of an internal system error

### SBG_ECOM_LOG_SHIP_MOTION (09) {#SBG_ECOM_LOG_SHIP_MOTION}

This log provides real-time ship motion data, including main heave period, surge, sway, heave, velocity, and accelerations.

- **Message Name (ID):** `SBG_ECOM_LOG_SHIP_MOTION (09)`
- **Compatibility:** AHRS/INS with marine/hydro options
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 46 bytes

| Field         | Description                                                               | Unit  | Format | Size | Offset |
|---------------|---------------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP    | Time since the sensor was powered up                                      | µs    | uint32 | 4    | 0      |
| HEAVE_PERIOD  | Main heave period                                                         | s     | float  | 4    | 4      |
| SURGE         | Surge longitudinal displacement (positive forward)                        | m     | float  | 4    | 8      |
| SWAY          | Sway lateral displacement (positive right)                                | m     | float  | 4    | 12     |
| HEAVE         | Heave vertical displacement (positive downward)                           | m     | float  | 4    | 16     |
| ACCEL_X       | Longitudinal acceleration (positive forward)                              | m/s²  | float  | 4    | 20     |
| ACCEL_Y       | Lateral acceleration (positive right)                                     | m/s²  | float  | 4    | 24     |
| ACCEL_Z       | Vertical acceleration (positive downward)                                 | m/s²  | float  | 4    | 28     |
| VEL_X         | Longitudinal velocity (positive forward)                                  | m/s   | float  | 4    | 32     |
| VEL_Y         | Lateral velocity (positive right)                                         | m/s   | float  | 4    | 36     |
| VEL_Z         | Vertical velocity (positive downward)                                     | m/s   | float  | 4    | 40     |
| STATUS        | Ship motion output status (see [SHIP_MOTION_STATUS](#SHIP_MOTION_STATUS)) | -     | uint16 | 2    | 44     |

> [!NOTE]
> ELLIPSE series only computes and outputs HEAVE_PERIOD, HEAVE, ACCEL_Z and VEL_Z values.  
> All other fields are set to zero and reported as invalid in the `STATUS` bitmask.

> [!WARNING]
> Surge, sway, X/Y velocities, and acceleration values are only valid when measured at the IMU's physical location (Bare IMU).

### SBG_ECOM_LOG_SHIP_MOTION_HP (32) {#SBG_ECOM_LOG_SHIP_MOTION_HP}

This log provides delayed heave measurements, offering higher accuracy compared to real-time at the cost of approximately 150 seconds of delayed output.

The timestamp reported in the message reflects the actual time of data validity.  
The data corresponds to conditions roughly 150 seconds earlier than other logs transmitted at the same time.

- **Message Name (ID):** `SBG_ECOM_LOG_SHIP_MOTION_HP (32)`
- **Compatibility:** High Performance INS (marine option)
- **Firmware:** ![HPINS](https://img.shields.io/badge/HPINS-2.0-blue)
- **Payload Size:** 46 bytes

| Field         | Description                                                               | Unit  | Format | Size | Offset |
|---------------|---------------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP    | Time since the sensor was powered up                                      | µs    | uint32 | 4    | 0      |
| HEAVE_PERIOD  | Reserved for future use. Always reported as 0                             | s     | float  | 4    | 4      |
| SURGE         | Reserved for future use. Always reported as 0                             | m     | float  | 4    | 8      |
| SWAY          | Reserved for future use. Always reported as 0                             | m     | float  | 4    | 12     |
| HEAVE         | Heave vertical displacement (positive downward)                           | m     | float  | 4    | 16     |
| ACCEL_X       | Longitudinal acceleration (positive forward)                              | m/s²  | float  | 4    | 20     |
| ACCEL_Y       | Lateral acceleration (positive right)                                     | m/s²  | float  | 4    | 24     |
| ACCEL_Z       | Vertical acceleration (positive downward)                                 | m/s²  | float  | 4    | 28     |
| VEL_X         | Reserved for future use. Always reported as 0                             | m/s   | float  | 4    | 32     |
| VEL_Y         | Reserved for future use. Always reported as 0                             | m/s   | float  | 4    | 36     |
| VEL_Z         | Vertical velocity (positive downward)                                     | m/s   | float  | 4    | 40     |
| STATUS        | Ship motion output status (see [SHIP_MOTION_STATUS](#SHIP_MOTION_STATUS)) | -     | uint16 | 2    | 44     |

> [!NOTE]
> The `SBG_ECOM_LOG_SHIP_MOTION_HP` message only computes and outputs delayed heave, vertical velocity and acceleration.  
> All other fields are reported as 0 and the `STATUS` bitmask is set accordingly.

> [!WARNING]
> Acceleration values are only valid when measured at the IMU's physical location (Bare IMU).
## GNSS aiding (PVT, HDT)

The following logs provide the navigation unit's output, including Euler angles, quaternions, velocity and position.  
These logs share common status flags and enumerations, detailed below.

### GNSS Velocity {#SBG_ECOM_LOG_GPSX_VEL}

This section details the velocity and course information from the primary or secondary GNSS receiver.  
The timestamp reflects the actual time of the GNSS velocity data, independent of the main processing loop.

- **Message Name (ID):** `SBG_ECOM_LOG_GPS1_VEL (13)`, `SBG_ECOM_LOG_GPS2_VEL (16)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 44 bytes

| Field           | Description                                                                         | Unit  | Format  | Size | Offset |
|-----------------|-------------------------------------------------------------------------------------|-------|---------|------|--------|
| TIME_STAMP      | Time since sensor was powered up                                                    | µs    | uint32  | 4    | 0      |
| STATUS_TYPE     | Velocity solution type and status (see [GPS_VEL_STATUS_TYPE](#GPS_VEL_STATUS_TYPE)) | -     | uint32  | 4    | 4      |
| TOW             | GPS Time of Week                                                                    | ms    | uint32  | 4    | 8      |
| VEL_N           | Velocity in North direction                                                         | m/s   | float   | 4    | 12     |
| VEL_E           | Velocity in East direction                                                          | m/s   | float   | 4    | 16     |
| VEL_D           | Velocity in Down direction                                                          | m/s   | float   | 4    | 20     |
| VEL_ACC_N       | 1σ Accuracy in North direction                                                      | m/s   | float   | 4    | 24     |
| VEL_ACC_E       | 1σ Accuracy in East direction                                                       | m/s   | float   | 4    | 28     |
| VEL_ACC_D       | 1σ Accuracy in Down direction                                                       | m/s   | float   | 4    | 32     |
| COURSE          | True direction of motion over ground                                                | °     | float   | 4    | 36     |
| COURSE_ACC      | 1σ course accuracy                                                                  | °     | float   | 4    | 40     |

#### Status and Solution Type Field {#GPS_VEL_STATUS_TYPE}

The `STATUS_TYPE` field provides both the GNSS velocity solution status and the type of computed velocity, encoded as two separate enumerations.

| Bits   | Type  | Name                     | Description                                                      |
|--------|-------|--------------------------|------------------------------------------------------------------|
| [0-5]  | Enum  | SBG_ECOM_GPS_VEL_STATUS  | The GPS velocity status (see [GPS_VEL_STATUS](#GPS_VEL_STATUS)). |
| [6-11] | Enum  | SBG_ECOM_GPS_VEL_TYPE    | The GPS velocity type (see [GPS_VEL_STATUS](#GPS_VEL_STATUS)).   |

##### GNSS Velocity Status Enumeration {#GPS_VEL_STATUS}

Indicates the general status of the GNSS regarding velocity computation.

| Value | Name                         | Description                                  |
|-------|------------------------------|----------------------------------------------|
| 0     | SBG_ECOM_VEL_SOL_COMPUTED    | A valid solution has been computed.          |
| 1     | SBG_ECOM_VEL_INSUFFICIENT_OBS| Not enough valid SVs to compute a solution.  |
| 2     | SBG_ECOM_VEL_INTERNAL_ERROR  | An internal error has occurred.              |
| 3     | SBG_ECOM_VEL_LIMIT           | Velocity limit exceeded.                     |

##### GNSS Velocity Type Enumeration {#GPS_VEL_TYPE}

Specifies the type of the computed velocity solution.

| Value | Name                       | Description                                                                                                |
|-------|----------------------------|------------------------------------------------------------------------------------------------------------|
| 0     | SBG_ECOM_VEL_NO_SOLUTION   | No valid velocity solution available.                                                                      |
| 1     | SBG_ECOM_VEL_UNKNOWN_TYPE  | An unknown solution type has been computed.                                                                |
| 2     | SBG_ECOM_VEL_DOPPLER       | A Doppler velocity has been computed.                                                                      |
| 3     | SBG_ECOM_VEL_DIFFERENTIAL  | A velocity has been determined by calculating the difference between two sequential position measurements. |

> [!NOTE]
> Ensure that both the GNSS velocity solution status and type are checked to validate the output velocity data.

### GNSS Position {#SBG_ECOM_LOG_GPSX_POS}

This section provides position information from the primary or secondary GNSS receiver.  
The timestamp indicates the actual GPS position data time, not synchronized with the main processing loop.

- **Message Name (ID):** `SBG_ECOM_LOG_GPS1_POS (14)`, `SBG_ECOM_LOG_GPS2_POS (17)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 62 bytes

| Field           | Description                                                                                | Unit  | Format  | Size | Offset |
|-----------------|--------------------------------------------------------------------------------------------|-------|---------|------|--------|
| TIME_STAMP      | Time since the sensor was powered up                                                       | µs    | uint32  | 4    | 0      |
| STATUS_TYPE     | Position solution type and status (see [GPS_POS_STATUS_TYPE](#GPS_POS_STATUS_TYPE))        | -     | uint32  | 4    | 4      |
| TOW             | GNSS Time of Week                                                                          | ms    | uint32  | 4    | 8      |
| LATITUDE        | Latitude, positive North                                                                   | °     | double  | 8    | 12     |
| LONGITUDE       | Longitude, positive East                                                                   | °     | double  | 8    | 20     |
| ALTITUDE        | Altitude Above Mean Sea Level                                                              | m     | double  | 8    | 28     |
| UNDULATION      | Altitude difference between the geoid and the Ellipsoid                                    | m     | float   | 4    | 36     |
| LAT_ACC         | 1σ Latitude Accuracy                                                                       | m     | float   | 4    | 40     |
| LONG_ACC        | 1σ Longitude Accuracy                                                                      | m     | float   | 4    | 44     |
| ALTI_ACC        | 1σ Altitude Accuracy                                                                       | m     | float   | 4    | 48     |
| NUM_SV_USED     | Number of space vehicles used in GNSS solution - 0xFF if N/A (since v1.4)                  | -     | uint8   | 1    | 52     |
| BASE_STATION_ID | ID of the DGPS/RTK base station in use - 0xFFFF if N/A (since v1.4)                        | -     | uint16  | 2    | 53     |
| DIFF_AGE        | Differential data age - 0xFFFF if N/A (since v1.4)                                         | 0.01s | uint16  | 2    | 55     |
| NUM_SV_TRACKED  | Number of tracked SV - 0xFF if N/A (since v4.0)                                            | -     | uint8   | 1    | 57     |
| STATUS_EXT      | Interference/spoofing status (see [GPS_POS_STATUS_EXT](#GPS_POS_STATUS_EXT)). (since v4.0) | -     | uint32  | 4    | 58     |

#### Status and Solution Type Field {#GPS_POS_STATUS_TYPE}

The `STATUS_TYPE` field provides both the GNSS position solution status and the type of computed position, encoded as two separate enumerations.  
It also includes bitmasks to reports which signals are being used to compute the solution.

| Bits   | Type  | Name                            | Description                                                         |
|--------|-------|---------------------------------|---------------------------------------------------------------------|
| [0-5]  | Enum  | SBG_ECOM_GPS_POS_STATUS         | GNSS position status (see [GPS_POS_STATUS](#GPS_POS_STATUS))        |
| [6-11] | Enum  | SBG_ECOM_GPS_POS_TYPE           | GNSS position type (see [GPS_POS_TYPE](#GPS_POS_TYPE))              |
| 12     | Mask  | SBG_ECOM_GPS_POS_GPS_L1_USED    | Set if GPS L1CA/L1P is used in the solution                         |
| 13     | Mask  | SBG_ECOM_GPS_POS_GPS_L2_USED    | Set if GPS L2P/L2C is used in the solution                          |
| 14     | Mask  | SBG_ECOM_GPS_POS_GPS_L5_USED    | Set if GPS L5 is used in the solution                               |
| 15     | Mask  | SBG_ECOM_GPS_POS_GLO_L1_USED    | Set if GLONASS L1CA is used in the solution                         |
| 16     | Mask  | SBG_ECOM_GPS_POS_GLO_L2_USED    | Set if GLONASS L2C/L2P is used in the solution                      |
| 17     | Mask  | SBG_ECOM_GPS_POS_GLO_L3_USED    | Set if GLONASS L3 is used in the solution                           |
| 18     | Mask  | SBG_ECOM_GPS_POS_GAL_E1_USED    | Set if Galileo E1 is used in the solution                           |
| 19     | Mask  | SBG_ECOM_GPS_POS_GAL_E5A_USED   | Set if Galileo E5a is used in the solution                          |
| 20     | Mask  | SBG_ECOM_GPS_POS_GAL_E5B_USED   | Set if Galileo E5b is used in the solution                          |
| 21     | Mask  | SBG_ECOM_GPS_POS_GAL_E5ALT_USED | Set if Galileo E5 AltBoc is used in the solution                    |
| 22     | Mask  | SBG_ECOM_GPS_POS_GAL_E6_USED    | Set if Galileo E6 is used in the solution                           |
| 23     | Mask  | SBG_ECOM_GPS_POS_BDS_B1_USED    | Set if BeiDou B1 is used in the solution                            |
| 24     | Mask  | SBG_ECOM_GPS_POS_BDS_B2_USED    | Set if BeiDou B2 is used in the solution                            |
| 25     | Mask  | SBG_ECOM_GPS_POS_BDS_B3_USED    | Set if BeiDou B3 is used in the solution                            |
| 26     | Mask  | SBG_ECOM_GPS_POS_QZSS_L1_USED   | Set if QZSS L1CA is used in the solution                            |
| 27     | Mask  | SBG_ECOM_GPS_POS_QZSS_L2_USED   | Set if QZSS L2C is used in the solution                             |
| 28     | Mask  | SBG_ECOM_GPS_POS_QZSS_L5_USED   | Set if QZSS L5 is used in the solution                              |
| 29     | Mask  | SBG_ECOM_GPS_POS_QZSS_L6_USED   | Set if QZSS L6 is used in the solution                              |

##### GNSS Position Status Enumeration {#GPS_POS_STATUS}

Specifies the type of the computed velocity solution.

| Value | Name                           | Description                                    |
|-------|--------------------------------|------------------------------------------------|
| 0     | SBG_ECOM_POS_SOL_COMPUTED      | A valid solution has been computed             |
| 1     | SBG_ECOM_POS_INSUFFICIENT_OBS  | Not enough valid SV to compute a solution      |
| 2     | SBG_ECOM_POS_INTERNAL_ERROR    | An internal error has occurred                 |
| 3     | SBG_ECOM_POS_HEIGHT_LIMIT      | The height limit has been exceeded             |

##### GNSS Position Type Enumeration {#GPS_POS_TYPE}

Specifies the type of the computed velocity solution.

| Value | Name                       | Description                                        |
|-------|----------------------------|----------------------------------------------------|
| 0     | SBG_ECOM_POS_NO_SOLUTION   | No valid solution available                        |
| 1     | SBG_ECOM_POS_UNKNOWN_TYPE  | An unknown solution type has been computed         |
| 2     | SBG_ECOM_POS_SINGLE        | Single point solution position                     |
| 3     | SBG_ECOM_POS_PSRDIFF       | Standard Pseudorange Differential Solution (DGPS)  |
| 4     | SBG_ECOM_POS_SBAS          | SBAS satellite used for differential corrections   |
| 5     | SBG_ECOM_POS_OMNISTAR      | Omnistar VBS Position (L1 sub-meter)               |
| 6     | SBG_ECOM_POS_RTK_FLOAT     | Floating RTK ambiguity solution (20 cm RTK)        |
| 7     | SBG_ECOM_POS_RTK_INT       | Integer RTK ambiguity solution (2 cm RTK)          |
| 8     | SBG_ECOM_POS_PPP_FLOAT     | Precise Point Positioning with float ambiguities   |
| 9     | SBG_ECOM_POS_PPP_INT       | Precise Point Positioning with fixed ambiguities   |
| 10    | SBG_ECOM_POS_FIXED         | Fixed location solution position                   |

> [!NOTE]
> Ensure that both the GNSS position solution status and type are checked to validate the output position data.

##### STATUS_EXT Definition {#GPS_POS_STATUS_EXT}

The `STATUS_EXT` field provides additional status details, including interference monitoring and spoofing detection.
The field encodes several enumerations as described below:

| Bits   | Type  | Name                          | Description                                                                                  |
|--------|-------|-------------------------------|----------------------------------------------------------------------------------------------|
| [0-3]  | Enum  | SBG_ECOM_GNSS_IFM_STATUS      | GNSS interference monitoring and mitigation (see [GNSS_IFM_STATUS](#GNSS_IFM_STATUS)).       |
| [4-7]  | Enum  | SBG_ECOM_GNSS_SPOOFING_STATUS | GNSS spoofing monitoring and mitigation (see [GNSS_SPOOFING_STATUS](#GNSS_SPOOFING_STATUS)). |
| [8-11] | Enum  | SBG_ECOM_GNSS_OSNMA_STATUS    | Galileo OSNMA authentication status (see [GNSS_OSNMA_STATUS](#GNSS_OSNMA_STATUS)).           |

##### Interference Monitoring Status Enumeration {#GNSS_IFM_STATUS}

| Value | Name                           | Description                                                             |
|-------|--------------------------------|-------------------------------------------------------------------------|
| 0     | IFM_STATUS_ERROR               | The interference monitoring system is in error and not operational      |
| 1     | IFM_STATUS_UNKNOWN             | Interference monitoring is disabled or not available                    |
| 2     | IFM_STATUS_CLEAN               | Interference monitored and the environment is clean                     |
| 3     | IFM_STATUS_MITIGATED           | Interference detected and mitigated, PVT is valid                       |
| 4     | IFM_STATUS_CRITICAL            | Interference detected and not mitigated, PVT is invalid                 |

##### Spoofing Status Enumeration {#GNSS_SPOOFING_STATUS}

| Value | Name                           | Description                                                             |
|-------|--------------------------------|-------------------------------------------------------------------------|
| 0     | SPOOFING_STATUS_ERROR          | Spoofing detection system error, not operational                        |
| 1     | SPOOFING_STATUS_UNKNOWN        | Spoofing detection disabled or not available                            |
| 2     | SPOOFING_STATUS_CLEAN          | Spoofing detection enabled, no spoofing detected                        |
| 3     | SPOOFING_STATUS_SINGLE         | Probable spoofing detected by one method                                |
| 4     | SPOOFING_STATUS_MULTIPLE       | Confirmed spoofing detected by multiple methods (Galileo OSNMA, RAIM).  |

##### OSNMA Status Enumeration {#GNSS_OSNMA_STATUS}

| Value | Name                           | Description                                                             |
|-------|--------------------------------|-------------------------------------------------------------------------|
| 0     | OSNMA_STATUS_ERROR             | OSNMA is in error state and doesn't work.                               |
| 1     | OSNMA_STATUS_DISABLED          | OSNMA is either disabled or not available.                              |
| 2     | OSNMA_STATUS_INITIALIZING      | OSNMA initialization in progress.                                       |
| 3     | OSNMA_STATUS_WAITING_NTP       | Awaiting valid time from NTP server for strict mode operation.          |
| 4     | OSNMA_STATUS_VALID             | Actively authenticating Galileo signals, no spoofing.                   |
| 5     | OSNMA_STATUS_SPOOFED           | Spoofing detected on Galileo signals.                                   |

### GNSS True Heading {#SBG_ECOM_LOG_GPSX_HDT}

Provides true heading data from the primary or secondary dual antenna GNSS receiver.  
The timestamp represents the actual time of the GPS true heading data.

- **Message Name (ID):** `SBG_ECOM_LOG_GPS1_HDT (15)`, `SBG_ECOM_LOG_GPS2_HDT (18)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 32 bytes

| Field            | Description                                                                  | Unit  | Format  | Size | Offset |
|------------------|------------------------------------------------------------------------------|-------|---------|------|--------|
| TIME_STAMP       | Time since the sensor was powered up                                         | µs    | uint32  | 4    | 0      |
| STATUS           | GNSS True Heading status (see [GPS_HDT_STATUS_BIT](#GPS_HDT_STATUS_BIT))     | -     | uint16  | 2    | 4      |
| TOW              | GPS Time of Week                                                             | ms    | uint32  | 4    | 6      |
| TRUE_HEADING     | True heading angle (0 to 360°)                                               | °     | float   | 4    | 10     |
| TRUE_HEADING_ACC | 1σ True heading estimated accuracy (0 to 360°)                               | °     | float   | 4    | 14     |
| PITCH            | Pitch angle from the master to the rover (-90 to +90°)                       | °     | float   | 4    | 18     |
| PITCH_ACC        | 1σ pitch estimated accuracy (0 to 90°)                                       | °     | float   | 4    | 22     |
| BASELINE         | Distance between main and auxiliary antenna (since v1.11)                    | m     | float   | 4    | 26     |
| NUM_SV_TRACKED   | Number of space vehicles tracked for true heading - 0xFF if N/A (since v4.0) | -     | uint8   | 1    | 30     |
| NUM_SV_USED      | Number of SV used in true heading solution - 0xFF if N/A (since v4.0)        | -     | uint8   | 1    | 31     |

#### Status and Validity {#GPS_HDT_STATUS_BIT}

The `STATUS` field indicates the status of the GNSS true heading solution along with additional validity information.

| Bits  | Type  | Name                             | Description                                                      |
|-------|-------|----------------------------------|------------------------------------------------------------------|
| [0-5] | Enum  | SBG_ECOM_GPS_HDT_STATUS          | GNSS true heading status (see [GPS_HDT_STATUS](#GPS_HDT_STATUS)) |
|   6   | Mask  | SBG_ECOM_GPS_HDT_BASELINE_VALID  | Set if the baseline length field is valid                        |

##### GNSS True Heading Status Enumeration {#GPS_HDT_STATUS}

| Value | Name                         | Description                                        |
|-------|------------------------------|----------------------------------------------------|
| 0     | SBG_ECOM_HDT_SOL_COMPUTED    | A valid solution has been computed                 |
| 1     | SBG_ECOM_HDT_INSUFFICIENT_OBS| Not enough valid SV to compute a solution          |
| 2     | SBG_ECOM_HDT_INTERNAL_ERROR  | An internal error has occurred                     |
| 3     | SBG_ECOM_HDT_HEIGHT_LIMIT    | The height limit has been exceeded                 |

### GNSS Raw Data {#SBG_ECOM_LOG_GPSX_RAW}

This log is used to store raw GPS data for post-processing purposes.  
It directly stores untouched binary messages from the GNSS receiver, relevant for post-processing.

Each message can contain up to 4086 bytes of raw GNSS data to fit within a standard sbgECom protocol frame.

- **Message Name (ID):** `SBG_ECOM_LOG_GPS1_RAW (31)`, `SBG_ECOM_LOG_GPS2_RAW (38)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.1-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.1-blue)
- **Payload Size:** Variable (up to 4086)

| Field       | Description                                              | Unit | Format | Size       | Offset |
|-------------|----------------------------------------------------------|------|--------|------------|--------|
| RAW_BUFFER  | Buffer storing GNSS raw data as returned by the receiver | -    | void   | [0-4086]   | 0      |

### GNSS Space Vehicles Information {#SBG_ECOM_LOG_GPSX_SAT}

List of all space vehicles in view by the primary or secondary GNSS receiver antenna.  
This message is typically sent every second with the latest information from the GNSS receiver.

It includes details on the constellation, identifier/PRN, elevation, azimuth, and health status of each satellite, along with tracked signals.

- **Message Name (ID):** `SBG_ECOM_LOG_GPS1_SAT (50)`, `SBG_ECOM_LOG_GPS2_SAT (51)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-4.1-blue)
- **Payload Size:** Variable (up to 4086)

| Field        | Description                                                     | Unit  | Format | Size       | Offset |
|--------------|-----------------------------------------------------------------|-------|--------|------------|--------|
| timeStamp    | Time since sensor is powered up                                 | µs    | uint32 | 4          | 0      |
| reserved     | Reserved, do not use                                            | -     | uint32 | 4          | 4      |
| nrSatellites | Number of reported satellites 0 to 64 (see [SatData](#SatData)) | -     | uint8  | 1          | 8      |

#### Repeated Group: SatData (nrSatellites times) {#SatData}

This block provides information for a satellite such as elevation, satellite id etc.  
It is repeated `nrSatellites` times.

| Field        | Description                                                       | Unit | Format | Size   | Offset |
|--------------|-------------------------------------------------------------------|------|--------|--------|--------|
| satelliteId  | RINEX Satellite number (see [GNSS_SAT_ID](#GNSS_SAT_ID))          | -    | uint8  | 1      | -      |
| elevation    | Satellite elevation (-90 to +90°)                                 | °    | int8   | 1      | -      |
| azimuth      | Satellite azimuth (0 to 359°)                                     | °    | uint16 | 2      | -      |
| satFlags     | Satellite status flags (see [satFlags](#satFlags))                | -    | uint16 | 2      | -      |
| nrSignals    | Number of reported signals 0 to 8 (see [SignalData](#SignalData)) | -    | uint8  | 1      | -      |

##### satFlags Definition {#satFlags}

| Bits   | Type  | Name               | Description                                                              |
|--------|-------|--------------------|--------------------------------------------------------------------------|
| [0-2]  | Enum  | Tracking status    | Tracking Status (see [SAT_TRACKING_STATUS](#SAT_TRACKING_STATUS)).       |
| [3-4]  | Enum  | Health status      | Health Status (see [SAT_HEALTH_STATUS](#SAT_HEALTH_STATUS)).             |
| [5-6]  | Enum  | Elevation status   | Elevation Status (see [SAT_ELEVATION_STATUS](#SAT_ELEVATION_STATUS)).    |
| [7-10] | Enum  | Constellation      | Satellite constellation (see [GNSS_CONSTELLATION](#GNSS_CONSTELLATION)). |

#### Repeated Group: SignalData (nrSignals times) {#SignalData}

This block provides information for a specific signal of a satellite.  
It is repeated `nrSignals` for a satellite.

| Field        | Description                                                | Unit | Format | Size | Offset |
|--------------|------------------------------------------------------------|------|--------|------|--------|
| signalId     | Signal unique identifier (see [GNSS_SAT_ID](#GNSS_SAT_ID)) | -    | uint8  | 1    | -      |
| sigFlags     | Signal status flags  (see [sigFlags](#sigFlags))           | -    | uint8  | 1    | -      |
| snr          | Signal-to-Noise ratio                                      | dB   | uint8  | 1    | -      |

##### sigFlags Definition {#sigFlags}

| Bits   | Type  | Name             | Description                                                           |
|--------|-------|------------------|-----------------------------------------------------------------------|
| [0-2]  | Enum  | Tracking status  | Tracking Status (see [SAT_TRACKING_STATUS](#SAT_TRACKING_STATUS)).    |
| [3-4]  | Enum  | Health status    | Health Status (see [SAT_HEALTH_STATUS](#SAT_HEALTH_STATUS)).          |
| 5      | Mask  | SNR valid        | Set if the Signal-to-Noise Ratio information is available.            |

#### Common Definitions

The definitions below are shared across [SatData](#SatData) and [SignalData](#SignalData) blocks.

##### Tracking Status Enum {#SAT_TRACKING_STATUS}

| Value | Name                                           | Description                                           |
|-------|------------------------------------------------|-------------------------------------------------------|
| 0     | SBG_ECOM_SAT_TRACKING_STATUS_UNKNOWN           | Unknown tracking status (e.g., no signal or idle)     |
| 1     | SBG_ECOM_SAT_TRACKING_STATUS_SEARCHING         | Signal being searched, not yet usable                 |
| 2     | SBG_ECOM_SAT_TRACKING_STATUS_TRACKING_UNKNOWN  | Signal tracked, use in solution unknown               |
| 3     | SBG_ECOM_SAT_TRACKING_STATUS_TRACKING_NOT_USED | Signal tracked, not used in the solution              |
| 4     | SBG_ECOM_SAT_TRACKING_STATUS_TRACKING_REJECTED | Signal tracked but rejected (e.g., due to jamming)    |
| 5     | SBG_ECOM_SAT_TRACKING_STATUS_TRACKING_USED     | Signal tracked and used in the GNSS solution          |

##### Health Status Enum {#SAT_HEALTH_STATUS}

| Value | Name                                           | Description                                           |
|-------|------------------------------------------------|-------------------------------------------------------|
| 0     | SBG_ECOM_SAT_HEALTH_STATUS_UNKNOWN             | Satellite or signal health is unknown                 |
| 1     | SBG_ECOM_SAT_HEALTH_STATUS_HEALTHY             | The satellite or signal is healthy                    |
| 2     | SBG_ECOM_SAT_HEALTH_STATUS_UNHEALTHY           | The satellite or signal is unhealthy                  |

##### Elevation Status Enum {#SAT_ELEVATION_STATUS}

| Value | Name                                           | Description                                           |
|-------|------------------------------------------------|-------------------------------------------------------|
| 0     | SBG_ECOM_SAT_ELEVATION_STATUS_UNKNOWN          | Elevation status unknown                              |
| 1     | SBG_ECOM_SAT_ELEVATION_STATUS_SETTING          | The satellite elevation is setting                    |
| 2     | SBG_ECOM_SAT_ELEVATION_STATUS_RISING           | The satellite elevation is rising                     |
## Aiding Equipments

The following logs provide the navigation unit's output, including Euler angles, quaternions, velocity and position.  
These logs share common status flags and enumerations, detailed below.

### SBG_ECOM_LOG_MAG (04) {#SBG_ECOM_LOG_MAG}

This log provides calibrated magnetometer data along with associated accelerometer readings.  
When an internal magnetometer is used, the accelerometer data is sourced from the internal IMU.

- **Message Name (ID):** `SBG_ECOM_LOG_MAG (04)`
- **Compatibility:** ELLIPSE AHRS/INS
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue)
- **Payload Size:** 30 bytes

| Field       | Description                                                       | Unit  | Format | Size | Offset |
|-------------|-------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP  | Time since the sensor was powered up                              | µs    | uint32 | 4    | 0      |
| MAG_STATUS  | Magnetometer status bitmask (see [MAG_STATUS](#MAG_STATUS))       | -     | uint16 | 2    | 4      |
| MAG_X       | Magnetic field along the X axis in the body frame.                | a.u   | float  | 4    | 6      |
| MAG_Y       | Magnetic field along the Y axis in the body frame.                | a.u   | float  | 4    | 10     |
| MAG_Z       | Magnetic field along the Z axis in the body frame.                | a.u   | float  | 4    | 14     |
| ACCEL_X     | Acceleration along the X axis in the body frame.                  | m/s²  | float  | 4    | 18     |
| ACCEL_Y     | Acceleration along the Y axis in the body frame.                  | m/s²  | float  | 4    | 22     |
| ACCEL_Z     | Acceleration along the Z axis in the body frame.                  | m/s²  | float  | 4    | 26     |

> [!NOTE]
> Magnetic field measurements are expressed in Arbitrary Units (A.U.) as they are used to determine magnetic heading.  
> After successful magnetic calibration, the magnetic field values are normalized to one.

#### MAG_STATUS Definition {#MAG_STATUS}

| Bit | Name                             | Description                                             |
|-----|----------------------------------|---------------------------------------------------------|
| 0   | SBG_ECOM_MAG_MAG_X_BIT           | Set if the magnetometer X-axis passes the self-test     |
| 1   | SBG_ECOM_MAG_MAG_Y_BIT           | Set if the magnetometer Y-axis passes the self-test     |
| 2   | SBG_ECOM_MAG_MAG_Z_BIT           | Set if the magnetometer Z-axis passes the self-test     |
| 3   | SBG_ECOM_MAG_ACCEL_X_BIT         | Set if the accelerometer X-axis passes the self-test    |
| 4   | SBG_ECOM_MAG_ACCEL_Y_BIT         | Set if the accelerometer Y-axis passes the self-test    |
| 5   | SBG_ECOM_MAG_ACCEL_Z_BIT         | Set if the accelerometer Z-axis passes the self-test    |
| 6   | SBG_ECOM_MAG_MAGS_IN_RANGE       | Set if the magnetometer is not saturated                |
| 7   | SBG_ECOM_MAG_ACCELS_IN_RANGE     | Set if the accelerometer is not saturated               |
| 8   | SBG_ECOM_MAG_CALIBRATION_OK      | Set if the magnetometer appears to be calibrated        |

### SBG_ECOM_LOG_MAG_CALIB (05) {#SBG_ECOM_LOG_MAG_CALIB}

This log provides a raw buffer for magnetic calibration procedures, such as those performed using the sbgCenter software.

- **Message Name (ID):** `SBG_ECOM_LOG_MAG_CALIB (05)`
- **Compatibility:** ELLIPSE AHRS/INS
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue)
- **Payload Size:** 22 bytes

| Field         | Description                                                     | Unit  | Format   | Size   | Offset |
|---------------|-----------------------------------------------------------------|-------|----------|--------|--------|
| TIME_STAMP    | Time since the sensor was powered up                            | µs    | uint32   | 4      | 0      |
| RESERVED      | Reserved field for future use                                   | -     | uint16   | 2      | 4      |
| BUFFER        | Raw magnetic calibration data buffer                            | -     | uint8[]  | 16     | 6      |

### SBG_ECOM_LOG_ODO_VEL (19) {#SBG_ECOM_LOG_ODO_VEL}

This log provides the external velocity aiding expressed in meters per second, measured along the IMU/vehicle X axis (direction of travel).  
This velocity can be sourced from various speed sensors, such as a quadrature/pulse odometer or integrated CAN bus vehicles odometer.

The log is asynchronous and is sent whenever new velocity aiding information is available, such as when an odometer pulse is detected.

- **Message Name (ID):** `SBG_ECOM_LOG_ODO_VEL (19)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 10 bytes

| Field        | Description                                                        | Unit  | Format | Size | Offset |
|--------------|--------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP   | Time since the sensor was powered up                               | µs    | uint32 | 4    | 0      |
| ODO_STATUS   | Odometer velocity status (see [ODO_STATUS](#ODO_STATUS))           | -     | uint16 | 2    | 4      |
| ODO_VEL      | Velocity along the odometer direction                              | m/s   | float  | 4    | 6      |

**Timestamp Considerations:**  
This log is asynchronous, with the timestamp indicating the actual time of the velocity data, rather than the time when the log was generated.  
Consequently, the timestamp may slightly precede other data, such as IMU readings, that are sent simultaneously.

#### ODO_STATUS Definition {#ODO_STATUS}

| Bit | Name                   | Description                                                                        |
|-----|------------------------|------------------------------------------------------------------------------------|
| 0   | SBG_ECOM_ODO_REAL_MEAS | Set if the log is from a real pulse measurement, 0 for virtual ZUPT.               |
| 1   | SBG_ECOM_ODO_TIME_SYNC | Set if the velocity information is correctly time-synchronized.                    |

### SBG_ECOM_LOG_AIR_DATA (36) {#SBG_ECOM_LOG_AIR_DATA}

The airdata log provides barometric altitude above a reference level and true airspeed.  
The altitude is typically measured using a barometer, while the true airspeed is obtained from a pitot tube.

By default, the altitude is referenced to a standard 1013.25 hPa zero level pressure.

This log serves both as an input and output.  
It is sent by the unit when new internal airdata information is available.  
It can also be used to inject external altitude and true airspeed aiding information.

**Timestamp Considerations:**  
This log is asynchronous, with the timestamp indicating the actual time of the altitude/airspeed measurement, rather than the time when the log was generated.

When used as an input for external altitude aiding, the `TIME_STAMP` field may either represent an absolute time or a measurement delay, depending on the status flags.  
The measurement delay allows the INS to calculate an absolute timestamp based on the reception time, transmission delay, and the specified delay.

- **Message Name (ID):** `SBG_ECOM_LOG_AIR_DATA (36)`
- **Compatibility:** ELLIPSE AHRS/INS
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-2.3-blue)
- **Payload Size:** 26 bytes

| Field              | Description                                                        | Unit   | Format | Size | Offset |
|--------------------|--------------------------------------------------------------------|--------|--------|------|--------|
| TIME_STAMP / DELAY | Time since sensor was powered up or measurement delay              | µs     | uint32 | 4    | 0      |
| AIRDATA_STATUS     | Airdata information status (see [AIRDATA_STATUS](#AIRDATA_STATUS)) | -      | uint16 | 2    | 4      |
| PRESSURE_ABS       | Raw absolute pressure measured by the barometer sensor             | Pa     | float  | 4    | 6      |
| ALTITUDE           | Altitude computed from barometric altimeter                        | m      | float  | 4    | 10     |
| PRESSURE_DIFF      | Raw differential pressure measured by the pitot tube               | Pa     | float  | 4    | 14     |
| TRUE_AIRSPEED      | True airspeed measured by the pitot tube                           | m/s    | float  | 4    | 18     |
| AIR_TEMPERATURE    | Outside air temperature used for airspeed computations             | °C     | float  | 4    | 22     |

#### AIRDATA_STATUS Definition {#AIRDATA_STATUS}

| Bit | Name                                  | Description                                                                  |
|-----|---------------------------------------|------------------------------------------------------------------------------|
| 0   | SBG_ECOM_AIR_DATA_TIME_IS_DELAY       | Set if `TIME_STAMP` represents a measurement delay instead of absolute time. |
| 1   | SBG_ECOM_AIR_DATA_PRESSURE_ABS_VALID  | Set if the absolute pressure field is filled and valid.                      |
| 2   | SBG_ECOM_AIR_DATA_ALTITUDE_VALID      | Set if the barometric altitude field is filled and valid.                    |
| 3   | SBG_ECOM_AIR_DATA_PRESSURE_DIFF_VALID | Set if the differential pressure field is filled and valid.                  |
| 4   | SBG_ECOM_AIR_DATA_AIRSPEED_VALID      | Set if the true airspeed field is filled and valid.                          |
| 5   | SBG_ECOM_AIR_DATA_TEMPERATURE_VALID   | Set if the outside air temperature field is filled and valid.                |

### SBG_ECOM_LOG_DVL_BOTTOM/WATER_TRACK {#SBG_ECOM_LOG_DVL_XXXX}

This log records velocity data from a Doppler Velocity Log (DVL), either from bottom tracking or water tracking.  
The timestamp indicates the actual time of the DVL velocity data, rather than being synchronized with the main loop.

- **Message Name (ID):** `SBG_ECOM_LOG_DVL_BOTTOM_TRACK (29)`, `SBG_ECOM_LOG_DVL_WATER_TRACK (30)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.1-blue)
- **Payload Size:** 30 bytes

| Field              | Description                                                      | Unit  | Format | Size | Offset |
|--------------------|------------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP         | Time since the sensor was powered up                             | µs    | uint32 | 4    | 0      |
| DVL_STATUS         | DVL velocity status bitmask (see [DVL_STATUS](#DVL_STATUS))      | -     | uint16 | 2    | 4      |
| VELOCITY_X         | Velocity in the X direction, DVL instrument frame                | m/s   | float  | 4    | 6      |
| VELOCITY_Y         | Velocity in the Y direction, DVL instrument frame                | m/s   | float  | 4    | 10     |
| VELOCITY_Z         | Velocity in the Z direction, DVL instrument frame                | m/s   | float  | 4    | 14     |
| VELOCITY_QUALITY_X | Quality of the X velocity measurement                            | m/s   | float  | 4    | 18     |
| VELOCITY_QUALITY_Y | Quality of the Y velocity measurement                            | m/s   | float  | 4    | 22     |
| VELOCITY_QUALITY_Z | Quality of the Z velocity measurement                            | m/s   | float  | 4    | 26     |

#### DVL_STATUS Definition {#DVL_STATUS}

| Bit | Name                       | Description                                                        |
|-----|----------------------------|--------------------------------------------------------------------|
| 0   | SBG_ECOM_DVL_VELOCITY_VALID| Set if the DVL measured a valid velocity                           |
| 1   | SBG_ECOM_DVL_TIME_SYNC     | Set if the data is accurately timestamped using Sync In/Out        |

### SBG_ECOM_LOG_DEPTH (47) {#SBG_ECOM_LOG_DEPTH}

This log captures pressure and depth information from a subsea depth sensor.  
The timestamp reflects the actual time of the depth measurement, rather than being synchronized with the main loop.

- **Message Name (ID):** `SBG_ECOM_LOG_DEPTH (47)`
- **Compatibility:** High Performance INS
- **Firmware:** ![HPINS](https://img.shields.io/badge/HPINS-1.3-blue)
- **Payload Size:** 14 bytes

| Field         | Description                                                     | Unit  | Format | Size | Offset |
|---------------|-----------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP    | Time since the sensor was powered up or measurement delay       | µs    | uint32 | 4    | 0      |
| DEPTH_STATUS  | Depth sensor status bitmask (see [DEPTH_STATUS](#DEPTH_STATUS)) | -     | uint16 | 2    | 4      |
| PRESSURE_ABS  | Absolute water pressure measured                                | Pa    | float  | 4    | 6      |
| DEPTH         | Measured depth, positive upward                                 | m     | float  | 4    | 10     |

#### DEPTH_STATUS Definition {#DEPTH_STATUS}

| Bit | Name                              | Description                                                      |
|-----|-----------------------------------|------------------------------------------------------------------|
| 0   | SBG_ECOM_DEPTH_TIME_IS_DELAY      | Set if the timestamp represents a delay instead of absolute time |
| 1   | SBG_ECOM_DEPTH_PRESSURE_ABS_VALID | Set if the pressure field is valid                               |
| 2   | SBG_ECOM_DEPTH_ALTITUDE_VALID     | Set if the depth field is valid                                  |

### SBG_ECOM_LOG_USBL (37) {#SBG_ECOM_LOG_USBL}

This log retrieves the position information as determined by a USBL (Ultra-Short Baseline) beacon.  
The timestamp indicates the actual time of the positioning data, rather than being synchronized with the main loop.

- **Message Name (ID):** `SBG_ECOM_LOG_USBL (37)`
- **Compatibility:** High Performance INS
- **Firmware:** ![HPINS](https://img.shields.io/badge/HPINS-1.3-blue)
- **Payload Size:** 38 bytes

| Field         | Description                                                    | Unit   | Format | Size | Offset |
|---------------|----------------------------------------------------------------|--------|--------|------|--------|
| TIME_STAMP    | Time since the sensor was powered up                           | µs     | uint32 | 4    | 0      |
| USBL_STATUS   | USBL system status bitmask (see [USBL_STATUS](#USBL_STATUS))   | -      | uint16 | 2    | 4      |
| LATITUDE      | Latitude in degrees, positive north                            | °      | double | 8    | 6      |
| LONGITUDE     | Longitude in degrees, positive east                            | °      | double | 8    | 14     |
| DEPTH         | Depth below mean sea level, positive down                      | m      | float  | 4    | 22     |
| LATITUDE_STD  | 1σ standard deviation of latitude accuracy                     | m      | float  | 4    | 26     |
| LONGITUDE_STD | 1σ standard deviation of longitude accuracy                    | m      | float  | 4    | 30     |
| DEPTH_STD     | 1σ standard deviation of depth accuracy                        | m      | float  | 4    | 34     |

#### USBL_STATUS Definition {#USBL_STATUS}

| Bit | Name                         | Description                                                        |
|-----|------------------------------|--------------------------------------------------------------------|
| 0   | SBG_ECOM_USBL_TIME_SYNC      | Set if the USBL data is accurately time-synchronized               |
| 1   | SBG_ECOM_USBL_POSITION_VALID | Set if the USBL data represents a valid 2D position                |
| 2   | SBG_ECOM_USBL_DEPTH_VALID    | Set if the USBL data has valid depth information                   |

> [!NOTE]
> USBL aiding is not currently supported by the INS filter.
> However, the unit can accept external USBL aiding data and log it for later post-processing.

### SBG_ECOM_LOG_VELOCITY_X {#SBG_ECOM_LOG_VELOCITY_X}

This log provides 1D, 2D, or 3D velocity aiding information.

This log serves both as an input and output:
 - It is used to inject external velocity aiding information into the INS.
 - The unit can send this log when new velocity information is received and processed.

**Velocity Frame**

The INS can utilize 1D, 2D, or 3D velocity information to enhance navigation performance, particularly during dead reckoning periods.

The velocity can be expressed in either:
 - The vehicle body frame (X/Y/Z)
 - The INS navigation frame (North, East, Down)
  
The velocity frame convention is determined by the generic velocity aiding error model configuration.
 - When body velocity is used, a 1D, 2D, or 3D velocity can be provided using the `VELOCITY_STATUS` bitmask.
 - When navigation frame velocity is used, all three components (North, East, Down) must be supplied.

**Timestamp Considerations:**
When used as an input for external velocity aiding, the `TIME_STAMP` field can represent:
 - The INS internal time since the INS was powered up (in µs).
 - A measurement delay to apply on the message reception and processing time (in µs).
 - A GPS time of the week (in ms).

You can control the type of supplied time with the `VELOCITY_STATUS` field.

> [!NOTE]
> The output velocity aiding message always uses the INS internal timestamp (in µs since the sensor power-up) to comply with other sbgECom output logs.

- **Message Name (ID):** `SBG_ECOM_LOG_VELOCITY_1 (58)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.1-blue) ![HPINS](https://img.shields.io/badge/HPINS-5.5-blue)
- **Payload Size:** 30 bytes

| Field           | Description                                                       | Unit     | Format | Size | Offset |
| --------------- | ----------------------------------------------------------------- | -------- | ------ | ---- | ------ |
| TIME_STAMP      | Time since the INS was powered up or delay or GPS TOW.            | µs or ms | uint32 | 4    | 0      |
| VELOCITY_STATUS | Velocity status bitmask (see [VELOCITY_STATUS](#VELOCITY_STATUS)) | -        | uint16 | 2    | 4      |
| VELOCITY_0      | Velcoity component 0, body X (forward) or navigation North        | m/s      | float  | 4    | 6      |
| VELOCITY_1      | Velcoity component 1, body Y (right) or navigation East           | m/s      | float  | 4    | 10     |
| VELOCITY_2      | Velcoity component 2, body Z (down) or navigation Down            | m/s      | float  | 4    | 14     |
| VELOCITY_STD_0  | Standard deviation of the velocity 0 component                    | m/s      | float  | 4    | 18     |
| VELOCITY_STD_1  | Standard deviation of the velocity 1 component                    | m/s      | float  | 4    | 22     |
| VELOCITY_STD_2  | Standard deviation of the velocity 2 component                    | m/s      | float  | 4    | 26     |

#### VELOCITY_STATUS Definition {#VELOCITY_STATUS}

| Bit   | Name                        | Description                                                                      |
| ----- | --------------------------- | -------------------------------------------------------------------------------- |
| [0-2] | SBG_ECOM_VELOCITY_TIME_TYPE | Indicates the velocity time type (see [VELOCITY_TIME_TYPE](#VELOCITY_TIME_TYPE)) |
| 3     | SBG_ECOM_VELOCITY_0_VALID   | Set if the velocity component 0 information is valid                             |
| 4     | SBG_ECOM_VELOCITY_1_VALID   | Set if the velocity component 1 information is valid                             |
| 5     | SBG_ECOM_VELOCITY_2_VALID   | Set if the velocity component 2 information is valid                             |
| 6     | SBG_ECOM_VELOCITY_STD_VALID | Set if the velocity standard deviation information is valid                      |

##### VELOCITY_TIME_TYPE Definition

| Value | Name                                     | Description                                             |
| ----- | ---------------------------------------- | ------------------------------------------------------- |
| 0     | SBG_ECOM_VELOCITY_TIME_TYPE_TIMESTAMP    | `TIME_STAMP` represents time since INS power-up (in µs) |
| 1     | SBG_ECOM_VELOCITY_TIME_TYPE_DELAY        | `TIME_STAMP` represents a measurement delay (in µs)     |
| 2     | SBG_ECOM_VELOCITY_TIME_TYPE_TIME_OF_WEEK | `TIME_STAMP` represents GPS time of the week (in ms)    |
## Event Markers

SBG Systems products support multiple input and output events, facilitating precise timestamping and synchronization of various equipment.  
For example, an electrical signal can timestamp the exact moment a picture is taken.

SBG Systems' INS can align their internal clock with GPS time, providing a reliable solution for synchronizing devices and measurements to a common clock.  

### SBG_ECOM_LOG_EVENT_X (In/Out) {#SBG_ECOM_LOG_EVENT_XXXX}

The Event In/Out log is designed to efficiently timestamp events at up to 1 kHz, while being transmitted at a maximum rate of 200 Hz.  
This log can report up to five events detected in the last 5 ms, minimizing transmission overhead.

Depending on the product options, input/output signals such as Sync A, B, C, D, and E may be available, each associated with a corresponding message.

#### Operating Principle

The TIME_STAMP field marks the time of the first event received in the last 5 ms.  
Subsequent events in this window are recorded with time offsets, optimizing log size.

Additionally, SBG Systems products can generate output event markers, providing both an electrical sync out signal and a corresponding output log for precise timing.

**Example:**  
If three events occur within 5 ms:
- The first event's time is stored in `TIME_STAMP`.
- The second event's time is `TIME_STAMP + TIME_OFFSET_0`.
- The third event's time is `TIME_STAMP + TIME_OFFSET_1`.

Unused time offset fields are set to 0, and `EVENT_STATUS` indicates which fields are valid.

- **Message Name (ID):** `SBG_ECOM_LOG_EVENT_A (24)`, `SBG_ECOM_LOG_EVENT_B (25)`, `SBG_ECOM_LOG_EVENT_C (26)`, `SBG_ECOM_LOG_EVENT_D (27)`, `SBG_ECOM_LOG_EVENT_E (28)`, `SBG_ECOM_LOG_EVENT_OUT_A (45)`, `SBG_ECOM_LOG_EVENT_OUT_B (46)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue) ![PULSE](https://img.shields.io/badge/PULSE-1.0-blue)
- **Payload Size:** 14 bytes

| Field         | Description                                                    | Unit  | Format | Size | Offset |
|---------------|----------------------------------------------------------------|-------|--------|------|--------|
| TIME_STAMP    | Time since the sensor was powered up                           | µs    | uint32 | 4    | 0      |
| EVENT_STATUS  | Status bitmask (see [EVENT_STATUS](#EVENT_STATUS))             | -     | uint16 | 2    | 4      |
| TIME_OFFSET_0 | Time offset for the second received event                      | µs    | uint16 | 2    | 6      |
| TIME_OFFSET_1 | Time offset for the third received event                       | µs    | uint16 | 2    | 8      |
| TIME_OFFSET_2 | Time offset for the fourth received event                      | µs    | uint16 | 2    | 10     |
| TIME_OFFSET_3 | Time offset for the fifth received event                       | µs    | uint16 | 2    | 12     |

#### EVENT_STATUS Definition {#EVENT_STATUS}

| Bit | Name                          | Description                                                               |
|-----|-------------------------------|---------------------------------------------------------------------------|
| 0   | SBG_ECOM_EVENT_OVERFLOW       | Set if events are received at a rate higher than 1 kHz                    |
| 1   | SBG_ECOM_EVENT_OFFSET_0_VALID | Set if at least two events have been received                             |
| 2   | SBG_ECOM_EVENT_OFFSET_1_VALID | Set if at least three events have been received                           |
| 3   | SBG_ECOM_EVENT_OFFSET_2_VALID | Set if at least four events have been received                            |
| 4   | SBG_ECOM_EVENT_OFFSET_3_VALID | Set if five events have been received                                     |

> [!NOTE]
> The device can handle event markers at up to 1 kHz. Excessive events may overload the internal CPU, decreasing performance and reliability.

> [!WARNING]
> Never leave an activated Sync In signal unconnected, as noise on the line may trigger spurious events at high rates.
## Vibration Monitoring

The IMU includes a built-in Fast Fourier Transform (FFT) engine that analyzes high-frequency accelerometer data to monitor vibration and acoustic noise levels.

### Configuration

Vibration monitoring can be enabled independently on each IMU axis **(X, Y, Z)** and configured with different window functions.  
Selecting the appropriate window function helps adapt to various signal types and improves measurement accuracy in diverse vibration or acoustic environments.

The processing cycles through each **enabled window**, then through each **enabled axis**, in sequence.  
This approach reduces bandwidth usage while preserving good temporal coverage across all selected channels.

Please check the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi) documentation from more details.

### Output

The device can output the following messages for one axis and one window function, triggered on a new data event:
 - **FFT data** Provides the full FFT spectrum for detailed frequency-domain analysis.
 - **Summary reports** Includes pre-computed metrics to assess vibration levels.
 
### Status Field {#VIB_MON_STATUS}

The vibration monitoring `STATUS` field is shared between `SBG_ECOM_LOG_VIB_MON_FFT` and `SBG_ECOM_LOG_VIB_MON_REPORT` messages.  
It provides information about the axis and window function used for the vibration monitoring.

| Bits   | Type  | Name                     | Description                                                                                    |
|--------|-------|--------------------------|------------------------------------------------------------------------------------------------|
| [0-2]  | Enum  | SBG_ECOM_VIB_MON_AXIS    | Axis on which the vibration monitoring has been computed. (see [VIB_MON_AXIS](#VIB_MON_AXIS)). |
| [3-5]  | Enum  | SBG_ECOM_VIB_MON_WINDOW  | Window used for vibration monitoring. (see [VIB_MON_WINDOW](#VIB_MON_WINDOW)).                 |

#### Axis Enumeration {#VIB_MON_AXIS}

| Value | Name                         | Description                                      |
|-------|------------------------------|--------------------------------------------------|
| 0     | SBG_ECOM_VIB_MON_AXIS_X      | Vibration monitoring on X axis.                  |
| 1     | SBG_ECOM_VIB_MON_AXIS_Y      | Vibration monitoring on Y axis.                  |
| 2     | SBG_ECOM_VIB_MON_AXIS_Z      | Vibration monitoring on Z axis.                  |

#### Window Enumeration {#VIB_MON_WINDOW}

| Value | Name                                 | Description                                  |
|-------|--------------------------------------|----------------------------------------------|
| 0     | SBG_ECOM_VIB_MON_WINDOW_RECTANGULAR  | Rectangular window (also called Uniform).    |
| 1     | SBG_ECOM_VIB_MON_WINDOW_HANNING      | Hanning window (also known as Hann).         |
| 2     | SBG_ECOM_VIB_MON_WINDOW_FLAT_TOP     | Flat Top window.                             |

### SBG_ECOM_LOG_VIB_MON_FFT (59) {#SBG_ECOM_LOG_VIB_MON_FFT}

This message provides the raw output of the IMU’s built-in Fast Fourier Transform (FFT) engine.  
It includes FFT magnitude values in m/s² from DC and up to the Nyquist frequency (FSAMPLING/2).  
Frequency bin values are not transmitted directly and must be reconstructed using the known sampling frequency and FFT length.

Due to the large size of the data, the full FFT message is split into multiple chunks. Each chunk is sent individually using the structure described below.  
The host is responsible for reassembling the chunks to reconstruct the complete FFT message.

- **Message Name (ID):** `SBG_ECOM_LOG_VIB_MON_FFT (59)`
- **Compatibility:** PULSE IMU & High Performance INS
- **Firmware:** ![PULSE](https://img.shields.io/badge/PULSE-2.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-5.5-blue)
- **Payload Size:** Variable (up to 70 bytes)

#### Chunk Frame Structure

Each chunk contains a partial payload of the full FFT message.

| Field        | Description                                               | Unit | Format   | Size       | Offset |
|--------------|-----------------------------------------------------------|------|----------|------------|--------|
| PAGE_INDEX   | Zero-based index of the current chunk.                    | -    | uint16   | 2          | 0      |
| NR_PAGES     | Total number of chunks for this FFT message.              | -    | uint16   | 2          | 2      |
| SIZE         | Size in bytes for the BUFFER field.                       | -    | uint16   | 2          | 4      |
| BUFFER       | FFT data chunk, part of the full FFT message.             | -    | uint8[]  | [0-64]     | 6      |

#### Reassembled Message Format

Once all chunks are reassembled, the full message has the following structure:

| Field        | Description                                                    | Unit | Format   | Size       | Offset |
|--------------|----------------------------------------------------------------|------|----------|------------|--------|
| TIMESTAMP    | Time since sensor was powered up.                              | µs   | uint32   | 4          |  0     |
| STATUS       | FFT processing status (see [VIB_MON_STATUS](#VIB_MON_STATUS)). | -    | uint16   | 2          |  4     |
| FSAMPLING    | Sampling frequency used for the FFT.                           | Hz   | uint16   | 2          |  6     |
| BIN_COUNT    | Number of FFT bins (i.e., number of magnitude values).         | -    | uint16   | 2          |  8     |
| SCALE_FACTOR | Scale factor to convert raw FFT values to physical units.      | -    | float    | 4          | 10     |
| OFFSET       | Offset to apply after scaling to get physical units.           | -    | float    | 4          | 14     |
| ACF          | Amplitude correction factor of the window.                     | -    | float    | 4          | 18     |
| ECF          | Energy correction factor of the window.                        | -    | float    | 4          | 22     |
| DATA         | Array of raw FFT magnitudes.                                   | m/s² | uint16[] | Variable   | 26     |

#### Reconstructing Frequency and Magnitude

Only FFT magnitudes are transmitted (to save bandwidth); associated frequency values must be reconstructed on the host using the following formula:

$$f_{bin} = \frac{bin\_{index} \cdot f_{sampling}}{2 \cdot bin_{count}}$$

The magnitude is defined as twice the norm of the FFT complex output, normalized by the size of the FFT. 

To convert raw FFT magnitudes into physical units (in m/s²):
$$magnitude = raw \cdot scale + offset$$

A window is applied before the FFT computation: the FFT raw magnitudes need to be corrected to retrieve the accurate values.

When analysing the amplitude of an harmonic:
$$magnitude_{corrected} = acf \cdot magnitude$$

When analysing the energy of an harmonic (e.g. to compute gRMS):
$$magnitude_{corrected} = ecf \cdot magnitude$$

The gRMS on the entire spectrum is:
$$gRMS = \sqrt{ (ecf \cdot magnitude_{0})^2 + \sum_{i=1}^{bin_{count}} { \frac { (ecf \cdot magnitude_{i})^2 } {2} } \cdot }$$

> [!NOTE]
> ACF and ECF should never be applied at the same time.

> [!NOTE]
> The maximum observable frequency is FSAMPLING / 2 (Nyquist limit).

### SBG_ECOM_LOG_VIB_MON_REPORT (60) {#SBG_ECOM_LOG_VIB_MON_REPORT}

This message provides a synthetic summary of the vibration monitoring results, offering key vibration indicators derived from the accelerometer signal.  
It includes an overall vibration level **gRMS**, as well as detailed information over four evenly spaced frequency bands covering the full spectrum from DC to the Nyquist frequency.

- **Message Name (ID):** `SBG_ECOM_LOG_VIB_MON_REPORT (60)`
- **Compatibility:** PULSE IMU & High Performance INS
- **Firmware:** ![PULSE](https://img.shields.io/badge/PULSE-2.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-5.5-blue)
- **Payload Size:** 94 bytes

| Field           | Description                                                                | Unit  | Format  | Size | Offset |
|-----------------|----------------------------------------------------------------------------|-------|---------|------|--------|
| TIMESTAMP       | Time since sensor was powered up.                                          | µs    | uint32  | 4    |  0     |
| STATUS          | Processing status (see [VIB_MON_STATUS](#VIB_MON_STATUS)).                 | -     | uint16  | 2    |  4     |
| MEAN            | Time-domain mean of the accelerometer signal (DC component).               | m/s²  | float   | 4    |  6     |
| RMS             | Overall vibration level (gRMS) computed from the FFT spectrum.             | m/s²  | float   | 4    | 10     |
| BAND1           | Metrics for the first frequency band (see [VIB_MON_BAND](#VIB_MON_BAND)).  | -     | -       | 20   | 14     |
| BAND2           | Metrics for the second frequency band (see [VIB_MON_BAND](#VIB_MON_BAND)). | -     | -       | 20   | 34     |
| BAND3           | Metrics for the third frequency band (see [VIB_MON_BAND](#VIB_MON_BAND)).  | -     | -       | 20   | 54     |
| BAND4           |	Metrics for the fourth frequency band (see [VIB_MON_BAND](#VIB_MON_BAND)). | -     | -       | 20   | 74     |

#### Frequency Band Structure {#VIB_MON_BAND}

Each band provides vibration metrics within a specific frequency range.

| Field           | Description                                                                         | Unit  | Format  | Size | Offset |
|-----------------|-------------------------------------------------------------------------------------|-------|---------|------|--------|
| FREQ1           | Start frequency of the band (inclusive).                                            | Hz    | float   | 4    | 0      |
| FREQ2           | End frequency of the band (inclusive).                                              | Hz    | float   | 4    | 4      |
| RMS             | RMS of the signal within the band (gRMS vibration level).                           | m/s²  | float   | 4    | 8      |
| PEAK_HARMONIC   | Frequency of the peak harmonic within the band.                                     | Hz    | float   | 4    | 12     |
| PEAK_MAGNITUDE  | Magnitude of the peak harmonic within the band.                                     | m/s²  | float   | 4    | 16     |

> [!NOTE]
> All reported values have already been corrected for the energy loss introduced by the FFT windowing function.
