# sbgECom Binary Protocol

The sbgECom protocol is designed for compact and secure communication, with its binary format and 16-bit CRC for data integrity.  
This makes it highly efficient for inertial navigation communications, which require high throughput and data reliability.

Using the sbgECom binary protocol is recommended to fully access a device's features and achieve the highest level of integration with host systems.

> [!NOTE]
> All multi-byte fields use little-endian organization, consistent with the rest of the sbgECom protocol.

## Standard Frame

The standard frame is the original format used since the sbgECom protocol's introduction in 2013.  
It is synchronized using Sync 1 (0xFF) and Sync 2 (0x5A) bytes, with the frame end marked by the ETX (0x33) byte.

This frame supports payloads of up to 4086 bytes, resulting in a maximum total frame size of 4096 bytes.

### Frame Definition

The standard frame structure is as follows:

| Field        | SYNC 1     | SYNC 2     | MSG         | CLASS         | LENGTH                 | DATA         | CRC         | ETX      |
|--------------|------------|------------|-------------|---------------|------------------------|--------------|-------------|--------- |
| Size (bytes) | 1          | 1          | 1           | 1             | 2                      | 0 to 4086    | 2           | 1        |
| Description  | Start Byte | Sync Byte  | Message ID  | Message Class | Length of DATA section | Payload data | 16 bit CRC  | End Byte |
| Value        | 0xFF       | 0x5A       | -           | -             | -                      | -            | -           | 0x33     |

### Fields Details

