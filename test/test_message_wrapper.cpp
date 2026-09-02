// File header
#include <message_wrapper.h>

// Test framework
#include <gtest/gtest.h>

// STL headers
#include <cmath>
#include <string>

namespace
{
  constexpr double  ANGLE_TOLERANCE   = 1.0e-5;
  constexpr double  QUAT_TOLERANCE    = 1.0e-9;

  /*!
   * Create a fully valid UTC log, so that the message wrapper synchronizes on the UTC time.
   */
  SbgEComLogUtc createValidUtcLog(uint32_t time_stamp, uint16_t year, int8_t month, int8_t day,
                                  int8_t hour, int8_t minute, int8_t second, int32_t nanosecond)
  {
    SbgEComLogUtc log;

    sbgEComLogUtcZeroInit(&log);

    log.timeStamp   = time_stamp;
    log.year        = year;
    log.month       = month;
    log.day         = day;
    log.hour        = hour;
    log.minute      = minute;
    log.second      = second;
    log.nanoSecond  = nanosecond;

    sbgEComLogUtcSetClockState(&log, SBG_ECOM_CLOCK_STATE_VALID);
    sbgEComLogUtcSetUtcStatus(&log, SBG_ECOM_UTC_STATUS_INITIALIZED);
    sbgEComLogUtcSetHasClockInput(&log, true);
    sbgEComLogUtcTimeSetIsAccurate(&log, true);

    return log;
  }

  /*!
   * Create an IMU log with distinct values on each axis.
   */
  SbgEComLogImuLegacy createImuLog(uint32_t time_stamp)
  {
    SbgEComLogImuLegacy log{};

    log.timeStamp           = time_stamp;
    log.temperature         = 25.0f;

    log.accelerometers[0]   = 1.0f;
    log.accelerometers[1]   = 2.0f;
    log.accelerometers[2]   = 3.0f;

    log.gyroscopes[0]       = 0.25f;
    log.gyroscopes[1]       = 0.5f;
    log.gyroscopes[2]       = 0.75f;

    log.deltaVelocity[0]    = 4.0f;
    log.deltaVelocity[1]    = 5.0f;
    log.deltaVelocity[2]    = 6.0f;

    log.deltaAngle[0]       = 0.125f;
    log.deltaAngle[1]       = 0.25f;
    log.deltaAngle[2]       = 0.375f;

    return log;
  }

  class MessageWrapperTest : public ::testing::Test
  {
  protected:
    static void SetUpTestSuite()
    {
      rclcpp::init(0, nullptr);
    }

    static void TearDownTestSuite()
    {
      rclcpp::shutdown();
    }
  };
}

//---------------------------------------------------------------------//
//- Frame ID                                                          -//
//---------------------------------------------------------------------//

TEST_F(MessageWrapperTest, imuMessageUsesTheConfiguredFrameId)
{
  sbg::MessageWrapper wrapper;

  wrapper.setFrameId("imu_link_ned");

  const auto imu_message = wrapper.createSbgImuDataMessage(createImuLog(1000));

  EXPECT_EQ(imu_message.header.frame_id, "imu_link_ned");
}

TEST_F(MessageWrapperTest, gpsMessagesUseTheConfiguredGpsFrameId)
{
  sbg::MessageWrapper   wrapper;
  SbgEComLogGnssPos     log;

  sbgEComLogGnssPosZeroInit(&log);

  wrapper.setFrameId("imu_link_ned");
  wrapper.setGpsFrameId("gnss_link");

  const auto gps_pos_message = wrapper.createSbgGpsPosMessage(log);

  EXPECT_EQ(gps_pos_message.header.frame_id, "gnss_link");
}

//---------------------------------------------------------------------//
//- IMU NED / ENU conversions                                         -//
//---------------------------------------------------------------------//

