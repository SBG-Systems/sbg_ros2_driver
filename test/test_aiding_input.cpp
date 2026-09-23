// File header
#include <aiding_input.h>

// Test framework
#include <gtest/gtest.h>

using sbg::aiding::VelocityFrame;

namespace
{
  /*!
   * Create a valid NavSatFix message with a distinct value on each covariance term.
   *
   * The covariance is a row major 3x3 matrix in East/North/Up order, filled with the index of
   * each term so that the mapping to the sbgECom log can be checked term by term.
   */
  sensor_msgs::msg::NavSatFix createNavSatFixMessage()
  {
    sensor_msgs::msg::NavSatFix nav_sat_fix_message;

    nav_sat_fix_message.status.status           = sensor_msgs::msg::NavSatStatus::STATUS_FIX;
    nav_sat_fix_message.latitude                = 48.419727;
    nav_sat_fix_message.longitude               = -4.472119;
    nav_sat_fix_message.altitude                = 100.5;
    nav_sat_fix_message.position_covariance_type = sensor_msgs::msg::NavSatFix::COVARIANCE_TYPE_KNOWN;

    for (size_t i = 0; i < nav_sat_fix_message.position_covariance.size(); i++)
    {
      nav_sat_fix_message.position_covariance[i] = static_cast<double>(i);
    }

    return nav_sat_fix_message;
  }

  /*!
   * Create a TwistWithCovarianceStamped message holding the given linear velocities and variances.
   */
  geometry_msgs::msg::TwistWithCovarianceStamped createTwistMessage(double x, double y, double z, double var_x, double var_y, double var_z)
  {
    geometry_msgs::msg::TwistWithCovarianceStamped twist_message;

    twist_message.twist.twist.linear.x  = x;
    twist_message.twist.twist.linear.y  = y;
    twist_message.twist.twist.linear.z  = z;

    twist_message.twist.covariance[0]   = var_x;
    twist_message.twist.covariance[7]   = var_y;
    twist_message.twist.covariance[14]  = var_z;

    return twist_message;
  }
}

//---------------------------------------------------------------------//
//- Measurement delay                                                 -//
//---------------------------------------------------------------------//

TEST(AidingInput, delayIsZeroForAnUnsetTimeStamp)
{
  builtin_interfaces::msg::Time stamp;

  stamp.sec     = 0;
  stamp.nanosec = 0;

  EXPECT_EQ(sbg::aiding::computeDelayUs(rclcpp::Time(100, 0), stamp), 0u);
}

TEST(AidingInput, delayIsComputedFromThePastTimeStamp)
{
  builtin_interfaces::msg::Time stamp;

  stamp.sec     = 99;
  stamp.nanosec = 500000000;

  EXPECT_EQ(sbg::aiding::computeDelayUs(rclcpp::Time(100, 0), stamp), 500000u);
}

TEST(AidingInput, delayIsZeroForAFutureTimeStamp)
{
  builtin_interfaces::msg::Time stamp;

  stamp.sec     = 101;
  stamp.nanosec = 0;

  EXPECT_EQ(sbg::aiding::computeDelayUs(rclcpp::Time(100, 0), stamp), 0u);
}

//---------------------------------------------------------------------//
//- Generic position                                                  -//
//---------------------------------------------------------------------//

TEST(AidingInput, navSatFixIsConvertedToAPositionLog)
{
  SbgEComLogPosition  log;

  ASSERT_TRUE(sbg::aiding::convertNavSatFix(createNavSatFixMessage(), 1500, log));

  EXPECT_EQ(log.timeStamp, 1500u);

  EXPECT_DOUBLE_EQ(log.position[0], 48.419727);
  EXPECT_DOUBLE_EQ(log.position[1], -4.472119);
  EXPECT_DOUBLE_EQ(log.position[2], 100.5);

  EXPECT_EQ(sbgEComLogPositionGetPositionType(&log), SBG_ECOM_POSITION_TYPE_VALID);
  EXPECT_EQ(sbgEComLogPositionGetTimeType(&log), SBG_ECOM_AIDING_TIME_TYPE_DELAY);
}

TEST(AidingInput, navSatFixCovarianceIsMappedToTheLogTerms)
{
  SbgEComLogPosition  log;

  ASSERT_TRUE(sbg::aiding::convertNavSatFix(createNavSatFixMessage(), 0, log));

  //
  // North maps to latitude, East to longitude and Up to height.
  //
  EXPECT_FLOAT_EQ(log.covLatLat, 4.0f);
  EXPECT_FLOAT_EQ(log.covLonLon, 0.0f);
  EXPECT_FLOAT_EQ(log.covHgtHgt, 8.0f);
  EXPECT_FLOAT_EQ(log.covLatLon, 1.0f);
  EXPECT_FLOAT_EQ(log.covLatHgt, 5.0f);
  EXPECT_FLOAT_EQ(log.covLonHgt, 2.0f);
}

TEST(AidingInput, navSatFixWithoutFixIsRejected)
{
  sensor_msgs::msg::NavSatFix   nav_sat_fix_message = createNavSatFixMessage();
  SbgEComLogPosition            log;

  nav_sat_fix_message.status.status = sensor_msgs::msg::NavSatStatus::STATUS_NO_FIX;

  EXPECT_FALSE(sbg::aiding::convertNavSatFix(nav_sat_fix_message, 0, log));
}

TEST(AidingInput, navSatFixWithUnknownCovarianceIsRejected)
{
  sensor_msgs::msg::NavSatFix   nav_sat_fix_message = createNavSatFixMessage();
  SbgEComLogPosition            log;

  nav_sat_fix_message.position_covariance_type = sensor_msgs::msg::NavSatFix::COVARIANCE_TYPE_UNKNOWN;

  EXPECT_FALSE(sbg::aiding::convertNavSatFix(nav_sat_fix_message, 0, log));
}

