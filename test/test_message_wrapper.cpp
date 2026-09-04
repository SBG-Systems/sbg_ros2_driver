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

  //
  // Odometry reference position, deliberately on the central meridian of UTM zone 32: the
  // convergence angle is zero there, which keeps the covariance expectations analytical.
  //
  constexpr double  ODOM_LATITUDE     = 45.0;
  constexpr double  ODOM_LONGITUDE    = 9.0;
  constexpr double  ODOM_ALTITUDE     = 100.0;
  constexpr double  ODOM_STEP_DEG     = 0.001;

  //
  // The projection agrees with the closed form expectations below to better than 10 um over
  // the displacements used here, so a millimetre is a meaningful tolerance.
  //
  constexpr double  METER_TOLERANCE   = 1.0e-3;
  constexpr double  CROSS_TOLERANCE   = 1.0e-2;

  //
  // sbgECom short IMU gyroscope scale factors, in LSB per rad.s-1. They are derived from their
  // documented definitions rather than copied from the driver: the standard scale is 2^26, and
  // the high range one spreads the full int32 range over 10000 deg.s-1.
  //
  const double  GYRO_SCALE_STD   = std::pow(2.0, 26);
  const double  GYRO_SCALE_HIGH  = std::floor((std::pow(2.0, 31) - 1.0) / (10000.0 * M_PI / 180.0));

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

  /*!
   * Expected easting displacement for a small longitude step, in meters.
   *
   * First order transverse Mercator: k0 * nu * cos(lat) * dlon, with nu the prime vertical
   * radius of curvature. Written out here on purpose, so the expectation doesn't come from
   * the projection under test.
   */
  double expectedEastingDelta(double latitude_deg, double delta_longitude_deg)
  {
    constexpr double  WGS84_A     = 6378137.0;
    constexpr double  WGS84_E     = 0.0818191908;
    constexpr double  UTM_K0      = 0.9996;

    const double latitude_rad = latitude_deg * M_PI / 180.0;
    const double eccentricity_sq = WGS84_E * WGS84_E;
    const double sin_latitude = std::sin(latitude_rad);
    const double prime_vertical = WGS84_A / std::sqrt(1.0 - eccentricity_sq * sin_latitude * sin_latitude);

    return UTM_K0 * prime_vertical * std::cos(latitude_rad) * (delta_longitude_deg * M_PI / 180.0);
  }

  /*!
   * Expected northing displacement for a small latitude step, in meters.
   *
   * First order meridian arc: k0 * rho * dlat, with rho the meridional radius of curvature.
   */
  double expectedNorthingDelta(double latitude_deg, double delta_latitude_deg)
  {
    constexpr double  WGS84_A     = 6378137.0;
    constexpr double  WGS84_E     = 0.0818191908;
    constexpr double  UTM_K0      = 0.9996;

    const double latitude_rad = latitude_deg * M_PI / 180.0;
    const double eccentricity_sq = WGS84_E * WGS84_E;
    const double sin_latitude = std::sin(latitude_rad);
    const double denominator = 1.0 - eccentricity_sq * sin_latitude * sin_latitude;
    const double meridional = WGS84_A * (1.0 - eccentricity_sq) / (denominator * std::sqrt(denominator));

    return UTM_K0 * meridional * (delta_latitude_deg * M_PI / 180.0);
  }

  /*!
   * Create an Ekf navigation message with distinct accuracies on each axis.
   */
  sbg_driver::msg::SbgEkfNav createNavMessage(double latitude, double longitude, double altitude)
  {
    sbg_driver::msg::SbgEkfNav nav_message;

    nav_message.time_stamp  = 1000;
    nav_message.latitude    = latitude;
    nav_message.longitude   = longitude;
    nav_message.altitude    = altitude;

    nav_message.velocity.x  = 1.0;
    nav_message.velocity.y  = 2.0;
    nav_message.velocity.z  = 3.0;

    nav_message.velocity_accuracy.x = 0.25;
    nav_message.velocity_accuracy.y = 0.5;
    nav_message.velocity_accuracy.z = 0.75;

    //
    // Equal east and north accuracies on purpose: the production code mixes them through the
    // convergence angle, and this test is not the place to pin down that mapping.
    //
    nav_message.position_accuracy.x = 3.0;
    nav_message.position_accuracy.y = 3.0;
    nav_message.position_accuracy.z = 5.0;

    return nav_message;
  }

  /*!
   * Create an Ekf Euler message with distinct accuracies on each axis.
   */
  sbg_driver::msg::SbgEkfEuler createEulerMessage()
  {
    sbg_driver::msg::SbgEkfEuler euler_message;

    euler_message.time_stamp  = 1000;
    euler_message.angle.x     = 0.25;
    euler_message.angle.y     = -0.125;
    euler_message.angle.z     = 0.5;
    euler_message.accuracy.x  = 2.0;
    euler_message.accuracy.y  = 3.0;
    euler_message.accuracy.z  = 4.0;

    return euler_message;
  }

  /*!
   * Configure a wrapper for the odometry output, with deterministic frames.
   */
  void configureForOdometry(sbg::MessageWrapper &ref_wrapper, bool publish_tf)
  {
    ref_wrapper.setUseEnu(true);
    ref_wrapper.setFrameId("imu_link_test");
    ref_wrapper.setOdomEnable(true);
    ref_wrapper.setOdomPublishTf(publish_tf);
    ref_wrapper.setOdomFrameId("odom_test");
    ref_wrapper.setOdomBaseFrameId("base_link_test");
    ref_wrapper.setOdomInitFrameId("map_test");
  }

  /*!
   * Create a short IMU message whose raw delta angle is an exact multiple of the gyroscope
   * scale factor, so the expected angular velocity is that multiple.
   *
   * Standard scale asks for 0.03125, -0.0625 and 0.125 rad.s-1, the high range one for 1.0,
   * -0.5 and 2.0 rad.s-1. Both sets are asymmetric and signed, and both land on integer raw
   * counts, as a device would send them.
   */
  sbg_driver::msg::SbgImuShort createImuShortMessage(bool use_high_scale)
  {
    sbg_driver::msg::SbgImuShort imu_message;

    imu_message.time_stamp = 2000;
    imu_message.imu_status.imu_gyros_use_high_scale = use_high_scale;

    const double gyro_scale = use_high_scale ? GYRO_SCALE_HIGH : GYRO_SCALE_STD;

    if (use_high_scale)
    {
      imu_message.delta_angle.x = gyro_scale * 1.0;
      imu_message.delta_angle.y = gyro_scale * -0.5;
      imu_message.delta_angle.z = gyro_scale * 2.0;
    }
    else
    {
      imu_message.delta_angle.x = gyro_scale * 0.03125;
      imu_message.delta_angle.y = gyro_scale * -0.0625;
      imu_message.delta_angle.z = gyro_scale * 0.125;
    }

    return imu_message;
  }

  /*!
   * Check the angular velocity of a message built from createImuShortMessage().
   */
  void expectImuShortAngularVelocity(const geometry_msgs::msg::Vector3 &ref_angular_velocity,
                                     bool use_high_scale)
  {
    if (use_high_scale)
    {
      EXPECT_DOUBLE_EQ(ref_angular_velocity.x, 1.0);
      EXPECT_DOUBLE_EQ(ref_angular_velocity.y, -0.5);
      EXPECT_DOUBLE_EQ(ref_angular_velocity.z, 2.0);
    }
    else
    {
      EXPECT_DOUBLE_EQ(ref_angular_velocity.x, 0.03125);
      EXPECT_DOUBLE_EQ(ref_angular_velocity.y, -0.0625);
      EXPECT_DOUBLE_EQ(ref_angular_velocity.z, 0.125);
    }
  }

  /*!
   * Create the navigation message of a step north from the reference, with a 10 m altitude gain.
   */
  sbg_driver::msg::SbgEkfNav createNorthStepNavMessage()
  {
    return createNavMessage(ODOM_LATITUDE + ODOM_STEP_DEG, ODOM_LONGITUDE, ODOM_ALTITUDE + 10.0);
  }

  /*!
   * Check an odometry message produced for createNorthStepNavMessage(), the reference position
   * having been latched first.
   *
   * The expectations are computed from the closed form helpers above and from the synthetic
   * inputs, never from another createRosOdoMessage() call, so both IMU overloads are checked
   * against the same external truth instead of against each other.
   *
   * The angular velocity is not checked here, because the two overloads take it from
   * different fields. Each test asserts it separately.
   */
  void expectNorthStepOdometry(const nav_msgs::msg::Odometry &ref_odo_message)
  {
    EXPECT_EQ(ref_odo_message.header.frame_id, "odom_test");
    EXPECT_EQ(ref_odo_message.child_frame_id, "imu_link_test");

    EXPECT_NEAR(ref_odo_message.pose.pose.position.x, 0.0, CROSS_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.pose.position.y,
                expectedNorthingDelta(ODOM_LATITUDE, ODOM_STEP_DEG), METER_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.pose.position.z, 10.0, METER_TOLERANCE);

    EXPECT_NEAR(ref_odo_message.pose.pose.orientation.x, 0.0, QUAT_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.pose.orientation.y, 0.0, QUAT_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.pose.orientation.z, 0.0, QUAT_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.pose.orientation.w, 1.0, QUAT_TOLERANCE);

    EXPECT_NEAR(ref_odo_message.pose.covariance[0], 9.0, METER_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.covariance[7], 9.0, METER_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.covariance[14], 25.0, METER_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.covariance[21], 4.0, METER_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.covariance[28], 9.0, METER_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.pose.covariance[35], 16.0, METER_TOLERANCE);

    EXPECT_DOUBLE_EQ(ref_odo_message.twist.twist.linear.x, 1.0);
    EXPECT_DOUBLE_EQ(ref_odo_message.twist.twist.linear.y, 2.0);
    EXPECT_DOUBLE_EQ(ref_odo_message.twist.twist.linear.z, 3.0);

    EXPECT_NEAR(ref_odo_message.twist.covariance[0], 0.0625, METER_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.twist.covariance[7], 0.25, METER_TOLERANCE);
    EXPECT_NEAR(ref_odo_message.twist.covariance[14], 0.5625, METER_TOLERANCE);

    EXPECT_DOUBLE_EQ(ref_odo_message.twist.covariance[21], 0.0);
    EXPECT_DOUBLE_EQ(ref_odo_message.twist.covariance[28], 0.0);
    EXPECT_DOUBLE_EQ(ref_odo_message.twist.covariance[35], 0.0);
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

//---------------------------------------------------------------------//
//- ROS odometry and UTM projection                                   -//
//---------------------------------------------------------------------//

TEST_F(MessageWrapperTest, odometryFirstFixIsTheOrigin)
{
  sbg::MessageWrapper           wrapper;
  sbg_driver::msg::SbgImuData   imu_message;

  configureForOdometry(wrapper, false);

  imu_message.time_stamp = 2000;

  const auto odo_message = wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE), createEulerMessage());

  //
  // The first navigation fix latches the UTM reference, so it is reported at the origin.
  //
  EXPECT_NEAR(odo_message.pose.pose.position.x, 0.0, METER_TOLERANCE);
  EXPECT_NEAR(odo_message.pose.pose.position.y, 0.0, METER_TOLERANCE);
  EXPECT_NEAR(odo_message.pose.pose.position.z, 0.0, METER_TOLERANCE);
}

