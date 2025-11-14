# Getting Started

This guide will help you quickly get started with the sbgECom ANSI C Library and the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) to configure your device and read data.  
It also explains how to use the CLI tools, [sbgEComApi](#sbgEComApi) and [sbgBasicLogger](#sbgBasicLogger), to configure your device and log data to CSV files.

## Before You Begin

SBG Systems products use a proprietary binary format, sbgECom, for data output and device communication.  
Device configuration is managed using the modern, JSON-based [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/), which provides a portable and easy-to-integrate solution.

### sbgECom Library Overview

The sbgECom library is the C implementation of the sbgECom binary protocol used by the device to send data messages.  
It also provides specific commands, such as [SBG_ECOM_CMD_API_GET](#SBG_ECOM_CMD_API_GET) and [SBG_ECOM_CMD_API_POST](#SBG_ECOM_CMD_API_POST), to support [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) configuration.

For example, the [SBG_ECOM_LOG_EKF_EULER](#SBG_ECOM_LOG_EKF_EULER) message returns the AHRS/INS roll, pitch, and yaw angles.

### sbgInsRestApi Overview

The [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) is a JSON-based RESTful API that handles device configuration and monitoring.  
Standard HTTP GET and POST requests are used to interact with the device via various endpoints (URLs).

For example, entering ``http://<my-device-ip>/api/v1/settings`` in a web browser retrieves all the device settings in JSON format.

- **Ethernet Connections**: For devices like HPINS with Ethernet interfaces, standard HTTP GET/POST requests are used to access the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).
- **Serial Connections**: For devices like ELLIPSE v3 with serial interfaces, the sbgECom binary protocol is used to transport HTTP GET/POST requests over the interface using [SBG_ECOM_CMD_API_GET](#SBG_ECOM_CMD_API_GET) and [SBG_ECOM_CMD_API_POST](#SBG_ECOM_CMD_API_POST).

> [!NOTE]
> ELLIPSE v1 and v2 do not support configuration via sbgInsRestApi and rely only on deprecated sbgECom configuration commands.

## Building sbgECom Library

The sbgECom library is designed for easy integration within your projects. Written in clean, well-documented C code with embedded systems in mind.

It is strongly recommended to use the sbgECom library for proper protocol support and future-proofing against updates.

> [!NOTE]
> SBG Systems doesn't provide the sbgECom as a pre-compiled static or dynamic library.

### Dependencies & Requirements

The sbgECom library and code samples have no third-party dependencies and are written in standard C99.  
However, the CLI tools [sbgBasicLogger](#sbgBasicLogger) and [sbgEComApi](#sbgEComApi) are written in C++ (14) and depend on [Argtable3](https://github.com/argtable/argtable3).

SBG Systems has validated the following tool-chain and libraries:
  - \>= CMake 3.0
  - \>= GNU GCC 8 (any platform)
  - \>= AppleClang 13 (Mac OS X)
  - \>= Visual Studio 2015 or MSBuild equivalent (Windows)
  - \>= [Argtable3](https://github.com/argtable/argtable3) (for building CLI tools)
  - \>= Git (to fetch [Argtable3](https://github.com/argtable/argtable3))

### Platform support

The sbgECom library can be easily ported to any platform with minimal changes.

To adapt the library to your specific platform, you'll need to provide or implement a few low-level methods and configurations:
  - **Platform Configuration**: Set the platform-specific configurations in `sbgECom/common/sbgConfig.h` to tailor the library to your environment.
  - **Time and Sleep Functions**: Implement the functions `sbgGetTime` and `sbgSleep` in `sbgECom/common/platform/sbgPlatform.c` to handle timing and sleep operations according to your platform's requirements.
  - **Logging Callback**: Install a callback using `sbgCommonLibSetLogCallback` to track and manage library error, warning, and info logs.
  - **Communication Interfaces**: Provide custom implementations for any required communication interfaces in the `sbgECom/common/interfaces/` directory to ensure data transmission and reception on your platform.

### Building sbgECom

To build the sbgECom static library, code examples, and command-line tools, go to the `sbgECom` directory and type:

```sh
cmake -Bbuild -DBUILD_EXAMPLES=ON -DBUILD_TOOLS=ON
cmake --build build --config Debug
```

The compiled library, examples, and tools will be located in the `sbgECom/build/Debug` folder.

> [!NOTE]
> Disable deprecated macros by adding `-DUSE_DEPRECATED_MACROS=OFF` to avoid using outdated defines, macros, and enum values.

### Installing sbgECom

To install the compiled sbgECom library on your system, use the following command:

```sh
cmake --install build
```

This will install the static library, headers and cmake files into the default system installation path (e.g., /usr/local on Linux/macOS or C:/Program Files on Windows).

If you want to customize the installation directory, specify it using --prefix option in the cmake command:

```sh
cmake --install build --prefix /path/to/install
```

Make sure to set the installation path in your system's PATH or library path (LD_LIBRARY_PATH on Linux, DYLD_LIBRARY_PATH on macOS, or environment variables on Windows) if needed.

## Command Line Tools

SBG Systems provides two CLI (Command Line Interface) tools to facilitate evaluation and integration.

You can find the C/C++ source code for each tool in the `sbgECom/tools` directory.
Pre-compiled 64-bit binaries for Windows, Linux, and Mac OS X platforms are available in the `sbgECom/bin/tools` directory.

### sbgBasicLogger {#sbgBasicLogger}

The `sbgBasicLogger` tool parses sbgECom messages from a serial or Ethernet interface and writes the log content to CSV files without altering the device configuration.  
It can also read sbgECom messages from a binary file, making it useful for converting sbgECom binary streams to text files.

Additionally, `sbgBasicLogger` can extract raw GNSS data streams, RTCM streams, device settings, information, and error logs.

For example, the following command displays received messages on the terminal:

```sh
sbgBasicLogger -s COM1 -r 115200 -p
```

Refer to the [sbgBasicLogger README](https://github.com/SBG-Systems/sbgECom/blob/main/tools/sbgBasicLogger/README.md) for more details.

### sbgEComApi {#sbgEComApi}

The `sbgEComApi` tool provides easy access to the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) over serial or Ethernet UDP interfaces.
It enables GET and POST queries through command-line arguments, making it ideal for configuring devices like HPINS, ELLIPSE v3, or PULSE using simple bash scripts.

For example, the following command fetches all device settings as JSON content:

```sh
sbgEComApi.exe -s COM1 -r 115200 -g /api/v1/settings
```

Refer to the [sbgEComApi README](https://github.com/SBG-Systems/sbgECom/blob/main/tools/sbgEComApi/README.md) for more details.

## Code Examples

SBG Systems provides several simple C code examples to help you quickly get started with the sbgECom library.  
Each example includes both the source code and pre-compiled binaries.

- **Source Code**: Located in the `sbgECom/examples` directory.
- **Binaries**: Available in the `sbgECom/bin/examples` folder, compiled for 64-bit systems.

### sbgEComExample {#sbgEComExample}

This C code sample demonstrates the usage of the `sbgECom` library and the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).  
This example supports both serial connections and Ethernet UDP communication.

**Functionality**:
  - Detects a device using `sbgECom` and `sbgInsRestApi`.
  - Configures the device to output data at a rate of 10 Hz.
  - Displays the received data on the console.

**Compatibility**:
  - ELLIPSE devices (firmware v3 and above)
  - High Performance INS (HPINS) products
  - PULSE Inertial Measurement Units (IMUs)

The code is written in plain ANSI C and is designed to work on any POSIX and Windows system.  
With minor modifications, it can also run on bare-metal devices.

The example source file is located in the `sbgECom/examples/sbgEComExample` directory.

#### Usage Examples

- **ELLIPSE v3 (Serial)**: ELLIPSE devices communicate at 115200 bps over the default serial port A.

    ```sh
    sbgEComExample -s COM1 -r 115200
    ```

- **HPINS (Ethernet UDP)**: Uses an Ethernet UDP connection with the device's default output port set to 1234 and input port set to 5678.  
    The IP address is typically acquired via DHCP. In this example, assume the device IP address is 192.168.1.1:

    ```sh
    sbgEComExample -a 192.168.1.1 -I 1234 -O 5678
    ```

- **PULSE (Serial)**: PULSE IMUs communicate at 921600 bps by default:

    ```sh
    sbgEComExample serial -s COM1 -r 921600
    ```

> [!NOTE]
> HPINS devices also support serial communication on Port A with a default baud rate of 115200 bps.

### ellipseLegacy

This legacy C code sample demonstrates the usage of `sbgECom` for ELLIPSE devices running firmware v2 or earlier.  
It uses only the legacy configuration commands and does not rely on the newer [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).

The example source file is located in the `sbgECom/examples/ellipseLegacy` directory.

#### Usage Example
To run the example for an ELLIPSE device communicating at 115200 bps over the default serial port A:

```sh
  ellipseLegacy COM1 115200
```

### Magnetic Calibration (sbgInsRestApi)

The [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) provides easy access to the ELLIPSE on-board magnetic field calibration.  

This guide demonstrates how to use the sbgEComApi CLI tool to interact with the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).  
Alternatively, you can implement this procedure using the [SBG_ECOM_CMD_API_GET](#SBG_ECOM_CMD_API_GET) and [SBG_ECOM_CMD_API_POST](#SBG_ECOM_CMD_API_POST) sbgECom commands.

In this example, we focus on a 3D on-board magnetic calibration.
For a 2D calibration, simply change the `mode` parameter to `2d` in the payload and ensure that the device is rotated only horizontally during the magnetic field acquisition.

#### Step 1: Start Magnetic Field Acquisition

Clear any previously acquired magnetic field data and prepare the device for a new 3D magnetic field acquisition:

```sh
sbgEComApi.exe -s COM1 -r 115200 api/v1/magnetometer/calibration/start -p -b {\"mode\":\"3d\"}
```

#### Step 2: Compute Magnetic Calibration

Slowly rotate the unit around each IMU axis to acquire representative magnetic field data.  
Once enough data has been collected, compute the new magnetic calibration parameters:

```sh
sbgEComApi.exe -s COM1 -r 115200 api/v1/magnetometer/calibration -g
```

If successful, you should receive output similar to the following:

```json
{
   "status":"success",
   "quality":"medium",
   "trust":"high",
   "maxNumPoints":1000,
   "numPointsTotal":1769,
   "numPointsUsed":783,
   "is2dCalibration":false,
   "enoughPts":true,
   "rollMotionValid":true,
   "pitchMotionValid":true,
   "yawMotionValid":true,
   "alignmentNumPtsTotal":533,
   "alignmentNumPtsUsed":533,
   "alignmentQuality":0.028159335255622864,
   "alignmentEnoughPts":true,
   "alignmentKeptEnoughPts":true,
   "hardIronCorrection":[
      0.10573061555624008,
      0.16595759987831116,
      -0.25122913718223572
   ],
   "softIronCorrection":[
      1.3221145868301392,
      -0.077110230922698975,
      0.098171010613441467,
      0.012074913829565048,
      1.1894962787628174,
      0.01619420014321804,
      0.031276974827051163,
      0.11012999713420868,
      1.2979389429092407
   ],
   "ellipsoidMatrix":[
      -0.25842645764350891,
      -0.73661297559738159,
      0.3940565288066864,
      -0.46735265851020813,
      0.41018226742744446,
      0.4602622389793396,
      -0.53718864917755127,
      -0.069976426661014557,
      -0.48310169577598572
   ],
   "beforeMeanError":0.16253998875617981,
   "beforeStdError":4.72862672805786,
   "beforeMaxError":10.900124549865723,
   "afterMeanError":0.00320892222225666,
   "afterStdError":0.79626452922821045,
   "afterMaxError":2.2561728954315186,
   "meanAccuracy":0.0052680773660540581,
   "stdAccuracy":0.01319846510887146,
   "maxAccuracy":0.0277938898652792
}
```

> [!NOTE]
> If you are not satisfied with the results, you can continue acquiring magnetic field data and recompute the calibration until the desired accuracy is achieved.

#### Step 3: Apply the New Magnetic Calibration

Extract the `hardIronCorrection` and `softIronCorrection` parameters from the calibration results to update the magnetic calibration.  
Ensure that the `calibrationMode` field is correctly set to reflect the method used, whether `2d` or `3d`.

```sh
sbgEComApi.exe -s COM1 -r 115200 api/v1/settings/aiding/magnetometer -p -b {\"hardIronCorrection\":[0.1057,0.1660,-0.2512],\"softIronCorrection\":[1.322114587,-0.077110231,0.098171011,0.012074914,1.189496279,0.016194200,0.031276975,0.110129997,1.297938943],\"calibrationMode\":\"3d\"}
```

> [!NOTE]
> In the example above, the values have been rounded to reduce the command line length.

#### Step 4: Save the New Magnetic Calibration

This step is essential to store the new magnetic calibration settings in the device's FLASH memory.  
The new calibration will only take effect after the device has been rebooted:

```sh
sbgEComApi.exe -s COM1 -r 115200 api/v1/settings/save -p
sbgEComApi.exe -s COM1 -r 115200 api/v1/system/reboot -p
```

### Magnetic Calibration (Legacy)

This minimal C example demonstrates how to use the ELLIPSE on-board magnetic calibration using the legacy sbgECom commands.

This example is relevant for legacy code that has not yet migrated to the new [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).  
It is also applicable to ELLIPSE v1/v2 devices that do not support the new [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/).

The example source file can be found in the `sbgECom/examples/onBoardMagCalib` directory.

You can try this example by typing the following command line:

```sh
onBoardMagCalib COM1 115200
```

### Air Data Input

A simple C example illustrating how to send air data aiding measurements to an ELLIPSE using the sbgECom binary protocol and library.

The following command will send air data aiding to the device while simultaneously reading output messages on the same serial interface:

```sh
airDataInput COM1 115200
```
