# sbgBasicLogger
The sbgBasicLogger let you log and display sbgECom binary messages into CSV like text files. 
The provided code source is written in C++ 14 to ensure maximum compatibility.

## Output
The output is as close as possible as the sbgECom message definition. 
You can output the data in files as well as in the terminal.

### Standard logs
Most sbgECom logs are output as CSV like text files with an optional header. 
Each file only contains one sbgECom log type. The log are written line by line. 
The tool can be used to extract error/warning/info messages from `SBG_ECOM_LOG_DIAG` messages.

### RAW GNSS data & RTCM
This tool can also extract the RAW GNSS data stream `SBG_ECOM_LOG_RTCM_RAW` to a binary file. 
For example an ELLIPSE-D Generation 3 has an internal ublox receiver so you should get a binary file containing ubx messages directly.

It is the same for real time differential correction data stream `SBG_ECOM_LOG_GPS#_RAW`. 
The tool can store the received stream to a binary file if you would like to extract RTCM data.

## Decimation
The tool can also decimate incoming IMU data using a simple moving average filter. 
You can decimate incoming data using two decimation factors. One for data displayed in the console and one for data written in text files.

Only the following logs are decimated:
 - IMU short
 - IMU fast
 - IMU data

## Interfaces
The sbgBasicLogger can be used to parse incoming data from a serial or an Ethernet UDP interface. 
You can also select an binary file containing raw sbgECom dump and easily export sbgECom data to CSV like text files.

## UTC Time & Timestamp
All sbgECom logs output the internal IMU/AHRS/INS timestamp in microseconds. 
This is the default time format used by the sbgBasicLogger.

However, if the INS receives valid GNSS time information, the sbgBasicLogger is able to output data referenced to UTC time.  
To enable this, use the `--time-mode=utcIso8601` option, which produces timestamps in the format: `YYYY-MM-DDTHH:MM:SS.SSSSSSZ`.

In this mode, before the INS acquires a valid UTC time, the logger will output the internal timestamp in microseconds.  
Once the UTC time is available and synchronized, it switches to the ISO 8601 format.  
To exclude logs recorded before a valid UTC time is available, use the option `--discard-invalid-time `.

# Usage
The sbgBasicLogger implements a simple to use command line interface (CLI):

```sh
sbgBasicLogger [-hvwpHt] [-a IP address] [-I UDP port in] [-O UDP port out] [-s SERIAL_DEVICE] [-r SERIAL_BAUDRATE] [-i INPUT-FILE] [-o DIRECTORY] [-d FILE DECIMATION] [-c CONSOLE DECIMATION] [-m timestamp or utcIso8601]
```

## Serial example and only print on console

```sh
sbgBasicLogger -s <SERIAL-PORT> -r <BAUDRATE> -p
```

## Serial example, print on console and write files into <directory>

```sh
sbgBasicLogger -s <SERIAL-PORT> -r <BAUDRATE> -p -w --dir=directory/
```

## UDP example and only print on console

```sh
sbgBasicLogger -a <IP_ADDR> -I <UDP_PORT_IN> -O <UDP_PORT_OUT> -p
```

## Input file example and only print on console

```sh
sbgBasicLogger -i <BINARY_FILE> -p
```

## Options
```
  -h, --help                                         display this help and exit
  -v, --version                                      display sbgECom version and exit
  -a, --addr-ip=IP address                           open an UDP interface
  -I, --udp-port-in=UDP port in                      UDP port in
  -O, --udp-port-out=UDP port out                    UDP port out
  -s, --serial-device=SERIAL_DEVICE                  open a serial interface
  -r, --serial-baudrate=SERIAL_BAUDRATE              serial baudrate
  -i, --input-file=INPUT-FILE                        input file
  -w, --write-logs                                   write logs in different files
  -o, --dir=DIRECTORY                                directory to write logs into
  -d, --file-decimation=FILE DECIMATION              file decimation
  -c, --console-decimation=CONSOLE DECIMATION        output stream decimation
  -p, --print-logs                                   print the logs on the output stream
  -H, --disable-header                               disable header for files
  -m, --time-mode=timestamp or utcIso8601            select time base to output
  -t, --discard-invalid-time                         discard data without a valid UTC time
```

## Vibration Monitoring

### Overview
SBG Systems IMUs embed a real-time **vibration analysis engine** that:
1. Computes an FFT on selected sensor axes (X, Y, Z).  
2. Extracts band-limited metrics (RMS & peak) for compact, easy-to-monitor diagnostics.
3. Logs the **full FFT spectrum** for advanced offline analysis and troubleshooting.

`sbgBasicLogger` suppors both kinds of messages:

| Message type          | File name pattern                  | Purpose                                                          |
|-----------------------|------------------------------------|------------------------------------------------------------------|
| **Monitoring report** | `vibMonReport_<axis>_<window>.txt` | One line per timestamp, rms, peak freq, peak magnitude per band. |
| **FFT spectrum**      | `vibFftAcf_<axis>_<window>.txt`    | Raw FFT (Amplitude-Scaled) for spectrograms & peak tracking.     |

> **Tip** – To improve refresh rate and focus on relevant data, log only the axes and FFT windows that are essential for your analysis.

### Axes & Windows
The `sbgBasicLogger` creates **one spectrum file per (axis × window)** so each combination is easy to analyse:

| Option          | Description                                              |
|-----------------|----------------------------------------------------------|
| **Axes**        | `X`, `Y`, `Z` – enable individually or together.         |
| **FFT windows** | `Rectangular`, `Hanning`, `FlatTop` (select any subset). |

### Plot a Spectrogram

A Python script is provided to visualize vibration monitoring FFT data as a **spectrogram**.

#### Requirements

* **Python 3.x**
* The following Python packages:

  ```bash
  pip install numpy pandas matplotlib
  ```

#### Usage

Run the script with the FFT file of interest:

```bash
python plotVibMonFft.py vibFftAcf_z_flatTop.txt
```

The script automatically:

* Parses the frequency axis and timestamps,
* Converts acceleration from **m/s² to g**,
* Plots a time-frequency spectrogram with an adaptive color scale.