TEST_F(MessageWrapperTest, odometryReportsDisplacementFromTheFirstFix)
{
  sbg::MessageWrapper           wrapper;
  sbg_driver::msg::SbgImuData   imu_message;

  configureForOdometry(wrapper, false);

  imu_message.time_stamp = 2000;

  const auto euler_message = createEulerMessage();

  wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE), euler_message);

  //
  // A step north, with a 10 m altitude gain.
  //
  const auto north_message = wrapper.createRosOdoMessage(
    imu_message,
    createNavMessage(ODOM_LATITUDE + ODOM_STEP_DEG, ODOM_LONGITUDE, ODOM_ALTITUDE + 10.0),
    euler_message);

  EXPECT_NEAR(north_message.pose.pose.position.x, 0.0, CROSS_TOLERANCE);
  EXPECT_NEAR(north_message.pose.pose.position.y,
              expectedNorthingDelta(ODOM_LATITUDE, ODOM_STEP_DEG), METER_TOLERANCE);
  EXPECT_NEAR(north_message.pose.pose.position.z, 10.0, METER_TOLERANCE);

  //
  // A step east, back at the reference altitude.
  //
  const auto east_message = wrapper.createRosOdoMessage(
    imu_message,
    createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE + ODOM_STEP_DEG, ODOM_ALTITUDE),
    euler_message);

  EXPECT_NEAR(east_message.pose.pose.position.x,
              expectedEastingDelta(ODOM_LATITUDE, ODOM_STEP_DEG), METER_TOLERANCE);
  EXPECT_NEAR(east_message.pose.pose.position.y, 0.0, CROSS_TOLERANCE);
  EXPECT_NEAR(east_message.pose.pose.position.z, 0.0, METER_TOLERANCE);

  //
  // Back to the reference position: the origin was latched on the first fix and is never
  // re-initialized, so this has to land on the origin again.
  //
  const auto back_message = wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE), euler_message);

  EXPECT_NEAR(back_message.pose.pose.position.x, 0.0, METER_TOLERANCE);
  EXPECT_NEAR(back_message.pose.pose.position.y, 0.0, METER_TOLERANCE);
  EXPECT_NEAR(back_message.pose.pose.position.z, 0.0, METER_TOLERANCE);
}

