# sbgECom Binary Commands

SBG Systems products support a question/answer protocol to execute commands and actions through the sbgECom binary protocol.  
As of ELLIPSE firmware v3 and for all High Performance INS products, these commands primarily encapsulate [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) GET/POST calls over serial interfaces.

For Ethernet-capable devices, the sbgECom binary commands are generally unnecessary, as traditional HTTP protocols should be used for all GET/POST requests.

All device actions, settings, and information are now managed via the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) rather than through sbgECom binary commands.

> [!NOTE]
> ELLIPSE firmware version 3.x still supports legacy sbgECom binary commands from version 2.x to facilitate migration.  
> However, users are encouraged to transition to the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) to leverage new features.

## General Principle

When a command is issued from the host to the device, the device may respond with an acknowledgment (ACK/NACK), or a specific payload.  
If a specific payload is returned, it shares the command identifier with the initial request, differing only in the payload data.

The sbgECom commands can be either accessed over standard serial interfaces or over virtual Ethernet interfaces.

### Serial Interface

SBG Systems products are equipped with multiple serial interfaces for both data output and device configuration.  
High Performance INS devices feature up to five serial interfaces (Port A to Port E), whereas PULSE IMUs may only have a single interface.

`sbgECom` commands are supported exclusively on Port A, meaning that all device configuration must be performed through this serial interface.  
Other serial interfaces are restricted to outputting `sbgECom` messages and do not accept configuration commands.

> [!NOTE]
> For detailed information and limitations regarding each serial interface, please refer to the product's hardware manual.  
> Ensure that the serial interface is not overloaded to maintain proper transmission of sbgECom commands.

### Ethernet Interface

High Performance INS products offer Ethernet connectivity with standard HTTP GET/POST support to access the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).  
This is the recommended method for configuring HPINS products using the JSON-based RestAPI.

Additionally, standard sbgECom commands can be used over the virtual Ethernet interface ETH 0.  
The other Ethernet interfaces (ETH 1 to ETH 4) are designated for outputting sbgECom messages or other supported protocols, such as NMEA.

Each virtual Ethernet interface provides a full-duplex input and output channel and can be configured for either TCP/IP (server or client) or UDP.  
The UDP option minimizes latency by eliminating readback but does not guarantee successful data transmission or reception.  
Both TCP/IP and UDP communications are secured with standard Ethernet CRC layers, ensuring data integrity.

Furthermore, High Performance INS devices automatically create an additional ETH CMD interface dedicated to sbgECom command communications.  
This interface is hardcoded to use UDP with a `52140` listen port and `52414` transmit port, and it is always active.

> [!WARNING]
> When using Ethernet, ensure that the INS is not receiving broadcast network traffic, as it may overload the CPU.  
> Broadcasting is generally not recommended in production environments.  
> If broadcasting cannot be avoided and results in a high volume of data being sent to the INS, consider isolating the INS on a separate sub-network.

## Commands Overview

Below is a list of all available commands for ELLIPSE v3 and High Performance INS products:

