# Introduction

The sbgECom protocol is a binary protocol specifically designed for [SBG Systems](https://www.sbg-systems.com/) IMU, AHRS, and INS products.  
It includes commands for device configuration and output messages for reading data and status.

The sbgECom Library, the official C implementation of this protocol, is provided under the [MIT License](#license), enabling customers to accelerate their development processes.  
The library's usage is demonstrated with clear C code samples, and useful CLI tools with source code are also provided to facilitate evaluation and integration.

## Scope

This documentation is essential for anyone integrating SBG Systems' devices, developing custom software applications, or needing an in-depth understanding of the sbgECom protocol.

The documentation covers the following items:

- **Frame Protocol Layer**: Detailed descriptions of protocol framing, including packet structures and error handling.
- **Configuration Commands**: A guide to the commands and methods used for configuring device settings and querying information or status.
- **Message Formats**: Specifications for messages output by devices, detailing data formats and field definitions.
- **C Library Doxygen**: Automatically generated documentation for the C library, covering API details, data structures, and functions.
- **C Code Samples**: Example C code demonstrating common use cases and best practices with the sbgECom library.
- **CLI Tools**: Information on command-line tools for interacting with and configuring devices.
- **Migration Guidelines**: Guidance on transitioning between different versions of the sbgECom protocol and firmware, ensuring smooth upgrades and compatibility.
- **CAN Messages**: Description of CAN message data definitions to quickly interface your product with CAN software or loggers.

## Resources

For more information and detailed documentation, please refer to the following resources:

- **sbgECom Documentation**: Access all versions of the sbgECom protocol and library documentation online [here](https://developer.sbg-systems.com/sbgECom).
- **sbgInsRestApi Documentation**: Refer to the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) documentation for configuring products using a modern and easy-to-use JSON-based RestAPI.
- **SBG Systems Support Center**: Visit the [Support Center](https://support.sbg-systems.com) for user manual, knowledge bases, FAQs, quick start guides and integration support.
- **SBG Systems Website**: Visit the [SBG Systems](https://www.sbg-systems.com) website for product information, FAQs, leaflets, and more.
- **CAN Messages Files**: All CAN messages are defined in `dbc` ([Vector](https://www.vector.com)) and `dbf` ([BusMaster](https://rbei-etas.github.io/busmaster/)) formats in the `sbgECom/can` directory.

## Supported Products

SBG Systems offers a range of products, from miniature Inertial Measurement Units (IMUs) to survey-grade Inertial Navigation Systems (INS).  
The sbgECom protocol, documentation, and C library implementation are shared across all these products.

The product lines can be organized into the following main families, each with common features and firmware:

- **ELLIPSE Series v1/v2**: Miniature AHRS/INS launched in 2014, now superseded by ELLIPSE v3. Configured using sbgECom binary commands; does not support the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).
- **ELLIPSE Micro**: Ultra-small, cost-effective IMU/AHRS. Configured using sbgECom binary commands; does not support the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).
- **ELLIPSE Series v3**: The latest generation of ELLIPSE AHRS/INS launched in 2020 with a more powerful CPU. Supports the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) starting with firmware v3.0 (2024).
- **High Performance INS (HPINS)**: Includes EKINOX, APOGEE, QUANTA, and NAVSIGHT series. These are feature-rich, survey-grade INS products with Ethernet, data logging, and web interface capabilities. They share the same architecture and support the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).
- **PULSE IMUs**: IMUs including gyroscopes and accelerometers, with fewer features and configurations compared to INS. Supports the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).

> [!WARNING]
> This documentation primarily covers HPINS running firmware v4.x and above, as well as ELLIPSE v3 running firmware v3.x and above.  
> For ELLIPSE Series v1/v2, ELLIPSE Micro, or devices running older firmware, please refer to the previous [Firmware Reference Manual](https://support.sbg-systems.com/sc/dev/latest/firmware-documentation) and [sbgECom library v4.0](https://developer.sbg-systems.com/sbgECom/4.0/).

## Configuration Overview

The sbgECom protocol and [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) work together to provide comprehensive solutions for device configuration and data output.

### Device Configuration

The [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) is a JSON-based REST API that supersedes sbgECom for configuring devices.  
It is the primary method for programmatically configuring products in newer integrations.

- **Ethernet Interfaces:** For products with Ethernet capabilities, such as the HPINS family, standard HTTP GET/POST methods are used to interact with the API.
- **Serial Interfaces:** For devices like the ELLIPSE v3, which use serial interfaces, the sbgECom protocol provides two specific commands, [SBG_ECOM_CMD_API_GET](#SBG_ECOM_CMD_API_GET) and [SBG_ECOM_CMD_API_POST](#SBG_ECOM_CMD_API_POST), to encapsulate [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) GET/POST requests.

### Reading Data and Status

The sbgECom protocol implements [binary messages](#binaryMessages) for high data rate and efficient retrieval of measurements, information, and status.  
These messages are optimized for real-time applications, offering a secure and efficient binary format ideal for embedded systems.

These binary messages are also used in post-processing workflows, such as with Qinertia software, and for validation and troubleshooting.

> [!NOTE]
> The [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) offers endpoints for querying information, status, and data at low frequencies (1Hz or less).  
> It should not be used for high-frequency data acquisition due to potential CPU overuse and the large data volume generated by JSON format.

### Configuration Method per Family

The table below summarizes the usage of sbgECom and the availability of sbgInsRestApi across different device families:

| Product       | sbgECom Usage    | sbgInsRestApi | Notes                                                                                                  |
|---------------|------------------|---------------|--------------------------------------------------------------------------------------------------------|
| ELLIPSE v1/v2 | Conf. & Messages | Not Available | Latest firmware 2.6 - Support ended in 2024.                                                           |
| ELLIPSE Micro | Conf. & Messages | Not Available | Configuration with sbgECom commands only, product still actively supported.                            |
| ELLIPSE v3    | Messages Only    | Available     | sbgInsRestApi over serial interface only - Since firmware v3.0                                         |
| HPINS         | Messages Only    | Available     | sbgInsRestApi through standard HTTP GET/POST methods and over serial interfaces - Since firmware v4.0. |
| PULSE         | Messages Only    | Available     | sbgInsRestApi over serial interface only.                                                              |

## Maintenance and Contribution

This documentation and the sbgECom C Library are developed and maintained by SBG Systems SAS.  
For assistance, feedback, or to report issues, please contact us at [support@sbg-systems.com](mailto:support@sbg-systems.com).

The sbgECom C Library and code sample is available on GitHub as a public repository.  
While you can open issues to report bugs or suggest improvements, please do not submit pull requests as we do not use GitHub for development purposes and cannot merge them.

## Support

If you need assistance with the sbgECom C Library or have any questions, please refer to the following resources:

1. **Documentation**: Detailed documentation is available within the repository and covers the usage, installation, and API of the sbgECom library.

2. **Reporting Issues**: If you encounter any bugs or have feature requests, please open an issue on the [GitHub issues page](https://github.com/SBG-Systems/sbgECom/issues). Our team will review and address it as soon as possible.

3. **Email Support**: For more direct support, including technical questions or troubleshooting, contact us at [support@sbg-systems.com](mailto:support@sbg-systems.com). Please include as much detail as possible in your request to help us assist you effectively.

4. **Online Support:** You can also visit our [Support Page](https://www.sbg-systems.com/support/) for additional resources, including product manuals, FAQs, and contact information for our technical support team.

