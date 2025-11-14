# NMEA Protocol & Messages {#nmeaMsg}

SBG Systems AHRS and INS support input and output standard NMEA protocol.  
The NMEA protocol is a an well known and simple ASCII based format that is widely used in different industry.

Using NMEA output messages allows straightforward and easy integration in a wide range of products and software without requiring any development.

However, NMEA outputs are less efficient as sbgECom binary messages and don't give access to all the features, options and status of SBG Systems products.

## NMEA Protocol Overview

The implemented NMEA sentences are based on NMEA 0183 Version 4.1.

### NMEA Sentence Structure

The NMEA sentence structure is detailed below with an example:

```
$GPZDA,201530.00,04,07,2002,00,00*60<CR><LF>
$<ID><MSG>[,<param>,<param>]*<CHK><CR><LF>
```

| Value        | Description                                                                                                 | Example                       |
|--------------|-------------------------------------------------------------------------------------------------------------|-------------------------------|
| `$`          | All frames start with a dollar sign (`$`).                                                                  | `$`                           |
| `<ID>`       | Talker ID, e.g., `GP` for GPS, `GL` for GLONASS, etc.                                                       | `GP`                          |
| `<MSG>`      | Sentence type, message name.                                                                                | `ZDA`                         |
| `[,<param>]` | Array of parameters separated by commas (`,`).                                                              | `,201530.00,04,07,2002,00,00` |
| `*<CHK>`     | Two characters 8-bit hex value checksum following `*` separator. (see []()).                                | `*60`                         |
| `<CR><LF>`   | All frames end with a carriage return and line feed.                                                        | `<CR><LF>`                    |

**Talker ID**  
The NMEA talker ID can be configured for each output interface.  
When NMEA messages are used to input aiding data such as an external GNSS, the AHRS/INS simply ignore the Talker ID field.

**Empty Fields**  
Each data field is comma-separated. When a field value is not defined or not available, the field is left empty, as shown below:

```
$GPZDA,,,,,,*XX<CR><LF>
```

### NMEA Checksum

The checksum for an NMEA sentence is calculated by XORing all characters between the `$` and `*` (exclusive).  
The following C code snippet demonstrates how to compute this checksum:

```c
/*!
 * Computes the NMEA 0183 checksum for a given NMEA sentence.
 *
 * The input sentence should be a complete NMEA message up to the checksum separator '*' 
 * (e.g., "$GPZDA,,,,,,*").
 * 
 * \param[in]  pSentence    Pointer to the NMEA sentence (NULL-terminated C string).
 * \return                  The computed checksum as an 8-bit unsigned integer.
 */
uint8_t nmea0183Checksum(const char *pSentence)
{
  uint8_t		checksum = 0;

  assert(pSentence);

  //
  // Start at index 1 to skip the leading '$'
  //
  for (size_t i = 1; (pSentence[i] != '*') && (pSentence[i] != '\0'); i++)
  {
    checksum ^= (uint8_t)pSentence[i];
  }

  return checksum;
}
```

### NMEA Types & Conventions

To simplify NMEA message definitions, we use specific conventions for representing both integer and decimal numbers.

#### Integer Numbers

- **Format**: Integer numbers are represented using the character `i`.  
              The number of `i` characters indicates the maximum number of digits that can be used to represent the integer.
- **Negative Integers**: The character `-` is prepended to denote a negative integer.
- **Example**: The format `iii` could represent the following integers: `-234`, `13`, `-3`.

#### Decimal Numbers

- **Format**: Decimal numbers are represented using the character `f`.  
              The character `.` is used to separate the integer part from the decimal part.  
              The number of `f` characters indicates the maximum number of digits that can be used for both the integer and decimal parts.
- **Sign**: The characters `-` or `+` can be prepended to represent negative or positive decimal numbers, respectively.
- **Example**: The format `ff.fff` could represent the following decimal numbers: `-34.2`, `1.205`, `24.126`.

#### Time Convention

- **Format**: Time in NMEA messages is always expressed in UTC. It follows the format `hhmmss.ss`.
- **Example**: The time `125430.0` corresponds to `12:54` and `30.00` seconds in UTC.

#### Date Convention

- **Format**: Date in NMEA messages is expressed in the format `ddmmyy`, where:
  - `dd` is the day of the month (01 to 31),
  - `mm` is the month (01 to 12),
  - `yy` is the last two digits of the year.
- **Example**: The date `070422` corresponds to `07 April 2022`.

#### Latitude and Longitude

- **Format**: Latitude and longitude are represented in the format `ddmm.mmmm`, where:
  - `dd` is degrees (00 to 90),
  - `mm.mmmm` is minutes with a fractional part,
  - It is followed by the hemisphere identifier `N` for North, `S` for South, `E` for East or `W` for West.
- **Example**: The latitude `4807.038,N` corresponds to `48 degrees, 07.038 minutes North`.

#### Status Indicators

- **Format**: Status indicators are typically represented by a single letter or number to denote the status of data or operations.  
              Common examples include:
                - `A` for Active or valid data,
                - `V` for Void or invalid data.
- **Example**: In a GGA sentence, the character `A` indicates that the GPS fix is active and the data is valid.

### Quality Indicators

In NMEA messages, quality indicators traditionally reflect the accuracy of a GNSS fix.  
However, when the data source is an INS, these indicators are interpreted differently based on the position standard deviation of the INS solution.  
The following table illustrates how these quality indicators are mapped from INS-derived horizontal accuracy to the NMEA standard.

When the data source is a GNSS, the standard DOP and quality indicator definitions apply.

| Horizontal Accuracy | NMEA Fix Type  | NMEA Status | GGA Quality | GGK Quality | RMC/VTG Mode | RMC Nav. Status |
|---------------------|----------------|-------------|-------------|-------------|--------------|-----------------|
| > 100 m             | Invalid Fix    | Not valid   | 0           | 0           | N            | V               |
| < 100 m             | Dead Reckoning | Caution     | 6           | 1           | E            | C               |
| < 10 m              | Standalone Fix | Safe        | 1           | 4           | A            | S               |
| < 1.2 m             | DGPS Fix       | Safe        | 2           | 2           | D            | S               |
| < 30 cm             | Floating RTK   | Safe        | 5           | 3           | F            | S               |
| < 10 cm             | Fixed RTK      | Safe        | 4           | 2           | R            | S               |

### DOP Computation

DOP (Dilution of Precision) is a quality indicator traditionally associated with GNSS, reflecting the geometry of the satellites and its impact on positional accuracy.  
When using an INS, DOP has no intrinsic meaning because the INS does not rely on satellite geometry.  
Instead, the INS provides more reliable quality indicators directly related to the estimated position standard deviation.

However, since some software still relies on DOP values to filter out outliers and inaccurate solutions, the INS can output a DOP value based on the estimated position standard deviation.  
This allows for backward compatibility with systems that use DOP for quality assessment, even though more modern and accurate indicators are available from the INS.

SBG Systems products implements the following computation to convert a position standard deviation to an horizontal DOP value:

$$DOP_{std} = \sqrt{North_{std}^2+East_{std}^2}$$
## Standard NMEA Messages

The `SBG_ECOM_CLASS_LOG_NMEA_0 (0x02)` message class encompasses all standard NMEA 0183 messages that can be output by the device.  
These messages implements the NMEA 0183 version 4.1 and can be used for GNSS drop in replacement in some integrations.

All NMEA outputs generated by the INS are based on the Inertial Navigation Solution and are not related directly to aiding data such as from a GNSS receiver.

For instance, even if a GNSS receiver is providing an RTK solution, if the INS is not aligned the product will output GGA messages with a quality indicator of 0 (invalid). This behavior is expected as an INS can still deliver RTK like accuracy solutions even during GNSS outages.

### Messages overview

The following list, provides a quick overview of all available logs for this message class.  
It briefly describe which parameters are contained in each output log.