TEST_F(MessageWrapperTest, odometryUsesTheConfiguredFrames)
{
  sbg::MessageWrapper           wrapper;
  sbg_driver::msg::SbgImuData   imu_message;

  configureForOdometry(wrapper, false);

  imu_message.time_stamp = 2000;

  const auto odo_message = wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE), createEulerMessage());

  EXPECT_EQ(odo_message.header.frame_id, "odom_test");

  //
  // The twist is reported in the child frame, which the driver sets to the device frame.
  //
  EXPECT_EQ(odo_message.child_frame_id, "imu_link_test");
}

TEST_F(MessageWrapperTest, odometryFillsPoseAndTwistCovariance)
{
  sbg::MessageWrapper           wrapper;
  sbg_driver::msg::SbgImuData   imu_message;

  configureForOdometry(wrapper, false);

  imu_message.time_stamp = 2000;

  const auto odo_message = wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE), createEulerMessage());

  //
  // On the zone central meridian the convergence angle is zero, so the horizontal position
  // variances are the squared horizontal accuracies. They are equal here, which keeps the
  // check independent of how east and north are mapped onto the grid axes.
  //
  EXPECT_NEAR(odo_message.pose.covariance[0], 9.0, METER_TOLERANCE);
  EXPECT_NEAR(odo_message.pose.covariance[7], 9.0, METER_TOLERANCE);
  EXPECT_NEAR(odo_message.pose.covariance[14], 25.0, METER_TOLERANCE);

  //
  // Orientation variances come from the Ekf Euler accuracies.
  //
  EXPECT_NEAR(odo_message.pose.covariance[21], 4.0, METER_TOLERANCE);
  EXPECT_NEAR(odo_message.pose.covariance[28], 9.0, METER_TOLERANCE);
  EXPECT_NEAR(odo_message.pose.covariance[35], 16.0, METER_TOLERANCE);

  //
  // The twist carries the navigation velocity and its squared accuracies. The angular part
  // has no reported accuracy.
  //
  EXPECT_DOUBLE_EQ(odo_message.twist.twist.linear.x, 1.0);
  EXPECT_DOUBLE_EQ(odo_message.twist.twist.linear.y, 2.0);
  EXPECT_DOUBLE_EQ(odo_message.twist.twist.linear.z, 3.0);

  EXPECT_NEAR(odo_message.twist.covariance[0], 0.0625, METER_TOLERANCE);
  EXPECT_NEAR(odo_message.twist.covariance[7], 0.25, METER_TOLERANCE);
  EXPECT_NEAR(odo_message.twist.covariance[14], 0.5625, METER_TOLERANCE);

  EXPECT_DOUBLE_EQ(odo_message.twist.covariance[21], 0.0);
  EXPECT_DOUBLE_EQ(odo_message.twist.covariance[28], 0.0);
  EXPECT_DOUBLE_EQ(odo_message.twist.covariance[35], 0.0);
}

