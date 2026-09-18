// File header
#include "imu_sample.h"

using sbg::ImuSample;

/*!
 * Standard gyroscope scale factor.
 */
#define SBG_ECOM_LOG_IMU_GYRO_SCALE_STD                     (67108864.0f)

/*!
 * High range gyroscope scale factor derived from 10000 degrees per second maximum range.
 *
 * Calculation: (2^31 - 1) / (10000 * π / 180)
 */
#define SBG_ECOM_LOG_IMU_GYRO_SCALE_HIGH                    (12304174.0f)

/*!
 * Standard accelerometer scale factor.
 */
#define SBG_ECOM_LOG_IMU_ACCEL_SCALE_STD                    (1048576.0f)

/*!
 * Standard temperature scale factor.
 */
#define SBG_ECOM_LOG_IMU_TEMP_SCALE_STD                     (256.0f)

//---------------------------------------------------------------------//
//- Constructor                                                       -//
//---------------------------------------------------------------------//

ImuSample::ImuSample(const sbg_driver::msg::SbgImuData& ref_sbg_imu_msg):
header_(ref_sbg_imu_msg.header),
angular_velocity_(ref_sbg_imu_msg.gyro),
linear_acceleration_(ref_sbg_imu_msg.accel),
temperature_(ref_sbg_imu_msg.temp)
{
}

ImuSample::ImuSample(const sbg_driver::msg::SbgImuShort& ref_sbg_imu_msg):
header_(ref_sbg_imu_msg.header),
temperature_(ref_sbg_imu_msg.temperature / SBG_ECOM_LOG_IMU_TEMP_SCALE_STD)
{
  float gyro_scale;

  if (ref_sbg_imu_msg.imu_status.imu_gyros_use_high_scale)
  {
    gyro_scale = SBG_ECOM_LOG_IMU_GYRO_SCALE_HIGH;
  }
  else
  {
    gyro_scale = SBG_ECOM_LOG_IMU_GYRO_SCALE_STD;
  }

  angular_velocity_.x     = ref_sbg_imu_msg.delta_angle.x / gyro_scale;
  angular_velocity_.y     = ref_sbg_imu_msg.delta_angle.y / gyro_scale;
  angular_velocity_.z     = ref_sbg_imu_msg.delta_angle.z / gyro_scale;

  linear_acceleration_.x  = ref_sbg_imu_msg.delta_velocity.x / SBG_ECOM_LOG_IMU_ACCEL_SCALE_STD;
  linear_acceleration_.y  = ref_sbg_imu_msg.delta_velocity.y / SBG_ECOM_LOG_IMU_ACCEL_SCALE_STD;
  linear_acceleration_.z  = ref_sbg_imu_msg.delta_velocity.z / SBG_ECOM_LOG_IMU_ACCEL_SCALE_STD;
}

//---------------------------------------------------------------------//
//- Parameters                                                        -//
//---------------------------------------------------------------------//

const std_msgs::msg::Header &ImuSample::getHeader() const
{
  return header_;
}

const geometry_msgs::msg::Vector3 &ImuSample::getAngularVelocity() const
{
  return angular_velocity_;
}

const geometry_msgs::msg::Vector3 &ImuSample::getLinearAcceleration() const
{
  return linear_acceleration_;
}

float ImuSample::getTemperature() const
{
  return temperature_;
}