//---------------------------------------------------------------------//
//- Generic velocity                                                  -//
//---------------------------------------------------------------------//

TEST(AidingInput, nedVelocityIsPassedThrough)
{
  SbgEComLogVelocity  log;

  sbg::aiding::convertTwistWithCovariance(createTwistMessage(1.0, 2.0, 3.0, 0.0, 0.0, 0.0), VelocityFrame::NED, 2500, log);

  EXPECT_EQ(log.timeStamp, 2500u);

  EXPECT_FLOAT_EQ(log.velocity[0], 1.0f);
  EXPECT_FLOAT_EQ(log.velocity[1], 2.0f);
  EXPECT_FLOAT_EQ(log.velocity[2], 3.0f);

  EXPECT_TRUE(log.status & SBG_ECOM_VELOCITY_0_VALID);
  EXPECT_TRUE(log.status & SBG_ECOM_VELOCITY_1_VALID);
  EXPECT_TRUE(log.status & SBG_ECOM_VELOCITY_2_VALID);

  EXPECT_EQ(sbgEComLogVelocityGetTimeType(&log), SBG_ECOM_AIDING_TIME_TYPE_DELAY);
}

TEST(AidingInput, frdVelocityIsPassedThrough)
{
  SbgEComLogVelocity  log;

  sbg::aiding::convertTwistWithCovariance(createTwistMessage(1.0, 2.0, 3.0, 0.0, 0.0, 0.0), VelocityFrame::FRD, 0, log);

  EXPECT_FLOAT_EQ(log.velocity[0], 1.0f);
  EXPECT_FLOAT_EQ(log.velocity[1], 2.0f);
  EXPECT_FLOAT_EQ(log.velocity[2], 3.0f);
}

TEST(AidingInput, enuVelocityIsConvertedToNed)
{
  SbgEComLogVelocity  log;

  sbg::aiding::convertTwistWithCovariance(createTwistMessage(1.0, 2.0, 3.0, 0.0, 0.0, 0.0), VelocityFrame::ENU, 0, log);

  EXPECT_FLOAT_EQ(log.velocity[0], 2.0f);
  EXPECT_FLOAT_EQ(log.velocity[1], 1.0f);
  EXPECT_FLOAT_EQ(log.velocity[2], -3.0f);
}

TEST(AidingInput, fluVelocityIsConvertedToFrd)
{
  SbgEComLogVelocity  log;

  sbg::aiding::convertTwistWithCovariance(createTwistMessage(1.0, 2.0, 3.0, 0.0, 0.0, 0.0), VelocityFrame::FLU, 0, log);

  EXPECT_FLOAT_EQ(log.velocity[0], 1.0f);
  EXPECT_FLOAT_EQ(log.velocity[1], -2.0f);
  EXPECT_FLOAT_EQ(log.velocity[2], -3.0f);
}

TEST(AidingInput, velocityStandardDeviationFollowsTheFrameConversion)
{
  SbgEComLogVelocity  log;

  sbg::aiding::convertTwistWithCovariance(createTwistMessage(0.0, 0.0, 0.0, 4.0, 9.0, 16.0), VelocityFrame::ENU, 0, log);

  EXPECT_TRUE(log.status & SBG_ECOM_VELOCITY_STD_VALID);

  EXPECT_FLOAT_EQ(log.velocityStd[0], 3.0f);
  EXPECT_FLOAT_EQ(log.velocityStd[1], 2.0f);
  EXPECT_FLOAT_EQ(log.velocityStd[2], 4.0f);
}

TEST(AidingInput, velocityStandardDeviationIsInvalidWithAnUnsetVariance)
{
  SbgEComLogVelocity  log;

  sbg::aiding::convertTwistWithCovariance(createTwistMessage(0.0, 0.0, 0.0, 4.0, 0.0, 16.0), VelocityFrame::NED, 0, log);

  EXPECT_FALSE(log.status & SBG_ECOM_VELOCITY_STD_VALID);

  EXPECT_FLOAT_EQ(log.velocityStd[0], 0.0f);
  EXPECT_FLOAT_EQ(log.velocityStd[1], 0.0f);
  EXPECT_FLOAT_EQ(log.velocityStd[2], 0.0f);
}

//---------------------------------------------------------------------//
//- Air data                                                          -//
//---------------------------------------------------------------------//

TEST(AidingInput, fluidPressureIsConvertedToAnAirDataLog)
{
  sensor_msgs::msg::FluidPressure   fluid_pressure_message;
  SbgEComLogAirData                 log;

  fluid_pressure_message.fluid_pressure = 101325.0;

  sbg::aiding::convertFluidPressure(fluid_pressure_message, 3500, log);

  EXPECT_EQ(log.timeStamp, 3500u);
  EXPECT_FLOAT_EQ(log.pressureAbs, 101325.0f);

  EXPECT_TRUE(log.status & SBG_ECOM_AIR_DATA_PRESSURE_ABS_VALID);
  EXPECT_FALSE(log.status & SBG_ECOM_AIR_DATA_ALTITUDE_VALID);
  EXPECT_FALSE(log.status & SBG_ECOM_AIR_DATA_AIRSPEED_VALID);

  EXPECT_TRUE(log.status & SBG_ECOM_AIR_DATA_TIME_IS_DELAY);
  EXPECT_EQ(sbgEComLogAirDataGetTimeType(&log), SBG_ECOM_AIDING_TIME_TYPE_DELAY);
}