TEST_F(MessageWrapperTest, odometryPassesTheOrientationThrough)
{
  sbg::MessageWrapper           wrapper;
  sbg_driver::msg::SbgImuData   imu_message;

  configureForOdometry(wrapper, false);

  imu_message.time_stamp = 2000;

  const tf2::Quaternion orientation(0.5, 0.5, 0.5, 0.5);

  const auto odo_message = wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE),
    orientation, createEulerMessage());

  EXPECT_NEAR(odo_message.pose.pose.orientation.x, 0.5, QUAT_TOLERANCE);
  EXPECT_NEAR(odo_message.pose.pose.orientation.y, 0.5, QUAT_TOLERANCE);
  EXPECT_NEAR(odo_message.pose.pose.orientation.z, 0.5, QUAT_TOLERANCE);
  EXPECT_NEAR(odo_message.pose.pose.orientation.w, 0.5, QUAT_TOLERANCE);
}

TEST_F(MessageWrapperTest, odometryFromImuDataMatchesExpectedValues)
{
  sbg::MessageWrapper           wrapper;
  sbg_driver::msg::SbgImuData   imu_message;

  configureForOdometry(wrapper, false);

  imu_message.time_stamp  = 2000;
  imu_message.gyro.x      = 0.1;
  imu_message.gyro.y      = 0.2;
  imu_message.gyro.z      = 0.3;

  const tf2::Quaternion orientation(0.0, 0.0, 0.0, 1.0);
  const auto euler_message = createEulerMessage();

  //
  // Latch the UTM reference on the first fix, then step north.
  //
  wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE),
    orientation, euler_message);

  const auto odo_message = wrapper.createRosOdoMessage(
    imu_message, createNorthStepNavMessage(), orientation, euler_message);

  SCOPED_TRACE("SbgImuData overload");
  expectNorthStepOdometry(odo_message);

  //
  // This overload reports the gyroscope rates as they are, in rad.s^-1.
  //
  EXPECT_DOUBLE_EQ(odo_message.twist.twist.angular.x, 0.1);
  EXPECT_DOUBLE_EQ(odo_message.twist.twist.angular.y, 0.2);
  EXPECT_DOUBLE_EQ(odo_message.twist.twist.angular.z, 0.3);
}

