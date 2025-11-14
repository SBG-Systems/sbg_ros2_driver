## External Aiding Protocols

Inertial Navigation Systems (INS) can interface with various external equipment to enhance their accuracy by integrating aiding information.  
The most common external aiding device is a GNSS receiver, which typically provides data using the NMEA standard protocol.  
However, for optimal accuracy and robustness, SBG Systems also supports native binary protocols from leading third-party equipment manufacturers.

This section provides details on the supported third-party equipment and protocols, as well as their integration with SBG Systems' INS products.  
Note that the availability of these input protocols varies by product family and type and you should check your product dedicated user manual.

### sbgECom Input Protocol

The sbgECom protocol allows the INS to receive specific aiding information, such as external velocity or altitude (airdata).  
This binary protocol is compact and secure, making it ideal for injecting real-time aiding data.

The following sbgECom logs are accepted based on the aiding configuration:

- [SBG_ECOM_LOG_AIR_DATA (36)]{#SBG_ECOM_LOG_AIR_DATA}: Used by the AirData module to inject external barometric altitude and true airspeed.

> [!NOTE]
> Please refer to the dedicated Operating Handbooks for more details and code samples.

### NMEA Protocol

The NMEA protocol is commonly used for GPS aiding data in a read-only mode.  
The following NMEA sentences are expected for real-time INS integration:

| Message                   | Rate | Required | Description                                                                                                  |
|---------------------------|------|----------|--------------------------------------------------------------------------------------------------------------|
| **GGA**                   | 5 Hz | Required | GNSS position solution and type.                                                                             |
| **GST**                   | 5 Hz | Required | GNSS position accuracy.                                                                                      |
| **RMC**                   | 5 Hz | Required | GNSS velocity and course.                                                                                    |
| **HDT**                   | 5 Hz | Optional | Dual antenna true heading solution.                                                                          |
| **GSV**                   | 5 Hz | Required | Satellites in view with SNR, azimuth and elevation.                                                          |
| **ZDA**                   | 1 Hz | Required | GNSS UTC time and date information associated with the PPS signal.                                           |

> [!NOTE]
> Refer to the NMEA GNSS Integration Operating Handbook for detailed information.

### UBX Protocol

The UBX binary protocol from u-blox delivers optimal performance when integrating an external u-blox GNSS receiver with the INS.  
SBG Systems INS devices support u-blox generations 8, 9, and 10, as well as high-precision u-blox products like the F9P.

This protocol allows the INS to interface with both primary and secondary positioning engines.  
It enabled a single u-blox F9P receiver to compute both a true heading solution and a standalone PVT (Position, Velocity, Time) solution.

| Message               | Rate | Required | Description                                                                                                  |
|-----------------------|------|----------|--------------------------------------------------------------------------------------------------------------|
| **UBX-NAV-DOP**       | 1 Hz | Optional | Returns GNSS solution Dilution of Precision. Used primarily for NMEA-like outputs.                           |
| **UBX-NAV-RELPOSNED** | 5 Hz | Optional | Provides relative position between GNSS 1 and 2 for true heading determination.                              |
| **UBX-NAV-HPPOSLLH**  | 5 Hz | Optional | High-precision geodetic position solution, essential for achieving RTK-level accuracy.                       |
| **UBX-NAV-PVT**       | 5 Hz | Required | Standard precision navigation solution, including position, velocity, and time.                              |
| **UBX-NAV-SIG**       | 1 Hz | Optional | Detailed signal information for each satellite.                                                              |
| **UBX-NAV-SAT**       | 1 Hz | Optional | Provides satellite data, including satellites in view, with azimuth and elevation.                           |
| **UBX-NAV-STATUS**    | 1 Hz | Optional | Reports receiver navigation status, useful for detecting and indicating spoofing events.                     |
| **UBX-NAV2-DOP**      | 1 Hz | Optional | Same as UBX_NAV_DOP but for the second positioning engine.                                                   |
| **UBX-NAV2-PVT**      | 5 Hz | Optional | Same as UBX_NAV_PVT but for the second positioning engine.                                                   |
| **UBX-NAV2-SIG**      | 1 Hz | Optional | Same as UBX_NAV_SIG but for the second positioning engine.                                                   |
| **UBX-NAV2-SAT**      | 1 Hz | Optional | Same as UBX_NAV_SAT but for the second positioning engine.                                                   |
| **UBX-NAV2-STATUS**   | 1 Hz | Optional | Same as UBX_NAV_STATUS but for the second positioning engine.                                                |
| **UBX-MON-RF**        | 1 Hz | Optional | RF information used to detect and report interface mitigation status, such as jamming.                       |
| **UBX-RXM-RAWX**      | 1 Hz | Optional | Multi-GNSS raw measurements supporting post-processing.                                                      |
| **UBX-RXM-SFRBX**     | 1 Hz | Optional | Broadcast navigation data subframe, used to extract broadcasted Ephemeris data.                              |
| **UBX-RXM-RTCM**      | 1 Hz | Optional | Status of the input real-time RTCM stream.                                                                   |

> [!WARNING]
> Support for external u-blox GNSS receivers is limited to ELLIPSE products only.

### Novatel Binary Protocol

The Novatel binary protocol is used to achieve the best performance when connecting an external Novatel GNSS receiver to the INS.  
The following messages are supported for real-time INS integration:

| Message                 | Rate     | Required     | Description                                                                                                      |
|-------------------------|----------|--------------|------------------------------------------------------------------------------------------------------------------|
| **BESTPOS**             | 5 Hz     | Required     | Best real-time position solution and accuracy.                                                                   |
| **PSRXYZ**              | 5 Hz     | Required     | Real-time doppler velocity and accuracy.                                                                         |
| **HEADING**             | 5 Hz     | Optional     | Dual antenna true heading and pitch angles with accuracies.                                                      |
| **TIME**                | 1 Hz     | Required     | GNSS UTC time and date information associated with the PPS signal.                                               |
| **SATVIS2**             | 1 Hz     | Optional     | Satellite in view with azimuth/elevation and tracked signals.                                                    |

Additionally, the following messages are stored as part of the GNSS RAW stream to support post-processing:
  - **RANGECMP**
  - **RANGECMP2**
  - **RANGECMP4**
  - **RAWEPHEM**
  - **GPSEPHEMERIS**
  - **IONUTC**
  - **GLOEPHEMERIS**
  - **QZSSEPHEMERIS**
  - **BDSEPHEMERIS**
  - **GALEPHEMERIS**
  - **GALFNAVEPHEMERIS**
  - **GALINAVEPHEMERIS**
  - **BDSIONO**
  - **QZSSIONUTC**
  - **GALIONO**

> [!NOTE]
> Refer to the Novatel GNSS Integration Operating Handbook for more information.

### Septentrio SBF Binary Protocol

The INS integrates with the Septentrio SBF binary protocol for GNSS data aiding.  
The following messages are supported for real-time INS integration:

| Message                 | Rate     | Required     | Description                                                                                                      |
|-------------------------|----------|--------------|------------------------------------------------------------------------------------------------------------------|
| **PVTGeodetic**         | 5 Hz     | Required     | Real-time position and velocity solution.                                                                        |
| **PosCovGeodetic**      | 5 Hz     | Required     | Real-time position solution accuracy.                                                                            |
| **VelCovGeodetic**      | 5 Hz     | Required     | Real-time velocity solution accuracy.                                                                            |
| **AttEuler**            | 5 Hz     | Optional     | Dual antenna true heading and pitch angles.                                                                      |
| **AttCovEuler**         | 5 Hz     | Optional     | Accuracy of dual antenna true heading and pitch angles.                                                          |
| **AuxAntPositions**     | 1 Hz     | Optional     | Baseline measurement for dual antenna systems.                                                                   |
| **ReceiverTime**        | 1 Hz     | Required     | GNSS UTC time and date information associated with the PPS signal.                                               |
| **DOP**                 | 1 Hz     | Optional     | GNSS Dilution of Precision.                                                                                      |
| **ChannelStatus**       | 1 Hz     | Optional     | Reports all space vehicles in view with tracked/used signals.                                                    |
| **RFStatus**            | 1 Hz     | Optional     | Reports interference (jamming) detections and mitigation.                                                        |
| **GALAuthStatus**       | 1 Hz     | Optional     | Reports Galileo OSNMA authentication/spoofing status.                                                            |

Additionally, the following messages are stored as part of the GNSS RAW stream to support post-processing:

  - **MeasEpoch**
  - **Meas3Ranges**
  - **Meas3CN0HiRes**
  - **Meas3Doppler**
  - **Meas3PP**
  - **Meas3MP**
  - **EndOfMeas**
  - **GPSNav**
  - **GPSIon**
  - **GPSUtc**
  - **GLONav**
  - **GLOTime**
  - **GALNav**
  - **GALIon**
  - **GALUtc**
  - **GALGstGps**
  - **BDSNav**
  - **BDSIon**
  - **BDSUtc**
  - **QZSNav**

> [!NOTE]
> Refer to the Septentrio GNSS Integration Operating Handbook for more details.

### Trimble GSOF Binary Protocol

The Trimble General Serial Output Format (GSOF) provides the necessary information for the INS to compute a real-time navigation solution.  
For post-processing, RT-27 frame parsing and storage are supported.

The following GSOF messages are supported:

| Message                   | Rate | Required | Description                                                                                                  |
|---------------------------|------|----------|--------------------------------------------------------------------------------------------------------------|
| **PositionTime (0x01)**   | 5 Hz | Required | Time of the navigation solution.                                                                             |
| **LatLongHeight (0x02)**  | 5 Hz | Required | Latitude, longitude, and altitude above the ellipsoid.                                                       |
| **Velocity (0x08)**       | 5 Hz | Required | 3D velocity information.                                                                                     |
| **PositionSigma (0x0C)**  | 5 Hz | Required | The real time PVT solution accuracy.                                                                         |
| **PositionType (0x26)**   | 5 Hz | Optional | The type of real time PVT solution.                                                                          |
| **AttitudeInfo (0x1B)**   | 5 Hz | Optional | Dual antenna-based heading.                                                                                  |
| **CurrentUtcTime (0x10)** | 1 Hz | Required | GNSS UTC time and date information associated with the PPS signal.                                           |
| **BriefAllSv (0x21)**     | 1 Hz | Optional | Space vehicle information brief information.                                                                 |
| **SvInfoAll (0x22)**      | 1 Hz | Optional | Space vehicle information detailed information - recommended.                                                |
| **SvInfoFull (0x30)**     | 1 Hz | Optional | Space vehicle information detailed information over several pages.                                           |

> [!NOTE]
> Refer to the Trimble GNSS Integration Operating Handbook for more details.

### Teledyne RDI PD0 Protocol

The PD0 binary frame is a Teledyne RDI proprietary log that outputs DVL data, including bottom tracking, water tracking, and water profiling.  
It is accepted as input for DVL aiding purposes.

### Teledyne RDI PD6 Protocol

The PD6 ASCII frame is a Teledyne RDI proprietary log that outputs DVL data, such as bottom tracking and water tracking.  
It is accepted as input for DVL aiding purposes.

### Teledyne RDI Wayfinder Protocol

The Wayfinder protocol, a Teledyne RDI proprietary protocol for their new miniature DVL, supports bottom tracking velocity information.  
It is accepted as input for DVL aiding purposes.
