/*!
*  \file         imu_sample.h
*  \author       SBG Systems
*  \date         17/09/2026
*
*  \brief        Normalized IMU measurements.
*
*  Single internal representation the ROS standard messages are built from, whatever the
*  IMU log the device outputs.
*
*  \section CodeCopyright Copyright Notice
*  MIT License
*
*  Copyright (c) 2026 SBG Systems
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
*/

#ifndef SBG_ROS_IMU_SAMPLE_H
#define SBG_ROS_IMU_SAMPLE_H

// ROS headers
#include <std_msgs/msg/header.hpp>
#include <geometry_msgs/msg/vector3.hpp>

// SbgRos message headers
#include "sbg_driver/msg/sbg_imu_data.hpp"
#include "sbg_driver/msg/sbg_imu_short.hpp"

namespace sbg
{

/*!
 * Class to hold IMU measurements normalized to SI units.
 *
 * Both SBG_ECOM_LOG_IMU_DATA and SBG_ECOM_LOG_IMU_SHORT are converted to this representation,
 * so the ROS standard messages are built from a single source whatever the device outputs.
 *
 * The frame convention has already been applied by the SBG message creators, so the axes follow
 * the configured convention, FRD or FLU.
 */
class ImuSample final
{
public:

  //---------------------------------------------------------------------//
  //- Constructor                                                       -//
  //---------------------------------------------------------------------//

  /*!
   * Default constructor.
   */
  ImuSample() = default;

  /*!
   * Constructor from a SBG-ROS IMU data message.
   *
   * The message already carries SI units, so the measurements are passed through. The angular
   * velocity and the linear acceleration are taken from the gyroscope and accelerometer fields;
   * the delta angle and delta velocity fields of SBG_ECOM_LOG_IMU_DATA hold the same values.
   *
   * \param[in] ref_sbg_imu_msg           SBG-ROS IMU data message.
   */
  explicit ImuSample(const sbg_driver::msg::SbgImuData& ref_sbg_imu_msg);

  /*!
   * Constructor from a SBG-ROS short IMU message.
   *
   * The message carries raw sensor counts, so each measurement is divided by its scale factor.
   * The gyroscope scale factor switches to the high range one when the device reports it in the
   * IMU status.
   *
   * \param[in] ref_sbg_imu_msg           SBG-ROS short IMU message.
   */
  explicit ImuSample(const sbg_driver::msg::SbgImuShort& ref_sbg_imu_msg);

  //---------------------------------------------------------------------//
  //- Parameters                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Returns the header of the SBG message the sample is built from.
   *
   * \return                              ROS header.
   */
  const std_msgs::msg::Header &getHeader() const;

  /*!
   * Returns the angular velocity.
   *
   * \return                              X, Y, Z angular velocity in rad.s^-1.
   */
  const geometry_msgs::msg::Vector3 &getAngularVelocity() const;

  /*!
   * Returns the linear acceleration.
   *
   * \return                              X, Y, Z linear acceleration in m.s^-2.
   */
  const geometry_msgs::msg::Vector3 &getLinearAcceleration() const;

  /*!
   * Returns the internal temperature.
   *
   * \return                              Temperature in degC.
   */
  float getTemperature() const;

private:

  std_msgs::msg::Header         header_;
  geometry_msgs::msg::Vector3   angular_velocity_;
  geometry_msgs::msg::Vector3   linear_acceleration_;
  float                         temperature_{};
};

}

#endif // SBG_ROS_IMU_SAMPLE_H