TEST_F(MessageWrapperTest, odometryFromImuShortMatchesExpectedValues)
{
  sbg::MessageWrapper wrapper;

  configureForOdometry(wrapper, false);

  const auto imu_message = createImuShortMessage(false);
  const tf2::Quaternion orientation(0.0, 0.0, 0.0, 1.0);
  const auto euler_message = createEulerMessage();

  wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE),
    orientation, euler_message);

  const auto odo_message = wrapper.createRosOdoMessage(
    imu_message, createNorthStepNavMessage(), orientation, euler_message);

  //
  // The short IMU overload shares the whole projection, covariance and frame logic with the
  // one above, and is checked against the very same expectations.
  //
  SCOPED_TRACE("SbgImuShort overload");
  expectNorthStepOdometry(odo_message);

  //
  // The short IMU logs carry the gyroscope output as scaled integers, so the odometry has to
  // divide them by the gyroscope scale factor, exactly like the ROS IMU message does.
  //
  expectImuShortAngularVelocity(odo_message.twist.twist.angular, false);
}

TEST_F(MessageWrapperTest, odometryFromImuShortUsesTheHighGyroScale)
{
  sbg::MessageWrapper wrapper;

  configureForOdometry(wrapper, false);

  const auto imu_message = createImuShortMessage(true);
  const tf2::Quaternion orientation(0.0, 0.0, 0.0, 1.0);
  const auto euler_message = createEulerMessage();

  wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE),
    orientation, euler_message);

  const auto odo_message = wrapper.createRosOdoMessage(
    imu_message, createNorthStepNavMessage(), orientation, euler_message);

  //
  // The status flag selects the high range scale factor, which is about 5.5 times smaller
  // than the standard one, so using the wrong one cannot pass.
  //
  expectImuShortAngularVelocity(odo_message.twist.twist.angular, true);
}

TEST_F(MessageWrapperTest, imuShortAngularVelocityIsConsistentBetweenImuAndOdometry)
{
  sbg::MessageWrapper wrapper;

  configureForOdometry(wrapper, false);

  const tf2::Quaternion orientation(0.0, 0.0, 0.0, 1.0);
  const auto euler_message = createEulerMessage();
  const sbg_driver::msg::SbgEkfQuat quat_message;

  for (const bool use_high_scale : {false, true})
  {
    SCOPED_TRACE(use_high_scale ? "high gyroscope scale" : "standard gyroscope scale");

    const auto imu_message = createImuShortMessage(use_high_scale);

    const auto ros_imu_message = wrapper.createRosImuMessage(imu_message, quat_message);
    const auto odo_message = wrapper.createRosOdoMessage(
      imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE),
      orientation, euler_message);

    //
    // Both outputs are checked against the independently derived expectation first, so the
    // equality below cannot be satisfied by both conversions being wrong the same way.
    //
    expectImuShortAngularVelocity(ros_imu_message.angular_velocity, use_high_scale);
    expectImuShortAngularVelocity(odo_message.twist.twist.angular, use_high_scale);

    EXPECT_DOUBLE_EQ(odo_message.twist.twist.angular.x, ros_imu_message.angular_velocity.x);
    EXPECT_DOUBLE_EQ(odo_message.twist.twist.angular.y, ros_imu_message.angular_velocity.y);
    EXPECT_DOUBLE_EQ(odo_message.twist.twist.angular.z, ros_imu_message.angular_velocity.z);
  }
}