TEST_F(MessageWrapperTest, imuMessageKeepsSbgAxesInNed)
{
  sbg::MessageWrapper wrapper;

  wrapper.setUseEnu(false);

  const auto imu_message = wrapper.createSbgImuDataMessage(createImuLog(1000));

  EXPECT_DOUBLE_EQ(imu_message.accel.x, 1.0);
  EXPECT_DOUBLE_EQ(imu_message.accel.y, 2.0);
  EXPECT_DOUBLE_EQ(imu_message.accel.z, 3.0);

  EXPECT_DOUBLE_EQ(imu_message.gyro.x, 0.25);
  EXPECT_DOUBLE_EQ(imu_message.gyro.y, 0.5);
  EXPECT_DOUBLE_EQ(imu_message.gyro.z, 0.75);

  EXPECT_DOUBLE_EQ(imu_message.delta_vel.x, 4.0);
  EXPECT_DOUBLE_EQ(imu_message.delta_vel.y, 5.0);
  EXPECT_DOUBLE_EQ(imu_message.delta_vel.z, 6.0);

  EXPECT_DOUBLE_EQ(imu_message.delta_angle.x, 0.125);
  EXPECT_DOUBLE_EQ(imu_message.delta_angle.y, 0.25);
  EXPECT_DOUBLE_EQ(imu_message.delta_angle.z, 0.375);

  EXPECT_FLOAT_EQ(imu_message.temp, 25.0f);
}

TEST_F(MessageWrapperTest, imuMessageFlipsYAndZInEnu)
{
  sbg::MessageWrapper wrapper;

  wrapper.setUseEnu(true);

  const auto imu_message = wrapper.createSbgImuDataMessage(createImuLog(1000));

  EXPECT_DOUBLE_EQ(imu_message.accel.x, 1.0);
  EXPECT_DOUBLE_EQ(imu_message.accel.y, -2.0);
  EXPECT_DOUBLE_EQ(imu_message.accel.z, -3.0);

  EXPECT_DOUBLE_EQ(imu_message.gyro.x, 0.25);
  EXPECT_DOUBLE_EQ(imu_message.gyro.y, -0.5);
  EXPECT_DOUBLE_EQ(imu_message.gyro.z, -0.75);

  EXPECT_DOUBLE_EQ(imu_message.delta_vel.x, 4.0);
  EXPECT_DOUBLE_EQ(imu_message.delta_vel.y, -5.0);
  EXPECT_DOUBLE_EQ(imu_message.delta_vel.z, -6.0);

  EXPECT_DOUBLE_EQ(imu_message.delta_angle.x, 0.125);
  EXPECT_DOUBLE_EQ(imu_message.delta_angle.y, -0.25);
  EXPECT_DOUBLE_EQ(imu_message.delta_angle.z, -0.375);
}

//---------------------------------------------------------------------//
//- EKF NED / ENU conversions                                         -//
//---------------------------------------------------------------------//

TEST_F(MessageWrapperTest, ekfEulerKeepsAnglesInNed)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogEkfEuler  log{};

  log.euler[0] = 0.25f;
  log.euler[1] = 0.5f;
  log.euler[2] = 0.75f;

  wrapper.setUseEnu(false);

  const auto euler_message = wrapper.createSbgEkfEulerMessage(log);

  EXPECT_DOUBLE_EQ(euler_message.angle.x, 0.25);
  EXPECT_DOUBLE_EQ(euler_message.angle.y, 0.5);
  EXPECT_DOUBLE_EQ(euler_message.angle.z, 0.75);
}

TEST_F(MessageWrapperTest, ekfEulerConvertsHeadingToEnu)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogEkfEuler  log{};

  log.euler[0] = 0.25f;
  log.euler[1] = 0.5f;
  log.euler[2] = 0.0f;

  wrapper.setUseEnu(true);

  const auto euler_message = wrapper.createSbgEkfEulerMessage(log);

  //
  // A null NED heading (pointing north) is a +90 degree ENU yaw (measured from east).
  //
  EXPECT_DOUBLE_EQ(euler_message.angle.x, 0.25);
  EXPECT_DOUBLE_EQ(euler_message.angle.y, -0.5);
  EXPECT_NEAR(euler_message.angle.z, M_PI / 2.0, ANGLE_TOLERANCE);
}

