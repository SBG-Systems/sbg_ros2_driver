// File header
#include "aiding_input.h"

// SBG headers
#include <sbgDefines.h>

// STL headers
#include <cmath>
#include <cstring>

using sbg::aiding::VelocityFrame;

namespace
{
  /*!
   * Mapping from the axes of an incoming velocity message to the axes of the sbgECom log.
   */
  struct AxisMapping
  {
    size_t  index[3];
    float   sign[3];
  };

  /*!
   * Get the axis mapping to apply to an incoming velocity message.
   *
   * \param[in] frame                   Frame convention of the incoming message.
   * \return                            Axis mapping to the sbgECom log axes.
   */
  AxisMapping getAxisMapping(VelocityFrame frame)
  {
    switch (frame)
    {
      case VelocityFrame::ENU:
        return AxisMapping{{1, 0, 2}, {1.0f, 1.0f, -1.0f}};

      case VelocityFrame::FLU:
        return AxisMapping{{0, 1, 2}, {1.0f, -1.0f, -1.0f}};

      case VelocityFrame::NED:
      case VelocityFrame::FRD:
      default:
        return AxisMapping{{0, 1, 2}, {1.0f, 1.0f, 1.0f}};
    }
  }

  /*!
   * Send a sbgECom log payload to the device.
   *
   * \param[in] ref_com_handle          SbgECom handle.
   * \param[in] msg_id                  Message ID of the log to send.
   * \param[in] ref_output_stream       Stream buffer holding the written log.
   * \return                            SBG_NO_ERROR if the log has been sent successfully.
   */
  SbgErrorCode sendLog(SbgEComHandle &ref_com_handle, SbgEComMsgId msg_id, SbgStreamBuffer &ref_output_stream)
  {
    return sbgEComProtocolSend(&ref_com_handle.protocolHandle, SBG_ECOM_CLASS_LOG_ECOM_0, msg_id,
                               sbgStreamBufferGetLinkedBuffer(&ref_output_stream), sbgStreamBufferGetLength(&ref_output_stream));
  }
}

uint32_t sbg::aiding::computeDelayUs(const rclcpp::Time &now, const builtin_interfaces::msg::Time &stamp)
{
  if ((stamp.sec == 0) && (stamp.nanosec == 0))
  {
    return 0;
  }

  const int64_t delay_ns = now.nanoseconds() - rclcpp::Time(stamp).nanoseconds();

  if (delay_ns <= 0)
  {
    return 0;
  }

  return static_cast<uint32_t>(delay_ns / 1000);
}

bool sbg::aiding::convertNavSatFix(const sensor_msgs::msg::NavSatFix &ref_msg, uint32_t delay_us, SbgEComLogPosition &ref_log)
{
  if (ref_msg.status.status == sensor_msgs::msg::NavSatStatus::STATUS_NO_FIX)
  {
    return false;
  }

  if (ref_msg.position_covariance_type == sensor_msgs::msg::NavSatFix::COVARIANCE_TYPE_UNKNOWN)
  {
    return false;
  }

  memset(&ref_log, 0x00, sizeof(ref_log));

  ref_log.timeStamp   = delay_us;

  ref_log.position[0] = ref_msg.latitude;
  ref_log.position[1] = ref_msg.longitude;
  ref_log.position[2] = ref_msg.altitude;

  //
  // The ROS covariance is a row major 3x3 matrix in East/North/Up order, the log expects
  // latitude/longitude/height, so North maps to latitude, East to longitude and Up to height.
  //
  ref_log.covLatLat = ref_msg.position_covariance[4];
  ref_log.covLonLon = ref_msg.position_covariance[0];
  ref_log.covHgtHgt = ref_msg.position_covariance[8];
  ref_log.covLatLon = ref_msg.position_covariance[1];
  ref_log.covLatHgt = ref_msg.position_covariance[5];
  ref_log.covLonHgt = ref_msg.position_covariance[2];

  sbgEComLogPositionSetPositionType(&ref_log, SBG_ECOM_POSITION_TYPE_VALID);
  sbgEComLogPositionSetTimeType(&ref_log, SBG_ECOM_AIDING_TIME_TYPE_DELAY);

  return true;
}

