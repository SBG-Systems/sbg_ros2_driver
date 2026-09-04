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
taken at the end of the first complete frame whose device timestamp is at least 20 s after the
first timestamped log. That offset is 493 412 bytes, so the fixture is equivalent to:

```
head -c 493412 20260806_15h_06082026_ekinox_fw63.000 > sbg_integration_replay.000
```

The fixture is 493 412 bytes and holds 7 984 complete frames. It starts on a frame boundary
(offset 0) and **ends immediately after a complete frame**: the last byte is `SBG_ECOM_ETX` and
no partial frame is left behind. Walking the file with the framing above consumes it exactly,
with no resynchronisation and no trailing bytes.

### Inventory

20.00 s of device time, all logs in `SBG_ECOM_CLASS_LOG_ECOM_0`:

| id | log              | count | rate    |
|----|------------------|-------|---------|
| 1  | `STATUS`         | 20    | 1 Hz    |
| 2  | `UTC_TIME`       | 20    | 1 Hz    |
| 6  | `EKF_EULER`      | 1000  | 50 Hz   |
| 8  | `EKF_NAV`        | 1000  | 50 Hz   |
| 13 | `GPS1_VEL`       | 100   | 5 Hz    |
| 14 | `GPS1_POS`       | 100   | 5 Hz    |
| 15 | `GPS1_HDT`       | 100   | 5 Hz    |
| 31 | `GPS1_RAW`       | 327   | 16.4 Hz |
| 44 | `IMU_SHORT`      | 4002  | 200 Hz  |
| 48 | `DIAG`           | 3     | async   |
| 50 | `GPS1_SAT`       | 20    | 1 Hz    |
| 59 | `VIB_MON_FFT`    | 1254  | 62.7 Hz |
| 60 | `VIB_MON_REPORT` | 38    | 1.9 Hz  |

Every log the integration test asserts on is present in large numbers: the test needs 100
`IMU_SHORT`, 25 `EKF_EULER`, 25 `EKF_NAV`, 3 `GPS1_POS`, 3 `GPS1_VEL` and 2 `STATUS`, which the
recorded rates deliver in about two seconds. The remaining window is start-up and discovery
slack, since the driver replays the file in real time and stops publishing at the end of it.

There is no `EKF_QUAT`, no `MAG` and no `AIR_DATA` log, so the driver reports the
`sensor_msgs/Imu` orientation as unavailable and never publishes `imu/mag` or `imu/pres`. The
first `SESSION_INFO` log of the original recording is past the cut and is not in the fixture;
the driver does not use it.

### Reference values

The device timestamps start at 860 568 061 us and end at 880 572 952 us, so the whole fixture
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
used out of 39 to 41 tracked, and the general status bitmask is `0x7F`, so every general status
flag is set.

The values above were extracted by decoding the sbgECom frames directly from the file, using the
log structures in `external/sbgECom/src/logs`. The single-axis accelerometer figures are
averages: individual samples are noisy (Z ranges from -10.14 to -9.36 m.s^-2), but the mean over
any window of 100 samples stays within a few milli-m.s^-2 of the values listed here.