TEST_F(MessageWrapperTest, ekfNavKeepsNedVelocityAndAccuracy)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogEkfNav    log{};

  log.position[0]       = 48.0;
  log.position[1]       = -4.0;
  log.position[2]       = 100.0;

  log.velocity[0]       = 1.0f;
  log.velocity[1]       = 2.0f;
  log.velocity[2]       = 3.0f;

  log.velocityStdDev[0] = 0.25f;
  log.velocityStdDev[1] = 0.5f;
  log.velocityStdDev[2] = 0.75f;

  log.positionStdDev[0] = 4.0f;
  log.positionStdDev[1] = 5.0f;
  log.positionStdDev[2] = 6.0f;

  wrapper.setUseEnu(false);

  const auto nav_message = wrapper.createSbgEkfNavMessage(log);

  EXPECT_DOUBLE_EQ(nav_message.latitude,  48.0);
  EXPECT_DOUBLE_EQ(nav_message.longitude, -4.0);
  EXPECT_DOUBLE_EQ(nav_message.altitude,  100.0);

  EXPECT_DOUBLE_EQ(nav_message.velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(nav_message.velocity.y, 2.0);
  EXPECT_DOUBLE_EQ(nav_message.velocity.z, 3.0);

  EXPECT_DOUBLE_EQ(nav_message.velocity_accuracy.x, 0.25);
  EXPECT_DOUBLE_EQ(nav_message.velocity_accuracy.y, 0.5);
  EXPECT_DOUBLE_EQ(nav_message.velocity_accuracy.z, 0.75);

  EXPECT_DOUBLE_EQ(nav_message.position_accuracy.x, 4.0);
  EXPECT_DOUBLE_EQ(nav_message.position_accuracy.y, 5.0);
  EXPECT_DOUBLE_EQ(nav_message.position_accuracy.z, 6.0);
}

TEST_F(MessageWrapperTest, ekfNavSwapsVelocityAndAccuracyInEnu)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogEkfNav    log{};

  log.position[0]       = 48.0;
  log.position[1]       = -4.0;
  log.position[2]       = 100.0;

  log.velocity[0]       = 1.0f;
  log.velocity[1]       = 2.0f;
  log.velocity[2]       = 3.0f;

  log.velocityStdDev[0] = 0.25f;
  log.velocityStdDev[1] = 0.5f;
  log.velocityStdDev[2] = 0.75f;

  log.positionStdDev[0] = 4.0f;
  log.positionStdDev[1] = 5.0f;
  log.positionStdDev[2] = 6.0f;

  wrapper.setUseEnu(true);

  const auto nav_message = wrapper.createSbgEkfNavMessage(log);

  //
  // The geographic position is not affected by the NED to ENU conversion.
  //
  EXPECT_DOUBLE_EQ(nav_message.latitude,  48.0);
  EXPECT_DOUBLE_EQ(nav_message.longitude, -4.0);
  EXPECT_DOUBLE_EQ(nav_message.altitude,  100.0);

  EXPECT_DOUBLE_EQ(nav_message.velocity.x, 2.0);
  EXPECT_DOUBLE_EQ(nav_message.velocity.y, 1.0);
  EXPECT_DOUBLE_EQ(nav_message.velocity.z, -3.0);

  //
  // Standard deviations are swapped but never negated.
  //
  EXPECT_DOUBLE_EQ(nav_message.velocity_accuracy.x, 0.5);
  EXPECT_DOUBLE_EQ(nav_message.velocity_accuracy.y, 0.25);
  EXPECT_DOUBLE_EQ(nav_message.velocity_accuracy.z, 0.75);

  EXPECT_DOUBLE_EQ(nav_message.position_accuracy.x, 5.0);
  EXPECT_DOUBLE_EQ(nav_message.position_accuracy.y, 4.0);
  EXPECT_DOUBLE_EQ(nav_message.position_accuracy.z, 6.0);
}