| Name (Msg ID)                                        | Description                                                                            |
|------------------------------------------------------|----------------------------------------------------------------------------------------|
| [SBG_ECOM_LOG_NMEA_GGA (00)](#SBG_ECOM_LOG_NMEA_GGA) | Latitude, Longitude, Altitude, Quality indicator.                                      |
| [SBG_ECOM_LOG_NMEA_RMC (01)](#SBG_ECOM_LOG_NMEA_RMC) | Latitude, Longitude, velocity, course over ground.                                     |
| [SBG_ECOM_LOG_NMEA_ZDA (02)](#SBG_ECOM_LOG_NMEA_ZDA) | INS UTC date and time.                                                                 |
| [SBG_ECOM_LOG_NMEA_HDT (03)](#SBG_ECOM_LOG_NMEA_HDT) | Heading from true geographic North.                                                    |
| [SBG_ECOM_LOG_NMEA_GST (04)](#SBG_ECOM_LOG_NMEA_GST) | GPS Pseudorange Noise Statistics.                                                      |
| [SBG_ECOM_LOG_NMEA_VBW (05)](#SBG_ECOM_LOG_NMEA_VBW) | Water referenced and ground referenced speed data.                                     |
| [SBG_ECOM_LOG_NMEA_DPT (07)](#SBG_ECOM_LOG_NMEA_DPT) | Depth sensor output.                                                                   |
| [SBG_ECOM_LOG_NMEA_VTG (08)](#SBG_ECOM_LOG_NMEA_VTG) | Track an Speed over the ground.                                                        |
| [SBG_ECOM_LOG_NMEA_ROT (09)](#SBG_ECOM_LOG_NMEA_ROT) | Rate and direction of turn.                                                            |
| [SBG_ECOM_LOG_NMEA_GSV (10)](#SBG_ECOM_LOG_NMEA_GSV) | GNSS Satellites in View with azimuth, elevation and SNR information.                   |
| [SBG_ECOM_LOG_NMEA_GSA (11)](#SBG_ECOM_LOG_NMEA_GSA) | GNSS DOP and active satellites.                                                        |

---

### SBG_ECOM_LOG_NMEA_GGA (00) {#SBG_ECOM_LOG_NMEA_GGA}

The `SBG_ECOM_LOG_NMEA_GGA` message reports the Inertial Navigation System (INS) position and time in NMEA GGA format.

#### Message Format

```
$GPGGA,,,,,,0,00,20.0,,,,,,*7A<CR><LF>
$GPGGA,000010.00,4852.10719,N,00209.42313,E,0,00,0.0,-44.7,M,0.0,M,,,*63<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_GGA (00)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field | Name             | Format         | Description                                                             |
|-------|------------------|----------------|-------------------------------------------------------------------------|
| 0     | $##GGA           | string         | Message ID – GGA frame                                                  |
| 1     | Time             | hhmmss.ss      | UTC Time, current time                                                  |
| 2     | Latitude         | ddmm.mmmmmmmm  | Latitude: degree + minutes with 8 decimal digits                        |
| 3     | N/S              | char           | North / South indicator                                                 |
| 4     | Longitude        | dddmm.mmmmmmmm | Longitude: degree + minutes with 8 decimal digits                       |
| 5     | E/W              | char           | East / West indicator                                                   |
| 6     | Quality          | i              | Solution quality (see definition in Quality indicators)                 |
| 7     | SV used          | ii             | Number of satellites used in solution                                   |
| 8     | Horizontal DOP   | ff.f           | Horizontal dilution of precision (see definition in Quality indicators) |
| 9     | Altitude MSL     | ffff.fff       | Altitude above Mean Sea Level in meters                                 |
| 10    | M                | M              | Altitude unit (Meters) fixed field.                                     |
| 11    | Undulation       | fff.fff        | Geoidal separation between WGS-84 and MSL in meters.                    |
| 12    | M                | M              | Units for geoidal separation (Meters) fixed field.                      |
| 13    | Diff. Age        | s.s            | Age of differential corrections in seconds, only filled if applicable.  |
| 14    | Diff. station ID | rrrr           | Differential station ID, only filled if applicable.                     |
| 15    | Check sum        | *cs            | XOR of all previous bytes except $                                      |
| 16    | End of frame     | <CR><LF>       | Carriage return and line feed                                           |

> [!NOTE]
> The position quality and DOP are calculated using the INS position's estimated standard deviation, and are not directly related to the GNSS solution.

---

### SBG_ECOM_LOG_NMEA_RMC (01) {#SBG_ECOM_LOG_NMEA_RMC}

The `SBG_ECOM_LOG_NMEA_RMC` message provides the minimum recommended GNSS data, which includes a Kalman-enhanced 2D position, velocity, course over ground, and quality indicators.

#### Message Format

```
$GPRMC,,V,,,,,,,,,,N,V*29<CR><LF>
$GPRMC,010802.26,A,4852.13326,N,00209.49001,E,0.2,195.49,290512,,,A*67<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_RMC (01)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field | Name         | Format         | Description                                                             |
|-------|--------------|----------------|-------------------------------------------------------------------------|
| 0     | $##RMC       | string         | Message ID – RMC frame                                                  |
| 1     | Time         | hhmmss.ss      | UTC Time, current time                                                  |
| 2     | Status       | char           | Status field: A = Valid data, V = Invalid data                          |
| 3     | Latitude     | ddmm.mmmmmmmm  | Latitude: degree + minutes with 8 decimal digits                        |
| 4     | N/S          | char           | North / South indicator                                                 |
| 5     | Longitude    | dddmm.mmmmmmmm | Longitude: degree + minutes with 8 decimal digits                       |
| 6     | E/W          | char           | East / West indicator                                                   |
| 7     | Speed        | fff.f          | Speed over ground in knots                                              |
| 8     | Course       | fff.f          | Course over ground in degrees [0; 360]                                  |
| 9     | Date         | ddmmyy         | UTC day, month, year                                                    |
| 10    | Variation    | fff.ff         | Magnetic variation value in degrees [0; 180]                            |
| 11    | E/W          | char           | Direction of magnetic variation (East / West)                           |
| 12    | Mode         | char           | Position mode indicator (see definition in Quality indicators section)  |
| 13    | Nav Status   | char           | Navigational status indicator (see definition in Quality indicators)    |
| 14    | Check sum    | *cs            | XOR of all previous bytes except $                                      |
| 15    | End of frame | <CR><LF>       | Carriage return and line feed                                           |

> [!NOTE]  
> The position mode and navigational status are calculated using the INS position's estimated standard deviation and are not directly related to the GNSS solution.

---

### SBG_ECOM_LOG_NMEA_ZDA (02) {#SBG_ECOM_LOG_NMEA_ZDA}

The `SBG_ECOM_LOG_NMEA_ZDA` message contains UTC time and date information.

#### Message Format

```
$GPZDA,,,,,,*48<CR><LF>
$GPZDA,201530.00,04,07,2002,00,00*60<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_ZDA (02)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field | Name     | Format      | Description                                          |
|-------|----------|-------------|------------------------------------------------------|
| 0     | $##ZDA   | string      | Message ID – ZDA frame                               |
| 1     | Time     | hhmmss.ss   | UTC Time, current time                               |
| 2     | Day      | dd          | Day of month [01 - 31]                               |
| 3     | Month    | mm          | Month of year [01 - 12]                              |
| 4     | Year     | yyyy        | Year (4 digits)                                      |
| 5     | Ltzh     | 0           | Local zone hours (not supported, fixed 00)           |
| 6     | Ltzh     | 0           | Local zone minutes (not supported, fixed 00)         |
| 7     | Check sum| *cs         | XOR of all previous bytes except $                   |
| 8     | End of frame | <CR><LF> | Carriage return and line feed                        |

---

### SBG_ECOM_LOG_NMEA_HDT (03) {#SBG_ECOM_LOG_NMEA_HDT}

The `SBG_ECOM_LOG_NMEA_HDT` message outputs the INS Kalman filtered true heading value. The true heading is the direction the vehicle is pointing, which is not necessarily the same as the direction of travel (course over ground).

#### Message Format

```
$GPHDT,,T*1B<CR><LF>
$GPHDT,191.94,T*01<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_HDT (03)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field | Name     | Format      | Description                                          |
|-------|----------|-------------|------------------------------------------------------|
| 0     | $##HDT   | string      | Message ID – HDT frame                               |
| 1     | Heading  | fff.ff      | True heading in degrees [0 - 360]                    |
| 2     | T        | char        | T indicates true heading                             |
| 3     | Check sum| *cs         | XOR of all previous bytes except $                   |
| 4     | End of frame | <CR><LF> | Carriage return and line feed                        |

---

### SBG_ECOM_LOG_NMEA_GST (04) {#SBG_ECOM_LOG_NMEA_GST}

The `SBG_ECOM_LOG_NMEA_GST` message provides detailed position error statistics of the Kalman filtered position solution.  
It is important to note that the data reflects the estimated inertial position and not the GNSS quality fix directly.

#### Message Format

```
$GPGST,,,,,,,,*57<CR><LF>
$GPGST,172814.00,,0.023,0.020,273.62,0.023,0.015,0.031*46<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_GST (04)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field             | Name             | Format        | Description                                                             |
|-------------------|------------------|---------------|-------------------------------------------------------------------------|
| 0                 | $##GST           | string        | Message ID – GST frame                                                  |
| 1                 | Time             | hhmmss.ss     | UTC Time, current time                                                  |
| 2                 | psrResidual      | NULL          | RMS value of pseudorange residuals. Always NULL, not supported.         |
| 3                 | sMajorAxisError  | fff.fff       | Error ellipse semi-major axis 1 sigma error, in meters                  |
| 4                 | sMinorAxisError  | fff.fff       | Error ellipse semi-minor axis 1 sigma error, in meters                  |
| 5                 | errorEllipseAng  | fff.ff        | Error ellipse orientation, degrees from true north [0 - 360]            |
| 6                 | latError         | fff.fff       | Latitude 1 sigma error, in meters                                       |
| 7                 | longError        | fff.fff       | Longitude 1 sigma error, in meters                                      |
| 8                 | altError         | fff.fff       | Height 1 sigma error, in meters                                         |
| 9                 | Check sum        | *cs           | XOR of all previous bytes except $                                      |
| 10                | End of frame     | <CR><LF>      | Carriage return and line feed                                           |

---

### SBG_ECOM_LOG_NMEA_VBW (05) {#SBG_ECOM_LOG_NMEA_VBW}

The `SBG_ECOM_LOG_NMEA_VBW` message outputs Ground and Water speed from the INS data fusion algorithm. The data are expressed in the INS (body) frame.  
This log is filled correctly only if the INS receives valid bottom tracking and water tracking DVL data. Bottom and water velocities are used by the Inertial Navigation System Kalman filter to estimate the water current and compute the water speed in the vessel coordinate frame.

#### Message Format

```
$GPVBW,,,,,,,,*54<CR><LF>
$GPVBW,0.312,0.910,A,0.410,0.950,A*55<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_VBW (05)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field              | Name             | Format        | Description                                                             |
|--------------------|------------------|---------------|-------------------------------------------------------------------------|
| 0                  | $##VBW           | string        | Message ID – VBW frame                                                  |
| 1                  | longWaterSpeed   | fff.fff       | Longitudinal water speed, knots (positive forward)                      |
| 2                  | transvWaterSpeed | fff.fff       | Transverse water speed, knots (positive right)                          |
| 3                  | waterSpeedValid  | char          | Status: Water speed, A = Data valid, V = Invalid                        |
| 4                  | longGroundSpeed  | fff.fff       | Longitudinal ground speed, knots (positive forward)                     |
| 5                  | transvGroundSpeed| fff.fff       | Transverse ground speed, knots (positive right)                         |
| 6                  | groundSpeedValid | char          | Status: Ground speed, A = Data valid, V = Invalid                       |
| 7                  | Check sum        | *cs           | XOR of all previous bytes except $                                      |
| 8                  | End of frame     | <CR><LF>      | Carriage return and line feed                                           |

---

### SBG_ECOM_LOG_NMEA_DPT (07) {#SBG_ECOM_LOG_NMEA_DPT}

The `SBG_ECOM_LOG_NMEA_DPT` message outputs the depth in meters as received by the device from an external pressure sensor.  
This value is untouched by the device and is basically a copy of the received data. This log is filled correctly only if valid pressure sensor data are fed to the INS.

#### Message Format

```
$GPDPT,,,*7B<CR><LF>
$GPDPT,21.393,,*6F<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_DPT (07)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field              | Name             | Format        | Description                                                             |
|--------------------|------------------|---------------|-------------------------------------------------------------------------|
| 0                  | $##DPT           | string        | Message ID – DPT frame                                                  |
| 1                  | Depth            | fff.fff       | Depth below surface in meters (positive down)                           |
| 2                  | Offset           | fff.fff       | Offset from transducer. Not supported, always empty.                    |
| 3                  | Range Scale      | fff.fff       | Maximum range scale in use. Not supported, always empty.                |
| 4                  | Check sum        | *cs           | XOR of all previous bytes except $                                      |
| 5                  | End of frame     | <CR><LF>      | Carriage return and line feed                                           |

---

### SBG_ECOM_LOG_NMEA_VTG (08) {#SBG_ECOM_LOG_NMEA_VTG}

The `SBG_ECOM_LOG_NMEA_VTG` message reports the track and velocity over ground as computed by the Kalman filter.  
It includes a quality indicator similar to the RMC message.

#### Message Format

```
$GPVTG,,,,,,,,,N*30<CR><LF>
$GPVTG,256.31,T,256.44,M,45.401,N,84.084,K,N*2A<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_VTG (08)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field | Name           | Format     | Description                                                             |
|-------|----------------|------------|-------------------------------------------------------------------------|
| 0     | $##VTG         | string     | Message ID – VTG frame                                                  |
| 1     | Course True    | fff.ff     | True course over ground in degrees [0; 360]                             |
| 2     | T              | char       | Course over ground is relative to true north                            |
| 3     | Course Magnetic| fff.ff     | Magnetic course over ground in degrees [0; 360]                         |
| 4     | M              | char       | Course over ground is relative to magnetic north                        |
| 5     | Speed Knots    | fff.fff    | Ground speed in knots                                                   |
| 6     | N              | char       | The speed is expressed in knots                                         |
| 7     | Speed Km/h     | fff.fff    | Ground speed in kilometers per hour                                     |
| 8     | K              | char       | The speed is expressed in kilometers per hour                           |
| 9     | Mode           | char       | Position mode indicator (see definition in Quality indicators section)  |
| 10    | Check sum      | *cs        | XOR of all previous bytes except $                                      |
| 11    | End of frame   | <CR><LF>   | Carriage return and line feed                                           |

> [!NOTE]  
> The position mode indicator is computed using the INS position's estimated standard deviation, and is not directly related to the GNSS solution.

---

### SBG_ECOM_LOG_NMEA_ROT (09) {#SBG_ECOM_LOG_NMEA_ROT}

The `SBG_ECOM_LOG_NMEA_ROT` message outputs the INS rate of turn around the down axis. This is not the IMU gyroscope Z value but rather the true heading derivative.  
This log is filled correctly only if the INS has valid roll, pitch, and heading values.

#### Message Format

```
$GPROT,,V*55<CR><LF>
$GPROT,31.61,A*55<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_ROT (09)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field              | Name             | Format        | Description                                                             |
|--------------------|------------------|---------------|-------------------------------------------------------------------------|
| 0                  | $##ROT           | string        | Message ID – ROT frame                                                  |
| 1                  | Rate             | fff.ff        | Down rate of turn in degrees/minute (positive clockwise) or empty if invalid |
| 2                  | Status           | -             | ‘A’ if the rate of turn is valid or ‘V’ if invalid                      |
| 3                  | Check sum        | *cs           | XOR of all previous bytes except $                                      |
| 4                  | End of frame     | <CR><LF>      | Carriage return and line feed                                           |

---

### SBG_ECOM_LOG_NMEA_GSV (10) {#SBG_ECOM_LOG_NMEA_GSV}

The `SBG_ECOM_LOG_NMEA_GSV` message reports GNSS satellites in view. For each satellite, information such as elevation, azimuth, and SNR is provided.

#### Multi-Constellations

The NMEA talker ID is used to output satellite information for each constellation:

- **GPGSV:** Lists all visible GPS and SBAS satellites with associated information.
- **GLGSV:** Lists all visible GLONASS satellites with associated information.
- **GAGSV:** Lists all visible Galileo satellites with associated information.
- **GBGSV:** Lists all visible BeiDou satellites with associated information.
- **GIGSV:** Lists all visible NavIC satellites with associated information.
- **GQGSV:** Lists all visible QZSS satellites with associated information.

> [!NOTE]
> SBG Systems products do not output GNGSV messages to comply with NMEA standards.

#### Message Re-Assembly

GSV messages are sent using multiple NMEA frames to comply with the 80-character limit. Each GSV message contains the total number of messages as well as the current message number. Using this information, it is possible to reassemble the GSV message and get all visible satellites for each constellation.

**Empty Message Example**

The following message is sent by the INS when there is no GNSS reception:
```
$GPGSV,1,1,00,,,,*79<CR><LF>
```

**Multi-Constellation Example**

The following example shows a typical GSV reporting when tracking multi-constellation satellites in a clean environment. Each constellation contains multiple GSV messages sent sequentially. An empty line has been intentionally inserted between each constellation for readability; this empty line is not output by the product.

```
$GPGSV,5,1,19,09,78,059,50,02,12,323,40,30,12,186,43,03,10,109,39*75<CR><LF>
$GPGSV,5,2,19,20,26,297,41,04,39,060,44,07,38,162,47,11,46,297,47*76<CR><LF>
$GPGSV,5,3,19,06,62,239,49,16,06,066,,26,08,041,,29,01,348,*7D<CR><LF>
$GPGSV,5,4,19,19,06,223,,49,34,176,45,61,21,130,38,38,31,204,44*74<CR><LF>
$GPGSV,5,5,19,36,27,143,44,34,33,189,,39,10,112,36*4C<CR><LF>

$GLGSV,3,1,10,67,64,030,51,77,30,105,37,84,18,336,36,66,18,070,45*68<CR><LF>
$GLGSV,3,2,10,76,29,048,48,82,30,210,48,68,46,284,49,83,52,268,40*62<CR><LF>
$GLGSV,3,3,10,69,01,265,,78,01,152,*63<CR><LF>

$GAGSV,3,1,10,08,70,254,49,26,23,319,40,24,10,035,38,25,13,087,38*6B<CR><LF>
$GAGSV,3,2,10,15,33,183,44,07,17,247,41,03,54,063,46,13,59,258,48*6B<CR><LF>
$GAGSV,3,3,10,05,03,066,,02,00,130,*6F<CR><LF>

$GBGSV,4,1,15,41,62,074,,23,19,149,43,24,15,038,41,05,13,117,37*62<CR><LF>
$GBGSV,4,2,15,27,19,212,46,29,16,323,42,33,11,088,42,09,19,046,38*61<CR><LF>
$GBGSV,4,3,15,56,22,046,,25,36,097,48,30,33,267,48,32,57,285,50*6C<CR><LF>
$GBGSV,4,4,15,31,04,087,,20,05,273,,06,03,043,*55<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_GSV (10)`
- **Compatibility:** GNSS-capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)

| Field | Name            | Format   | Description                                                                              |
|-------|-----------------|----------|------------------------------------------------------------------------------------------|
| 0     | $##GSV          | string   | Message ID – GSV frame                                                                   |
| 1     | Total Msg Count | d        | Total number of messages to reassemble for this constellation (1-9)                      |
| 2     | Msg Number      | d        | Current message being transmitted for this constellation (1-9)                           |
| 3     | Sv Count        | dd       | Total number of satellites in view for this constellation                                |
| 4     | PRN/ID          | dd       | Satellite PRN numbers or ID depending on the constellation                               |
| 5     | Elevation       | dd       | Satellite elevation in degrees [-90 to +90°]                                             |
| 6     | Azimuth         | ddd      | Satellite azimuth in degrees [000 to +359°]                                              |
| 7     | SNR             | dd       | Signal Noise Ratio (C/No) in dB [00 to 99 dB] - Empty if satellite is not tracked        |
| ...   | ...             | ...      | Repetition of PRN, Elevation, Azimuth, and SNR fields for each satellite in this message |
| 8     | Check sum       | *cs      | XOR of all previous bytes except $                                                       |
| 9     | End of frame    | <CR><LF> | Carriage return and line feed                                                            |

> [!NOTE]
> If a satellite is tracked but the SNR is unknown, a default SNR of 60 dB is reported.

---

### SBG_ECOM_LOG_NMEA_GSA (11) {#SBG_ECOM_LOG_NMEA_GSA}

The `SBG_ECOM_LOG_NMEA_GSA` message provides information about the GNSS fix type, the satellites used in the navigation solution, and Dilution of Precision (DOP) values, which indicate the quality of the position solution.

#### Multi-Constellations

The NMEA talker ID is used to output satellite information for each constellation:

- **GPGSV:** Lists all visible GPS and SBAS satellites with associated information.
- **GLGSV:** Lists all visible GLONASS satellites with associated information.
- **GAGSV:** Lists all visible Galileo satellites with associated information.
- **GBGSV:** Lists all visible BeiDou satellites with associated information.
- **GIGSV:** Lists all visible NavIC satellites with associated information.
- **GQGSV:** Lists all visible QZSS satellites with associated information.

> [!NOTE]
> SBG Systems products do not output GNGSV messages to comply with NMEA standards.

#### Message Re-Assembly

GSA messages are sent using multiple NMEA frames to comply with the 80-character limit. Each GSA message contains the total number of messages as well as the current message number. Using this information, it is possible to reassemble the GSA message and get all visible satellites for each constellation.

**Empty Message Example**

The following message is sent by the INS when there is no GNSS reception:
```
$GPGSA,M,1,,,,,,,,,,,,,99.9,99.9,99.9,1*18<CR><LF>
```

**Multi-Constellation Example**

The following example shows a typical GSA reporting when tracking multi-constellation satellites in a clean environment.

```
$GPGSA,A,3,8,2,22,3,14,1,27,10,32,,,,1.9,1.1,1.6,1*28<CR><LF>
$GLGSA,A,3,87,79,71,78,72,88,86,65,,,,,1.9,1.1,1.6,2*3F<CR><LF>
$GAGSA,A,3,11,2,24,19,25,16,12,4,,,,,1.9,1.1,1.6,3*38<CR><LF>
$GBGSA,A,3,41,27,13,5,29,20,32,,,,,,1.9,1.1,1.6,4*08<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_GSA (11)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.2-blue) ![HPINS](https://img.shields.io/badge/HPINS-5.5-blue)

| Field | Name         | Format   | Description                                                        |
| ----- | ------------ | -------- | ------------------------------------------------------------------ |
| 0     | $##GSA       | string   | Message ID – GSA frame                                             |
| 1     | Mode         | char     | Positioning mode: `M` = Manual, `A` = Automatic                    |
| 2     | Fix Type     | d        | `1` = No fix, `2` = 2D fix, `3` = 3D fix                           |
| 3–14  | PRNs         | ii       | Up to 12 PRN numbers used in position solution (empty if not used) |
| 15    | PDOP         | ff.f     | Position Dilution of Precision                                     |
| 16    | HDOP         | ff.f     | Horizontal Dilution of Precision                                   |
| 17    | VDOP         | ff.f     | Vertical Dilution of Precision                                     |
| 18    | Checksum     | *cs      | XOR of all previous bytes except $                                 |
| 19    | End of frame | <CR><LF> | Carriage return and line feed                                      |

---## Proprietary NMEA Messages

The `SBG_ECOM_CLASS_LOG_NMEA_1 (0x03)` message class includes all proprietary NMEA messages and other non-standard messages that utilize NMEA formatting.  
These messages primarily provide Kalman filtered data, including navigation, velocity, and attitude information.

The logs are generated mainly from the AHRS/INS solution, rather than directly from GNSS data.

### Messages overview

The following list, provides a quick overview of all available logs for this message class.  
It briefly describe which parameters are contained in each output log.

| Name (MSG ID)                                                | Description                                                                                       |
|--------------------------------------------------------------|---------------------------------------------------------------------------------------------------|
| [SBG_ECOM_LOG_NMEA_1_PSBGI (01)](#SBG_ECOM_LOG_NMEA_1_PSBGI) | SBG Systems proprietary sentence. Contains rotation rates and accelerations.                      |
| [SBG_ECOM_LOG_NMEA_1_PSBGA (13)](#SBG_ECOM_LOG_NMEA_1_PSBGA) | SBG Systems proprietary sentence. Contains EKF attitude and status.                               |
| [SBG_ECOM_LOG_NMEA_1_PSBGB (04)](#SBG_ECOM_LOG_NMEA_1_PSBGB) | SBG Systems proprietary sentence. Contains attitude, heading, heave, angular rates, and velocity. |
| [SBG_ECOM_LOG_NMEA_1_PRDID (00)](#SBG_ECOM_LOG_NMEA_1_PRDID) | RDI proprietary sentence. Reports pitch, roll, and heading.                                       |
| [SBG_ECOM_LOG_NMEA_1_PASHR (02)](#SBG_ECOM_LOG_NMEA_1_PASHR) | Proprietary NMEA-like message. Reports roll, pitch, heading, and heave.                           |
| [SBG_ECOM_LOG_NMEA_1_WASSP (12)](#SBG_ECOM_LOG_NMEA_1_WASSP) | Proprietary NMEA-like message for WASSP. Reports roll, pitch, heading, and heave.                 |
| [SBG_ECOM_LOG_NMEA_1_PHINF (05)](#SBG_ECOM_LOG_NMEA_1_PHINF) | iXblue NMEA-like log used to output status information.                                           |
| [SBG_ECOM_LOG_NMEA_1_PHTRO (06)](#SBG_ECOM_LOG_NMEA_1_PHTRO) | iXblue NMEA-like log used to output attitude and ship motion.                                     |
| [SBG_ECOM_LOG_NMEA_1_PHLIN (07)](#SBG_ECOM_LOG_NMEA_1_PHLIN) | iXblue NMEA-like log used to output roll and pitch.                                               |
| [SBG_ECOM_LOG_NMEA_1_PHOCT (08)](#SBG_ECOM_LOG_NMEA_1_PHOCT) | iXblue NMEA-like log used to output surge, sway, and heave.                                       |
| [SBG_ECOM_LOG_NMEA_1_INDYN (09)](#SBG_ECOM_LOG_NMEA_1_INDYN) | iXblue NMEA-like log used to output position, heading, attitude, rate, and velocity.              |
| [SBG_ECOM_LOG_NMEA_1_GGK (10)](#SBG_ECOM_LOG_NMEA_1_GGK)     | Trimble NMEA-like log containing time, latitude, longitude, and ellipsoidal height.               |

---

### SBG_ECOM_LOG_NMEA_1_PSBGI (00) {#SBG_ECOM_LOG_NMEA_1_PSBGI}

The `SBG_ECOM_LOG_NMEA_1_PSBGI` message is an SBG Systems proprietary NMEA log that provides accelerations and rotation rates in the body frame, along with accurate UTC timestamping.

#### Message Format

```
$PSBGI,003944.74,-0.08,0.07,0.00,-0.02,0.06,-9.72,*42<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_PSBGI (01)`
- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name         | Format       | Description                                                           |
|-------|--------------|--------------|-----------------------------------------------------------------------|
| 0     | $PSBGI       | string       | Message ID – SBG Systems proprietary NMEA identifier                  |
| 1     | timeUTC      | hhmmss.sss   | Current UTC time                                                      |
| 2     | gyroX        | fff.fff      | X Rotation rate in °/s from -999.999 to +999.999                      |
| 3     | gyroY        | fff.fff      | Y Rotation rate in °/s from -999.999 to +999.999                      |
| 4     | gyroZ        | fff.fff      | Z Rotation rate in °/s from -999.999 to +999.999                      |
| 5     | accelX       | fff.fff      | X acceleration in m/s² from -999.999 to +999.999                      |
| 6     | accelY       | fff.fff      | Y acceleration in m/s² from -999.999 to +999.999                      |
| 7     | accelZ       | fff.fff      | Z acceleration in m/s² from -999.999 to +999.999                      |
| 8     | Check sum    | *cs          | XOR of all previous bytes except `$`                                  |
| 9     | End of frame | <CR><LF>  | Carriage return and line feed                                            |

> [!NOTE]
> The outputs are direct measurements from the Inertial Measurement Unit (IMU). They are not compensated for sensor bias, earth rotation rate, or gravity.

---

### SBG_ECOM_LOG_NMEA_1_PSBGA (13) {#SBG_ECOM_LOG_NMEA_1_PSBGA}

The `SBG_ECOM_LOG_NMEA_1_PSBGA` is an SBG Systems proprietary NMEA log that provides UTC time of day, heading, roll, pitch, and associated status and standard deviations.

#### Message Format

```
$PSBGA,155513.685,V,13.684,-63.139,269.130,0.024,0.006,0.196,p,v,v*74<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_PSBGA (13)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name            | Format     | Description                                                                                                   |
|-------|-----------------|------------|---------------------------------------------------------------------------------------------------------------|
| 0     | $PSBGA          | string     | Message ID – SBG Systems proprietary NMEA identifier                                                          |
| 1     | timeUTC         | hhmmss.sss | Current UTC time                                                                                              |
| 2     | utcStatus       | char       | UTC time validity indicator:<br>‘i’ = Invalid<br>‘v’ = Valid with known leap second<br>‘d’ = Unknown leap second<br>Status is uppercase if UTC is synchronized with a PPS signal. |
| 3     | roll            | fff.fff    | Roll angle in decimal degrees, positive port side up [-180; 180°].                                            |
| 4     | pitch           | fff.fff    | Pitch angle in decimal degrees, positive bow up [-90; 90°].                                                   |
| 5     | heading         | fff.fff    | True Heading angle in decimal degrees [0; 360°].                                                              |
| 6     | rollStd         | fff.fff    | Roll standard deviation in decimal degrees [0; 180°].                                                         |
| 7     | pitchStd        | fff.fff    | Pitch standard deviation in decimal degrees [0; 180°].                                                        |
| 8     | headingStd      | fff.fff    | True Heading standard deviation in decimal degrees [0; 180°].                                                 |
| 9     | solType         | char       | EKF smart solution type (see [Smart Solution Type](#smart-solution-type)).                                    |
| 10    | rollPitchStatus | char       | Roll and Pitch validity flag:<br>‘i’ = Invalid (Standard deviation above threshold)<br>‘v’ = Optimal accuracy |
| 11    | headingStatus   | char       | Heading validity flag:<br>‘i’ = Invalid (Standard deviation above threshold)<br>‘v’ = Optimal accuracy        |
| 12    | Check sum       | *cs        | XOR of all previous bytes except `$`.                                                                         |
| 13    | End of frame    | <CR><LF>   | Carriage return and line feed                                                                                 |

#### Smart Solution Type {#smart-solution-type}

This status indicates the operating mode of the navigation filter and the type of input position or velocity being used.  
It also denotes whether the INS is aligned, using lowercase or uppercase characters:

- **Lowercase**: INS is not yet aligned.
- **Uppercase**: INS is aligned and delivering optimal accuracy.

| Value | Name              | Description                                                                                  |
|-------|-------------------|----------------------------------------------------------------------------------------------|
| a     | Uninitialized     | EKF filter is not yet initialized. All measurements are invalid.                             |
| b     | Vertical Gyro     | EKF filter only computes valid roll and pitch information using the gravity reference.       |
| c     | AHRS              | EKF filter computes roll, pitch, and heading using either magnetometers or dual GNSS.        |
| d     | Nav Inertial      | Full INS solution by integrating accelerations – inertial only.                              |
| e     | Nav Vel. Const    | Full INS solution by integrating accelerations with vehicle constraints applied.             |
| f     | Nav Odometer      | Full INS solution by integrating accelerations and odometer velocity.                        |
| g     | Nav ZUPT          | Full INS solution with Zero Velocity Updates to limit position drift.                        |
| h     | Nav DVL           | Full INS solution by integrating accelerations and DVL velocity.                             |
| i     | Nav GNSS Vel.     | Full INS solution using GNSS velocity but not the position.                                  |
| j     | Nav USBL          | Full INS solution using USBL position.                                                       |
| k     | Nav GNSS Unkw.    | Full INS solution using unknown GNSS position type.                                          |
| l     | Nav Single Point  | Full INS solution using GNSS single point position type.                                     |
| m     | Nav DGPS          | Full INS solution using GNSS DGPS position type.                                             |
| n     | Nav SBAS          | Full INS solution using GNSS SBAS position type.                                             |
| o     | Nav RTK Float     | Full INS solution using GNSS RTK with float ambiguities position type.                       |
| p     | Nav RTK Fixed     | Full INS solution using GNSS RTK with fixed ambiguities position type.                       |
| q     | Nav PPP Float     | Full INS solution using GNSS PPP with float ambiguities position type.                       |
| r     | Nav PPP Fixed     | Full INS solution using GNSS PPP with fixed ambiguities position type.                       |

> [!NOTE]
> If the INS is not fully aligned, the solution type is reported in lowercase. Once aligned, the solution type is reported in uppercase.

---

### SBG_ECOM_LOG_NMEA_1_PSBGB (04) {#SBG_ECOM_LOG_NMEA_1_PSBGB}

The `SBG_ECOM_LOG_NMEA_1_PSBGB` is an SBG Systems proprietary NMEA log that provides UTC time of day, heading, roll, pitch, heave, angular rates, and body velocities, along with standard deviations and status indicators.

#### Message Format

```
$PSBGB,1,000344.000,0,3.529,-12.821,6.122,0.101,0.098,10.117,0,0,0.004,0.050,2,0.772,0.004,‑0.017,1.043,4.476,0.171,866.025,0,*53<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_PSBGB (04)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name             | Format       | Description                                                                 |
|-------|------------------|--------------|-----------------------------------------------------------------------------|
| 0     | $PSBGB           | string       | Message ID – SBG Systems proprietary NMEA identifier                        |
| 1     | Version          | char         | Version of this message – typically `1`                                     |
| 2     | timeUTC          | hhmmss.sss   | Current UTC time                                                            |
| 3     | utcStatus        | char         | UTC time validity indicator:<br>‘0’ = Invalid<br>‘1’ = Valid and PPS synchronized<br>‘2’ = Valid but no PPS synchronized<br>‘3’ = Unknown leap second and PPS synchronized<br>‘4’ = Unknown leap second and no PPS synchronized |
| 4     | roll             | fff.fff      | Roll angle in decimal degrees, positive port side up [-180; 180°].          |
| 5     | pitch            | fff.fff      | Pitch angle in decimal degrees, positive bow up [-90; 90°].                 |
| 6     | heading          | fff.fff      | True Heading angle, in decimal degrees [0; 360°].                           |
| 7     | rollStd          | fff.fff      | Roll standard deviation in decimal degrees [0; 180°].                       |
| 8     | pitchStd         | fff.fff      | Pitch standard deviation in decimal degrees [0; 180°].                      |
| 9     | headingStd       | fff.fff      | True Heading standard deviation in decimal degrees [0; 180°].               |
| 10    | rollPitchStatus  | char         | Roll and Pitch validity flag:<br>‘0’ = Invalid (Standard deviation above threshold)<br>‘1’ = Optimal accuracy<br>‘2’ = Degraded accuracy, alignment in progress |
| 11    | headingStatus    | char         | Heading validity flag:<br>‘0’ = Invalid (Standard deviation above threshold)<br>‘1’ = Optimal accuracy<br>‘2’ = Degraded accuracy, alignment in progress |
| 12    | heave            | ff.fff       | Heave, in meters, positive down.                                            |
| 13    | heaveStd         | ff.fff       | Heave standard deviation in meters. – Fixed to 5 cm                         |
| 14    | heaveStatus      | char         | Heave status flag:<br>‘0’ = Invalid or initializing<br>‘1’ = Valid and velocity aided<br>‘2’ = Valid, standalone |
| 15    | rateX            | fff.fff      | Compensated body X angular rate in °/s, positive port side up.              |
| 16    | rateY            | fff.fff      | Compensated body Y angular rate in °/s, positive bow up.                    |
| 17    | rateZ            | fff.fff      | Compensated body Z angular rate in °/s, positive clockwise.                 |
| 18    | velocityX        | fff.fff      | Velocity in X body axis in m/s, positive forward.                           |
| 19    | velocityY        | fff.fff      | Velocity in Y body axis in m/s, positive starboard.                         |
| 20    | velocityZ        | fff.fff      | Velocity in Z body axis in m/s, positive down.                              |
| 21    | velocityStd      | fff.fff      | Norm of X, Y, and Z velocity standard deviation in m/s.                     |
| 22    | velocityStatus   | char         | Velocity validity flag:<br>‘0’ = Invalid (Standard deviation above threshold)<br>‘1’ = Optimal accuracy<br>‘2’ = Degraded accuracy, alignment in progress |
| 23    | Check sum        | *cs          | XOR of all previous bytes except `$`.                                       |
| 24    | End of frame     | <CR><LF>     | Carriage return and line feed                                               |

---

### SBG_ECOM_LOG_NMEA_1_PRDID (00) {#SBG_ECOM_LOG_NMEA_1_PRDID}

The `SBG_ECOM_LOG_NMEA_1_PRDID` message is a Teledyne RDI proprietary message that outputs the vessel's pitch, roll, and true heading angles in degrees.  
It uses an NMEA-style formatting.

#### Message Format

```
$PRDID,-012.39,+002.14,366.91*7A<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_PRDID (00)`
- **Compatibility:** AHRS/INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name         | Format   | Description                                                            |
|-------|--------------|----------|------------------------------------------------------------------------|
| 0     | $PRDID       | string   | Message ID – Teledyne RDI proprietary NMEA identifier                  |
| 1     | Pitch        | fff.ff   | Signed vessel pitch in degrees, positive bow up.                       |
| 2     | Roll         | fff.ff   | Signed vessel roll in degrees, positive port up.                       |
| 3     | Heading      | fff.ff   | Vessel true heading in degrees [0 - 360]                               |
| 4     | Check sum    | *cs      | XOR of all previous bytes except `$`                                   |
| 5     | End of frame | <CR><LF> | Carriage return and line feed                                          |

---

### SBG_ECOM_LOG_NMEA_1_PASHR (02) {#SBG_ECOM_LOG_NMEA_1_PASHR}

The `SBG_ECOM_LOG_NMEA_1_PASHR` message is a proprietary NMEA log that provides roll, pitch, heading, and heave outputs.

#### Message Format

```
$PASHR,,,T,,,,,,,0,1*21<CR><LF>
$PASHR,123816.80,312.95,T,-000.83,-000.42,-000.01,0.234,0.224,0.298,1,0*09<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_PASHR (02)`
- **Compatibility:** AHRS/INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name       | Format     | Description                                                                 |
|-------|------------|------------|-----------------------------------------------------------------------------|
| 0     | $PASHR     | string     | Message ID – Proprietary NMEA identifier                                    |
| 1     | timeUTC    | hhmmss.ss  | Current UTC time – empty if invalid                                         |
| 2     | heading    | fff.ff     | Heading angle, in decimal degrees [0 – 360] – empty if invalid              |
| 3     | T          | char       | True Heading                                                                |
| 4     | roll       | fff.ff     | Roll in decimal degrees. From [-180 – 180°] – empty if invalid              |
| 5     | pitch      | fff.ff     | Pitch in decimal degrees. From [-90 – 90°] – empty if invalid               |
| 6     | heave      | fff.ff     | Heave, in meters, positive down – empty if invalid                          |
| 7     | rollStd    | fff.ff     | Roll angle standard deviation in decimal degrees – empty if invalid         |
| 8     | pitchStd   | fff.ff     | Pitch angle standard deviation in decimal degrees – empty if invalid        |
| 9     | headingStd | fff.ff     | Heading angle standard deviation in decimal degrees – empty if invalid      |
| 10    | posStatus  | char       | Position Quality status<br>0 = No position<br>1 = All non-RTK fixed integer positions<br>2 = RTK fixed integer position |
| 11    | imuStatus  | char       | IMU & Sensor Status<br>0 = IMU is working correctly<br>1 = IMU sensor error |
| 12    | Check sum  | *cs        | XOR of all previous bytes except `$`                                        |
| 13    | End of frame | <CR><LF> | Carriage return and line feed                                               |

---

### SBG_ECOM_LOG_NMEA_1_WASSP (12) {#SBG_ECOM_LOG_NMEA_1_WASSP}

The `SBG_ECOM_LOG_NMEA_1_WASSP` message is a proprietary NMEA log that provides roll, pitch, heading, and heave outputs.  
This message shares the same structure as the `PASHR` message but has slight differences, such as the heave sign being positive up.

#### Message Format

```
$PASHR,,,T,,,,,,,0,1*21<CR><LF>
$PASHR,002258.15,320.99,T,+032.46,-008.15,-012.239,0.454,0.095,1.070,1,0*39<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_WASSP (12)`
- **Compatibility:** AHRS/INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name       | Format     | Description                                                                 |
|-------|------------|------------|-----------------------------------------------------------------------------|
| 0     | $PASHR     | string     | Message ID – Proprietary NMEA identifier                                    |
| 1     | timeUTC    | hhmmss.ss  | Current UTC time – empty if invalid                                         |
| 2     | heading    | fff.ff     | Heading angle, in decimal degrees [0 – 360] – empty if invalid              |
| 3     | T          | char       | True Heading                                                                |
| 4     | roll       | fff.ff     | Roll in decimal degrees. From [-180 – 180°] – empty if invalid              |
| 5     | pitch      | fff.ff     | Pitch in decimal degrees. From [-90 – 90°] – empty if invalid               |
| 6     | heave      | fff.fff    | Heave, in meters, positive up – empty if invalid                            |
| 7     | rollStd    | fff.ff     | Roll angle standard deviation in decimal degrees – empty if invalid         |
| 8     | pitchStd   | fff.ff     | Pitch angle standard deviation in decimal degrees – empty if invalid        |
| 9     | headingStd | fff.ff     | Heading angle standard deviation in decimal degrees – empty if invalid      |
| 10    | posStatus  | char       | Position Quality status<br>0 = No position<br>1 = All non-RTK fixed integer positions<br>2 = RTK fixed integer position |
| 11    | imuStatus  | char       | IMU & Sensor Status<br>0 = IMU is working correctly<br>1 = IMU sensor error |
| 12    | Check sum  | *cs        | XOR of all previous bytes except `$`                                        |
| 13    | End of frame | <CR><LF> | Carriage return and line feed                                               |

---

### SBG_ECOM_LOG_NMEA_1_PHINF (05) {#SBG_ECOM_LOG_NMEA_1_PHINF}

The `SBG_ECOM_LOG_NMEA_1_PHINF` message is an Ixblue proprietary NMEA log that outputs the general system status.

#### Message Format

```
$PHINF,08030027*7B<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_PHINF (05)`
- **Compatibility:** AHRS/INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name         | Format   | Description                                                    |
|-------|--------------|----------|----------------------------------------------------------------|
| 0     | $PHINF       | string   | Message ID – Ixblue proprietary NMEA identifier                |
| 1     | Status       | hhhhhhhh | Hexadecimal value representing the INS status                  |
| 2     | Check sum    | *cs      | XOR of all previous bytes except `$`                           |
| 3     | End of frame | <CR><LF> | Carriage return and line feed                                  |

**STATUS Definition:**

The `Status` field is a 32-bit word that acts as a built-in test and control system status indicator.  
It monitors different modes and sub-part statuses of the system in real-time, setting specific flags as needed.

Each flag is a bit set to “1” when the status is active (ON) and “0” when inactive (OFF).

| Bit | Name                             | Description                                                      |
|-----|----------------------------------|------------------------------------------------------------------|
| 0   | IXBLUE_STAT_HEADING_UNVALID      | Set when the provided heading is invalid or still converging     |
| 1   | IXBLUE_STAT_ROLL_UNVALID         | Set when the provided roll is invalid or still converging        |
| 2   | IXBLUE_STAT_PITCH_UNVALID        | Set when the provided pitch is invalid or still converging       |
| 3   | IXBLUE_STAT_HEAVE_INIT           | Set when the heave filter is in initialization phase             |
| 4   | Reserved                         | Reserved, not used                                               |
| 5   | IXBLUE_STAT_ALIGNMENT            | Set during the alignment phase                                   |
| 6   | IXBLUE_STAT_CONFIG_SAVED         | Not implemented                                                  |
| 7   | IXBLUE_STAT_COMPUTATION_OVERLOAD | Set when the CPU is overloaded                                   |
| 8   | IXBLUE_STAT_FOG_X1_ANOMALY       | Set when the gyroscope X built-in test has failed                |
| 9   | IXBLUE_STAT_FOG_X2_ANOMALY       | Set when the gyroscope Y built-in test has failed                |
| 10  | IXBLUE_STAT_FOG_X3_ANOMALY       | Set when the gyroscope Z built-in test has failed                |
| 11  | IXBLUE_STAT_FOG_ACQ_ERROR        | Set when at least one gyroscope is out of range                  |
| 12  | IXBLUE_STAT_ACC_X1_ANOMALY       | Set when the accelerometer X built-in test has failed            |
| 13  | IXBLUE_STAT_ACC_X2_ANOMALY       | Set when the accelerometer Y built-in test has failed            |
| 14  | IXBLUE_STAT_ACC_X3_ANOMALY       | Set when the accelerometer Z built-in test has failed            |
| 15  | IXBLUE_STAT_SENSOR_ERROR         | Set when at least one sensor is failing or out of range          |
| 16  | IXBLUE_STAT_SERIAL_IN_A_ERROR    | Set when errors are detected on serial port A RX                 |
| 17  | IXBLUE_STAT_SERIAL_IN_B_ERROR    | Set when errors are detected on serial port B RX                 |
| 18  | IXBLUE_STAT_SERIAL_IN_C_ERROR    | Set when errors are detected on serial port C RX                 |
| 19  | IXBLUE_STAT_OUTPUT_OVERLOADED    | Set when at least one serial output is overloaded                |
| 20  | IXBLUE_STAT_SERIAL_OUT_A_FULL    | Set when the serial output A is overloaded                       |
| 21  | IXBLUE_STAT_SERIAL_OUT_B_FULL    | Set when the serial output B is overloaded                       |
| 22  | IXBLUE_STAT_SERIAL_OUT_C_FULL    | Set when the serial output C is overloaded                       |
| 23  | IXBLUE_STAT_SERIAL_OUT_D_FULL    | Not implemented                                                  |
| 24  | IXBLUE_STAT_MANUAL_LOG_USED      | Not implemented                                                  |
| 25  | IXBLUE_STAT_MANUAL_LAT_USED      | Not implemented                                                  |
| 27  | IXBLUE_STAT_HRP_INVALID          | Set if at least one roll, pitch, or heading component is invalid |
| 31  | IXBLUE_STAT_RESTART_SYSTEM       | Not implemented                                                  |

---

### SBG_ECOM_LOG_NMEA_1_PHTRO (06) {#SBG_ECOM_LOG_NMEA_1_PHTRO}

The `SBG_ECOM_LOG_NMEA_1_PHTRO` message is an Ixblue proprietary NMEA log that outputs the unit's pitch and roll angles.

#### Message Format

```
$PHTRO,0.03,P,0.22,T*56<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_PHTRO (06)`
- **Compatibility:** AHRS/INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name         | Format   | Description                                                          |
|-------|--------------|----------|----------------------------------------------------------------------|
| 0     | $PHTRO       | string   | Message ID – Ixblue proprietary NMEA identifier                      |
| 1     | Pitch        | x.xx     | Pitch angle in degrees                                               |
| 2     | Pitch sign   | char     | 'M' for bow up and 'P' for bow down                                  |
| 3     | Roll         | y.yy     | Roll angle in degrees                                                |
| 4     | Roll sign    | char     | 'B' for port down and 'T' for port up                                |
| 5     | Check sum    | *cs      | XOR of all previous bytes except `$`                                 |
| 6     | End of frame | <CR><LF> | Carriage return and line feed                                        |

---

### SBG_ECOM_LOG_NMEA_1_PHLIN (07) {#SBG_ECOM_LOG_NMEA_1_PHLIN}

The `SBG_ECOM_LOG_NMEA_1_PHLIN` message is an Ixblue proprietary NMEA log that outputs surge, sway, and heave data.

#### Message Format

```
$PHLIN,0.03,0.22,-0.15*68<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_PHLIN (07)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name         | Format   | Description                                                                                       |
|-------|--------------|----------|---------------------------------------------------------------------------------------------------|
| 0     | $PHLIN       | string   | Message ID – Ixblue proprietary NMEA identifier                                                   |
| 1     | Surge        | x.xxx    | Signed surge in meters (positive forward)                                                         |
| 2     | Sway         | y.yyy    | Signed sway in meters (positive left)                                                             |
| 3     | Heave        | z.zzz    | Signed heave in meters (positive up)                                                              |
| 4     | Check sum    | *cs      | XOR of all previous bytes except `$`                                                              |
| 5     | End of frame | <CR><LF> | Carriage return and line feed                                                                     |

> [!WARNING]
> Ixblue uses different conventions for ship motion measurements. In this frame, both sway and heave values are reversed compared to SBG Systems conventions.

---

### SBG_ECOM_LOG_NMEA_1_PHOCT (08) {#SBG_ECOM_LOG_NMEA_1_PHOCT}

The `SBG_ECOM_LOG_NMEA_1_PHOCT` message is an Ixblue proprietary NMEA log that outputs time, attitude, heading, and ship motion data.

#### Message Format

```
$PHOCT,01,000201.000,E,00,356.592,E,+000.225,E,+00.039,E,+00.023,T,+00.023,+00.016,+00.003,+00.002,-00.001,+00.000,+0001.96*04<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_PHOCT (08)`
- **Compatibility:** Heave capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name           | Format    | Description                                                                              |
|-------|----------------|-----------|------------------------------------------------------------------------------------------|
| 0     | $PHOCT         | string    | Message ID – Ixblue proprietary NMEA identifier                                          |
| 1     | Protocol Ver.  | 01        | Protocol version identifier                                                              |
| 2     | Time           | hhmmss.ss | UTC Time, current time                                                                   |
| 3     | UTC Status     | G         | UTC Time status: 'T'=Valid 'E'=Invalid                                                   |
| 4     | Latency        | AA        | INS latency for heading, roll, pitch (Not implemented, always set to 0)                  |
| 5     | True Heading   | HHH.HHH   | True heading in degrees (000.000 to 359.999)                                             |
| 6     | Heading Status | N         | True Heading status: 'T'=Valid;  'E'=Invalid;  'I'=Initializing                          |
| 7     | Roll Angle     | RRR.RRR   | Roll in degrees (positive if port side up); -180.000 to +180.000                         |
| 8     | Roll Status    | L         | Roll status: 'T'=Valid 'E'=Invalid 'I'=Initializing                                      |
| 9     | Pitch Angle    | PP.PPP    | Pitch in degrees (positive if bow down); -90.000 to +90.000                              |
| 10    | Pitch Status   | K         | Pitch status: 'T'=Valid 'E'=Invalid 'I'=Initializing                                     |
| 11    | Primary Heave  | FF.FFF    | Heave at Primary Lever arm in meters (positive up); -99.999 to +99.999                   |
| 12    | Heave Status   | M         | Heave status (also used for surge, sway & speed): 'T'=Valid 'E'=Invalid 'I'=Initializing |
| 13    | Heave          | HH.HHH    | Heave at desired lever arm in meters (positive up); -99.999 to +99.999                   |
| 14    | Surge          | SS.SSS    | Surge with Lever arms applied in meters (positive forward); -99.999 to +99.999           |
| 15    | Sway           | WW.WWW    | Sway at desired lever arm in meters (positive left); -99.999 to +99.999                  |
| 16    | Heave Speed    | ZZ.ZZZ    | Heave speed at desired lever arm in m/s (positive up); -99.999 to +99.999                |
| 17    | Surge Speed    | YY.YYY    | Surge speed at desired lever arm in m/s (positive forward); -99.999 to +99.999           |
| 18    | Sway Speed     | XX.XXX    | Sway speed at desired lever arm in m/s (positive left); -99.999 to +99.999               |
| 19    | Heading Rate   | QQQQ.QQ   | Heading rate of turn in °/min (positive clockwise); -9999.99 to +9999.99                 |
| 20    | Check sum      | *cs       | XOR of all previous bytes except `$`                                                     |
| 21    | End of frame   | <CR><LF>  | Carriage return and line feed                                                            |

> [!WARNING]
> Ixblue uses different conventions for ship motion measurements. In this frame, both sway and heave values are reversed compared to SBG Systems conventions.

---

### SBG_ECOM_LOG_NMEA_1_INDYN (09) {#SBG_ECOM_LOG_NMEA_1_INDYN}

The `SBG_ECOM_LOG_NMEA_1_INDYN` message is an Ixblue proprietary NMEA log that outputs position, heading, attitude, attitude rate, and speed data.

#### Message Format

```
$INDYN,48.87949927,1.99962275,0.000,218.714,-0.909,0.291,-0.011,-0.073,-0.024,0.019*6A<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_INDYN (09)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name         | Format        | Description                                                                 |
|-------|--------------|---------------|-----------------------------------------------------------------------------|
| 0     | $INDYN       | string        | Message ID – Ixblue proprietary NMEA identifier                             |
| 1     | Latitude     | x.xxxxxxxx    | INS latitude in degrees with 8 decimals                                     |
| 2     | Longitude    | y.yyyyyyyy    | INS longitude in degrees with 8 decimals                                    |
| 3     | Altitude     | z.zzz         | INS altitude above Mean Sea Level in meters, positive upward                |
| 4     | True Heading | h.hhh         | True heading in degrees (0.000 to 359.999)                                  |
| 5     | Roll Angle   | r.rrr         | Roll angle in degrees, positive for port side up (-180.000 to +180.000)     |
| 6     | Pitch Angle  | p.ppp         | Pitch angle in degrees, positive when bow down (-90.000 to +90.000)         |
| 7     | Heading Rate | a.aaa         | Heading rate in °/s (positive when heading increases)                       |
| 8     | Roll Rate    | b.bbb         | Roll rate in °/s (positive when roll increases)                             |
| 9     | Pitch Rate   | c.ccc         | Pitch rate in °/s (positive when pitch increases)                           |
| 10    | Ground Speed | s.sss         | Horizon speed in m/s (positive toward the bow)                              |
| 11    | Check sum    | *cs           | XOR of all previous bytes except `$`                                        |
| 12    | End of frame | <CR><LF>      | Carriage return and line feed                                               |

> [!NOTE]
> The roll, pitch, and heading rates are not the same as the gyroscope values. Gyroscope values are expressed in the body frame (X, Y, Z) and are not unbiased.

> [!WARNING]
> In this message, Ixblue uses a different convention for orientation. The pitch angle is reversed compared to SBG Systems conventions.

---

### SBG_ECOM_LOG_NMEA_1_GGK (10) {#SBG_ECOM_LOG_NMEA_1_GGK}

The `SBG_ECOM_LOG_NMEA_1_GGK` message is a Trimble proprietary NMEA log that outputs time, position, and GNSS-related quality indicators.

#### Message Format

```
$PTNL,GGK,,,,,,,0,00,,,M*30<CR><LF>
$PTNL,GGK,161159.00,013020,4854.61758182,N,00210.08881241,E,1,07,8.3,EHT140.509,M*75<CR><LF>
```

#### Message Structure

- **Message Name (ID):** `SBG_ECOM_LOG_NMEA_1_GGK (10)`
- **Compatibility:** INS capable products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-X.X-blue) ![HPINS](https://img.shields.io/badge/HPINS-X.X-blue)

| Field | Name         | Format         | Description                                                                      |
|-------|--------------|----------------|----------------------------------------------------------------------------------|
| 0     | $PTNL        | string         | Talker ID – Trimble proprietary NMEA identifier, always `PTNL`                   |
| 1     | GGK          | string         | Message ID – Trimble proprietary NMEA identifier, always `GGK`                   |
| 2     | Time         | hhmmss.ss      | UTC time of solution, hours must be two chars                                    |
| 3     | Date         | mmddyy         | UTC date of solution, month must be two chars                                    |
| 4     | Latitude     | ddmm.mmmmmmmm  | INS latitude in degrees (0 to 90) and decimal minutes (8 decimal digits)         |
| 5     | N/S          | char           | ‘N’ for Northern hemisphere latitude, ‘S’ for Southern hemisphere latitude       |
| 6     | Longitude    | dddmm.mmmmmmmm | INS longitude in degrees (0 to 180) and decimal minutes (8 decimal digits)       |
| 7     | E/W          | char           | ‘E’ for Eastern longitude, ‘W’ for Western longitude                             |
| 8     | Pos. Quality | char           | Position quality indicator using INS standard deviation (see Quality indicators) |
| 9     | SV used      | xx             | Number of satellites used in the solution (latest received GNSS fix)             |
| 10    | HDOP         | z.z            | Horizontal Dilution of Precision (1 decimal digit) (see Quality indicators)      |
| 11    | Height       | EHTx.xxx       | Height above ellipsoid with 3 decimal digits and must start with `EHT`           |
| 12    | Height Unit  | M              | Always `M`: Height above ellipsoid measured in meters                            |
| 13    | Check sum    | *cs            | XOR of all previous bytes except `$`                                             |
| 14    | End of frame | <CR><LF>       | Carriage return and line feed                                                    |

> [!NOTE]
> The position quality and DOP are computed using the INS position's estimated standard deviation and are not directly related to the GNSS solution.

---
