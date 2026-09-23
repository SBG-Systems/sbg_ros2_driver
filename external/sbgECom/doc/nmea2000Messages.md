## NMEA 2000 Protocol & Messages {#nmea2000Msg}

SBG Systems’ AHRS and INS devices support output via the standard NMEA 2000 (N2K) protocol. NMEA 2000 is a widely adopted marine communication standard built on a Controller Area Network (CAN) bus architecture, designed to provide robust, real-time data exchange between multiple electronic devices.

Using NMEA 2000 messages enables seamless integration with a broad range of marine electronics such as chartplotters, autopilots, and monitoring systems without relying on proprietary interfaces. However, NMEA 2000 messages provide a predefined and standardized set of Parameter Group Numbers (PGNs),  are less efficient as sbgECom binary messages and don't give access to all the features, options and status of SBG Systems products.

## NMEA 2000 Protocol Overview

Unlike the older NMEA 0183 standard, which uses ASCII text messages, NMEA 2000 is a binary protocol that operates over a CAN bus. Data is structured into Parameter Group Numbers (PGNs), where each PGN defines a specific message type and purpose for example, position, heading, or attitude.

## Key Features:

* Binary protocol optimized for efficiency and reliability

* Multi-device communication over a shared CAN bus

* Higher data rates than NMEA 0183, enabling real-time sensor fusion

* Standardized PGNs to ensure interoperability across different manufacturers

* Built-in prioritization and error handling mechanisms

* Messages comply with official NMEA 2000 specifications and include PGNs relevant for navigation and motion data

## NMEA 2000 Messages Overview

NMEA 2000 messages are transmitted over CAN frames, each with a maximum payload of 8 bytes. For example, the CAN messages SBG_ECAN_MSG_NMEA_2000_SYSTEM_TIME and SBG_ECAN_MSG_NMEA_2000_HEART_BEAT together represent a single, coherent data set that must be interpreted collectively.

This approach allows NMEA 2000 to support complex datasets such as attitude, velocity, position, and sensor measurements, while maintaining the efficiency and robustness of the CAN bus.

By using standardized messages and PGNs, NMEA 2000 ensures that devices from different manufacturers can communicate seamlessly, while still allowing manufacturers like SBG Systems to expose additional proprietary features through their native protocols.

## NMEA 2000 Fast Packet Protocol Overview

The NMEA 2000 Fast Packet protocol is a transport mechanism designed to efficiently transmit Parameter Group Numbers (PGNs) whose payload exceeds the standard 8-byte limit of a single CAN frame. Instead of relying on the larger and more complex multi-packet transport protocol, Fast Packet allows up to 223 bytes of data to be streamed while preserving the PGN identity and message priority in every transmitted frame.

The first frame transmitted uses 2 bytes to identify sequential Fast Packet parameter groups and sequential frames within a single parameter group transmission. The first byte contains a sequence counter to distinguish consecutive transmission of the same parameter groups and a frame counter set to frame zero. The second byte in the first frame identifies the total size of the parameter group to follow. Successive frames use just single data byte for the sequence counter and the frame counter.
## Standard NMEA 2000 Messages