TEST_F(MessageWrapperTest, ekfQuatKeepsNedQuaternion)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogEkfQuat   log{};

  log.quaternion[0] = 0.5f;
  log.quaternion[1] = 0.5f;
  log.quaternion[2] = 0.5f;
  log.quaternion[3] = 0.5f;

  wrapper.setUseEnu(false);

  const auto quat_message = wrapper.createSbgEkfQuatMessage(log);

  EXPECT_NEAR(quat_message.quaternion.x, 0.5, QUAT_TOLERANCE);
  EXPECT_NEAR(quat_message.quaternion.y, 0.5, QUAT_TOLERANCE);
  EXPECT_NEAR(quat_message.quaternion.z, 0.5, QUAT_TOLERANCE);
  EXPECT_NEAR(quat_message.quaternion.w, 0.5, QUAT_TOLERANCE);
}

TEST_F(MessageWrapperTest, ekfQuatRotatesIdentityToEnu)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogEkfQuat   log{};

  log.quaternion[0] = 1.0f;
  log.quaternion[1] = 0.0f;
  log.quaternion[2] = 0.0f;
  log.quaternion[3] = 0.0f;

  wrapper.setUseEnu(true);

  const auto quat_message = wrapper.createSbgEkfQuatMessage(log);

  //
  // The NED identity is a 90 degree rotation around the up axis in ENU.
  //
  EXPECT_NEAR(quat_message.quaternion.x, 0.0,            QUAT_TOLERANCE);
  EXPECT_NEAR(quat_message.quaternion.y, 0.0,            QUAT_TOLERANCE);
  EXPECT_NEAR(quat_message.quaternion.z, M_SQRT2 / 2.0,  QUAT_TOLERANCE);
  EXPECT_NEAR(quat_message.quaternion.w, M_SQRT2 / 2.0,  QUAT_TOLERANCE);
}

//---------------------------------------------------------------------//
//- GNSS NED / ENU conversions                                        -//
//---------------------------------------------------------------------//

TEST_F(MessageWrapperTest, gpsVelSwapsVelocityAndCourseInEnu)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogGnssVel   log{};

  log.velocity[0]     = 1.0f;
  log.velocity[1]     = 2.0f;
  log.velocity[2]     = 3.0f;

  log.velocityAcc[0]  = 0.25f;
  log.velocityAcc[1]  = 0.5f;
  log.velocityAcc[2]  = 0.75f;

  log.course          = 30.0f;

  wrapper.setUseEnu(true);

  const auto gps_vel_message = wrapper.createSbgGpsVelMessage(log);

  EXPECT_DOUBLE_EQ(gps_vel_message.velocity.x, 2.0);
  EXPECT_DOUBLE_EQ(gps_vel_message.velocity.y, 1.0);
  EXPECT_DOUBLE_EQ(gps_vel_message.velocity.z, -3.0);

  EXPECT_DOUBLE_EQ(gps_vel_message.velocity_accuracy.x, 0.5);
  EXPECT_DOUBLE_EQ(gps_vel_message.velocity_accuracy.y, 0.25);
  EXPECT_DOUBLE_EQ(gps_vel_message.velocity_accuracy.z, 0.75);

  EXPECT_NEAR(gps_vel_message.course, 60.0f, ANGLE_TOLERANCE);
}

TEST_F(MessageWrapperTest, gpsHdtConvertsHeadingAndPitchToEnu)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogGnssHdt   log{};

  log.heading = 120.0f;
  log.pitch   = 5.0f;

  wrapper.setUseEnu(true);

  const auto gps_hdt_message = wrapper.createSbgGpsHdtMessage(log);

  //
  // wrapAngle360(90 - 120) keeps the heading in the [0 ; 360] range.
  //
  EXPECT_NEAR(gps_hdt_message.true_heading, 330.0f, ANGLE_TOLERANCE);
  EXPECT_NEAR(gps_hdt_message.pitch, -5.0f, ANGLE_TOLERANCE);
}

TEST_F(MessageWrapperTest, gpsPosSwapsPositionAccuracyInEnu)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogGnssPos   log;

  sbgEComLogGnssPosZeroInit(&log);

  log.latitudeAccuracy  = 2.0f;
  log.longitudeAccuracy = 3.0f;
  log.altitudeAccuracy  = 4.0f;

  wrapper.setUseEnu(true);

  const auto gps_pos_message = wrapper.createSbgGpsPosMessage(log);

  EXPECT_DOUBLE_EQ(gps_pos_message.position_accuracy.x, 3.0);
  EXPECT_DOUBLE_EQ(gps_pos_message.position_accuracy.y, 2.0);
  EXPECT_DOUBLE_EQ(gps_pos_message.position_accuracy.z, 4.0);
}

