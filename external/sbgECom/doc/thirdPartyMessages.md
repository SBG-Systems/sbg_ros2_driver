# Third Party Binary Messages

The `SBG_ECOM_CLASS_LOG_THIRD_PARTY_0 (0x04)` message class consolidates a variety of widely recognized binary outputs from leading third-party hardware and software providers.  
By supporting protocols from industry leaders such as Teledyne, Kongsberg, Cobham, KVH, and Crossbow, SBG Systems' inertial navigation systems ensure seamless integration and broad compatibility with various external equipment.

This support allows for straightforward data exchange and system interoperability, making it easier for users to incorporate SBG Systems' INS products into complex multi-vendor environments.

## Messages overview

The following list, provides a quick overview of all available logs for this message class.  
It briefly describe which parameters are contained in each output log.

| Name (MSG ID)                                                                | Description                                                                    |
|------------------------------------------------------------------------------|--------------------------------------------------------------------------------|
| [SBG_ECOM_THIRD_PARTY_TSS1 (00)](#SBG_ECOM_THIRD_PARTY_TSS1)                 | Latitude, Longitude, Altitude, Quality indicator                               |
| [SBG_ECOM_THIRD_PARTY_KVH (01)](#SBG_ECOM_THIRD_PARTY_KVH)                   | KVH Roll, Pitch, and Heading output                                            |
| [SBG_ECOM_THIRD_PARTY_PD0 (02)](#SBG_ECOM_THIRD_PARTY_PD0)                   | Teledyne RDI PD0 message                                                       |
| [SBG_ECOM_THIRD_PARTY_SIMRAD_1000 (03)](#SBG_ECOM_THIRD_PARTY_SIMRAD_1000)   | Konsberg EM1000 proprietary frame that outputs Roll, Pitch, and Heading        |
| [SBG_ECOM_THIRD_PARTY_SIMRAD_3000 (04)](#SBG_ECOM_THIRD_PARTY_SIMRAD_3000)   | Konsberg EM3000 proprietary frame that outputs Roll, Pitch, and Heading        |
| [SBG_ECOM_THIRD_PARTY_SEAPATH_B26 (05)](#SBG_ECOM_THIRD_PARTY_SEAPATH_B26)   | Konsberg Seapath Binary Log 26 used for MBES FM mode                           |
| [SBG_ECOM_THIRD_PARTY_DOLOG_HRP (06)](#SBG_ECOM_THIRD_PARTY_DOLOG_HRP)       | DOLOG Heading, Roll, Pitch, and HRP rates                                      |
| [SBG_ECOM_THIRD_PARTY_AHRS_500 (07)](#SBG_ECOM_THIRD_PARTY_AHRS_500)         | Crossbow AHRS-500 attitude, rate, acceleration, and status                     |
| [SBG_ECOM_THIRD_PARTY_ADA_01 (08)](#SBG_ECOM_THIRD_PARTY_ADA_01)             | ADA-specific Data Packet with IMU/INS/Status data                              |
| [SBG_ECOM_THIRD_PARTY_AT_ITINS (09)](#SBG_ECOM_THIRD_PARTY_AT_ITINS)         | Cobham AVIATOR UAV 200 AT command                                              |
| [SBG_ECOM_THIRD_PARTY_KONGSBERG_MB (10)](#SBG_ECOM_THIRD_PARTY_KONGSBERG_MB) | Kongsberg multi-beam binary aiding log                                         |

---

## SBG_ECOM_THIRD_PARTY_TSS1 (00) {#SBG_ECOM_THIRD_PARTY_TSS1}

The `SBG_ECOM_THIRD_PARTY_TSS1` message is a proprietary log specifically designed for marine survey applications, providing heave, roll, pitch, sway, and vertical accelerations data.

The log data is deported to the output lever arms configured for each output interface, ensuring precise measurements at specific points on the vessel.  
With the capability to output multiple TSS1 frames across various interfaces, it allows seamless data integration for equipment located in different positions on a vessel.

### Message Format

```
:XXAAAASMHHHHQMRRRRSMPPPP<CR><LF>
```

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_TSS1 (00)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field      | Name                  | Description                                                                                                              |
|------------|-----------------------|--------------------------------------------------------------------------------------------------------------------------|
| `:`        | Start character       | Start of the TSS1 message frame.                                                                                         |
| `XX`       | Sway acceleration     | Hex value of sway acceleration in 3.835 cm/s², with a range from 0 to 9.81 m/s².                                         |
| `AAAA`     | Vertical acceleration | Hex value (2’s complement) of vertical acceleration in 0.0625 cm/s², with a range of –20.48 to +20.48 m/s².              |
| `S`        | Separator character   | Delimiter space character.                                                                                               |
| `M`        | Sign of heave         | Space if positive, minus if negative.                                                                                    |
| `HHHH`     | Heave measurement     | ASCII value of heave in centimeters, with a range of –99.99 to +99.99 meters.                                            |
| `Q`        | Status flag           | See [TSS1 Status Flags](#tss1-status-flags) for details.                                                                 |
| `M`        | Sign of roll          | Space if positive, minus if negative.                                                                                    |
| `RRRR`     | Roll                  | Roll in units of 0.01 degrees (e.g., 1000 = 10°), with a range of –99.99° to +99.99°.                                    |
| `S`        | Separator character   | Delimiter space character.                                                                                               |
| `M`        | Sign of pitch         | Space if positive, minus if negative.                                                                                    |
| `PPPP`     | Pitch                 | Pitch in units of 0.01 degrees (e.g., 1000 = 10°), with a range of –99.99° to +99.99°.                                   |
| `<CR><LF>` | End of frame          | Carriage return and line feed, marking the end of the frame.                                                             |

### TSS1 Status Flags {#tss1-status-flags}

The status flag character identifies the specific algorithms used to compute the heave data.  
For instance, it indicates whether the heave computation uses the INS velocity and rate of turn to greatly improve the accuracy of heave measurements during vessel maneuvers.

| Value | Description                                                                                  |
|-------|----------------------------------------------------------------------------------------------|
| `U`   | Unaided mode and stable measurements.                                                        |
| `u`   | Unaided mode but unstable heave data.                                                        |
| `G`   | Velocity-aided mode and stable measurements.                                                 |
| `g`   | Velocity-aided mode but unstable data.                                                       |
| `H`   | Heading-aided mode and stable measurements.                                                  |
| `h`   | Heading-aided mode but unstable data.                                                        |
| `F`   | Both velocity and heading-aided mode and stable measurements.                                |
| `f`   | Both velocity and heading-aided mode but unstable measurements.                              |

### Message Example

```
:1A4770 -0016H 0429 -0680<CR><LF>
```

Please find below a detailed breakdown and explanation of each field:

| Field  | Value   | Explanation                                                                                                                |
|--------|:-------:|----------------------------------------------------------------------------------------------------------------------------|
| `XX`   | 1A      | Sway acceleration, which is 0.9971 m/s²<br>*0x1A (hex) = 26 (decimal), multiplied by 0.03835 m/s² yields 0.9971 m/s²*      |
| `AAAA` | 4770    | Heave acceleration, which is 11.43 m/s²<br>*0x4770 (hex) = 18288 (decimal), multiplied by 0.000625 m/s² yields 11.43 m/s²* |
| `S`    | (space) | Delimiter space character.                                                                                                 |
| `M`    | (minus) | Indicates the heave value is negative.                                                                                     |
| `HHHH` | 0016    | Heave value, which is -16 cm.                                                                                              |
| `Q`    | H       | Status flag indicating stable heading-aided mode.                                                                          |
| `M`    | (space) | Indicates the roll value is positive.                                                                                      |
| `RRRR` | 0429    | Roll, which is 4.29°.                                                                                                      |
| `S`    | (space) | Delimiter space character.                                                                                                 |
| `M`    | (minus) | Indicates the pitch value is negative.                                                                                     |
| `PPPP` | 0680    | Pitch, which is -6.80°.                                                                                                    |

> [!WARNING]
> The TSS1 frame uses different conventions for heave measurements. In this frame, sway is expressed as positive left, and heave is positive up.

---

## SBG_ECOM_THIRD_PARTY_KVH (01) {#SBG_ECOM_THIRD_PARTY_KVH}

The `SBG_ECOM_THIRD_PARTY_KVH` message is a basic proprietary log that provides pitch, roll, heading, and heading rate data.  
This format is straightforward, making it easy to integrate with systems requiring these essential orientation parameters.

### Message Format
```
%pitch,roll,heading<CR><LF>
```

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_KVH (01)`
- **Compatibility:** AHRS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue)

| Field | Name         | Format   | Description                                                                                             |
|-------|--------------|----------|---------------------------------------------------------------------------------------------------------|
| 0     | %            | string   | Sentence Identifier                                                                                     |
| 1     | pitch        | ddd      | Pitch angle in tenths of degrees. Divide by 10 to get the pitch in degrees. Positive when bow up.       |
| 2     | roll         | ddd      | Roll angle in tenths of degrees. Divide by 10 to get the roll in degrees. Positive when port side down. |
| 3     | heading      | ddd      | Heading in tenths of degrees. Values range from 0 to 3600.                                              |
| 4     | End of frame | <CR><LF> | End of frame: carriage return and line feed.                                                            |

### Example Frame
```
%10,-5,3489<CR><LF>
```

This example shows a frame where the pitch is 1.0°, the roll is -0.5° (port side down), and the heading is 348.9°.

> [!WARNING]
> Roll sign is opposite sign of the usual SBG Systems convention.

## SBG_ECOM_THIRD_PARTY_PD0 (02) {#SBG_ECOM_THIRD_PARTY_PD0}

The `SBG_ECOM_THIRD_PARTY_PD0` message is a proprietary binary log format from Teledyne RDI, primarily used for outputting Doppler Velocity Log (DVL) data, including bottom tracking, water tracking, and water profiling information.

This log can be generated by the Inertial Navigation System (INS) only if valid PD0 frames are received from a connected DVL device.  
The INS does not modify or supplement the PD0 data; it simply forwards the original PD0 frame as received.  
This feature is particularly useful for recording DVL data in the internal datalogger or for real-time output to be processed by third-party software applications.

For detailed information on the PD0 frame structure and its contents, please refer to the Teledyne RDI documentation.

## SBG_ECOM_THIRD_PARTY_SIMRAD_1000 (03) {#SBG_ECOM_THIRD_PARTY_SIMRAD_1000}

The `SBG_ECOM_THIRD_PARTY_SIMRAD_1000` message is a proprietary binary log from Kongsberg, primarily used for transmitting attitude and heave data to echo sounders.  
This logs provide roll, pitch, heading, and heave measurements, essential for marine survey applications.

All fields in this log are stored in Little Endian format (LSB first), with signed fields represented using standard 2's complement.

The log data is deported to the output lever arms configured for each output interface, ensuring precise measurements at specific points on the vessel.  
With the capability to output multiple SIMRAD frames across various interfaces, it allows seamless data integration for equipment located in different positions on a vessel.

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_SIMRAD_1000 (03)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)
- **Total Size:** 10 bytes

| Field       | Description                                            | Unit   | Format | Size | Offset |
|-------------|--------------------------------------------------------|--------|--------|------|--------|
| Sync Byte 1 | Sync Byte 1 = 0x00                                     | -      | uint8  | 1    | 0      |
| Sync Byte 2 | Sync Byte 2 = 0x90                                     | -      | uint8  | 1    | 1      |
| Roll        | Roll, positive with port side up, ±179.99°             | 0.01°  | int16  | 2    | 2      |
| Pitch       | Pitch, positive with bow up, ±89.99°                   | 0.01°  | int16  | 2    | 4      |
| Heave       | Heave, positive up, ±99.99 meters                      | cm     | int16  | 2    | 6      |
| Heading     | Heading, positive clockwise, [0 to 359.99°]            | 0.01°  | int16  | 2    | 8      |

> [!WARNING]
> The Simrad 1000 message uses a different convention from SBG Systems' standard. In this frame, the heave measurement is positive upward, which is the opposite of the SBG Systems' convention.


## SBG_ECOM_THIRD_PARTY_SIMRAD_3000 (04) {#SBG_ECOM_THIRD_PARTY_SIMRAD_3000}

The `SBG_ECOM_THIRD_PARTY_SIMRAD_3000` message is similar to the [SBG_ECOM_THIRD_PARTY_SIMRAD_1000](#SBG_ECOM_THIRD_PARTY_SIMRAD_1000) message.  
The only difference is the first field that provide the heave/INS status.

Please refer to [SBG_ECOM_THIRD_PARTY_SIMRAD_1000](#SBG_ECOM_THIRD_PARTY_SIMRAD_1000) message for more information.

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_SIMRAD_3000 (04)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)
- **Total Size:** 10 bytes

| Field       | Description                                                           | Unit   | Format | Size | Offset |
|-------------|-----------------------------------------------------------------------|--------|--------|------|--------|
| Status      | Solution status (see [Simrad 3000 Status](#simrad-3000-status-flags)) | -      | uint8  | 1    | 0      |
| Sync Byte 2 | Sync Byte 2 = 0x90                                                    | -      | uint8  | 1    | 1      |
| Roll        | Roll, positive with port side up, ±179.99°                            | 0.01°  | int16  | 2    | 2      |
| Pitch       | Pitch, positive with bow up, ±89.99°                                  | 0.01°  | int16  | 2    | 4      |
| Heave       | Heave, positive up, ±99.99 meters                                     | cm     | int16  | 2    | 6      |
| Heading     | Heading, positive clockwise, [0 to 359.99°]                           | 0.01°  | int16  | 2    | 8      |

> [!WARNING]
> The Simrad 3000 message uses a different convention from SBG Systems' standard. In this frame, the heave measurement is positive upward, which is the opposite of the SBG Systems' convention.

### Simrad 3000 Status Flags {#simrad-3000-status-flags}

The status flag in the Simrad 3000 protocol is used to indicate the quality and validity of the orientation and heave data.  

| Value | Description                                                                |
|-------|----------------------------------------------------------------------------|
| 0x90  | Valid measurements with full accuracy                                      |
| 0x91  | Valid measurements with reduced accuracy (unaided mode)                    |
| 0x9A  | Invalid measurements; device is aligning, or the heave filter is unsettled |
| 0xA0  | Error reported in the motion sensor                                        |

## SBG_ECOM_THIRD_PARTY_SEAPATH_B26 (05) {#SBG_ECOM_THIRD_PARTY_SEAPATH_B26}

The `SBG_ECOM_THIRD_PARTY_SEAPATH_B26` message is a proprietary binary log from Kongsberg Seapath.  
It provides marine related data including INS position, velocity, attitude, heave, and rotation rates.

This log is primarily used with Kongsberg MBES systems to support the FM mode, facilitating precise marine survey operations.

All fields in this log are stored in Big Endian format (MSB first). Signed data uses the 2's complement representation.

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_SEAPATH_B26 (05)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)
- **Total Size:** 52 bytes

| Field                               | Description                                                            | Unit          | Format | Size | Offset |
|-------------------------------------|------------------------------------------------------------------------|---------------|--------|------|--------|
| Header 1                            | First header value, always 0xAA                                        | -             | uint8  | 1    | 0      |
| Header 2                            | Second header value, always 0x55                                       | -             | uint8  | 1    | 1      |
| Time, Seconds                       | UTC Time in seconds that timestamps the INS data                       | Seconds       | int32  | 4    | 2      |
| Time, Fraction of Sec.              | UTC Time fraction of a second that timestamps the INS data             | 0.0001 sec    | uint16 | 2    | 6      |
| Latitude                            | INS latitude, positive North of the Equator                            | 2^30 = 90°    | int32  | 4    | 8      |
| Longitude                           | INS longitude, positive East of Greenwich                              | 2^30 = 90°    | int32  | 4    | 12     |
| Height                              | INS altitude above ellipsoid                                           | Centimeters   | int32  | 4    | 16     |
| Real-time Heave                     | Heave, positive down                                                   | Centimeters   | int16  | 2    | 20     |
| North Velocity                      | INS North velocity, positive North                                     | cm/s          | int16  | 2    | 22     |
| East Velocity                       | INS East velocity, positive East                                       | cm/s          | int16  | 2    | 24     |
| Down Velocity                       | INS Down velocity, positive Down                                       | cm/s          | int16  | 2    | 26     |
| Roll                                | Vessel roll angle, positive with port side up                          | 2^14 = 90°    | int16  | 2    | 28     |
| Pitch                               | Vessel pitch angle, positive with bow up                               | 2^14 = 90°    | int16  | 2    | 30     |
| Heading                             | Vessel true heading, positive clockwise [0 to 359.99°]                 | 2^14 = 90°    | uint16 | 2    | 32     |
| Vessel X Rate                       | Vessel body X rotation rate, positive with port side up                | 2^14 = 90 °/s | int16  | 2    | 34     |
| Vessel Y Rate                       | Vessel body Y rotation rate, positive with bow up                      | 2^14 = 90 °/s | int16  | 2    | 36     |
| Vessel Z Rate                       | Vessel body Z rotation rate, positive clockwise                        | 2^14 = 90 °/s | int16  | 2    | 38     |
| Delayed Heave Time, Seconds         | UTC Time in seconds that timestamps the delayed heave information      | Seconds       | int32  | 4    | 40     |
| Delayed Heave Time, Fraction of Sec.| UTC Time fraction of a second that timestamps the delayed heave info   | 0.0001 sec    | uint16 | 2    | 44     |
| Delayed Heave                       | Delayed heave, positive down                                           | Centimeters   | int16  | 2    | 46     |
| Status Word                         | Device status bitmask (see [Status Word](#seapath-b26-status))         | -             | uint16 | 2    | 48     |
| Checksum                            | CRC of all bytes excluding header and checksum                         | -             | uint16 | 2    | 50     |

> [!WARNING]
> The Seapath Binary 26 message uses a different convention for heave measurements, where heave is positive downward.

### Time Format
The UTC time is divided into an integer seconds part and a fractional seconds part.  
The integer part is UNIX time counted from 1970-01-01 UTC.  

### Checksum

Checksum is calculated as a 16-bit Block Cyclic Redundancy Check of all bytes except the header and checksum.  
The CRC algorithm uses the polynomial 0x8408 with an initial value of 0xFFFF.

### Seapath Binary 26 Status Word {#seapath-b26-status}

The Status Word consists of 16 single-bit flags, numbered from 0 to 15, where position 0 is the least significant bit.  
If a bit is set (1), it indicates:

| Bit Position | Description                                                                                  |
|--------------|----------------------------------------------------------------------------------------------|
| 0            | Reduced horizontal position and velocity performance. Data is valid, but INS is not aligned. |
| 1            | Invalid horizontal position and velocity data as per user-defined quality thresholds.        |
| 2            | Reduced heave and vertical velocity performance. Heave is valid but not aided by INS.        |
| 3            | Invalid heave and vertical velocity data. The heave filter is not converged or has diverged. |
| 4            | Reduced roll and pitch performance. Data is valid, but the INS is not aligned.               |
| 5            | Invalid roll and pitch data as per user-defined quality thresholds.                          |
| 6            | Reduced heading performance. Heading is valid, but the INS is not aligned.                   |
| 7            | Invalid heading data as per user-defined quality thresholds.                                 |
| 8            | Invalid delayed heave data. The delayed heave filter has not converged.                      |

> [!NOTE]
> Validity thresholds for each parameter can be configured by the user in the device settings.

## SBG_ECOM_THIRD_PARTY_DOLOG_HRP (06) {#SBG_ECOM_THIRD_PARTY_DOLOG_HRP}

The `SBG_ECOM_THIRD_PARTY_DOLOG_HRP` message is a proprietary binary log that provides Euler angles (roll, pitch, heading) and vehicle/body rotation rates.  
This message is commonly used in stabilization systems where high-throughput attitude and rotational measurements are essential.

All fields in this log are stored in Big Endian format (MSB first), and signed data uses the 2's complement representation.

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_DOLOG_HRP (06)`
- **Compatibility:** AHRS/INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)
- **Total Size:** 16 bytes

| Field    | Description                                                               | Unit        | Format | Size | Offset |
|----------|---------------------------------------------------------------------------|-------------|--------|------|--------|
| Header   | Start of sentence – always 0x02                                           | -           | uint8  | 1    | 0      |
| Status   | Status bitmask (see [Status Bitmask](#dolog-hrp-status-bitmask))          | -           | uint8  | 1    | 1      |
| Heading  | Heading angle [0° to 359.99°]                                             | 2^15 = 180° | uint16 | 2    | 2      |
| Roll     | Roll angle, positive with port side up [-90 to 89.99°]                    | 2^15 = 90°  | int16  | 2    | 4      |
| Pitch    | Pitch angle, positive with bow up [-90 to 89.99°]                         | 2^15 = 90°  | int16  | 2    | 6      |
| Z Rate   | Rate about the body/vehicle Z axis, positive clockwise [-45 to +44.99°/s] | 2^15 = 45°/s | int16  | 2    | 8      |
| X Rate   | Rate about the body/vehicle X axis, positive clockwise [-45 to +44.99°/s] | 2^15 = 45°/s | int16  | 2    | 10     |
| Y Rate   | Rate about the body/vehicle Y axis, positive clockwise [-45 to +44.99°/s] | 2^15 = 45°/s | int16  | 2    | 12     |
| Checksum | Negative sum of all the bytes before the checksum, ignoring overflow      | -           | uint8  | 1    | 14     |
| Footer   | End of frame byte – always 0x03                                           | -           | uint8  | 1    | 15     |

### Status Bitmask {#dolog-hrp-status-bitmask}

The Status Word is an 8-bit integer with various single-bit flags, numbered from 0 to 7, where position 0 is the least significant bit.  
Each bit indicates specific status information:

| Bit Position | Description                                                                                 |
|--------------|---------------------------------------------------------------------------------------------|
| 0            | Set if the Heading, Pitch & Roll measurements are valid (based on user-defined thresholds). |
| 1-3          | Not used                                                                                    |
| 4            | Set if the device is aligned and ready to deliver full accuracy data                        |
| 5            | Set if an error has been detected on the IMU (accelerometers or gyroscopes)                 |
| 6-7          | Not used                                                                                    |

## SBG_ECOM_THIRD_PARTY_AHRS_500 (07) {#SBG_ECOM_THIRD_PARTY_AHRS_500}

The `SBG_ECOM_THIRD_PARTY_AHRS_500` message is a proprietary binary log that ensures compatibility with the Crossbow AHRS-500 series.  
This log outputs essential data such as attitude (roll, pitch, yaw), rotation rates, accelerations, and device status, enabling seamless integration with AHRS systems.

All values are stored in Big Endian format (MSB first), with 16-bit signed integers represented in 2's complement.  
While this message mirrors Crossbow's format, SBG Systems has implemented certain conversions to align with its AHRS/INS technology.

Each data packet begins with a two-byte header (`0xAA 0x55`) and ends with a two-byte checksum, ensuring data integrity.  
The checksum is computed by summing all packet contents except the header and checksum bytes, and then taking the result modulo `0xFFFF`.

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_AHRS_500 (07)`
- **Compatibility:** AHRS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)
- **Total Size:** 26 bytes

| Field      | Description                                                                   | Resolution | Unit   | Format | Size | Offset |
|------------|-------------------------------------------------------------------------------|------------|--------|--------|------|--------|
| Header #1  | First header byte, always `0xAA`                                              | -          | -      | uint8  | 1    | 0      |
| Header #2  | Second header byte, always `0x55`                                             | -          | -      | uint8  | 1    | 1      |
| Roll       | Roll angle [-180° to 180°]                                                    | 180/2^15   | °      | int16  | 2    | 2      |
| Pitch      | Pitch angle [-90° to 90°]                                                     | 180/2^15   | °      | int16  | 2    | 4      |
| Heading    | Heading angle [-180° to 180°]                                                 | 180/2^15   | °      | int16  | 2    | 6      |
| Rate X     | X body rotation rate [-1200°/s to +1200°/s]                                   | 1200/2^15  | °/s    | int16  | 2    | 8      |
| Rate Y     | Y body rotation rate [-1200°/s to +1200°/s]                                   | 1200/2^15  | °/s    | int16  | 2    | 10     |
| Rate Z     | Z body rotation rate [-1200°/s to +1200°/s]                                   | 1200/2^15  | °/s    | int16  | 2    | 12     |
| Accel X    | X body acceleration [-15g to +15g], where 1g = 9.80 m/s²                      | 15/2^15    | G      | int16  | 2    | 14     |
| Accel Y    | Y body acceleration [-15g to +15g], where 1g = 9.80 m/s²                      | 15/2^15    | G      | int16  | 2    | 16     |
| Accel Z    | Z body acceleration [-15g to +15g], where 1g = 9.80 m/s²                      | 15/2^15    | G      | int16  | 2    | 18     |
| Model #    | Model number, always set to 226                                               | -          | -      | uint16 | 2    | 20     |
| Status BIT | Device status, see [Status BIT definitions](#status-bit-definitions)          | -          | -      | uint16 | 2    | 22     |
| Checksum   | Checksum computed as described above.                                         | -          | -      | uint16 | 2    | 24     |

> [!NOTE]
> SBG Systems products and the AHRS-500 both use the North-East-Down frame convention. Euler angles, rotation rates, and accelerations are consistent with SBG Systems' standards.

### Status BIT Definitions {#status-bit-definitions}

The Status BIT field contains several status indicators to monitor the health of the product.  
A bit set to `1` indicates an error or performance degradation, while a bit set to `0` reports normal or optimal operation.

Please note that due to architectural differences, Crossbow AHRS-500 status may not directly translate to SBG Systems products.

| Bit | Name               | Description                                                                                            |
|-----|--------------------|--------------------------------------------------------------------------------------------------------|
| 0   | Hard Failure       | `0`: Unit is functioning correctly; `1`: Fatal error detected, do not use unit data                    |
| 1   | Soft Failure       | Not implemented, always `0`                                                                            |
| 2   | Reserved           | Reserved, always `0`                                                                                   |
| 3   | Power Failure      | Not implemented, always `0`                                                                            |
| 4   | Comm Error         | `0`: Port A communication normal; `1`: Port A communication errors or saturation                       |
| 5   | Reboot Detect      | Not implemented, always `0`                                                                            |
| 6   | Calibration Failure| `0`: Sensor calibration valid; `1`: Sensor calibration corrupted, data invalid                         |
| 7   | External Aiding    | `0`: External air data aiding; `1`: External air data not present or not used in the AHRS/INS solution |
| 8   | Coordinated Turn   | Not implemented, always `0`                                                                            |
| 9   | Reserved           | Reserved, always `0`                                                                                   |
| 10  | Algorithm Status   | `0`: Roll and Pitch valid, alignment done; `1`: Alignment in progress or Roll/Pitch invalid            |
| 11  | Mag Align Status   | `0`: Heading valid and aligned; `1`: Heading invalid, free drifting or not aligned                     |
| 12  | Reserved           | Reserved, always `0`                                                                                   |
| 13  | Mag Calibration    | `0`: Magnetometers valid, used in AHRS/INS solution; `1`: Magnetometers not used or poorly calibrated  |
| 14  | Reserved           | Reserved, always `0`                                                                                   |
| 15  | Remote Mag         | Not implemented, always `0`                                                                            |

Here's the improved and reformatted version of the ADA Data Packet message:

## SBG_ECOM_THIRD_PARTY_ADA_01 (08) {#SBG_ECOM_THIRD_PARTY_ADA_01}

The `SBG_ECOM_THIRD_PARTY_ADA_01` message is a proprietary binary log that encapsulates all INS, IMU, and status data within a single, compact frame.  
This message is designed for efficient processing on low-end platforms by using only integer representations with specific scaling.

All values are stored in Big Endian format (MSB first) and are encoded using 16/32-bit signed integers in 2's complement representation or unsigned integers.  
The status indicators align with those defined in standard sbgECom logs, ensuring consistent behavior across different log types.

The data packet begins with a two-byte header (`0xAA 0x5A`) and includes a two-byte checksum to ensure data integrity.  
The checksum is calculated as the sum of all packet contents (excluding the header and checksum bytes) modulo `0xFFFF`.

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_ADA_01 (08)`
- **Compatibility:** INS and IMU capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)
- **Total Size:** 100 bytes

| Field           | Description                                                           | Resolution  | Unit    | Format | Size | Offset |
|-----------------|-----------------------------------------------------------------------|-------------|---------|--------|------|--------|
| Header #1       | First header byte, always `0xAA`                                      | -           | -       | uint8  | 1    | 0      |
| Header #2       | Second header byte, always `0x5A`                                     | -           | -       | uint8  | 1    | 1      |
| Roll            | INS Roll angle [-180° to 180°]                                        | 180/2^15    | °       | int16  | 2    | 2      |
| Pitch           | INS Pitch angle [-90° to 90°]                                         | 180/2^15    | °       | int16  | 2    | 4      |
| Heading         | INS Heading angle [-180° to 180°]                                     | 180/2^15    | °       | int16  | 2    | 6      |
| Rate X          | X body rotation rate [-1200°/s to +1200°/s]                           | 1200/2^15   | °/s     | int16  | 2    | 8      |
| Rate Y          | Y body rotation rate [-1200°/s to +1200°/s]                           | 1200/2^15   | °/s     | int16  | 2    | 10     |
| Rate Z          | Z body rotation rate [-1200°/s to +1200°/s]                           | 1200/2^15   | °/s     | int16  | 2    | 12     |
| Accel X         | X body acceleration [-15g to +15g], with 1g = 9.80 m/s²               | 15/2^15     | G       | int16  | 2    | 14     |
| Accel Y         | Y body acceleration [-15g to +15g], with 1g = 9.80 m/s²               | 15/2^15     | G       | int16  | 2    | 16     |
| Accel Z         | Z body acceleration [-15g to +15g], with 1g = 9.80 m/s²               | 15/2^15     | G       | int16  | 2    | 18     |
| Temperature     | Internal INS temperature [-50° to +100°]                              | 100/2^15    | °C      | int16  | 2    | 20     |
| Velocity N      | INS North Velocity [-1500 m/s to +1500 m/s]                           | 1500/2^31   | m/s     | int32  | 4    | 22     |
| Velocity E      | INS East Velocity [-1500 m/s to +1500 m/s]                            | 1500/2^31   | m/s     | int32  | 4    | 26     |
| Velocity D      | INS Down Velocity [-1500 m/s to +1500 m/s]                            | 1500/2^31   | m/s     | int32  | 4    | 30     |
| Latitude        | INS Latitude positive North [-90° to +90°]                            | 180/2^31    | °       | int32  | 4    | 34     |
| Longitude       | INS Longitude positive East [-180° to +180°]                          | 180/2^31    | °       | int32  | 4    | 38     |
| Altitude        | INS Altitude above MSL positive upward [-100000 m to +100000 m]       | 100000/2^31 | m       | int32  | 4    | 42     |
| Roll Std        | INS Roll accuracy 1σ standard deviation [0° to +180°]                 | 180/2^15    | °       | uint16 | 2    | 46     |
| Pitch Std       | INS Pitch accuracy 1σ standard deviation [0° to +180°]                | 180/2^15    | °       | uint16 | 2    | 48     |
| Heading Std     | INS Heading accuracy 1σ standard deviation [0° to +180°]              | 180/2^15    | °       | uint16 | 2    | 50     |
| Velocity N Std. | INS North Velocity accuracy 1σ standard deviation [0 m/s to +200 m/s] | 100/2^15    | m/s     | uint16 | 2    | 52     |
| Velocity E Std. | INS East Velocity accuracy 1σ standard deviation [0 m/s to +200 m/s]  | 100/2^15    | m/s     | uint16 | 2    | 54     |
| Velocity D Std. | INS Down Velocity accuracy 1σ standard deviation [0 m/s to +200 m/s]  | 100/2^15    | m/s     | uint16 | 2    | 56     |
| Latitude Std.   | INS Latitude accuracy 1σ standard deviation [0 m to +3276 m]          | 0.05        |  m      | uint16 | 2    | 58     |
| Longitude Std.  | INS Longitude accuracy 1σ standard deviation [0 m to +3276 m]         | 0.05        | m       | uint16 | 2    | 60     |
| Altitude Std.   | INS Altitude accuracy 1σ standard deviation [0 m to +3276 m]          | 0.05        | m       | uint16 | 2    | 62     |
| Timestamp       | Free running time since INS powered up                                | -           | µs      | uint32 | 4    | 64     |
| General Status  | See GENERAL_STATUS definition                                         | -           | -       | uint16 | 2    | 68     |
| Com Status      | See COM_STATUS definition                                             | -           | -       | uint32 | 4    | 70     |
| Aiding Status   | See AIDING_STATUS definition                                          | -           | -       | uint32 | 4    | 74     |
| Up Time         | INS system up time since power on                                     | -           | s       | uint32 | 4    | 78     |
| IMU Status      | See IMU_STATUS definition                                             | -           | -       | uint16 | 2    | 82     |
| Solution Status | See SOLUTION_STATUS definition                                        | -           | -       | uint32 | 4    | 84     |
| Mag X           | X body magnetic field in Arbitrary Unit (AU)                          | 1/2^10      | AU      | int16  | 2    | 88     |
| Mag Y           | Y body magnetic field in Arbitrary Unit (AU)                          | 1/2^10      | AU      | int16  | 2    | 90     |
| Mag Z           | Z body magnetic field in Arbitrary Unit (AU)                          | 1/2^10      | AU      | int16  | 2    | 92     |
| Reserved        | Reserved bytes for future use                                         | -           | -       | -      | 4    | 94     |
| Checksum        | Checksum computed as described above.                                 | -           | -       | uint16 | 2    | 98     |

> [!NOTE]
> SBG Systems products and ADA Data Packet messages use the same conventions and frame definitions, ensuring seamless integration.

## SBG_ECOM_THIRD_PARTY_AT_ITINS (09) {#SBG_ECOM_THIRD_PARTY_AT_ITINS}

The `SBG_ECOM_THIRD_PARTY_AT_ITINS` message is a proprietary ASCII log designed to provide all essential navigation and orientation data required by the AVIATOR UAV 200 Satcom terminal.  
The Cobham Aviator UAV 200 utilizes SBG Systems' INS data to precisely align with the satellite, thereby enhancing transmission range and reliability.

The message begins with the header `AT_ITINS=` and ends with `<CR><LF>`.  
Each parameter is separated by a comma, and the total message length does not exceed 128 characters.

### Example Message
```
AT_ITINS=48.9102,2.1677,66,112,2020-06-17/14:10:15,0,0,0,-1.6,0.2,0.1,127.3,126.3,0.1,0.2,0.0<CR><LF>
```

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_AT_ITINS (09)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)
- **Maximum Length:** 128 characters

| Field           | Description                                         | Decimal Digits | Unit  |
|-----------------|-----------------------------------------------------|----------------|-------|
| AT_ITINS=       | AT-Command header                                   | -              | -     |
| latitude        | INS latitude, positive North [-90 to +90]           | 4              | °     |
| longitude       | INS longitude, positive East [-180 to +180]         | 4              | °     |
| baroAltitude    | Barometric altitude [-500 to +25000]                | 0              | m     |
| height          | INS altitude above ellipsoid [-500 to +25000]       | 0              | m     |
| UTC Time        | UTC Time as a string: `yyyy-mm-dd/hh:mm:ss`         | -              | -     |
| velocityNorth   | INS north velocity, positive north [-500 to +500]   | 0              | m/s   |
| velocityEast    | INS east velocity, positive east [-500 to +500]     | 0              | m/s   |
| gndSpeed        | INS ground 2D velocity [0 to +500]                  | 0              | m/s   |
| trackAngle      | INS track course, positive clockwise [-180 to +180] | 1              | °     |
| roll            | INS roll angle, positive right up [-180 to +180]    | 1              | °     |
| pitch           | INS pitch angle, positive nose up [-180 to +180]    | 1              | °     |
| heading         | INS heading, positive clockwise [-180 to +180]      | 1              | °     |
| magHeading      | Magnetic heading, positive clockwise [-180 to +180] | 1              | °     |
| rollRate        | Roll rate, positive right up [-90 to +90]           | 1              | °/s   |
| pitchRate       | Pitch rate, positive nose up [-90 to +90]           | 1              | °/s   |
| yawRate         | Yaw rate, positive clockwise [-90 to +90]           | 1              | °/s   |

> [!NOTE]
> The roll, pitch, and yaw rates are expressed in the device’s body/vehicle frame.  
> These measurements differ from IMU gyroscope outputs as they are compensated by the INS filter for residual sensor biases, scale factors, and the Earth’s rotation rate.

> [!NOTE]
> Fields that are unavailable or invalid are represented with a value of zero.

## SBG_ECOM_THIRD_PARTY_KONGSBERG_MB (10) {#SBG_ECOM_THIRD_PARTY_KONGSBERG_MB}

The `SBG_ECOM_THIRD_PARTY_KONGSBERG_MB` message is a proprietary binary log from Kongsberg MBES.  
This log is primarily used to support FM mode by integrating INS and heave data, which is essential for precise marine survey operations.

All fields in this log are stored in Little Endian format (LSB first). Signed data uses the 2's complement representation.

### Message Structure

- **Message Name (ID):** `SBG_ECOM_THIRD_PARTY_KONGSBERG_MB (10)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)
- **Total Size:** 132 bytes

| Field                   | Description                                                     | Unit        | Format | Size | Offset |
|-------------------------|-----------------------------------------------------------------|-------------|--------|------|--------|
| Start ID                | 4 chars header, always set to `#KMB`                            | -           | char   | 4    | 0      |
| Datagram Length         | Message length, always set to 132 bytes                         | -           | uint16 | 2    | 4      |
| Datagram Version        | Message version, always set to 1                                | -           | uint16 | 2    | 6      |
| UTC Seconds             | UTC time in seconds since epoch 1970-01-01                      | Seconds     | uint32 | 4    | 8      |
| UTC Nanoseconds         | UTC time second decimal part in nanoseconds                     | Nanoseconds | uint32 | 4    | 12     |
| Status Word             | Status and quality (see [KMB status](#kmb-status-word))         | -           | uint32 | 4    | 16     |
| Latitude                | INS latitude, positive North of the Equator                     | Degree      | double | 8    | 20     |
| Longitude               | INS longitude, positive East of Greenwich                       | Degree      | double | 8    | 28     |
| Ellipsoid Height        | INS height above ellipsoid, positive upward                     | Meter       | float  | 4    | 36     |
| Roll                    | INS roll angle, positive with port side up                      | Degree      | float  | 4    | 40     |
| Pitch                   | INS pitch angle, positive with bow up                           | Degree      | float  | 4    | 44     |
| Heading                 | INS true heading                                                | Degree      | float  | 4    | 48     |
| Heave                   | Real-time heave, positive downward                              | Meter       | float  | 4    | 52     |
| Roll Rate               | Vessel roll rate, positive with port side up                    | °/s         | float  | 4    | 56     |
| Pitch Rate              | Vessel pitch rate, positive with bow up                         | °/s         | float  | 4    | 60     |
| Yaw Rate                | Vessel yaw rate, positive clockwise                             | °/s         | float  | 4    | 64     |
| North Velocity          | INS north velocity, positive North                              | m/s         | float  | 4    | 68     |
| East Velocity           | INS east velocity, positive East                                | m/s         | float  | 4    | 72     |
| Down Velocity           | INS down velocity, positive Down                                | m/s         | float  | 4    | 76     |
| Latitude Error          | INS estimated 1-sigma latitude error                            | Meter       | float  | 4    | 80     |
| Longitude Error         | INS estimated 1-sigma longitude error                           | Meter       | float  | 4    | 84     |
| Height Error            | INS estimated 1-sigma height error                              | Meter       | float  | 4    | 88     |
| Roll Error              | INS estimated 1-sigma roll error                                | Degree      | float  | 4    | 92     |
| Pitch Error             | INS estimated 1-sigma pitch error                               | Degree      | float  | 4    | 96     |
| Heading Error           | INS estimated 1-sigma heading error                             | Degree      | float  | 4    | 100    |
| Heave Error             | INS estimated 1-sigma real-time heave error                     | Meter       | float  | 4    | 104    |
| North Acceleration      | INS north acceleration (gravity-free)                           | m/s²        | float  | 4    | 108    |
| East Acceleration       | INS east acceleration (gravity-free)                            | m/s²        | float  | 4    | 112    |
| Down Acceleration       | INS down acceleration (gravity-free)                            | m/s²        | float  | 4    | 116    |
| Delayed Heave UTC Sec   | UTC time integer seconds for delayed heave data                 | Seconds     | uint32 | 4    | 120    |
| Delayed Heave UTC Ns    | UTC time fractional seconds part for delayed heave data         | Nanoseconds | uint32 | 4    | 124    |
| Delayed Heave           | Delayed heave value, positive downward                          | Meter       | float  | 4    | 128    |

### Time Format

The UTC time is divided into an integer seconds part and a fractional second part. The integer part is UNIX time counted from 1970-01-01 UTC.

### KMB Status Word {#kmb-status-word}

The Status Word consists of 32 single-bit flags, numbered from 0 to 31, where position 0 is the least significant bit.  
A bit set to `1` indicates the following:

| Bit Position | Description                                                                                  |
|--------------|----------------------------------------------------------------------------------------------|
| 0            | Invalid INS position and velocity data, as defined by user quality thresholds.               |
| 1            | Invalid INS roll and pitch data, as defined by user quality thresholds.                      |
| 2            | Invalid INS heading data, as defined by user quality thresholds.                             |
| 3            | Invalid heave and vertical velocity data. The heave filter is not converged or has diverged. |
| 4            | Invalid INS acceleration data (NED). Same behavior as invalid INS position and velocity.     |
| 5            | Invalid delayed heave data. The delayed heave filter has not converged.                      |
| 16           | Reduced INS position and velocity data. Data is valid but the INS is not aligned yet.        |
| 17           | Reduced INS roll and pitch data. Data is valid but the INS is not aligned yet.               |
| 18           | Reduced INS heading data. Data is valid but the INS is not aligned yet.                      |
| 19           | Reduced heave and vertical velocity data. The heave is valid but not aided by INS.           |
| 20           | Reduced INS acceleration data (NED). Same behavior as reduced INS position and velocity.     |
| 21           | Reduced delayed heave data. The delayed heave is valid but not aided by INS.                 |

> [!NOTE]
> Validity thresholds for each parameter can be configured by the user in the device settings.