void sbg::aiding::convertTwistWithCovariance(const geometry_msgs::msg::TwistWithCovarianceStamped &ref_msg, VelocityFrame frame, uint32_t delay_us, SbgEComLogVelocity &ref_log)
{
  const AxisMapping mapping    = getAxisMapping(frame);
  const double      velocity[] = {ref_msg.twist.twist.linear.x, ref_msg.twist.twist.linear.y, ref_msg.twist.twist.linear.z};

  //
  // The ROS covariance is a row major 6x6 matrix, the linear velocity variances are on the
  // three first diagonal terms.
  //
  const double      variance[] = {ref_msg.twist.covariance[0], ref_msg.twist.covariance[7], ref_msg.twist.covariance[14]};

  sbgEComLogVelocityConstruct(&ref_log);

  ref_log.timeStamp = delay_us;

  for (size_t i = 0; i < 3; i++)
  {
    ref_log.velocity[i] = static_cast<float>(mapping.sign[i] * velocity[mapping.index[i]]);
  }

  ref_log.status |= SBG_ECOM_VELOCITY_0_VALID | SBG_ECOM_VELOCITY_1_VALID | SBG_ECOM_VELOCITY_2_VALID;

  if ((variance[0] > 0.0) && (variance[1] > 0.0) && (variance[2] > 0.0))
  {
    for (size_t i = 0; i < 3; i++)
    {
      ref_log.velocityStd[i] = static_cast<float>(sqrt(variance[mapping.index[i]]));
    }

    ref_log.status |= SBG_ECOM_VELOCITY_STD_VALID;
  }

  sbgEComLogVelocitySetTimeType(&ref_log, SBG_ECOM_AIDING_TIME_TYPE_DELAY);
}

void sbg::aiding::convertFluidPressure(const sensor_msgs::msg::FluidPressure &ref_msg, uint32_t delay_us, SbgEComLogAirData &ref_log)
{
  sbgEComLogAirDataConstruct(&ref_log);

  ref_log.timeStamp   = delay_us;
  ref_log.pressureAbs = static_cast<float>(ref_msg.fluid_pressure);
  ref_log.status     |= SBG_ECOM_AIR_DATA_PRESSURE_ABS_VALID;

  //
  // The air data log reports the time type twice, on the deprecated delay bit and on the
  // generic aiding time type field. Both are set so that the log is understood whatever the
  // device firmware version.
  //
  ref_log.status     |= SBG_ECOM_AIR_DATA_TIME_IS_DELAY;

  sbgEComLogAirDataSetTimeType(&ref_log, SBG_ECOM_AIDING_TIME_TYPE_DELAY);
}

SbgErrorCode sbg::aiding::sendPosition(SbgEComHandle &ref_com_handle, const SbgEComLogPosition &ref_log)
{
  uint8_t           output_buffer[64];
  SbgStreamBuffer   output_stream;
  SbgErrorCode      error_code;

  sbgStreamBufferInitForWrite(&output_stream, output_buffer, sizeof(output_buffer));

  error_code = sbgEComLogPositionWriteToStream(&ref_log, &output_stream);

  if (error_code == SBG_NO_ERROR)
  {
    error_code = sendLog(ref_com_handle, SBG_ECOM_LOG_POSITION_1, output_stream);
  }

  return error_code;
}

SbgErrorCode sbg::aiding::sendVelocity(SbgEComHandle &ref_com_handle, const SbgEComLogVelocity &ref_log)
{
  uint8_t           output_buffer[64];
  SbgStreamBuffer   output_stream;
  SbgErrorCode      error_code;

  sbgStreamBufferInitForWrite(&output_stream, output_buffer, sizeof(output_buffer));

  error_code = sbgEComLogVelocityWriteToStream(&ref_log, &output_stream);

  if (error_code == SBG_NO_ERROR)
  {
    error_code = sendLog(ref_com_handle, SBG_ECOM_LOG_VELOCITY_1, output_stream);
  }

  return error_code;
}

SbgErrorCode sbg::aiding::sendAirData(SbgEComHandle &ref_com_handle, const SbgEComLogAirData &ref_log)
{
  uint8_t           output_buffer[64];
  SbgStreamBuffer   output_stream;
  SbgErrorCode      error_code;

  sbgStreamBufferInitForWrite(&output_stream, output_buffer, sizeof(output_buffer));

  error_code = sbgEComLogAirDataWriteToStream(&ref_log, &output_stream);

  if (error_code == SBG_NO_ERROR)
  {
    error_code = sendLog(ref_com_handle, SBG_ECOM_LOG_AIR_DATA, output_stream);
  }

  return error_code;
}
