"""
Integration test replaying a recorded sbgECom stream through the real driver.

The test launches the sbg_device executable with the file interface pointed at a recording
installed in the package share directory, subscribes to a representative set of topics and
checks the messages the assembled driver actually publishes.

The recording is a stationary EKINOX (firmware 6.3) log, so most of the reference values are
constant over the replayed window. See test/data/README.md for the recording inventory and
for how the reference values below were extracted.
"""

import os
import statistics
import tempfile
import time
import unittest

from ament_index_python.packages import get_package_share_directory

import launch
import launch_ros.actions
import launch_testing
import launch_testing.actions
import launch_testing.asserts

import pytest

import rclpy
from rclpy.qos import HistoryPolicy
from rclpy.qos import QoSProfile
from rclpy.qos import ReliabilityPolicy

from sbg_driver.msg import SbgEkfEuler
from sbg_driver.msg import SbgEkfNav
from sbg_driver.msg import SbgGpsPos
from sbg_driver.msg import SbgGpsVel
from sbg_driver.msg import SbgImuShort
from sbg_driver.msg import SbgStatus

from sensor_msgs.msg import Imu

RECORDING = 'sbg_integration_replay.000'
CONFIGURATION = 'sbg_device_integration_test.yaml'

DEVICE_FRAME_ID = 'imu_link'
GNSS_FRAME_ID = 'gnss_link'

# Topic, message type, number of messages the test needs.
TOPICS = (
    ('sbg/imu_short', SbgImuShort, 100),
    ('imu/data', Imu, 100),
    ('sbg/ekf_euler', SbgEkfEuler, 25),
    ('sbg/ekf_nav', SbgEkfNav, 25),
    ('sbg/gps_pos', SbgGpsPos, 3),
    ('sbg/gps_vel', SbgGpsVel, 3),
    ('sbg/status', SbgStatus, 2),
)

# The replay is real time, so the recorded rates (200 Hz IMU, 50 Hz EKF, 5 Hz GNSS, 1 Hz
# status) give every topic the messages above in about two seconds. The timeout is generous
# to absorb the node start-up and discovery, and bounds the test if nothing is published.
COLLECT_TIMEOUT_SEC = 60.0

# Header stamps come from the ROS clock (output.time_reference is "ros"), they are only
# checked to be inside the wall clock window of the test.
STAMP_MARGIN_SEC = 60.0

# SBG_ECOM_LOG_IMU_ACCEL_SCALE_STD, the raw sbg/imu_short delta velocity LSB per m.s^-2.
ACCEL_SCALE = 1048576.0

# Device timestamps in microseconds since power up, the recording starts at 860.5 s.
DEVICE_TIMESTAMP_MIN = 860_000_000
DEVICE_TIMESTAMP_MAX = 1_000_000_000

# Reference values. The NED columns are what the recording contains, the ENU columns are what
# the driver is expected to publish once it has applied the frame conversion.
#
#                       recorded (NED)      published (ENU)
# accelerometer Y       -0.385 m.s^-2       +0.385 m.s^-2
# accelerometer Z       -9.800 m.s^-2       +9.800 m.s^-2
# EKF pitch             +0.00506 rad        -0.00506 rad
# EKF heading           +1.53639 rad        +0.03441 rad  (pi/2 - heading)
# GNSS course           321.157 deg         128.843 deg   (90 - course, wrapped)
REF_ACCEL_EAST = 0.030
REF_ACCEL_NORTH = 0.385
REF_ACCEL_UP = 9.800
REF_IMU_TEMPERATURE = 36.70
REF_ROLL = 0.04000
REF_PITCH = -0.00506
REF_HEADING = 0.03441
REF_LATITUDE = 48.9100628
REF_LONGITUDE = 2.1679265
REF_ALTITUDE = 55.5
REF_UNDULATION = 46.115
REF_GNSS_COURSE = 128.843

