# General definitions

General types, enums, values definitions that are used across the sbgECom protocol documentation and sbgECom C library implementation.

## Endianness

The sbgECom binary protocol uses the little-endian data format.  
However, some third-party binary messages may use the big-endian format. These instances are explicitly noted in this documentation.

The sbgECom communication library is designed to be compatible with both little-endian and big-endian platforms.  
You can configure the host platform's endianness by setting the macro `SBG_CONFIG_BIG_ENDIAN` to `1`.

> [!NOTE]
> For ASCII or NMEA messages, the platform's endianness does not impact the parsing or generation of these messages.

## Types Definitions

This section defines common types that are using through the documentation and protocol.

### Scalar Types

The following fundamental scalar types are used throughout the protocol documentation:

| Type   | Description                                                                               |
|--------|-------------------------------------------------------------------------------------------|
| Mask   | An unsigned integer variable used to store a set of bit masks.                            |
| Enum   | A set of named integer values representing a status or state.                             |
| bool   | 8-bit boolean; `0x00` is FALSE, `0x01` is TRUE.                                           |
| uint8  | 8-bit unsigned integer.                                                                   |
| int8   | 8-bit signed integer.                                                                     |
| uint16 | 16-bit unsigned integer.                                                                  |
| int16  | 16-bit signed integer.                                                                    |
| uint32 | 32-bit unsigned integer.                                                                  |
| int32  | 32-bit signed integer.                                                                    |
| uint64 | 64-bit unsigned integer.                                                                  |
| int64  | 64-bit signed integer.                                                                    |
| float  | 32-bit single-precision floating point, standard IEEE 754 format.                         |
| double | 64-bit double-precision floating point, standard IEEE 754 format.                         |
| void[] | Data buffer with variable length.                                                         |
| string | Null-terminated ASCII C string. The maximum size of the string is defined in the message. |

> [!NOTE]
> The `Mask` and `Enum` types do not have a predefined size in bytes. Refer to the specific message or command payload documentation for the corresponding size.

> [!NOTE]
> The [] symbol denotes an array of scalar types. For example, float[] indicates an array of single-precision floating point values.

### Mathematical Types

Please find below the documentation for 3D mathematical vectors and 3x3 matrices used through the protocol documentation:

### Vector Objects

Vectors are stored as a 1D array of `float` or `double` components, with the components arranged sequentially in memory.

Mathematically, a 3D vector *V* with components *x*, *y*, *z* can be represented as:

```
      / x \
  V = | y |
      \ z /
```

In memory, the vector is stored as:

```C
[x, y, z]
```

### Matrix Objects

Matrices are stored as a 1D array of `float` or `double` elements, organized in a column-major format.  
This means that the elements of the matrix are stored in memory column by column.

For example, a 3x3 matrix *M* with elements a<sub>ij</sub> (where *i* is the row index and *j* is the column index) is represented as:

```
      / a11  a12  a13 \
  M = | a21  a22  a23 |
      \ a31  a32  a33 /
```

In memory, this matrix is stored as:

```C
[a11, a21, a31, a12, a22, a32, a13, a23, a33]
```

In this layout, the first three elements correspond to the first column of the matrix, the next three elements correspond to the second column, and the last three elements correspond to the third column.

### Version/Revision Type {#VERSION_TYPE}

Some commands and messages contain version/revision numbers encoded as 32-bit unsigned integers.  
These are used to track different versions of hardware and software components. 

There are two schemes, each with distinct usage and interpretation:
 - **Basic Revision**: Used for hardware revisions, following a simple `A.B.C.D` format.
 - **Software Version**: Used for firmware and software, following a `Major.Minor.Build-Qualifier` format.

The most significant bit (MSB) of the version number determines the scheme in use:
- **MSB = 0**: Indicates the basic revision scheme.
- **MSB = 1**: Indicates the software version scheme.

#### Basic Revision Scheme

This scheme follows the `A.B.C.D` format and is used for versioning hardware and components, excluding firmware or software.

The basic revision scheme is organized in memory as follows:

| Field   | Size (bits) | Definition                                                                  |
|---------|-------------|-----------------------------------------------------------------------------|
| SCHEME  | 1 (MSB)     | Set to 0 for basic revision scheme.                                         |
| A       | 7           | A version number (0 to 127).                                                |
| B       | 8           | B version number (0 to 255).                                                |
| C       | 8           | C version number (0 to 255).                                                |
| D       | 8           | D version number (0 to 255).                                                |

> [!NOTE]
> It is common to use only `A.B` as `Major.Minor` for hardware revisions, leaving `C.D` as zero.

#### Software Version Scheme

This scheme uses the `Major.Minor.Build-Qualifier` format and applies to all firmware and software items.

The software version scheme is organized in memory as follows:

| Field     | Size (bits) | Definition                                                                |
|-----------|-------------|---------------------------------------------------------------------------|
| SCHEME    | 1 (MSB)     | Set to 1 for software version scheme.                                     |
| QUALIFIER | 3           | Software release qualifier (see [VERSION_QUALIFIER](#VERSION_QUALIFIER)). |
| MAJOR     | 6           | Major version number (0 to 63).                                           |
| MINOR     | 6           | Minor version number (0 to 63).                                           |
| BUILD     | 16          | Build number (0 to 65535).                                                |

##### Version Qualifier Enum {#VERSION_QUALIFIER}

The `QUALIFIER` field indicates the build type for a firmware release:

| Name                          | Value | Description                                                                                  |
|-------------------------------|-------|----------------------------------------------------------------------------------------------|
| SBG_VERSION_QUALIFIER_DEV     | 0     | Development build (pre-alpha). Used for testing new technologies.                            |
| SBG_VERSION_QUALIFIER_ALPHA   | 1     | Alpha version, may be unstable, missing features, API can change.                            |
| SBG_VERSION_QUALIFIER_BETA    | 2     | Beta version, feature-freeze, can be unstable, API should not change.                        |
| SBG_VERSION_QUALIFIER_RC      | 3     | Release Candidate, feature-freeze, no known bugs, API is stable.                             |
| SBG_VERSION_QUALIFIER_STABLE  | 4     | Stable release, suitable for production environments.                                        |
| SBG_VERSION_QUALIFIER_HOT_FIX | 5     | Bug fixes applied to a stable version, under validation before releasing a new stable build. |

### IPv4 Address Type

The sbgECom protocol defines the type `sbgIpAddress` to handle an IPv4 address.  
The underlying type is a `uint32` that stores the four 8-bit IP address fields in big-endian order to comply with Ethernet endianness.  

As a result, the IP address 192.168.1.2 (A.B.C.D) is organized in memory as follows:

#### Little Endian Platform

| Byte Index   | 0 (LSB) | 1     | 2     | 3 (MSB) |
|--------------|---------|-------|-------|---------|
| Decimal      | 192     | 168   | 1     | 2       |
| Hex          | 0xC0    | 0xA8  | 0x01  | 0x02    |

**uint32 value**: `33 663 168` (in decimal).

#### Big Endian Platform

| Byte Index | 0 (MSB) | 1     | 2     | 3 (LSB) |
|------------|---------|-------|-------|---------|
| Decimal    | 192     | 168   | 1     | 2       |
| Hex        | 0xC0    | 0xA8  | 0x01  | 0x02    |

**uint32 value**: `3 232 235 778` (in decimal).

## Error Code Enumeration {#SbgErrorCode}

Through the protocol documentation and sbgECom library implementation, the following error codes are being used:

| Error Code                          | Value | Description                                                                               |
|-------------------------------------|-------|-------------------------------------------------------------------------------------------|
| SBG_NO_ERROR                        |   0   | The command has been properly executed.                                                   |
| SBG_ERROR                           |   1   | Command could not be executed properly due to a generic error.                            |
| SBG_NULL_POINTER                    |   2   | A pointer is null and it is not expected.                                                 |
| SBG_INVALID_CRC                     |   3   | A frame with an invalid CRC has been received.                                            |
| SBG_INVALID_FRAME                   |   4   |	The received frame is invalid and mal-formatted.                                          |
| SBG_TIME_OUT                        |   5   |	An operation has timed out and no answer has been received.                               |
| SBG_WRITE_ERROR                     |   6   |	Some data couldn't be written over an interface (serial/file/ethernet).                   |
| SBG_READ_ERROR                      |   7   |	Some data couldn't be read over an interface (serial/file/ethernet).                      |
| SBG_BUFFER_OVERFLOW                 |   8   | A buffer is too small to contain so much data.                                            |
| SBG_INVALID_PARAMETER               |   9   | A parameter has a non valid value.                                                        |                          
| SBG_NOT_READY                       |  10   | An operation or request couldn't be completed such as no data received or device is busy. |
| SBG_MALLOC_FAILED                   |  11   | Failed to allocate a buffer (out of memory).                                              |
| SBG_DEVICE_NOT_FOUND                |  16   | A resource or device couldn't be found.                                                   |
| SBG_OPERATION_CANCELLED             |  17   | An operation has been cancelled by the user.                                              |
| SBG_NOT_CONTINUOUS_FRAME            |  18   | We have received a frame that is part of a sequence but are missing the end.              |
| SBG_INCOMPATIBLE_HARDWARE           |  19   | The command cannot be executed because of hardware incompatibility.                       |
| SBG_INVALID_VERSION                 |  20   | Command cannot be executed because of software incompatibility.                           |

## Conventions & Units

SBG Systems IMU, AHRS and INS devices adhere to the International System of Units (SI) where applicable.  
The device coordinate frame is defined as North East Down (NED).

| Physical Quantity   | Unit Description                                                         |
|---------------------|--------------------------------------------------------------------------|
| Angle               | Radians (rad), representing roll, pitch, and yaw.                        |
| Rotational Speed    | Radians per second (rad/s or rad.s⁻¹).                                   |
| Acceleration        | Meters per second squared (m/s² or m.s⁻²).                               |
| Velocity            | Meters per second (m/s or m.s⁻¹).                                        |
| Latitude            | Degrees, positive values indicate North, negative values indicate South. |
| Longitude           | Degrees, positive values indicate East, negative values indicate West.   |
| Altitude            | Meters, altitude above Mean Sea Level (MSL), positive upward.            |
| Height              | Meters, height above the WGS-84 Ellipsoid, positive upward.              |
| Heave               | Vertical displacement, positive downward.                                |
| Surge               | Longitudinal displacement, positive toward the vessel's bow.             |
| Sway                | Transverse displacement, positive toward the vessel's starboard side.    |

## GNSS Definitions
This section covers common definitions related to GNSS used by the sbgECom binary protocol.

### Constellations {#GNSS_CONSTELLATION}
Below are the enum definitions for all GNSS constellations.

| Enum                              | Value | Description                      |
|-----------------------------------|-------|----------------------------------|
| SBG_ECOM_CONSTELLATION_ID_UNKNOWN | 0     | Unknown                          |
| SBG_ECOM_CONSTELLATION_ID_GPS     | 1     | GPS                              |
| SBG_ECOM_CONSTELLATION_ID_GLONASS | 2     | GLONASS                          |
| SBG_ECOM_CONSTELLATION_ID_GALILEO | 3     | Galileo                          |
| SBG_ECOM_CONSTELLATION_ID_BEIDOU  | 4     | BeiDou                           |
| SBG_ECOM_CONSTELLATION_ID_QZSS    | 5     | QZSS                             |
| SBG_ECOM_CONSTELLATION_ID_SBAS    | 6     | SBAS                             |
| SBG_ECOM_CONSTELLATION_ID_IRNSS   | 7     | IRNSS                            |
| SBG_ECOM_CONSTELLATION_ID_LBAND   | 8     | L-Band                           |

### Satellite Id {#GNSS_SAT_ID}

The sbgECom protocol uses the RINEX satellite Id definition to identify each space vehicle tracked by a GNSS receiver.  
The satellite Id definition depends on the GNSS constellation:
- **PRN** for GPS, Galileo, BeiDou, IRNSS
- **Slot number** for GLONASS
- **PRN-100** for SBAS
- **PRN-192** for QZSS

### Signals {#GNSS_SIGNAL_ID}
Below are the enum definitions for GNSS signals.

#### GPS constellation (10 to 39)
| Enum                                 | Value |
|--------------------------------------|-------|
| SBG_ECOM_SIGNAL_ID_UNKNOWN           | 0     |
| SBG_ECOM_SIGNAL_ID_GPS_L1C_DP        | 10    |
| SBG_ECOM_SIGNAL_ID_GPS_L1C_D         | 11    |
| SBG_ECOM_SIGNAL_ID_GPS_L1C_P         | 12    |
| SBG_ECOM_SIGNAL_ID_GPS_L1_W          | 13    |
| SBG_ECOM_SIGNAL_ID_GPS_L1_CA         | 14    |
| SBG_ECOM_SIGNAL_ID_GPS_L1P           | 15    |
| SBG_ECOM_SIGNAL_ID_GPS_L1_PY         | 16    |
| SBG_ECOM_SIGNAL_ID_GPS_L1M           | 17    |
| SBG_ECOM_SIGNAL_ID_GPS_L2C_ML        | 18    |
| SBG_ECOM_SIGNAL_ID_GPS_L2C_L         | 19    |
| SBG_ECOM_SIGNAL_ID_GPS_L2_SEMICL     | 20    |
| SBG_ECOM_SIGNAL_ID_GPS_L2_W          | 21    |
| SBG_ECOM_SIGNAL_ID_GPS_L2_CA         | 22    |
| SBG_ECOM_SIGNAL_ID_GPS_L2C_M         | 23    |
| SBG_ECOM_SIGNAL_ID_GPS_L2_PY         | 24    |
| SBG_ECOM_SIGNAL_ID_GPS_L2M           | 25    |
| SBG_ECOM_SIGNAL_ID_GPS_L2P           | 26    |
| SBG_ECOM_SIGNAL_ID_GPS_L5_IQ         | 27    |
| SBG_ECOM_SIGNAL_ID_GPS_L5_I          | 28    |
| SBG_ECOM_SIGNAL_ID_GPS_L5_Q          | 29    |

#### GLONASS constellation (40 to 59)
| Enum                                 | Value |
|--------------------------------------|-------|
| SBG_ECOM_SIGNAL_ID_GLONASS_G1_P      | 40    |
| SBG_ECOM_SIGNAL_ID_GLONASS_G1_CA     | 41    |
| SBG_ECOM_SIGNAL_ID_GLONASS_G2_P      | 42    |
| SBG_ECOM_SIGNAL_ID_GLONASS_G2_CA     | 43    |
| SBG_ECOM_SIGNAL_ID_GLONASS_G3_I      | 44    |
| SBG_ECOM_SIGNAL_ID_GLONASS_G3_Q      | 45    |
| SBG_ECOM_SIGNAL_ID_GLONASS_G3_IQ     | 46    |

#### Galileo constellation (60 to 99)
| Enum                                 | Value |
|--------------------------------------|-------|
| SBG_ECOM_SIGNAL_ID_GALILEO_E1_BC     | 60    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E1_C      | 61    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E1_B      | 62    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E1_A      | 63    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E1_ABC    | 64    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E5B_IQ    | 65    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E5B_I     | 66    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E5B_Q     | 67    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E5A_IQ    | 68    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E5A_I     | 69    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E5A_Q     | 70    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E5_IQ     | 71    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E5_I      | 72    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E5_Q      | 73    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E6_BC     | 74    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E6_C      | 75    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E6_B      | 76    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E6_ABC    | 77    |
| SBG_ECOM_SIGNAL_ID_GALILEO_E6_A      | 78    |

#### Beidou constellation (100 to 149)
| Enum                                 | Value |
|--------------------------------------|-------|
| SBG_ECOM_SIGNAL_ID_BEIDOU_B1IQ       | 100   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B1I        | 101   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B1Q        | 102   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_P      | 103   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_DP     | 104   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_D      | 105   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_P      | 106   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_DP     | 107   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_D      | 108   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2IQ       | 109   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2I        | 110   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_P      | 111   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_DP     | 112   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_D      | 113   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2Q        | 114   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_P      | 115   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_DP     | 116   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_D      | 117   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_P     | 118   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_DP    | 119   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_D     | 120   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B3IQ       | 121   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B3I        | 122   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B3Q        | 123   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_D      | 124   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_P      | 125   |
| SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_DP     | 126   |

#### QZSS constellation (150 to 179)
| Enum                                 | Value |
|--------------------------------------|-------|
| SBG_ECOM_SIGNAL_ID_QZSS_L1C_DP       | 150   |
| SBG_ECOM_SIGNAL_ID_QZSS_L1C_D        | 151   |
| SBG_ECOM_SIGNAL_ID_QZSS_L1C_P        | 152   |
| SBG_ECOM_SIGNAL_ID_QZSS_L1_CA        | 153   |
| SBG_ECOM_SIGNAL_ID_QZSS_L1_SAIF      | 154   |
| SBG_ECOM_SIGNAL_ID_QZSS_L1_SB        | 155   |
| SBG_ECOM_SIGNAL_ID_QZSS_L2C_ML       | 156   |
| SBG_ECOM_SIGNAL_ID_QZSS_L2C_L        | 157   |
| SBG_ECOM_SIGNAL_ID_QZSS_L2C_M        | 158   |
| SBG_ECOM_SIGNAL_ID_QZSS_L5_IQ        | 159   |
| SBG_ECOM_SIGNAL_ID_QZSS_L5_I         | 160   |
| SBG_ECOM_SIGNAL_ID_QZSS_L5_Q         | 161   |
| SBG_ECOM_SIGNAL_ID_QZSS_L5S_IQ       | 162   |
| SBG_ECOM_SIGNAL_ID_QZSS_L5S_I        | 163   |
| SBG_ECOM_SIGNAL_ID_QZSS_L5S_Q        | 164   |
| SBG_ECOM_SIGNAL_ID_QZSS_L6_P         | 165   |
| SBG_ECOM_SIGNAL_ID_QZSS_L6_DP        | 166   |
| SBG_ECOM_SIGNAL_ID_QZSS_L6_D         | 167   |
| SBG_ECOM_SIGNAL_ID_QZSS_L6_E         | 168   |
| SBG_ECOM_SIGNAL_ID_QZSS_L6_DE        | 169   |

#### SBAS system (180 to 199)
| Enum                                 | Value |
|--------------------------------------|-------|
| SBG_ECOM_SIGNAL_ID_SBAS_L1_CA        | 180   |
| SBG_ECOM_SIGNAL_ID_SBAS_L5_I         | 181   |
| SBG_ECOM_SIGNAL_ID_SBAS_L5_Q         | 182   |
| SBG_ECOM_SIGNAL_ID_SBAS_L5_IQ        | 183   |

#### IRNSS / NAVIC constellation (200 to 219)
| Enum                                 | Value |
|--------------------------------------|-------|
| SBG_ECOM_SIGNAL_ID_IRNSS_L5_A        | 200   |
| SBG_ECOM_SIGNAL_ID_IRNSS_L5_B        | 201   |
| SBG_ECOM_SIGNAL_ID_IRNSS_L5_C        | 202   |
| SBG_ECOM_SIGNAL_ID_IRNSS_L5_BC       | 203   |
| SBG_ECOM_SIGNAL_ID_IRNSS_S9_A        | 204   |
| SBG_ECOM_SIGNAL_ID_IRNSS_S9_B        | 205   |
| SBG_ECOM_SIGNAL_ID_IRNSS_S9_C        | 206   |
| SBG_ECOM_SIGNAL_ID_IRNSS_S9_BC       | 207   |

#### L-Band system (220 to 230)
| Enum                                 | Value |
|--------------------------------------|-------|
| SBG_ECOM_SIGNAL_ID_LBAND             | 220   |
