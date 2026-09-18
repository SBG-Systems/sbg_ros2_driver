// File header
#include <imu_sample.h>

// Test framework
#include <gtest/gtest.h>

// STL headers
#include <cmath>

namespace
{
  //
  // The scale factors are derived from their documented definitions rather than copied from the
  // driver macros, so a change to a macro cannot silently change what the tests expect.
  //
  // SbgEComLogImuShort: delta velocity in 1048576 LSB per m.s^-2, delta angle in 67108864 LSB
  // per rad.s^-1 at standard scale, 12304174 LSB per rad.s^-1 at high scale, temperature in
  // 256 LSB per degC.
  //
  const double ACCEL_SCALE      = pow(2.0, 20);
  const double GYRO_SCALE_STD   = pow(2.0, 26);
  const double GYRO_SCALE_HIGH  = floor((pow(2.0, 31) - 1.0) / (10000.0 * M_PI / 180.0));
  const double TEMP_SCALE       = pow(2.0, 8);

  /*!
   * Create a short IMU message holding the given raw counts.
   */
  sbg_driver::msg::SbgImuShort createImuShortMessage(bool use_high_scale)
  {
    sbg_driver::msg::SbgImuShort imu_message;

    imu_message.header.frame_id = "imu_link";
    imu_message.time_stamp      = 1000;

    imu_message.imu_status.imu_gyros_use_high_scale = use_high_scale;

    imu_message.temperature     = static_cast<int16_t>(25.0 * TEMP_SCALE);

    imu_message.delta_velocity.x = 1.0 * ACCEL_SCALE;
    imu_message.delta_velocity.y = -2.0 * ACCEL_SCALE;
    imu_message.delta_velocity.z = 3.0 * ACCEL_SCALE;

    return imu_message;
  }
}

//---------------------------------------------------------------------//
//- IMU data source                                                   -//
//---------------------------------------------------------------------//

TEST(ImuSample, imuDataMeasurementsArePassedThrough)
{
  sbg_driver::msg::SbgImuData imu_message;

  imu_message.header.frame_id = "imu_link";
  imu_message.time_stamp      = 1000;

  imu_message.gyro.x          = 0.25;
  imu_message.gyro.y          = -0.5;
  imu_message.gyro.z          = 0.75;

  imu_message.accel.x         = 1.0;
  imu_message.accel.y         = -2.0;
  imu_message.accel.z         = 3.0;

  //
  // SbgEComLogImuLegacy reports the temperature in degC, so it must not be scaled.
  //
  imu_message.temp            = 25.0f;

  const sbg::ImuSample imu_sample(imu_message);

  EXPECT_EQ(imu_sample.getHeader().frame_id, "imu_link");

  EXPECT_DOUBLE_EQ(imu_sample.getAngularVelocity().x, 0.25);
  EXPECT_DOUBLE_EQ(imu_sample.getAngularVelocity().y, -0.5);
  EXPECT_DOUBLE_EQ(imu_sample.getAngularVelocity().z, 0.75);

  EXPECT_DOUBLE_EQ(imu_sample.getLinearAcceleration().x, 1.0);
  EXPECT_DOUBLE_EQ(imu_sample.getLinearAcceleration().y, -2.0);
  EXPECT_DOUBLE_EQ(imu_sample.getLinearAcceleration().z, 3.0);

  EXPECT_FLOAT_EQ(imu_sample.getTemperature(), 25.0f);
}

//---------------------------------------------------------------------//
//- IMU short source                                                  -//
//---------------------------------------------------------------------//

TEST(ImuSample, imuShortUsesTheStandardGyroScale)
{
  auto imu_message = createImuShortMessage(false);

  imu_message.delta_angle.x = 0.03125 * GYRO_SCALE_STD;
  imu_message.delta_angle.y = -0.0625 * GYRO_SCALE_STD;
  imu_message.delta_angle.z = 0.125 * GYRO_SCALE_STD;

  const sbg::ImuSample imu_sample(imu_message);

  EXPECT_DOUBLE_EQ(imu_sample.getAngularVelocity().x, 0.03125);
  EXPECT_DOUBLE_EQ(imu_sample.getAngularVelocity().y, -0.0625);
  EXPECT_DOUBLE_EQ(imu_sample.getAngularVelocity().z, 0.125);
}