//---------------------------------------------------------------------//
//- ROS standard messages                                             -//
//---------------------------------------------------------------------//

TEST_F(MessageWrapperTest, navSatFixReportsStatusServiceAndCovariance)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogGnssPos   log;

  sbgEComLogGnssPosZeroInit(&log);

  log.latitude          = 48.0;
  log.longitude         = -4.0;
  log.altitude          = 100.0;
  log.undulation        = 50.0f;
  log.latitudeAccuracy  = 2.0f;
  log.longitudeAccuracy = 3.0f;
  log.altitudeAccuracy  = 4.0f;

  sbgEComLogGnssPosSetType(&log, SBG_ECOM_GNSS_POS_TYPE_SBAS);
  log.status |= SBG_ECOM_GNSS_POS_GLO_L1_USED;

  wrapper.setUseEnu(false);
  wrapper.setGpsFrameId("gnss_link");

  const auto nav_sat_fix_message = wrapper.createRosNavSatFixMessage(wrapper.createSbgGpsPosMessage(log));

  EXPECT_EQ(nav_sat_fix_message.header.frame_id, "gnss_link");

  EXPECT_EQ(nav_sat_fix_message.status.status, static_cast<int8_t>(nav_sat_fix_message.status.STATUS_SBAS_FIX));
  EXPECT_EQ(nav_sat_fix_message.status.service, static_cast<uint16_t>(nav_sat_fix_message.status.SERVICE_GLONASS));

  EXPECT_DOUBLE_EQ(nav_sat_fix_message.latitude,  48.0);
  EXPECT_DOUBLE_EQ(nav_sat_fix_message.longitude, -4.0);

  //
  // The NavSatFix altitude is expressed above the ellipsoid.
  //
  EXPECT_DOUBLE_EQ(nav_sat_fix_message.altitude, 150.0);

  EXPECT_DOUBLE_EQ(nav_sat_fix_message.position_covariance[0], 4.0);
  EXPECT_DOUBLE_EQ(nav_sat_fix_message.position_covariance[4], 9.0);
  EXPECT_DOUBLE_EQ(nav_sat_fix_message.position_covariance[8], 16.0);

  EXPECT_EQ(nav_sat_fix_message.position_covariance_type,
            static_cast<uint8_t>(nav_sat_fix_message.COVARIANCE_TYPE_DIAGONAL_KNOWN));
}

TEST_F(MessageWrapperTest, navSatFixReportsNoFixWithoutSolution)
{
  sbg::MessageWrapper wrapper;
  SbgEComLogGnssPos   log;

  sbgEComLogGnssPosZeroInit(&log);

  sbgEComLogGnssPosSetType(&log, SBG_ECOM_GNSS_POS_TYPE_NO_SOLUTION);

  const auto nav_sat_fix_message = wrapper.createRosNavSatFixMessage(wrapper.createSbgGpsPosMessage(log));

  EXPECT_EQ(nav_sat_fix_message.status.status, static_cast<int8_t>(nav_sat_fix_message.status.STATUS_NO_FIX));
  EXPECT_EQ(nav_sat_fix_message.status.service, static_cast<uint16_t>(nav_sat_fix_message.status.SERVICE_GPS));
}

TEST_F(MessageWrapperTest, rosImuMessageInvalidatesOrientationWhenQuaternionIsMissing)
{
  sbg::MessageWrapper           wrapper;
  sbg_driver::msg::SbgEkfQuat   quat_message;

  const auto imu_message      = wrapper.createSbgImuDataMessage(createImuLog(1000));
  const auto ros_imu_message  = wrapper.createRosImuMessage(imu_message, quat_message);

  EXPECT_DOUBLE_EQ(ros_imu_message.orientation_covariance[0], -1.0);
}