TEST_F(MessageWrapperTest, odometryWithTransformPublishingMatchesExpectedValues)
{
  sbg::MessageWrapper           wrapper;
  sbg_driver::msg::SbgImuData   imu_message;

  configureForOdometry(wrapper, true);

  imu_message.time_stamp = 2000;

  const tf2::Quaternion orientation(0.0, 0.0, 0.0, 1.0);
  const auto euler_message = createEulerMessage();

  wrapper.createRosOdoMessage(
    imu_message, createNavMessage(ODOM_LATITUDE, ODOM_LONGITUDE, ODOM_ALTITUDE),
    orientation, euler_message);

  const auto odo_message = wrapper.createRosOdoMessage(
    imu_message, createNorthStepNavMessage(), orientation, euler_message);

  //
  // Broadcasting the initial and the odometry transforms must leave the message alone, so the
  // expectations are exactly the ones checked without transform publishing. The transforms
  // themselves mirror the pose asserted here.
  //
  SCOPED_TRACE("transform publishing enabled");
  expectNorthStepOdometry(odo_message);
}

//---------------------------------------------------------------------//
//- EKF rotation rate and acceleration, three frame paths             -//
//---------------------------------------------------------------------//

namespace
{
  /*!
   * Create an Ekf rotation rate and acceleration log with asymmetric axes.
   */
  SbgEComLogEkfRotAccel createEkfRotAccelLog()
  {
    SbgEComLogEkfRotAccel log{};

    log.timeStamp         = 1000;

    log.rate[0]           = 1.0f;
    log.rate[1]           = 2.0f;
    log.rate[2]           = 3.0f;

    log.acceleration[0]   = -4.0f;
    log.acceleration[1]   = 5.0f;
    log.acceleration[2]   = -6.0f;

    return log;
  }
}

TEST_F(MessageWrapperTest, ekfRotAccelKeepsAxesInNed)
{
  sbg::MessageWrapper wrapper;

  wrapper.setUseEnu(false);

  const auto body_message = wrapper.createSbgEkfRotAccelMessage(createEkfRotAccelLog(), true);
  const auto nav_message = wrapper.createSbgEkfRotAccelMessage(createEkfRotAccelLog(), false);

  //
  // In NED both the body and the navigation frame logs are passed through untouched.
  //
  for (const auto &message : {body_message, nav_message})
  {
    EXPECT_DOUBLE_EQ(message.rate.x, 1.0);
    EXPECT_DOUBLE_EQ(message.rate.y, 2.0);
    EXPECT_DOUBLE_EQ(message.rate.z, 3.0);

    EXPECT_DOUBLE_EQ(message.acceleration.x, -4.0);
    EXPECT_DOUBLE_EQ(message.acceleration.y, 5.0);
    EXPECT_DOUBLE_EQ(message.acceleration.z, -6.0);
  }
}

TEST_F(MessageWrapperTest, ekfRotAccelFlipsYAndZInEnuBodyFrame)
{
  sbg::MessageWrapper wrapper;

  wrapper.setUseEnu(true);

  const auto message = wrapper.createSbgEkfRotAccelMessage(createEkfRotAccelLog(), true);

  //
  // Body frame, FRD to FLU: X is kept, Y and Z change sign. No axis is swapped.
  //
  EXPECT_DOUBLE_EQ(message.rate.x, 1.0);
  EXPECT_DOUBLE_EQ(message.rate.y, -2.0);
  EXPECT_DOUBLE_EQ(message.rate.z, -3.0);

  EXPECT_DOUBLE_EQ(message.acceleration.x, -4.0);
  EXPECT_DOUBLE_EQ(message.acceleration.y, -5.0);
  EXPECT_DOUBLE_EQ(message.acceleration.z, 6.0);
}

TEST_F(MessageWrapperTest, ekfRotAccelSwapsAxesInEnuNavigationFrame)
{
  sbg::MessageWrapper wrapper;

  wrapper.setUseEnu(true);

  const auto message = wrapper.createSbgEkfRotAccelMessage(createEkfRotAccelLog(), false);

  //
  // Navigation frame, NED to ENU: X and Y are swapped and Z changes sign.
  //
  EXPECT_DOUBLE_EQ(message.rate.x, 2.0);
  EXPECT_DOUBLE_EQ(message.rate.y, 1.0);
  EXPECT_DOUBLE_EQ(message.rate.z, -3.0);

  EXPECT_DOUBLE_EQ(message.acceleration.x, 5.0);
  EXPECT_DOUBLE_EQ(message.acceleration.y, -4.0);
  EXPECT_DOUBLE_EQ(message.acceleration.z, 6.0);
}