Please find below more explanations on each field used in the frame protocol:

  - **SYNC 1:** First synchronization byte, always 0xFF.
  - **SYNC 2:** Second synchronization byte, always 0x5A.
  - **MSG:**    Message identifier within the message class.
  - **CLASS:**  Message class identifier with MSB bit set to 0 (see [Message Class](#msgClass)).
  - **LENGTH:** Data length in bytes, from 0 to 4086.
  - **DATA:**   Full payload as a raw binary stream (optional if `LENGTH` is 0).
  - **CRC:**    16 bits CRC computed from the `MSG` to `DATA` fields (see [CRC Definition](#crcDefinition)).
  - **ETX:**    End-of-frame byte, always 0x33.

> [!NOTE]
> **Third-Party Frames:** Some third-party frames may not conform to this format. Specific frame formats are defined separately, and users must handle different protocols accordingly.

## Extended Large Frame

The sbgECom low-level protocol layer supports payloads of up to 4086 bytes. Since sbgECom version 3.x, introduced in January 2022, the protocol has been extended to support larger payloads, primarily for the new [SBG_ECOM_CMD_API_GET](#SBG_ECOM_CMD_API_GET) and [SBG_ECOM_CMD_API_POST](#SBG_ECOM_CMD_API_POST) commands.

### Extended Protocol Usage

The extended large frame protocol is designed for commands that require payloads exceeding the standard maximum size of 4086 bytes.  
These extended frames are used exclusively for commands and are not applied to binary output messages, even if the data size exceeds 4086 bytes.

sbgECom commands follow a question/answer scheme between the host and the device, where the device sends the complete response at once, without interleaving other output messages.  
If a large response is required, the extended large frames are sent sequentially, ensuring that no other sbgECom output messages are transmitted in the meantime.

For binary output messages with potentially large data, such as [SBG_ECOM_LOG_GPSX_RAW](#SBG_ECOM_LOG_GPSX_RAW), the data is divided into smaller individual chunks.  
This approach prevents large data bursts and minimizes delays for other *time-critical* outputs on the serial interface.  
In these cases, it is the responsibility of the application layer to reassemble the payloads as needed.

The sbgECom library manages all these mechanisms automatically, and SBG Systems strongly recommends using the sbgECom library implementation when possible.

### Backward Compatibility

This protocol extension is fully backward compatible. If the new [SBG_ECOM_CMD_API_GET](#SBG_ECOM_CMD_API_GET) and [SBG_ECOM_CMD_API_POST](#SBG_ECOM_CMD_API_POST) commands are not used, there is no need to update existing implementations.

### Large Frame Definition

To support payloads larger than 4086 bytes, a pagination system is used, with a Transmission ID (`TX ID`) field added to improve robustness and prevent frame mixing.

A large frame is identified when the MSB of the CLASS field is set to 1 (i.e., 0x80). The large frame structure is as follows:

| Field       | SYNC 1     | SYNC 2    | MSG        | CLASS         | LENGTH          | TX ID       | PAGE IDX   | NR PAGES    | DATA         | CRC        | ETX      |
|-------------|------------|-----------|------------|---------------|-----------------|-------------|------------|-------------|--------------|------------|----------|
| Size (bytes)| 1          | 1         | 1          | 1             | 2               | 1           | 2          | 2           | 0 to 4081    | 2          | 1        |
| Description | Start Byte | Sync Byte | Message ID | Message Class | Data length + 5 | Transfer ID | Page Index | Pages Count | Payload data | 16 bit CRC | End Byte |
| Value       | 0xFF       | 0x5A      | -          | 0b1xxxxxxx    | -               | -           | -          | -           | -            | -          | 0x33     |

> [!WARNING]
> Large frames are identified by setting the MSB in the CLASS field to 1 (i.e., 0x80).

### Fields Details

Please find below more explanations on each field used in the large frame protocol:

  - **SYNC 1:**   First synchronization byte, always 0xFF.
  - **SYNC 2:**   Second synchronization byte, always 0x5A.
  - **MSG:**      Message identifier within the message class.
  - **CLASS:**    Message class identifier with MSB bit set to 1 (see [Message Class](#msgClass)).
  - **LENGTH:**   Total length in bytes, including `TX ID`, `PAGE IDX`, `NR PAGES` and `DATA`.
  - **TX ID**:    Transmission identifier, incremented with each new transmission.
  - **PAGE IDX**: Zero-based index of the current page being transmitted.
  - **NR PAGES**: Total number of pages in the transmission.
  - **DATA**:     Partial payload as a raw binary stream, to be reassembled by the host.
  - **CRC:**      16 bits CRC computed from the `MSG` to `DATA` fields (see [CRC Definition](#crcDefinition)).
  - **ETX:**      End-of-frame byte, always 0x33.

> [!NOTE]
> Large frames are only used for `SBG_ECOM_CMD_API_GET` and `SBG_ECOM_CMD_API_POST` commands, not for binary output messages.

> [!NOTE]
> The theoretical maximum payload size is 65535 pages, yielding up to 255 MB of data. However, in practice, it is rare to exceed 10 pages.

## Messages ID/Classes {#msgClass}

The sbgECom protocol uses a combination of a Message Class and a Message ID within that class to identify each message or command.

The table below lists all the available message classes used in the sbgECom protocol:

| Class                            | ID   | Description                                                                                                |
|----------------------------------|------|------------------------------------------------------------------------------------------------------------|
| SBG_ECOM_CLASS_LOG_ECOM_0        | 0x00 | Primary sbgECom binary output messages used for reading data and status.                                   |
| SBG_ECOM_CLASS_LOG_ECOM_1        | 0x01 | Reserved, not used currently.                                                                              |
| SBG_ECOM_CLASS_LOG_NMEA_0        | 0x02 | Standard NMEA output messages generated from the AHRS/INS solution (e.g. *GGA*, *HDT*, *VTG*).             |
| SBG_ECOM_CLASS_LOG_NMEA_1        | 0x03 | Proprietary output messages using NMEA-style formatting, typically starting with `$P` (e.g. *PRDID* log).  |
| SBG_ECOM_CLASS_LOG_THIRD_PARTY_0 | 0x04 | Third party output messages using binary or ASCII custom formats (e.g *TSS1* log).                         |
| SBG_ECOM_CLASS_LOG_NMEA_GNSS     | 0x05 | Standard NMEA messages generated directly from GNSS data, rather than the INS (*GGA*, *RMC*, *ZDA*).       |
| SBG_ECOM_CLASS_CMD_0             | 0x10 | Main sbgECom binary commands for configuration and queries.                                                |

## CRC Definition {#crcDefinition}

The sbgECom protocol uses a 16-bit Cyclic Redundancy Check (CRC) to detect corrupted messages.

The CRC is calculated over the `MSG` field up to the end of the `DATA` field.  
The polynomial used is *0x8408*, with an initial value of *0*.

To optimize the CRC computation, the sbgECom library includes an efficient C implementation located in the file: `sbgECom/common/crc/sbgCrc.c`.  
This implementation leverages a lookup table for faster processing.

For reference, below is a basic (non-optimized) C implementation of the 16-bit CRC calculation:

```C
/*!
 * Compute a CRC for a specified buffer.
 *
 * \param[in]	pBuffer       Read only buffer to compute the CRC on.
 * \param[in]	bufferSize    Buffer size in bytes.
 * \return                  The computed 16 bit CRC.
 */
uint16_t calcCRC(const void *pBuffer, size_t bufferSize)
{
  const uint8_t *pBytesArray = (const uint8_t*)pBuffer;
  uint16_t       poly = 0x8408;
  uint16_t       crc = 0;
  uint8_t        carry;

  assert(pBuffer);
       
  for (size_t j = 0; j < bufferSize; j++)
  {
    crc = crc ^ pBytesArray[j];
            
    for (size_t i = 0; i < 8; i++)
    {
      carry = crc & 1;
      crc   = crc / 2;

      if (carry)
      {
        crc = crc^poly;
      }
    }
  }

  return crc;
}
```

## sbgECom Protocol Version {#SBG_ECOM_VERSION_DEF}

Determining the sbgECom protocol version supported by your device's firmware is crucial for compatibility and to access latest features.  
This section explains how to identify the sbgECom version your device supports and details the versioning policy.

### Versioning Policy

The sbgECom protocol version is denoted by a `Major.Minor` format, derived from the full sbgECom C library version (`Major.Minor.Build-Qualifier`).

  - **Major Version:**  Incremented for changes that may require protocol or C API implementation updates indicating potential breaking changes.
  - **Minor Version:**  Incremented for backward-compatible changes, ensuring that no action is required from users.

> [!WARNING]
> While SBG Systems strives to maintain forward and backward compatibility across versions, it is the customer's responsibility to re-qualify their integration following any firmware or sbgECom updates.

### Protocol Version via sbgInsRestApi

For ELLIPSE v3 (firmware v3 and above), High Performance INS (HPINS), and PULSE IMUs, the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi) provides an easy way to retrieve the sbgECom protocol version supported by your device.

To retrieve the sbgECom protocol version, execute a GET request on the `api/v1/info` endpoint. You can do this in several ways:
  - **Ethernet Capable Products:**  Use a web browser or a tool like `curl` on the url `http://DEVICE_IP/api/v1/info`.
  - **Serial Only Products:**       Use the sbgEComApi CLI tool: `sbgEComApi -s COM1 -r 115200 -g api/v1/info`.
  - **Programmatically:**           Implement the sbgECom `SBG_ECOM_CMD_API_GET` command or perform HTTP GET requests to interact with the REST API.

The response will include:
  - **eComVersion:** Indicates the sbgECom protocol and library version (Major.Minor).
  - **apiVersion:** Indicates the version of the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi) implemented on the device.

Example JSON response:

```json
{
  "productCode": "EKINOX-UG-00",
  "serialNumber": "000000001",
  "hwRevision": "1.0.0.0",
  "mnfVersion": "5.0.2275-stable",
  "mnfDate": "2023-10-11",
  "macAddress": "98:5C:93:00:00:00",
  "productType": "ekinoxUGV1",
  "fmwVersion": "5.3.2436-dev",
  "btVersion": "3.6.9-stable",
  "eComVersion": "4.0",
  "apiVersion": "1.3"
}
```

For more details, refer to the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi) documentation.

### Protocol Version for Legacy Products

For older products or firmware versions, refer to the tables below to determine the corresponding sbgECom version for a specific product and firmware version.

#### ELLIPSE Series

The following table provides information on the sbgECom protocol version supported by each ELLIPSE firmware release before version `3.0`:

| Date       | Firmware Version | sbgECom Version |
|------------|------------------|-----------------|
| 2024-03-26 | 2.6              | 3.2             |
| 2022-12-09 | 2.5              | 3.2             |
| 2022-06-09 | 2.4              | 3.2             |
| 2022-01-05 | 2.3              | 3.1             |
| 2021-08-19 | 2.2              | 2.1             |
| 2020-12-01 | 2.1              | 2.0             |
| 2020-07-20 | 2.0              | 2.0             |
| 2019-09-16 | 1.7              | 1.11            |
| 2018-11-08 | 1.6              | 1.10            |
| 2018-07-18 | 1.5              | 1.8             |

#### High Performance INS (HPINS)

The following table details the sbgECom protocol version supported by each High Performance INS product firmware release before version `5.0`:

| Date       | Firmware Version | sbgECom Version |
|------------|------------------|-----------------|
| 2023-04-04 | 4.2              | 3.2             |
| 2022-07-08 | 4.0              | 3.1             |
| 2021-10-06 | 3.1              | 2.0             |
| 2020-07-17 | 3.0              | 2.0             |
| 2019-10-11 | 2.1              | 1.11            |
| 2019-12-17 | 2.0              | 1.10            |
| 2017-12-06 | 1.5              | 1.7             |

> [!NOTE]
> SBG Systems offers a free firmware update policy for the entire product lifetime.  
> Please update your device to the latest firmware whenever possible to ensure compatibility and access to the latest features.