| Name (Log ID)                                                                                              | Description                                                                |
|------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------|
| [SBG_ECAN_MSG_NMEA_2000_ATTITUDE (0x0DF11900)](#SBG_ECAN_MSG_NMEA_2000_ATTITUDE)                           | Orientation of the vessel.                                                 |
| [SBG_ECAN_MSG_NMEA_2000_COG_SOG_RAPID_UPDATE (0x09F80200)](#SBG_ECAN_MSG_NMEA_2000_COG_SOG_RAPID_UPDATE)   | Rapid update of Course Over Ground (COG) and Speed Over Ground (SOG).      |
| [SBG_ECAN_MSG_NMEA_2000_HEART_BEAT (0x1DF01100)](#SBG_ECAN_MSG_NMEA_2000_HEART_BEAT)                       | Transmitted by all NMEA devices to indicate their presence on the network. |
| [SBG_ECAN_MSG_NMEA_2000_HEAVE (0x0DF11400)](#SBG_ECAN_MSG_NMEA_2000_HEAVE)                                 | Vertical distance relative to the average sea level.                       |
| [SBG_ECAN_MSG_NMEA_2000_POSITION_RAPID_UPDATE (0x09F80100)](#SBG_ECAN_MSG_NMEA_2000_POSITION_RAPID_UPDATE) | Rapid update of latitude and longitude.                                    |
| [SBG_ECAN_MSG_NMEA_2000_RATE_OF_TURN (0x09F11300)](#SBG_ECAN_MSG_NMEA_2000_RATE_OF_TURN)                   | Rate of change of the heading.                                             |
| [SBG_ECAN_MSG_NMEA_2000_SYSTEM_TIME (0x0DF01000)](#SBG_ECAN_MSG_NMEA_2000_SYSTEM_TIME)                     | UTC time and date.                                                         |
| [SBG_ECAN_MSG_NMEA_2000_VESSEL_HEADING (0x09F11200)](#SBG_ECAN_MSG_NMEA_2000_VESSEL_HEADING)               | True heading.                                                              |

---

### SBG_ECAN_MSG_NMEA_2000_ATTITUDE (0x0DF11900)

The `SBG_ECAN_MSG_NMEA_2000_ATTITUDE` message provides a regular transmission of attitude.

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_ATTITUDE (0x0DF11900)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 8 bytes

| Field          | Description         | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|----------------|---------------------|---------|------|--------|-------------|------------|--------|
| Sequence ID    | Generic counter     |    -    |  -   | uint8  |      1      |      -     |   0    |
| Yaw            | Yaw                 |  10^-4  | rad  | int16  |      2      |      -     |   1    |
| Pitch          | Pitch               |  10^-4  | rad  | int16  |      2      |      -     |   3    |
| Roll           | Roll                |  10^-4  | rad  | int16  |      2      |      -     |   5    |
| NMEA Reserved  | Reserved field      |    -    |  -   | uint8  |      1      |      -     |   7    |

---

### SBG_ECAN_MSG_NMEA_2000_COG_SOG_RAPID_UPDATE (0x09F80200)

The `SBG_ECAN_MSG_NMEA_2000_COG_SOG_RAPID_UPDATE` message provides a regular transmission of Course Over Ground (COG) and Speed Over Ground (SOG).

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_COG_SOG_RAPID_UPDATE (0x09F80200)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 8 bytes

| Field              | Description              | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|--------------------|--------------------------|---------|------|--------|-------------|------------|--------|
| Sequence ID        | Generic counter          |    -    |  -   | uint8  |      1      |      -     |   0    |
| COG Reference      | Direction reference      |    -    |  -   | uint8  |      -      |      2     |   -    |
| NMEA Reserved      | Reserved field           |    -    |  -   | uint8  |      -      |      6     |   1    |
| Course Over Ground | Course-Over-Ground (COG) |  10^-4  | rad  | uint16 |      2      |      -     |   2    |
| Speed Over Ground  | Generic Speed            |  10^-2  | m/s  | uint16 |      2      |      -     |   4    |
| NMEA Reserved      | Reserved field           |    -    |  -   | uint16 |      2      |      -     |   6    |

---

### SBG_ECAN_MSG_NMEA_2000_HEART_BEAT (0x1DF01100)

The `SBG_ECAN_MSG_NMEA_2000_HEART_BEAT` message confirms that a device is still present on the network.

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_HEART_BEAT (0x1DF01100)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 8 bytes

| Field                                  | Description     | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|----------------------------------------|-----------------|---------|------|--------|-------------|------------|--------|
| Update Rate                            | Data interval   |  10^-3  |  s   | uint16 |      2      |      -     |   0    |
| Heartbeat Sequence Counter             | Generic counter |    -    |  -   | uint8  |      1      |      -     |   2    |
| Class 1 CAN Controller State           | CAN state       |    -    |  -   | uint8  |      -      |      2     |   -    |
| Class 2 Second CAN Controller State    | CAN state       |    -    |  -   | int32  |      -      |      2     |   -    |
| Equipment Status                       | Device status   |    -    |  -   | uint8  |      -      |      2     |   -    |
| NMEA Reserved                          | Reserved field  |    -    |  -   | uint8  |      -      |      2     |   3    |
| NMEA Reserved                          | Reserved field  |    -    |  -   | uint32 |      4      |      -     |   4    |

---

### SBG_ECAN_MSG_NMEA_2000_HEAVE (0x0DF11400)

The `SBG_ECAN_MSG_NMEA_2000_HEAVE` message provides a regular transmission of vertical displacement (heave).

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_HEAVE (0x0DF11400)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 8 bytes

| Field           | Description               | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|-----------------|---------------------------|---------|------|--------|-------------|------------|--------|
| Sequence ID     | Generic counter           |    -    |  -   | uint8  |      1      |      -     |   0    |
| Heave           | Heave                     |  10^-2  |  -   | uint8  |      2      |      -     |   1    |
| Delay           | Delay                     |  10^-2  |  -   | uint8  |      2      |      -     |   3    |
| Delay Source    | Measurement Delay Source  |    -    |  -   | int32  |      -      |      4     |   -    |
| NMEA Reserved   | Reserved field            |    -    |  -   | uint8  |      -      |      20    |   5    |

---

### SBG_ECAN_MSG_NMEA_2000_RATE_OF_TURN (0x09F11300)

The `SBG_ECAN_MSG_NMEA_2000_RATE_OF_TURN` message provides a regular transmission of the vessel’s rate of turn (angular velocity around the vertical axis).

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_RATE_OF_TURN (0x09F11300)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 8 bytes

| Field          | Description         | Scaling       | Unit  | Format | Size(bytes) | Size(bits) | Offset |
|----------------|---------------------|---------------|-------|--------|-------------|------------|--------|
| Sequence ID    | Generic counter     |      -        |   -   | uint8  |      1      |      -     |   0    |
| Rate of Turn   | Rate of Turn        | 1/32 * 10^-7  | rad/s | int32  |      4      |      -     |   1    |
| NMEA Reserved  | Reserved field      |      -        |   -   | uint8  |      3      |      -     |   5    |

---

### SBG_ECAN_MSG_NMEA_2000_POSITION_RAPID_UPDATE (0x09F80100)

The `SBG_ECAN_MSG_NMEA_2000_POSITION_RAPID_UPDATE` message provides a regular transmission of position data, including latitude and longitude.

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_POSITION_RAPID_UPDATE (0x09F80100)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 8 bytes

| Field          | Description         | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|----------------|---------------------|---------|------|--------|-------------|------------|--------|
| Latitude       | Latitude, WGS-84    |  10^-7  | deg  | int32  |      4      |      -     |   0    |
| Longitude      | Longitude, WGS-84   |  10^-7  | deg  | int32  |      4      |      -     |   4    |

---

### SBG_ECAN_MSG_NMEA_2000_SYSTEM_TIME (0x0DF01000)

The `SBG_ECAN_MSG_NMEA_2000_SYSTEM_TIME` message provides a regular transmission of UTC time and date.

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_SYSTEM_TIME (0x0DF01000)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 8 bytes

| Field          | Description         | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|----------------|---------------------|---------|------|--------|-------------|------------|--------|
| Sequence ID    | Generic counter     |    -    |  -   | uint8  |      1      |      -     |   0    |
| Source         | Time source         |    -    |  -   | uint8  |      -      |      4     |   -    |
| NMEA Reserved  | Reserved field      |    -    |  -   | uint8  |      -      |      4     |   1    |
| Date           | Generic date        |    -    | day  | uint16 |      2      |      -     |   2    |
| Time           | Generic time of day |  10^-4  |  s   | uint32 |      4      |      -     |   4    |

---

### SBG_ECAN_MSG_NMEA_2000_VESSEL_HEADING (0x09F11200)

The `SBG_ECAN_MSG_NMEA_2000_VESSEL_HEADING` message represents the vessel’s heading, indicating the direction the boat’s bow is pointing relative to true north.

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_VESSEL_HEADING (0x09F11200)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 8 bytes

| Field                    | Description                 | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|--------------------------|-----------------------------|---------|------|--------|-------------|------------|--------|
| Sequence ID              | Generic counter             |    -    |  -   | uint8  |      1      |      -     |   0    |
| Heading Sensor Reading   | Heading Sensor Reading      |  10^-4  | rad  | uint16 |      2      |      -     |   1    |
| Deviation                | Magnetic Heading Correction |  10^-4  | rad  | int16  |      2      |      -     |   3    |
| Variation                | Magnetic Heading Correction |  10^-4  | rad  | int16  |      2      |      -     |   5    |
| Heading Sensor Reference | Direction reference         |    -    |  -   | uint8  |      -      |      2     |   -    |
| NMEA Reserved            | Reserved field              |    -    |  -   | uint8  |      -      |      6     |   7    |
## NMEA 2000 Fast Packets Messages


| Name (Log ID)                                                                                              | Description                                                                |
|------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------|
| [SBG_ECAN_MSG_NMEA_2000_GNSS_POSITION_DATA (0x0DF80500)](#SBG_ECAN_MSG_NMEA_2000_GNSS_POSITION_DATA)                    | Position and GNSS fix status.          |
| [SBG_ECAN_MSG_NMEA_2000_GNSS_SATS_IN_VIEW (0x19FA0400)](#SBG_ECAN_MSG_NMEA_2000_GNSS_SATS_IN_VIEW)                      | Detailed information about satellites currently visible to the GNSS receiver, including PRN, elevation, azimuth, and signal strength (SNR).    |
| [SBG_ECAN_MSG_NMEA_2000_VESSEL_SPEED_COMPONENTS (0x09FE1200)](#SBG_ECAN_MSG_NMEA_2000_VESSEL_SPEED_COMPONENTS)          | Longitudinal, transverse and sternward speed.    |

---

### SBG_ECAN_MSG_NMEA_2000_GNSS_POSITION_DATA (0X0DF80500)

The `SBG_ECAN_MSG_NMEA_2000_GNSS_POSITION_DATA` message provides a position and GNSS fix status.

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_GNSS_POSITION_DATA (0X0DF80500)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 43 + 4 * (Number of Reference Stations) in bytes

| Field                              | Description                                                        | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|------------------------------------|--------------------------------------------------------------------|---------|------|--------|-------------|---------------|--------|
| Sequence ID                        | Generic counter                                                    |    -    |  -   | uint8  |      1      |       -       |   0    |
| Position date                      | Generic date                                                       |    -    | day  | uint16 |      2      |       -       |   1    |
| Position time                      | Generic time of day                                                |  10^-4  |  s   | uint32 |      4      |       -       |   3    |
| Latitude                           | Latitude (Extended Resolution)                                     |  10^-16 | deg  | int64  |      8      |       -       |   7    |
| Longitude                          | Longitude (Extended Resolution)                                    |  10^-16 | deg  | int64  |      8      |       -       |   15   |
| Altitude                           | Altitude (Extended Resolution)                                     |  10^-6  |  m   | int64  |      8      |       -       |   23   |
| Type of System                     | GNSS constellation system                                          |    -    |  -   | uint8  |      -      |       4       |   31   |
| Method, GNSS                       | GNSS Quality                                                       |    -    |  -   | uint8  |      -      |       4       |   -    |
| Integrity                          | GNSS Integrity                                                     |    -    |  -   | uint8  |      -      |       2       |   32   |
| NMEA Reserved                      | Reserved field                                                     |    -    |  -   | uint8  |      -      |       6       |   -    |
| Number of SVs                      | Number of satellites used in the position solution                 |    -    |  -   | uint8  |      1      |       -       |   33   |
| HDOP                               | Horizontal dilution of precision                                   |  10^-2  |  -   | int16  |      2      |       -       |   34   |
| PDOP                               | Position dilution of precision                                     |  10^-2  |  -   | int16  |      2      |       -       |   36   |
| Geoidal Separation                 | Difference between geoid and WGS84 ellipsoid                       |  10^-2  |  m   | int32  |      4      |       -       |   38   |
| Number of Reference Stations       | Number of DGNSS reference stations used                            |    -    |  -   | uint8  |      1      |       -       |   42   |
| Reference Station Type "1"         | Type of the first DGNSS reference station                          |    -    |  -   | uint8  |      -      |       4       |   -    |
| Reference Station ID "1"           | Identifier of the first DGNSS reference station                    |    -    |  -   | uint16 |      -      |       12      |   44   |
| Age of DGNSS Corrections "1"       | Age of corrections received from the first DGNSS reference station |  10^-2  |  s   | uint16 |      2      |       -       |   46   |
| Reference Station Type "n"         | Type of the nth DGNSS reference station                            |    -    |  -   | uint8  |      -      |       4       |   -    |
| Reference Station ID "n"           | Identifier of the nth DGNSS reference station                      |    -    |  -   | uint16 |      -      |       12      |   -    |
| Age of DGNSS Reference Station "n" | Age of corrections received from the nth DGNSS reference station   |  10^-2  |  s   | uint16 |      2      |       -       |   48   |

---

### SBG_ECAN_MSG_NMEA_2000_GNSS_SATS_IN_VIEW (0x19FA0400)

The ` BG_ECAN_MSG_NMEA_2000_GNSS_SATS_IN_VIEW` message provides a detailed information about satellites currently visible to the GNSS receiver, including PRN, elevation, azimuth, and signal strength (SNR).

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_GNSS_SATS_IN_VIEW (0x19FA0400)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 3 + 12 * (Number of SVs) in bytes

| Field                 | Description                                 | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|-----------------------|---------------------------------------------|---------|------|--------|-------------|---------------|--------|
| Sequence ID           | Generic counter                             |    -    |  -   | uint8  |      1      |       -       |   0    |
| Mode                  | GNSS operating mode                         |    -    |  -   | uint8  |      -      |       2       |   1    |
| NMEA Reserved         | Reserved field                              |    -    |  -   | uint8  |      -      |       6       |   -    |
| Number of SVs         | Total number of visible satellites          |    -    |  -   | uint8  |      1      |       -       |   2    |
| PRN "1"               | PRN identifier of satellite                 |    -    |  -   | uint8  |      1      |       -       |   3    |
| Elevation "1"         | Elevation angle of satellite                |  10^-4  | rad  | int16  |      2      |       -       |   4    |
| Azimuth "1"           | Azimuth angle of satellite                  |  10^-4  | rad  | uint16 |      2      |       -       |   6    |
| SNR "1"               | Signal-to-noise ratio of satellite          |  10^-2  | dB   | int16  |      2      |       -       |   8    |
| Range Residuals "1"   | Pseudorange/range residual for satellite    |  10^-5  | m    | int32  |      4      |       -       |   10   |
| PRN Status "1"        | Status flags of satellite                   |    -    |  -   | uint8  |      -      |       4       |   14   |
| NMEA Reserved         | Reserved field                              |    -    |  -   | uint8  |      -      |       4       |    -   |
| PRN "n"               | PRN identifier of satellite                 |    -    |  -   | uint8  |      1      |       -       |   15   |
| Elevation "n"         | Elevation angle of satellite                |  10^-4  | rad  | int16  |      2      |       -       |   16   |
| Azimuth "n"           | Azimuth angle of satellite                  |  10^-4  | rad  | uint16 |      2      |       -       |   18   |
| SNR "n"               | Signal-to-noise ratio of satellite          |  10^-2  | db   | int16  |      2      |       -       |   20   |
| Range Residuals "n"   | Pseudorange/range residual for satellite    |  10^-5  | m    | int32  |      4      |       -       |   22   |
| PRN Status "n"        | Status flags of satellite                   |    -    |  -   | uint8  |      -      |       4       |   26   |
| NMEA Reserved         | Reserved field                              |    -    |  -   | uint8  |      -      |       4       |    -   |

---

### SBG_ECAN_MSG_NMEA_2000_VESSEL_SPEED_COMPONENTS (0x09FE1200)

The `SBG_ECAN_MSG_NMEA_2000_VESSEL_SPEED_COMPONENTS` message provides a longitudinal, transverse and sternward speed.

#### Message Structure

- **Message Name (ID):** `SBG_ECAN_MSG_NMEA_2000_VESSEL_SPEED_COMPONENTS (0x09FE1200)`
- **Compatibility:** limited to ELLIPSE products only.
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.7-blue)

- **Payload Size:** 12 bytes

| Field                                 | Description                                                  | Scaling | Unit | Format | Size(bytes) | Size(bits) | Offset |
|---------------------------------------|--------------------------------------------------------------|---------|------|--------|-------------|---------------|--------|
| Longitudinal Speed, Water-referenced  | Vessel longitudinal speed relative to the surrounding water  |  10^-3  | m/s  | int16  |      2      |       -       |   0    |
| Transverse Speed, Water-referenced    | Vessel transverse speed relative to the surrounding water    |  10^-3  | m/s  | int16  |      2      |       -       |   2    |
| Longitudinal Speed, Ground-referenced | Vessel longitudinal speed relative to the ground             |  10^-3  | m/s  | int16  |      2      |       -       |   4    |
| Transverse Speed, Ground-referenced   | Vessel transverse speed relative to the ground               |  10^-3  | m/s  | int16  |      2      |       -       |   6    |
| Stern Speed, Water-referenced         | Vessel sternward speed relative to the surrounding water     |  10^-3  | m/s  | int16  |      2      |       -       |   8    |
| Stern Speed, Ground-referenced        | Vessel sternward speed relative to the ground                |  10^-3  | m/s  | int16  |      2      |       -       |   10   |