//---------------------------------------------------------------------//
//- EKF body velocity                                                 -//
//---------------------------------------------------------------------//

namespace
{
  /*!
   * Create an Ekf body velocity log with asymmetric axes.
   */
  SbgEComLogEkfVelBody createEkfVelBodyLog()
  {
    SbgEComLogEkfVelBody log{};

    log.timeStamp           = 1000;

    log.velocity[0]         = 1.0f;
    log.velocity[1]         = 2.0f;
    log.velocity[2]         = 3.0f;

    log.velocityStdDev[0]   = 0.25f;
    log.velocityStdDev[1]   = 0.5f;
    log.velocityStdDev[2]   = 0.75f;

    return log;
  }
}

TEST_F(MessageWrapperTest, ekfVelBodyKeepsAxesInNed)
{
  sbg::MessageWrapper wrapper;

  wrapper.setUseEnu(false);

  const auto message = wrapper.createSbgEkfVelBodyMessage(createEkfVelBodyLog());

  EXPECT_DOUBLE_EQ(message.velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(message.velocity.y, 2.0);
  EXPECT_DOUBLE_EQ(message.velocity.z, 3.0);

  EXPECT_DOUBLE_EQ(message.velocity_accuracy.x, 0.25);
  EXPECT_DOUBLE_EQ(message.velocity_accuracy.y, 0.5);
  EXPECT_DOUBLE_EQ(message.velocity_accuracy.z, 0.75);
}

TEST_F(MessageWrapperTest, ekfVelBodyFlipsYAndZInEnu)
{
  sbg::MessageWrapper wrapper;

  wrapper.setUseEnu(true);

  const auto message = wrapper.createSbgEkfVelBodyMessage(createEkfVelBodyLog());

  //
  // Body frame, so Y and Z change sign and no axis is swapped.
  //
  EXPECT_DOUBLE_EQ(message.velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(message.velocity.y, -2.0);
  EXPECT_DOUBLE_EQ(message.velocity.z, -3.0);

  //
  // Standard deviations describe a magnitude, they are never negated nor reordered.
  //
  EXPECT_DOUBLE_EQ(message.velocity_accuracy.x, 0.25);
  EXPECT_DOUBLE_EQ(message.velocity_accuracy.y, 0.5);
  EXPECT_DOUBLE_EQ(message.velocity_accuracy.z, 0.75);
}

//---------------------------------------------------------------------//
//- Magnetometer                                                      -//
//---------------------------------------------------------------------//

namespace
{
  /*!
   * Create a magnetometer log with asymmetric axes and a known status bitmask.
   */
  SbgEComLogMag createMagLog()
  {
    SbgEComLogMag log{};

    log.timeStamp           = 1000;
    log.status              = SBG_ECOM_MAG_MAG_X_BIT | SBG_ECOM_MAG_MAG_Z_BIT
                            | SBG_ECOM_MAG_ACCEL_Y_BIT | SBG_ECOM_MAG_CALIBRATION_OK;

    log.magnetometers[0]    = 0.25f;
    log.magnetometers[1]    = 0.5f;
    log.magnetometers[2]    = 0.75f;

    log.accelerometers[0]   = 1.0f;
    log.accelerometers[1]   = 2.0f;
    log.accelerometers[2]   = 3.0f;

    return log;
  }
}

TEST_F(MessageWrapperTest, magKeepsAxesInNed)
{
  sbg::MessageWrapper wrapper;

  wrapper.setUseEnu(false);

  const auto message = wrapper.createSbgMagMessage(createMagLog());

  EXPECT_DOUBLE_EQ(message.mag.x, 0.25);
  EXPECT_DOUBLE_EQ(message.mag.y, 0.5);
  EXPECT_DOUBLE_EQ(message.mag.z, 0.75);

  EXPECT_DOUBLE_EQ(message.accel.x, 1.0);
  EXPECT_DOUBLE_EQ(message.accel.y, 2.0);
  EXPECT_DOUBLE_EQ(message.accel.z, 3.0);
}

TEST_F(MessageWrapperTest, magFlipsYAndZInEnu)
{
  sbg::MessageWrapper wrapper;

  wrapper.setUseEnu(true);

  const auto message = wrapper.createSbgMagMessage(createMagLog());

  //
  // Both the magnetic field and the companion accelerometers are body frame vectors, so Y
  // and Z change sign.
  //
  EXPECT_DOUBLE_EQ(message.mag.x, 0.25);
  EXPECT_DOUBLE_EQ(message.mag.y, -0.5);
  EXPECT_DOUBLE_EQ(message.mag.z, -0.75);

  EXPECT_DOUBLE_EQ(message.accel.x, 1.0);
  EXPECT_DOUBLE_EQ(message.accel.y, -2.0);
  EXPECT_DOUBLE_EQ(message.accel.z, -3.0);
}

TEST_F(MessageWrapperTest, magDecodesTheStatusBitmask)
{
  sbg::MessageWrapper wrapper;

  const auto message = wrapper.createSbgMagMessage(createMagLog());

  EXPECT_TRUE(message.status.mag_x);
  EXPECT_FALSE(message.status.mag_y);
  EXPECT_TRUE(message.status.mag_z);

  EXPECT_FALSE(message.status.accel_x);
  EXPECT_TRUE(message.status.accel_y);
  EXPECT_FALSE(message.status.accel_z);

  EXPECT_FALSE(message.status.mags_in_range);
  EXPECT_FALSE(message.status.accels_in_range);
  EXPECT_TRUE(message.status.calibration);
}

//---------------------------------------------------------------------//
//- Ship motion                                                       -//
//---------------------------------------------------------------------//

namespace
{
  /*!
   * Create a ship motion log with asymmetric axes and a known status bitmask.
   */
  SbgEComLogShipMotion createShipMotionLog()
  {
    SbgEComLogShipMotion log{};

    log.timeStamp         = 1000;
    log.status            = SBG_ECOM_SHIP_MOTION_HEAVE_VALID | SBG_ECOM_SHIP_MOTION_SURGE_SWAY_VALID
                          | SBG_ECOM_SHIP_MOTION_ACCEL_VALID;
    log.mainHeavePeriod   = 7.5f;

    log.shipMotion[0]     = 1.0f;
    log.shipMotion[1]     = 2.0f;
    log.shipMotion[2]     = 3.0f;

    log.shipAccel[0]      = -4.0f;
    log.shipAccel[1]      = 5.0f;
    log.shipAccel[2]      = -6.0f;

    log.shipVel[0]        = 0.25f;
    log.shipVel[1]        = -0.5f;
    log.shipVel[2]        = 0.75f;

    return log;
  }
}

TEST_F(MessageWrapperTest, shipMotionIsPassedThroughInBothConventions)
{
  sbg::MessageWrapper wrapper;

  //
  // The ship motion conversion is stateless, so one wrapper can be reconfigured between the
  // two conventions instead of running two nodes at once.
  //
  wrapper.setUseEnu(false);
  const auto ned_message = wrapper.createSbgShipMotionMessage(createShipMotionLog());

  wrapper.setUseEnu(true);
  const auto enu_message = wrapper.createSbgShipMotionMessage(createShipMotionLog());

  //
  // This documents the current behavior: surge, sway and heave and their derivatives are
  // reported exactly as the device sends them, with no NED to ENU conversion, unlike every
  // other body frame vector the driver publishes.
  //
  for (const auto &message : {ned_message, enu_message})
  {
    EXPECT_DOUBLE_EQ(message.ship_motion.x, 1.0);
    EXPECT_DOUBLE_EQ(message.ship_motion.y, 2.0);
    EXPECT_DOUBLE_EQ(message.ship_motion.z, 3.0);

    EXPECT_DOUBLE_EQ(message.acceleration.x, -4.0);
    EXPECT_DOUBLE_EQ(message.acceleration.y, 5.0);
    EXPECT_DOUBLE_EQ(message.acceleration.z, -6.0);

    EXPECT_DOUBLE_EQ(message.velocity.x, 0.25);
    EXPECT_DOUBLE_EQ(message.velocity.y, -0.5);
    EXPECT_DOUBLE_EQ(message.velocity.z, 0.75);
  }
}

TEST_F(MessageWrapperTest, shipMotionDecodesTheStatusBitmask)
{
  sbg::MessageWrapper wrapper;

  const auto message = wrapper.createSbgShipMotionMessage(createShipMotionLog());

  EXPECT_TRUE(message.status.heave_valid);
  EXPECT_FALSE(message.status.heave_vel_aided);
  EXPECT_TRUE(message.status.surge_sway_included);
  EXPECT_FALSE(message.status.period_valid);
  EXPECT_FALSE(message.status.swell_mode);
  EXPECT_TRUE(message.status.accel_valid);
}