TEST_F(MessageWrapperTest, rosImuMessageFillsOrientationCovarianceFromAccuracy)
{
  sbg::MessageWrapper           wrapper;
  sbg_driver::msg::SbgEkfQuat   quat_message;

  quat_message.quaternion.w = 1.0;
  quat_message.accuracy.x   = 2.0;
  quat_message.accuracy.y   = 3.0;
  quat_message.accuracy.z   = 4.0;

  const auto imu_message      = wrapper.createSbgImuDataMessage(createImuLog(1000));
  const auto ros_imu_message  = wrapper.createRosImuMessage(imu_message, quat_message);

  EXPECT_DOUBLE_EQ(ros_imu_message.orientation.w, 1.0);

  EXPECT_DOUBLE_EQ(ros_imu_message.orientation_covariance[0], 4.0);
  EXPECT_DOUBLE_EQ(ros_imu_message.orientation_covariance[4], 9.0);
  EXPECT_DOUBLE_EQ(ros_imu_message.orientation_covariance[8], 16.0);

  //
  // Angular velocity and linear acceleration covariances are not provided by the device.
  //
  for (size_t i = 0; i < 9; i++)
  {
    EXPECT_DOUBLE_EQ(ros_imu_message.angular_velocity_covariance[i], 0.0);
    EXPECT_DOUBLE_EQ(ros_imu_message.linear_acceleration_covariance[i], 0.0);
  }
}

//---------------------------------------------------------------------//
//- Time conversion                                                   -//
//---------------------------------------------------------------------//

TEST_F(MessageWrapperTest, utcTimeIsConvertedToUnixEpoch)
{
  sbg::MessageWrapper wrapper;

  wrapper.setTimeReference(sbg::TimeReference::INS_UNIX);

  const auto utc_message  = wrapper.createSbgUtcTimeMessage(createValidUtcLog(1000, 2023, 1, 1, 0, 0, 0, 0));
  const auto time_ref     = wrapper.createRosUtcTimeReferenceMessage(utc_message);

  EXPECT_EQ(rclcpp::Time(time_ref.time_ref).nanoseconds(), 1672531200000000000LL);
}

TEST_F(MessageWrapperTest, utcTimeConversionHandlesLeapDayAndNanoseconds)
{
  sbg::MessageWrapper wrapper;

  wrapper.setTimeReference(sbg::TimeReference::INS_UNIX);

  const auto utc_message  = wrapper.createSbgUtcTimeMessage(createValidUtcLog(1000, 2024, 2, 29, 12, 34, 56, 123456789));
  const auto time_ref     = wrapper.createRosUtcTimeReferenceMessage(utc_message);

  EXPECT_EQ(rclcpp::Time(time_ref.time_ref).nanoseconds(), 1709210096123456789LL);
}

TEST_F(MessageWrapperTest, insTimestampIsOffsetFromTheLastUtcMessage)
{
  sbg::MessageWrapper wrapper;

  wrapper.setTimeReference(sbg::TimeReference::INS_UNIX);
  wrapper.createSbgUtcTimeMessage(createValidUtcLog(1000, 2023, 1, 1, 0, 0, 0, 0));

  //
  // The device timestamp is expressed in microseconds since the device boot.
  //
  const auto imu_message = wrapper.createSbgImuDataMessage(createImuLog(2000));

  EXPECT_EQ(rclcpp::Time(imu_message.header.stamp).nanoseconds(), 1672531200001000000LL);
}

TEST_F(MessageWrapperTest, insTimestampHandlesDeviceTimestampRollover)
{
  sbg::MessageWrapper wrapper;

  wrapper.setTimeReference(sbg::TimeReference::INS_UNIX);
  wrapper.createSbgUtcTimeMessage(createValidUtcLog(0xFFFFFF00, 2023, 1, 1, 0, 0, 0, 0));

  //
  // The 32 bit device timestamp wrapped around, so the elapsed time is 512 us and not a
  // negative offset of about 4295 s.
  //
  const auto imu_message = wrapper.createSbgImuDataMessage(createImuLog(0x100));

  EXPECT_EQ(rclcpp::Time(imu_message.header.stamp).nanoseconds(), 1672531200000512000LL);
}