TEST(ImuSample, imuShortUsesTheHighGyroScale)
{
  auto imu_message = createImuShortMessage(true);

  imu_message.delta_angle.x = 1.0 * GYRO_SCALE_HIGH;
  imu_message.delta_angle.y = -0.5 * GYRO_SCALE_HIGH;
  imu_message.delta_angle.z = 2.0 * GYRO_SCALE_HIGH;

  const sbg::ImuSample imu_sample(imu_message);

  EXPECT_DOUBLE_EQ(imu_sample.getAngularVelocity().x, 1.0);
  EXPECT_DOUBLE_EQ(imu_sample.getAngularVelocity().y, -0.5);
  EXPECT_DOUBLE_EQ(imu_sample.getAngularVelocity().z, 2.0);
}

TEST(ImuSample, imuShortScalesAccelerationAndTemperature)
{
  const sbg::ImuSample imu_sample(createImuShortMessage(false));

  EXPECT_EQ(imu_sample.getHeader().frame_id, "imu_link");

  EXPECT_DOUBLE_EQ(imu_sample.getLinearAcceleration().x, 1.0);
  EXPECT_DOUBLE_EQ(imu_sample.getLinearAcceleration().y, -2.0);
  EXPECT_DOUBLE_EQ(imu_sample.getLinearAcceleration().z, 3.0);

  EXPECT_FLOAT_EQ(imu_sample.getTemperature(), 25.0f);
}

//---------------------------------------------------------------------//
//- Cross checks                                                      -//
//---------------------------------------------------------------------//

TEST(ImuSample, bothSourcesAgreeOnEquivalentMeasurements)
{
  auto imu_short_message = createImuShortMessage(false);

  imu_short_message.delta_angle.x = 0.03125 * GYRO_SCALE_STD;
  imu_short_message.delta_angle.y = -0.0625 * GYRO_SCALE_STD;
  imu_short_message.delta_angle.z = 0.125 * GYRO_SCALE_STD;

  sbg_driver::msg::SbgImuData imu_data_message;

  imu_data_message.header.frame_id  = imu_short_message.header.frame_id;
  imu_data_message.time_stamp       = imu_short_message.time_stamp;

  imu_data_message.gyro.x           = 0.03125;
  imu_data_message.gyro.y           = -0.0625;
  imu_data_message.gyro.z           = 0.125;

  imu_data_message.accel.x          = 1.0;
  imu_data_message.accel.y          = -2.0;
  imu_data_message.accel.z          = 3.0;

  imu_data_message.temp             = 25.0f;

  const sbg::ImuSample short_sample(imu_short_message);
  const sbg::ImuSample data_sample(imu_data_message);

  EXPECT_EQ(short_sample.getHeader().frame_id, data_sample.getHeader().frame_id);

  EXPECT_DOUBLE_EQ(short_sample.getAngularVelocity().x, data_sample.getAngularVelocity().x);
  EXPECT_DOUBLE_EQ(short_sample.getAngularVelocity().y, data_sample.getAngularVelocity().y);
  EXPECT_DOUBLE_EQ(short_sample.getAngularVelocity().z, data_sample.getAngularVelocity().z);

  EXPECT_DOUBLE_EQ(short_sample.getLinearAcceleration().x, data_sample.getLinearAcceleration().x);
  EXPECT_DOUBLE_EQ(short_sample.getLinearAcceleration().y, data_sample.getLinearAcceleration().y);
  EXPECT_DOUBLE_EQ(short_sample.getLinearAcceleration().z, data_sample.getLinearAcceleration().z);

  EXPECT_FLOAT_EQ(short_sample.getTemperature(), data_sample.getTemperature());
}
