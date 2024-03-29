# sbgECom Library
sbgECom is a C library provided under the MIT License and used to interface easily [SBG Systems](https://www.sbg-systems.com/) IMU, AHRS and INS to your application.

This library has been designed to be platform and OS independent and can safely be used on small micro-controller or larger multi-core CPUs.  
This package contains the following items:
 - sbgECom library full C source code
 - Doxygen library source code documentation
 - Examples C source code and binaries to quickly start using the sbgECom library
 - Several useful tools provided as binary package

The sbgECom library and examples can be compiled on any platform using CMake.  
The pre-compiled sbgECom examples and tools are 64 bits binaries available on Windows, Linux and Mac OS X platforms.

The library is written and maintained by SBG Systems SAS. You can contact the support@sbg-systems.com for if you need assistance.

# Documentation

You can access the full online sbgECom Doxygen documentation [here](https://developer.sbg-systems.com/sbgECom/4.0).  
You should also read the SBG Systems [Support Center](https://support.sbg-systems.com) to quickly start using and integrating your products.  
Please also have a look at the [sbgInsRestApi](https://developer.sbg-systems.com/sbgInsRestApi/) documentation that is used to configure High Performance INS products.

# Platform support
The library has been designed to be easily ported to any platform by just providing/implementing a few low level methods and some configuration:
 - The platform configuration is set in `common/sbgConfig.h`
 - In the file `common/platform/sbgPlatform.c` please provide _sbgGetTime_, _sbgSleep_ and _sbgPlatformDebugLogMsg_
 - In the directory `common/interfaces/` you can provide custom open/read/write implementations for serial communications

# Building sbgECom Library
The sbgECom library and code examples are very easy to compile on any platform using CMake.  
The sbgECom library and code samples have no third party library dependencies.

However, if you would like to build sbgECom CLI tools such as the `sbgBasicLogger` or `sbgInsRestApi` there is a dependency on Argtable3.

SBG Systems doesn't provide the sbgECom as a pre-compiled library for obvious and good reasons.

## Dependencies
SBG Systems has validated the following tool-chain and libraries:
- \>= CMake 3.0
- \>= GNU GCC 8 (any platform)
- \>= AppleClang 13 (Mac OS X)
- \>= Visual Studio 2015 or MSBuild equivalent (Windows)
- \>= Argtable3 (to build sbgECom tools)
- \>= Git (to fetch Argtable3)

## Building sbgECom
To build the sbgECom static library, the C example and the command line tools go to the sbgECom library folder and type the following commands:

```sh
cmake -Bbuild -DBUILD_EXAMPLES=ON -DBUILD_TOOLS=ON
cmake --build build
```

You should find the sbgECom static library, examples and tools binaries in the `build/Debug` folder.

> **Disable Deprecated Macros**
> Make sure to add `-DUSE_DEPRECATED_MACROS=OFF` to disable support of deprecated defines, macros and enum values.

# Code Examples
SBG Systems provides several and simple C code examples to quickly use the sbgECom library.
You can find both the source code as well as a binary for each example.

All examples source code are located in the `examples` directory. You can find pre-compiled 64 bits binaries in the `bin/examples` folder.

## Ellipse Minimal
Simple C example to illustrate how to connect and read data from an ELLIPSE using the sbgECom library.

You can test this example using the command below:

```sh
ellipseMinimal COM4 115200
```

## High Performance INS Minimal
Simple C example to illustrate how to read data from an High Performance INS over an UDP connection and using the sbgECom library.

You can test this example using the command below. The INS ip address is *192.168.1.1* and send logs on the UDP port *1234*:

```sh
hpInsMinimal COM4 192.168.1.1 5678 1234
```

## Pulse Minimal
Simple C example to illustrate how to connect and read data from a PULSE IMU using the sbgECom library.

You can test this example using the command below:

```sh
pulseMinimal COM4 921600
```

## Ellipse On Board Magnetic Calibration
Simple C example to illustrate how to use the ELLIPSE on board magnetic calibration algorithms.

You can test this example using the command below:

```sh
ellipseOnBoardMagCalib COM4 115200
```

## Air Date Input
Simple C example to illustrate how to send air date aiding measurements to an ELLIPSE using the sbgECom library.

You can test this example using the command below:

```sh
airDataInput COM4 115200
```

# Command Line Tools
SBG Systems offers two very useful tools to ease evaluation and integration. You can find the C source code for each tool in the `tools` directory.  
The `bin/tools` directory contains pre-compiled 64 bits binaries for Windows, Linux and Mac OS X platforms.

> Please read the dedicated README.md files provided with each tool.

## sbgBasicLogger
Simply parse sbgECom logs from a serial or ethernet interface and write log content to CSV like files.
This tool can also read sbgECom logs from a binary file making it very interesting to convert ELLIPSE binary streams to easy to use text files.
It can also extract RAW GNSS data stream as well as real time differential correction (RTCM) stream to binary files.

## sbgEComApi 
Easily access sbgInsRest API configuration over a serial or UDP interface. You can execute GET and POST queries using simple to use command lines arguments.
This tool is perfect if you would like to setup a High Performance INS product over a serial or ethernet interface and using only bash scripts for example.

# CAN messages
SBG Systems provides a DBC and BusMaster CAN messages database definition to quickly interface your product with a CAN logger and application.
You can find these CAN database in the `can` directory

<span class="next_section_button">

Read Next: [Migrations](doc/migrations.md)
</span>