| Name (Msg ID)                                                      | Description                                                                                 |
|--------------------------------------------------------------------|---------------------------------------------------------------------------------------------|
| [SBG_ECOM_CMD_ACK (00)](#SBG_ECOM_CMD_ACK)                         | Sends an ACK or NACK in response to a received command.                                     |
| [SBG_ECOM_CMD_SETTINGS_ACTION (01)](#SBG_ECOM_CMD_SETTINGS_ACTION) | Save settings, restore defaults, or reboot (use sbgInsRestApi instead).                     |
| [SBG_ECOM_CMD_INFO (04)](#SBG_ECOM_CMD_INFO)                       | Retrieves device information, such as hardware code and serial number.                      |
| [SBG_ECOM_CMD_API_POST (47)](#SBG_ECOM_CMD_API_POST)               | Executes a POST request to a resource endpoint via sbgInsRestApi (update settings/actions). |
| [SBG_ECOM_CMD_API_GET (48)](#SBG_ECOM_CMD_API_GET)                 | Executes a GET request to a resource endpoint via sbgInsRestApi (query settings/status).    |

> [!NOTE]
> For ELLIPSE firmware versions 2.x and earlier, refer to the [Firmware Reference Manual PDF](https://support.sbg-systems.com/sc/dev/latest/firmware-documentation).

## General Commands

### SBG_ECOM_CMD_ACK (Class: 0x10 | ID: 00) {#SBG_ECOM_CMD_ACK}

This specific command is only sent by the device to acknowledge that a specific command has been properly executed or not.
This frame returns which command is being acknowledged as well as the status (ie successful ACK or an error has occurred NACK).

Sending this command to the device has no effect.

#### Command Details

- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue) ![PULSE](https://img.shields.io/badge/PULSE-1.0-blue)
- **Payload Size:** 4

##### Answer

If the device receives a command that only returns a OK/NOK status or a command that is mal formatted or invalid, the device should answer with the following payload:

| Field                       | Description                                                                       | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|--------|------|--------|
| cmdId                       | The command identifier this ACK/NACK applies to.                                  | uint8  | 1    | 0      |
| classId                     | The message class identifier this ACK/NACK applies to.                            | uint8  | 1    | 1      |
| errorCode                   | Associated error code (see [SbgErrorCode](#SbgErrorCode)).                        | uint16 | 4    | 2      |   

### SBG_ECOM_CMD_SETTINGS_ACTION (Class: 0x10 | ID: 01) {#SBG_ECOM_CMD_SETTINGS_ACTION}

This command provides special settings actions such as saving settings or restoring default settings.  
User calls this command with the payload as described below.

#### Command Details

- **Compatibility:** ELLIPSE, HPINS
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 1

##### Query

The host should send to the device the following payload:

| Field                       | Description                                                                       | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|--------|------|--------|
| action                      | The action to execute as defined in the enum below.                               | uint8  | 1    | 0      |

###### Enum Actions

| Name                              | Value | Description                                                                
|-----------------------------------|-------|---------------------------------------------------------------------------|
| SBG_ECOM_REBOOT_ONLY              |   0   | Software reboot the device.                                               |
| SBG_ECOM_SAVE_SETTINGS            |   1   | Save current settings to FLASH and reboot the device.                     |
| SBG_ECOM_RESTORE_DEFAULT_SETTINGS |   2   | Restore default settings and reboot the device.                           |

##### Answer

If the command is received and processed successfully, the device should answer with an ACK.

> [!NOTE]
> Please update your implementation to use new sbgInsRestApi `api/v1/settings/default`, `api/v1/settings/save` and `api/v1/system/reboot` endpoints.

### SBG_ECOM_CMD_INFO (Class: 0x10 | ID: 04) {#SBG_ECOM_CMD_INFO}

Query the device information such as product code, serial number, firmware version and so on.
Send the command with an empty payload to get the following answer:

#### Command Details

- **Compatibility:** ELLIPSE, HPINS
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-1.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-1.0-blue)
- **Payload Size:** 52

##### Answer

If the command is received and processed successfully, the device should answer with the following payload:

| Field                       | Description                                                                       | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|--------|------|--------|
| productCode                 | The device product code such as ELLIPSE-D-G4A3-B1.                                | string |  32  |    0   |
| serialNumber                | The device unique serial number.                                                  | uint32 |   4  |   32   |
| mnfVersion                  | Procedure and tools version used to manufacture and validate the product.         | uint32 |   4  |   36   |
| mnfYear                     | Year the product was manufactured.                                                | uint16 |   2  |   40   |
| mnfMonth                    | Month the product was manufactured.                                               | uint8  |   1  |   42   |
| mnfDay                      | Day the product was manufactured.                                                 | uint8  |   1  |   43   |
| hwRevision                  | The device hardware revision (Major.Minor.0.0).                                   | uint32 |   4  |   44   |
| swRevision                  | The device firmware version using.                                                | uint32 |   4  |   48   |

The productCode field is a NULL terminated C String. All versions fields should be interpreted as described in this [section](#VERSION_TYPE).

> [!NOTE]
> Please update your implementation to use new sbgInsRestApi `api/v1/info` endpoint to query device information.

## GET/POST (sbgInsRestApi)
High Performance INS products as well as new SBG Systems products use a new configuration and commands layer based on REST API concepts. REST API are very common in Internet of Things (IoT) devices and rely on Ethernet/HTTP connections.

The REST API is based on JSON (JavaScript Object Notation) specification only. The JSON is widely used in the industry and there are plenty of libraries available to manipulate JSON.

SBG Systems products’ offer REST API configuration and commands over Ethernet links using standard HTTP GET and POST methods. This manual doesn’t cover the sbgInsRest API usage and definition so please read the dedicated sbgInsRest API manual for more information.

The REST API can also be accessed and used over serial connections. This is this part that is covered in this documentation. The sbgECom uses two dedicated commands to execute GET and POST requests over a standard serial link.

### SBG_ECOM_CMD_API_POST (Class: 0x10 | ID: 47) {#SBG_ECOM_CMD_API_POST}

Execute a POST method on a specified sbgInsRestApi path.
The POST method is used to update a setting value or to execute a command.

Some POST paths require a body (ie a payload) while others, such as commands for a system reboot, have no body at all.

The example below, ask the device to save settings in FLASH memory:

```sh
/api/v1/settings/save
```

#### Command Details

- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-4.0-blue) ![PULSE](https://img.shields.io/badge/PULSE-1.0-blue)
- **Payload Size:** variable

##### Query

The host should send to the device the following payload:

| Field                       | Description                                                                       | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|--------|------|--------|
| path                        | The path string to execute POST method on.                                        | string | -    | -      |
| query                       | The query parameter string or empty string if not used.                           | string | -    | -      |
| body                        | The JSON body string or empty string if not used.                                 | string | -    | -      |

##### Answer

If the command is received and processed successfully, the device should answer with the following payload:

| Field                       | Description                                                                       | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|--------|------|--------|
| statusCode                  | HTTP return code for the POST request. 200 for success.                           | uint16 | 2    | 0      |
| jsonPayload                 | Variable JSON payload returned by the device.                                     | string | -    | 2      |

> [!NOTE]
> All strings such as path, query and body fields have to be NULL terminated.

> [!NOTE]
> The device might answer with payload exceeding the standard 4096 frame size. In this case, the extended frame format is used.

### SBG_ECOM_CMD_API_GET (Class: 0x10 | ID: 48) {#SBG_ECOM_CMD_API_GET}

Used to GET a resource from the device on a sbgInsRestApi path.  
For example, you can query the device settings or the device information / status using this command.

An optional query parameters string might be provided as shown in the example below:

```sh
/api/v1/settings?delta=true&format=pretty
```

This path query only the settings that have changed from the device default settings and pretty print the returned JSON.  
The path itself is `/api/v1/settings` with two query parameters {delta} and {format}.

#### Command Details

- **Compatibility:** All products
- **Firmware:** ![ELLIPSE](https://img.shields.io/badge/ELLIPSE-3.0-blue) ![HPINS](https://img.shields.io/badge/HPINS-4.0-blue) ![PULSE](https://img.shields.io/badge/PULSE-1.0-blue)
- **Payload Size:** variable

##### Query

The host should send to the device the following payload:

| Field                       | Description                                                                       | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|--------|------|--------|
| path                        | The path string to execute GET method on.                                         | string | -    | -      |
| query                       | The query parameter string or empty string if not used.                           | string | -    | -      |

##### Answer

If the command is received and processed successfully, the device should answer with the following payload:

| Field                       | Description                                                                       | Format | Size | Offset |
|-----------------------------|-----------------------------------------------------------------------------------|--------|------|--------|
| statusCode                  | HTTP return code for the GET request. 200 for success.                            | uint16 | 2    | 0      |
| jsonPayload                 | Variable JSON payload returned by the device.                                     | string | -    | 2      |

> [!NOTE]
> All strings such as path and query fields have to be NULL terminated.

> [!NOTE]
> The device might answer with payload exceeding the standard 4096 frame size. In this case, the extended frame format is used.
