# NMEA 2000 Protocol & Messages {#nmea2000Msg}

SBG Systems’ AHRS and INS devices support output via the standard NMEA 2000 (N2K) protocol. NMEA 2000 is a widely adopted marine communication standard built on a Controller Area Network (CAN) bus architecture, designed to provide robust, real-time data exchange between multiple electronic devices.

Using NMEA 2000 messages enables seamless integration with a broad range of marine electronics such as chartplotters, autopilots, and monitoring systems without relying on proprietary interfaces. However, NMEA 2000 messages provide a predefined and standardized set of Parameter Group Numbers (PGNs),  are less efficient as sbgECom binary messages and don't give access to all the features, options and status of SBG Systems products.

Unlike the older NMEA 0183 standard, which uses ASCII text messages, NMEA 2000 is a binary protocol that operates over a CAN bus. Data is structured into Parameter Group Numbers (PGNs), where each PGN defines a specific message type and purpose for example, position, heading, or attitude.

## Key Features

* Binary protocol optimized for efficiency and reliability

* Multi-device communication over a shared CAN bus

* Higher data rates than NMEA 0183, enabling real-time sensor fusion

* Standardized PGNs to ensure interoperability across different manufacturers

* Built-in prioritization and error handling mechanisms

* Messages comply with official NMEA 2000 specifications and include PGNs relevant for navigation and motion data

## NMEA 2000 Standard Messages

NMEA 2000 messages are transmitted over CAN frames, each with a maximum payload of 8 bytes. For example, the CAN messages SBG_ECAN_MSG_NMEA_2000_SYSTEM_TIME and SBG_ECAN_MSG_NMEA_2000_HEART_BEAT together represent a single, coherent data set that must be interpreted collectively.

This approach allows NMEA 2000 to support complex datasets such as attitude, velocity, position, and sensor measurements, while maintaining the efficiency and robustness of the CAN bus.

By using standardized messages and PGNs, NMEA 2000 ensures that devices from different manufacturers can communicate seamlessly, while still allowing manufacturers like SBG Systems to expose additional proprietary features through their native protocols.

| Name (Log ID)                                             | Description                                                           | Firmware                                                  |
|-----------------------------------------------------------|-----------------------------------------------------------------------|-----------------------------------------------------------|
| SBG_ECAN_MSG_NMEA_2000_ATTITUDE (0x0DF11900)              | Orientation of the vessel.                                            | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
| SBG_ECAN_MSG_NMEA_2000_COG_SOG_RAPID_UPDATE (0x09F80200)  | Rapid update of Course Over Ground (COG) and Speed Over Ground (SOG). | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
| SBG_ECAN_MSG_NMEA_2000_HEART_BEAT (0x1DF01100)            | Transmitted to indicate the device presence on the network.           | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
| SBG_ECAN_MSG_NMEA_2000_HEAVE (0x0DF11400)                 | Vertical distance relative to the average sea level.                  | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
| SBG_ECAN_MSG_NMEA_2000_POSITION_RAPID_UPDATE (0x09F80100) | Rapid update of latitude and longitude.                               | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
| SBG_ECAN_MSG_NMEA_2000_RATE_OF_TURN (0x09F11300)          | Rate of change of the heading.                                        | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
| SBG_ECAN_MSG_NMEA_2000_SYSTEM_TIME (0x0DF01000)           | UTC time and date.                                                    | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
| SBG_ECAN_MSG_NMEA_2000_VESSEL_HEADING (0x09F11200)        | Vessel heading.                                                       | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |

## NMEA 2000 Fast Packet Messages

The NMEA 2000 Fast Packet protocol is a transport mechanism designed to efficiently transmit Parameter Group Numbers (PGNs) whose payload exceeds the standard 8-byte limit of a single CAN frame. Instead of relying on the larger and more complex multi-packet transport protocol, Fast Packet allows up to 223 bytes of data to be streamed while preserving the PGN identity and message priority in every transmitted frame.

The first frame transmitted uses 2 bytes to identify sequential Fast Packet parameter groups and sequential frames within a single parameter group transmission. The first byte contains a sequence counter to distinguish consecutive transmission of the same parameter groups and a frame counter set to frame zero. The second byte in the first frame identifies the total size of the parameter group to follow. Successive frames use just single data byte for the sequence counter and the frame counter.

| Name (Log ID)                                               | Description                                                                | Firmware                                                  |
|-------------------------------------------------------------|----------------------------------------------------------------------------|-----------------------------------------------------------|
| SBG_ECAN_MSG_NMEA_2000_GNSS_POSITION_DATA (0x0DF80500)      | Position and GNSS fix status.                                              | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
| SBG_ECAN_MSG_NMEA_2000_GNSS_SATS_IN_VIEW (0x19FA0400)       | Detailed information about visible satellites, including PRN, elevation, azimuth, and signal strength (SNR). | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
| SBG_ECAN_MSG_NMEA_2000_VESSEL_SPEED_COMPONENTS (0x09FE1200) | Longitudinal, transverse and sternward speed.                              | ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.4-blue) |