SBG_ECOM_SOL_MODE_VERTICAL_GYRO = 1
SBG_ECOM_GNSS_POS_TYPE_PSRDIFF = 3


@pytest.mark.launch_test
def generate_test_description():
    share_directory = get_package_share_directory('sbg_driver')
    recording = os.path.join(share_directory, 'test', 'data', RECORDING)
    configuration = os.path.join(share_directory, 'test', 'config', CONFIGURATION)

    assert os.path.isfile(recording), 'missing installed recording: ' + recording
    assert os.path.isfile(configuration), 'missing installed configuration: ' + configuration

    #
    # The driver loads its parameters from a private node, so the recording path has to be
    # written under the /** wildcard rather than under the executable node name.
    #
    overrides = tempfile.NamedTemporaryFile(
        mode='w', prefix='sbg_replay_', suffix='.yaml', delete=False)
    overrides.write('/**:\n  ros__parameters:\n    fileConf:\n      path: "%s"\n' % recording)
    overrides.close()

    sbg_device = launch_ros.actions.Node(
        package='sbg_driver',
        executable='sbg_device',
        output='screen',
        parameters=[configuration, overrides.name],
    )

    return (
        launch.LaunchDescription([
            sbg_device,
            launch_testing.actions.ReadyToTest(),
        ]),
        {'sbg_device': sbg_device},
    )


def collect_messages(node, topics, timeout_sec):
    """Subscribe to every topic and spin until each one got the messages it needs."""
    received = {topic: [] for topic, _, _ in topics}
    subscriptions = []
    qos = QoSProfile(
        depth=200,
        reliability=ReliabilityPolicy.RELIABLE,
        history=HistoryPolicy.KEEP_LAST,
    )

    for topic, message_type, _ in topics:
        subscriptions.append(node.create_subscription(
            message_type,
            topic,
            lambda message, store=received[topic]: store.append(message),
            qos,
        ))

    deadline = time.monotonic() + timeout_sec

    while time.monotonic() < deadline:
        if all(len(received[topic]) >= count for topic, _, count in topics):
            break
        rclpy.spin_once(node, timeout_sec=0.1)

    for subscription in subscriptions:
        node.destroy_subscription(subscription)

    return received


