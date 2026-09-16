# Integration test data

## `sbg_integration_replay.000`

Shortened test fixture replayed by `test/test_replay_launch.py` through the driver file
interface. It is only ever opened read-only: it is the reference input of the integration test
and must not be regenerated or edited without revalidating the reference values below.

### Provenance

Extracted from `20260806_15h_06082026_ekinox_fw63.000`, a 47 673 370 byte raw sbgECom stream
recorded on an EKINOX running firmware 6.3, on 2026-08-06. The unit is stationary outdoors near
Paris. The original recording covers about 31 minutes; the integration test only needs the first
few seconds of it, so the committed fixture is the initial portion of that stream and nothing
else. The full recording is not kept in the repository.

### How the fixture was generated

The sbgECom frame layout is `SBG_ECOM_SYNC_1` (0xFF), `SBG_ECOM_SYNC_2` (0x5A), message id,
message class, 16 bit little endian payload length, payload, 16 bit CRC and `SBG_ECOM_ETX`
(0x33), as defined in `external/sbgECom/src/protocol/sbgEComProtocol.h`. The original recording
was walked frame by frame from its first byte, following that length field, and the cut was
taken at the end of the first complete frame whose device timestamp is at least 4 s after the
first timestamped log. That offset is 99 797 bytes, so the fixture is equivalent to:

```
head -c 99797 20260806_15h_06082026_ekinox_fw63.000 > sbg_integration_replay.000
```

The fixture is 99 797 bytes and holds 1 616 complete frames. It starts on a frame boundary
(offset 0) and **ends immediately after a complete frame**: the last byte is `SBG_ECOM_ETX` and
no partial frame is left behind. Walking the file with the framing above consumes it exactly,
with no resynchronisation and no trailing bytes.

The 4 s length is deliberate. The driver replays a file in real time and only leaves
`sbgEComHandle()` once the file is exhausted, so the recording also bounds how long the node
can take to react to the shutdown signal that ends the launch test. Four seconds is comfortably
inside the launch shutdown grace period while still delivering every message the test counts.

### Inventory

4.00 s of device time, all logs in `SBG_ECOM_CLASS_LOG_ECOM_0`:

| id | log              | count | rate    |
|----|------------------|-------|---------|
| 1  | `STATUS`         | 4     | 1 Hz    |
| 2  | `UTC_TIME`       | 4     | 1 Hz    |
| 6  | `EKF_EULER`      | 200   | 50 Hz   |
| 8  | `EKF_NAV`        | 200   | 50 Hz   |
| 13 | `GPS1_VEL`       | 20    | 5 Hz    |
| 14 | `GPS1_POS`       | 20    | 5 Hz    |
| 15 | `GPS1_HDT`       | 20    | 5 Hz    |
| 31 | `GPS1_RAW`       | 67    | 16.7 Hz |
| 44 | `IMU_SHORT`      | 802   | 200 Hz  |
| 48 | `DIAG`           | 3     | async   |
| 50 | `GPS1_SAT`       | 4     | 1 Hz    |
| 59 | `VIB_MON_FFT`    | 264   | 65.9 Hz |
| 60 | `VIB_MON_REPORT` | 8     | 2.0 Hz  |

Every log the integration test asserts on is present with margin: the test needs 100
`IMU_SHORT`, 25 `EKF_EULER`, 25 `EKF_NAV`, 3 `GPS1_POS`, 3 `GPS1_VEL` and 2 `STATUS`, which the
recorded rates deliver in about two seconds. `STATUS` is the binding one at 1 Hz, and the
fixture holds four of them, so the test still collects what it needs even if node start-up and
discovery eat the first two seconds of the replay.

There is no `EKF_QUAT`, no `MAG` and no `AIR_DATA` log, so the driver reports the
`sensor_msgs/Imu` orientation as unavailable and never publishes `imu/mag` or `imu/pres`. The
first `SESSION_INFO` log of the original recording is past the cut and is not in the fixture;
the driver does not use it.

### Reference values

The device timestamps start at 860 568 061 us and end at 864 573 039 us, so the whole fixture
sits inside the range the test checks. Over that window the recorded values are stable enough to
assert on directly, which is what makes the integration test deterministic:

| quantity                     | recorded (NED)          | published (ENU)       |
|------------------------------|-------------------------|-----------------------|
| accelerometer X              | +0.030 m.s^-2           | +0.030 m.s^-2         |
| accelerometer Y              | -0.385 m.s^-2           | +0.385 m.s^-2         |
| accelerometer Z              | -9.800 m.s^-2           | +9.800 m.s^-2         |
| IMU temperature              | 36.7 degC               | 36.7 degC             |
| EKF roll                     | +0.04000 rad            | +0.04000 rad          |
| EKF pitch                    | +0.00506 rad            | -0.00506 rad          |
| EKF heading                  | +1.53639 rad            | +0.03441 rad          |
| GNSS latitude                | 48.9100628 deg          | 48.9100628 deg        |
| GNSS longitude               | 2.1679265 deg           | 2.1679265 deg         |
| GNSS altitude (MSL)          | 55.5 m                  | 55.5 m                |
| GNSS undulation              | 46.115 m                | 46.115 m              |
| GNSS course                  | 321.157 deg             | 128.843 deg           |

Other constants over that window: the IMU status bitmask is `0x3FF`, so the gyroscopes use the
standard scale factor. The EKF status is `0x28000111`, that is
`SBG_ECOM_SOL_MODE_VERTICAL_GYRO` with a valid attitude but an invalid heading and position, so
the navigation solution is not usable and the integration test does not assert on its numerical
content. The GNSS position type is `SBG_ECOM_GNSS_POS_TYPE_PSRDIFF` with 26 to 28 satellites
used out of 39 to 40 tracked, and the general status bitmask is `0x7F`, so every general status
flag is set.

The values above were extracted by decoding the sbgECom frames directly from the file, using the
log structures in `external/sbgECom/src/logs`. The single-axis accelerometer figures are
averages: individual samples are noisy (Z ranges from -10.13 to -9.39 m.s^-2), but the mean over
any window of 100 samples stays within a few milli-m.s^-2 of the values listed here.
