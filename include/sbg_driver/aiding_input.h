/*!
*  \file         aiding_input.h
*  \author       SBG Systems
*  \date         22/09/2026
*
*  \brief        Conversion and sending of the aiding measurements the driver receives from ROS.
*
*  The driver subscribes to standard ROS topics and injects the measurements into the INS as
*  sbgECom aiding logs. The conversions are free functions so they can be tested without a device.
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

#ifndef SBG_ROS_AIDING_INPUT_H
#define SBG_ROS_AIDING_INPUT_H

// SbgECom headers
#include <sbgEComLib.h>

// ROS headers
#include <rclcpp/rclcpp.hpp>
#include <builtin_interfaces/msg/time.hpp>
#include <geometry_msgs/msg/twist_with_covariance_stamped.hpp>
#include <sensor_msgs/msg/fluid_pressure.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>

// STL headers
#include <cstdint>

namespace sbg::aiding
{
  /*!
   * Frame convention of the velocity messages the driver subscribes to.
   *
   * The sbgECom velocity log carries either North/East/Down or X/Y/Z body velocities, both
   * right handed with the Z axis pointing down. The incoming ROS message is converted to that
   * handedness before being sent.
   */
  enum class VelocityFrame
  {
    NED,
    ENU,
    FRD,
    FLU,
  };

  /*!
   * Compute the measurement delay to report in an aiding log.
   *
   * \param[in] now                     Current time.
   * \param[in] stamp                   Time stamp of the received ROS message.
   * \return                            Delay in us, 0 for an unset or future time stamp.
   */
  uint32_t computeDelayUs(const rclcpp::Time &now, const builtin_interfaces::msg::Time &stamp);

  /*!
   * Convert a ROS NavSatFix message into a sbgECom generic position log.
   *
   * The message is rejected when the fix is invalid or when the covariance is unknown, as the
   * sbgECom log has no way to report an unknown accuracy.
   *
   * \param[in]  ref_msg                ROS NavSatFix message.
   * \param[in]  delay_us               Measurement delay in us.
   * \param[out] ref_log                sbgECom generic position log.
   * \return                            True if the log has been filled and should be sent.
   */
  bool convertNavSatFix(const sensor_msgs::msg::NavSatFix &ref_msg, uint32_t delay_us, SbgEComLogPosition &ref_log);

  /*!
   * Convert a ROS TwistWithCovarianceStamped message into a sbgECom generic velocity log.
   *
   * Only the linear velocities are used, the angular ones are ignored. The standard deviations
   * are reported only when the three linear variances are strictly positive.
   *
   * \param[in]  ref_msg                ROS TwistWithCovarianceStamped message.
   * \param[in]  frame                  Frame convention of the incoming message.
   * \param[in]  delay_us               Measurement delay in us.
   * \param[out] ref_log                sbgECom generic velocity log.
   */
  void convertTwistWithCovariance(const geometry_msgs::msg::TwistWithCovarianceStamped &ref_msg, VelocityFrame frame, uint32_t delay_us, SbgEComLogVelocity &ref_log);

  /*!
   * Convert a ROS FluidPressure message into a sbgECom air data log.
   *
   * Only the absolute pressure is filled, the INS derives the barometric altitude from it.
   *
   * \param[in]  ref_msg                ROS FluidPressure message.
   * \param[in]  delay_us               Measurement delay in us.
   * \param[out] ref_log                sbgECom air data log.
   */
  void convertFluidPressure(const sensor_msgs::msg::FluidPressure &ref_msg, uint32_t delay_us, SbgEComLogAirData &ref_log);

  /*!
   * Send a generic position log to the device.
   *
   * \param[in] ref_com_handle          SbgECom handle.
   * \param[in] ref_log                 sbgECom generic position log.
   * \return                            SBG_NO_ERROR if the log has been sent successfully.
   */
  SbgErrorCode sendPosition(SbgEComHandle &ref_com_handle, const SbgEComLogPosition &ref_log);

  /*!
   * Send a generic velocity log to the device.
   *
   * \param[in] ref_com_handle          SbgECom handle.
   * \param[in] ref_log                 sbgECom generic velocity log.
   * \return                            SBG_NO_ERROR if the log has been sent successfully.
   */
  SbgErrorCode sendVelocity(SbgEComHandle &ref_com_handle, const SbgEComLogVelocity &ref_log);

  /*!
   * Send an air data log to the device.
   *
   * \param[in] ref_com_handle          SbgECom handle.
   * \param[in] ref_log                 sbgECom air data log.
   * \return                            SBG_NO_ERROR if the log has been sent successfully.
   */
  SbgErrorCode sendAirData(SbgEComHandle &ref_com_handle, const SbgEComLogAirData &ref_log);
}

#endif // SBG_ROS_AIDING_INPUT_H