class TestSbgDeviceReplay(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()
        cls.node = rclpy.create_node('sbg_replay_test')
        cls.wall_clock_start = time.time()
        cls.received = collect_messages(cls.node, TOPICS, COLLECT_TIMEOUT_SEC)
        cls.wall_clock_end = time.time()

    @classmethod
    def tearDownClass(cls):
        cls.node.destroy_node()
        rclpy.shutdown()

    #
    # Helpers
    #

    def messages_for(self, topic):
        """Return the messages received on a topic, after checking there are enough of them."""
        expected = dict((name, count) for name, _, count in TOPICS)[topic]
        messages = self.received[topic]

        self.assertGreaterEqual(
            len(messages), expected,
            'received %d/%d messages on %s' % (len(messages), expected, topic))

        return messages

    def assert_header(self, messages, topic, frame_id):
        for message in messages:
            self.assertEqual(message.header.frame_id, frame_id, 'wrong frame_id on ' + topic)

            stamp = message.header.stamp.sec + message.header.stamp.nanosec / 1e9

            self.assertGreater(stamp, 0.0, 'unpopulated stamp on ' + topic)
            self.assertGreater(stamp, self.wall_clock_start - STAMP_MARGIN_SEC,
                               'stamp before the test started on ' + topic)
            self.assertLess(stamp, self.wall_clock_end + STAMP_MARGIN_SEC,
                            'stamp after the test ended on ' + topic)

    def assert_device_timestamps(self, messages, topic):
        timestamps = [message.time_stamp for message in messages]

        for timestamp in timestamps:
            self.assertGreater(timestamp, DEVICE_TIMESTAMP_MIN, 'timestamp outside the recording')
            self.assertLess(timestamp, DEVICE_TIMESTAMP_MAX, 'timestamp outside the recording')

        self.assertEqual(timestamps, sorted(timestamps), 'timestamps went backwards on ' + topic)

    #
    # IMU
    #

    def test_sbg_imu_short(self):
        messages = self.messages_for('sbg/imu_short')

        self.assert_header(messages, 'sbg/imu_short', DEVICE_FRAME_ID)
        self.assert_device_timestamps(messages, 'sbg/imu_short')

        for message in messages:
            self.assertTrue(message.imu_status.imu_com)
            self.assertTrue(message.imu_status.imu_status)
            self.assertTrue(message.imu_status.imu_accels_in_range)
            self.assertTrue(message.imu_status.imu_gyros_in_range)

            #
            # The recording uses the standard gyroscope scale, which is what the driver
            # assumes when it builds sensor_msgs/Imu out of sbg/imu_short.
            #
            self.assertFalse(message.imu_status.imu_gyros_use_high_scale)

            self.assertAlmostEqual(message.temperature / 256.0, REF_IMU_TEMPERATURE, delta=1.0)

            #
            # The recorded Z delta velocity is always below -9.35 m.s^-2, so a positive value
            # here can only come from the NED to ENU conversion.
            #
            self.assertGreater(message.delta_velocity.z / ACCEL_SCALE, 0.0)

        mean_up = statistics.mean(m.delta_velocity.z / ACCEL_SCALE for m in messages)

        self.assertAlmostEqual(mean_up, REF_ACCEL_UP, delta=0.20)

    def test_ros_imu(self):
        messages = self.messages_for('imu/data')

        self.assert_header(messages, 'imu/data', DEVICE_FRAME_ID)

        for message in messages:
            self.assertGreater(message.linear_acceleration.z, 8.5)
            self.assertLess(message.linear_acceleration.z, 11.0)

            for axis in (message.angular_velocity.x,
                         message.angular_velocity.y,
                         message.angular_velocity.z):
                self.assertLess(abs(axis), 0.05, 'the unit is stationary in the recording')

            #
            # The recording has no EKF quaternion log, so the driver has to report the
            # orientation as unavailable.
            #
            self.assertEqual(message.orientation_covariance[0], -1.0)

        self.assertAlmostEqual(
            statistics.mean(m.linear_acceleration.x for m in messages),
            REF_ACCEL_EAST, delta=0.20)
        self.assertAlmostEqual(
            statistics.mean(m.linear_acceleration.y for m in messages),
            REF_ACCEL_NORTH, delta=0.20)
        self.assertAlmostEqual(
            statistics.mean(m.linear_acceleration.z for m in messages),
            REF_ACCEL_UP, delta=0.20)

    #
    # EKF attitude and navigation
    #

    def test_sbg_ekf_euler(self):
        messages = self.messages_for('sbg/ekf_euler')

        self.assert_header(messages, 'sbg/ekf_euler', DEVICE_FRAME_ID)
        self.assert_device_timestamps(messages, 'sbg/ekf_euler')

        for message in messages:
            self.assertAlmostEqual(message.angle.x, REF_ROLL, delta=0.003)

            #
            # The recorded pitch is positive and the ENU heading is the recorded heading
            # mirrored around pi/2: both only hold if the driver converted the frame.
            #
            self.assertLess(message.angle.y, 0.0)
            self.assertAlmostEqual(message.angle.y, REF_PITCH, delta=0.003)
            self.assertAlmostEqual(message.angle.z, REF_HEADING, delta=0.004)

            #
            # The EKF runs in vertical gyro mode over the replayed window: roll and pitch are
            # reliable, heading and position are not.
            #
            self.assertEqual(message.status.solution_mode, SBG_ECOM_SOL_MODE_VERTICAL_GYRO)
            self.assertTrue(message.status.attitude_valid)
            self.assertFalse(message.status.heading_valid)
            self.assertFalse(message.status.position_valid)

            self.assertGreater(message.accuracy.z, 3.0, 'the heading accuracy reports pi')

    def test_sbg_ekf_nav(self):
        messages = self.messages_for('sbg/ekf_nav')

        self.assert_header(messages, 'sbg/ekf_nav', DEVICE_FRAME_ID)
        self.assert_device_timestamps(messages, 'sbg/ekf_nav')

        #
        # The navigation solution is not initialized over the replayed window, so only the
        # plumbing and the reported status are checked here. The numerical content of the
        # navigation solution is deliberately not asserted.
        #
        for message in messages:
            self.assertEqual(message.status.solution_mode, SBG_ECOM_SOL_MODE_VERTICAL_GYRO)
            self.assertFalse(message.status.position_valid)
            self.assertGreater(message.position_accuracy.x, 1000.0)

    #
    # GNSS
    #

    def test_sbg_gps_pos(self):
        messages = self.messages_for('sbg/gps_pos')

        self.assert_header(messages, 'sbg/gps_pos', GNSS_FRAME_ID)
        self.assert_device_timestamps(messages, 'sbg/gps_pos')

        for message in messages:
            self.assertAlmostEqual(message.latitude, REF_LATITUDE, delta=1.0e-5)
            self.assertAlmostEqual(message.longitude, REF_LONGITUDE, delta=1.0e-4)
            self.assertAlmostEqual(message.altitude, REF_ALTITUDE, delta=1.5)
            self.assertAlmostEqual(message.undulation, REF_UNDULATION, delta=0.05)

            self.assertEqual(message.status.type, SBG_ECOM_GNSS_POS_TYPE_PSRDIFF)

            self.assertGreater(message.num_sv_used, 10)
            self.assertGreater(message.num_sv_tracked, message.num_sv_used)

            for axis in (message.position_accuracy.x,
                         message.position_accuracy.y,
                         message.position_accuracy.z):
                self.assertGreater(axis, 0.05)
                self.assertLess(axis, 1.0)

    def test_sbg_gps_vel(self):
        messages = self.messages_for('sbg/gps_vel')

        self.assert_header(messages, 'sbg/gps_vel', GNSS_FRAME_ID)
        self.assert_device_timestamps(messages, 'sbg/gps_vel')

        for message in messages:
            for axis in (message.velocity.x, message.velocity.y, message.velocity.z):
                self.assertLess(abs(axis), 0.1, 'the unit is stationary in the recording')

            #
            # The recorded course is a constant 321.157 degrees, the ENU course is measured
            # from the east instead of the north.
            #
            self.assertAlmostEqual(message.course, REF_GNSS_COURSE, delta=0.05)

    #
    # Status
    #

    def test_sbg_status(self):
        messages = self.messages_for('sbg/status')

        self.assert_header(messages, 'sbg/status', DEVICE_FRAME_ID)
        self.assert_device_timestamps(messages, 'sbg/status')

        for message in messages:
            self.assertTrue(message.status_general.main_power)
            self.assertTrue(message.status_general.imu_power)
            self.assertTrue(message.status_general.gps_power)
            self.assertTrue(message.status_general.settings)
            self.assertTrue(message.status_general.temperature)
            self.assertTrue(message.status_general.datalogger)
            self.assertTrue(message.status_general.cpu)


@launch_testing.post_shutdown_test()
class TestSbgDeviceShutdown(unittest.TestCase):

    def test_shutdown_is_clean(self, proc_info):
        #
        # The file interface doesn't stop the node at the end of the recording, so the driver
        # is still running when the active tests are over and launch_testing signals it. The
        # node handles SIGINT and leaves its main loop, but the signal exit codes are accepted
        # as well in case it is terminated before it gets there.
        #
        launch_testing.asserts.assertExitCodes(
            proc_info, allowable_exit_codes=[0, -2, -15, 130, 143])
