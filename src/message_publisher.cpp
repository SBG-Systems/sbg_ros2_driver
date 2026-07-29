#include "message_publisher.h"

using sbg::MessagePublisher;

namespace
{
bool areStampsClose(const builtin_interfaces::msg::Time &lhs, const builtin_interfaces::msg::Time &rhs, const rclcpp::Duration &tolerance = rclcpp::Duration::from_seconds(25e-4))
{
  const auto delta = rclcpp::Time(lhs) - rclcpp::Time(rhs);
  return (delta > tolerance*-1) && (delta < tolerance);
}
}

/*!
 * Class to publish all SBG-ROS messages to the corresponding publishers. 
 */
//---------------------------------------------------------------------//
//- Constructor                                                       -//
//---------------------------------------------------------------------//

MessagePublisher::MessagePublisher():
max_messages_(10),
imu_data_received_(false),
imu_short_received_(false),
ekf_euler_received_(false),
ekf_quat_received_(false),
ekf_nav_received_(false)
{
}

//---------------------------------------------------------------------//
//- Private methods                                                   -//
//---------------------------------------------------------------------//

void MessagePublisher::defineSbgPublishers(rclcpp::Node& ref_ros_node_handle)
{
  sbg_status_pub_               = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgStatus>("sbg/status", max_messages_);
  sbg_utc_time_pub_             = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgUtcTime>("sbg/utc_time", max_messages_);
  sbg_imu_data_pub_             = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgImuData>("sbg/imu_data", max_messages_);
  sbg_mag_pub_                  = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgMag>("sbg/mag", max_messages_);
  sbg_mag_calib_pub_            = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgMagCalib>("sbg/mag_calib", max_messages_);
  sbg_ekf_euler_pub_            = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfEuler>("sbg/ekf_euler", max_messages_);
  sbg_ekf_quat_pub_             = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfQuat>("sbg/ekf_quat", max_messages_);
  sbg_ekf_nav_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfNav>("sbg/ekf_nav", max_messages_);
  sbg_ekf_vel_body_pub_         = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfVelBody>("sbg/ekf_vel_body", max_messages_);
  sbg_ekf_rot_accel_body_pub_   = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfRotAccel>("sbg/ekf_rot_accel_body", max_messages_);
  sbg_ekf_rot_accel_ned_pub_    = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfRotAccel>("sbg/ekf_rot_accel_ned", max_messages_);
  sbg_ship_motion_pub_          = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgShipMotion>("sbg/ship_motion", max_messages_);
  sbg_gps_vel_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgGpsVel>("sbg/gps_vel", max_messages_);
  sbg_gps_pos_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgGpsPos>("sbg/gps_pos", max_messages_);
  sbg_gps_hdt_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgGpsHdt>("sbg/gps_hdt", max_messages_);
  sbg_gps_raw_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgGpsRaw>("sbg/gps_raw", max_messages_);
  sbg_odo_vel_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgOdoVel>("sbg/odo_vel", max_messages_);
  sbg_event_a_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>("sbg/eventA", max_messages_);
  sbg_event_b_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>("sbg/eventB", max_messages_);
  sbg_event_c_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>("sbg/eventC", max_messages_);
  sbg_event_d_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>("sbg/eventD", max_messages_);
  sbg_event_e_pub_              = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>("sbg/eventE", max_messages_);
  sbg_air_data_pub_             = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgAirData>("sbg/air_data", max_messages_);
  sbg_imu_short_pub_            = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgImuShort>("sbg/imu_short", max_messages_);
}

void MessagePublisher::defineRosStandardPublishers(rclcpp::Node& ref_ros_node_handle, bool odom_enable, bool enu_enable)
{
  if (!enu_enable)
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] Driver is configured in NED frame convention, ROS standard message are disabled.");
    return;
  }

  //
  // The device outputs are not configured by the driver anymore, so which SBG logs are
  // available is unknown until they are received. All the publishers are advertised, and
  // each message is published once the SBG logs it is built from have been received.
  //
  imu_pub_            = ref_ros_node_handle.create_publisher<sensor_msgs::msg::Imu>("imu/data", max_messages_);
  temp_pub_           = ref_ros_node_handle.create_publisher<sensor_msgs::msg::Temperature>("imu/temp", max_messages_);
  mag_pub_            = ref_ros_node_handle.create_publisher<sensor_msgs::msg::MagneticField>("imu/mag", max_messages_);
  velocity_pub_       = ref_ros_node_handle.create_publisher<geometry_msgs::msg::TwistStamped>("imu/velocity", max_messages_);
  fluid_pub_          = ref_ros_node_handle.create_publisher<sensor_msgs::msg::FluidPressure>("imu/pres", max_messages_);
  pos_ecef_pub_       = ref_ros_node_handle.create_publisher<geometry_msgs::msg::PointStamped>("imu/pos_ecef", max_messages_);
  utc_reference_pub_  = ref_ros_node_handle.create_publisher<sensor_msgs::msg::TimeReference>("imu/utc_ref", max_messages_);
  nav_sat_fix_pub_    = ref_ros_node_handle.create_publisher<sensor_msgs::msg::NavSatFix>("imu/nav_sat_fix", max_messages_);

  if (odom_enable)
  {
    odometry_pub_ = ref_ros_node_handle.create_publisher<nav_msgs::msg::Odometry>("imu/odometry", max_messages_);
  }
}

void MessagePublisher::processImuMessage()
{
  if (temp_pub_)
  {
    if (imu_short_received_)
    {
      temp_pub_->publish(message_wrapper_.createRosTemperatureMessage(sbg_imu_short_message_));
    }
    else if (imu_data_received_)
    {
      temp_pub_->publish(message_wrapper_.createRosTemperatureMessage(sbg_imu_message_));
    }
  }

  processRosImuMessage();
  processRosVelMessage();
  processRosOdoMessage();
}

void MessagePublisher::processRosVelMessage()
{
  //
  // Either Euler or quaternion angles are needed, as well as Nav and IMU data, to compute
  // the body and angular velocity.
  //
  if (velocity_pub_ && ekf_nav_received_)
  {
    if (imu_short_received_)
    {
      if (ekf_quat_received_)
      {
        velocity_pub_->publish(message_wrapper_.createRosTwistStampedMessage(sbg_ekf_quat_message_, sbg_ekf_nav_message_, sbg_imu_short_message_));
      }
      else if (ekf_euler_received_)
      {
        velocity_pub_->publish(message_wrapper_.createRosTwistStampedMessage(sbg_ekf_euler_message_, sbg_ekf_nav_message_, sbg_imu_short_message_));
      }
    }
    else if (imu_data_received_)
    {
      if (ekf_quat_received_)
      {
        velocity_pub_->publish(message_wrapper_.createRosTwistStampedMessage(sbg_ekf_quat_message_, sbg_ekf_nav_message_, sbg_imu_message_));
      }
      else if (ekf_euler_received_)
      {
        velocity_pub_->publish(message_wrapper_.createRosTwistStampedMessage(sbg_ekf_euler_message_, sbg_ekf_nav_message_, sbg_imu_message_));
      }
    }
  }
}

void MessagePublisher::processRosImuMessage()
{
  if (imu_pub_)
  {
    sbg_driver::msg::SbgEkfQuat ekf_quat_message_zero;

    if (imu_short_received_)
    {
      if ((sbg_ekf_quat_message_ == ekf_quat_message_zero) || areStampsClose(sbg_imu_short_message_.header.stamp, sbg_ekf_quat_message_.header.stamp))
      {
        imu_pub_->publish(message_wrapper_.createRosImuMessage(sbg_imu_short_message_, sbg_ekf_quat_message_));
      }
    }
    else if (imu_data_received_)
    {
      if ((sbg_ekf_quat_message_ == ekf_quat_message_zero) || (sbg_imu_message_.time_stamp == sbg_ekf_quat_message_.time_stamp))
      {
        imu_pub_->publish(message_wrapper_.createRosImuMessage(sbg_imu_message_, sbg_ekf_quat_message_));
      }
    }
  }
}

void MessagePublisher::processRosOdoMessage()
{
  if (odometry_pub_)
  {
    if (sbg_ekf_nav_message_.status.solution_mode == SBG_ECOM_SOL_MODE_NAV_POSITION)
    {
      if (imu_short_received_)
      {
        if (areStampsClose(sbg_imu_short_message_.header.stamp, sbg_ekf_nav_message_.header.stamp))
        {
          /*
          * Odometry message can be generated from quaternion or euler angles.
          * Quaternion is prefered if they are available.
          */
          if (ekf_quat_received_)
          {
            if (areStampsClose(sbg_imu_short_message_.header.stamp, sbg_ekf_quat_message_.header.stamp))
            {
              odometry_pub_->publish(message_wrapper_.createRosOdoMessage(sbg_imu_short_message_, sbg_ekf_nav_message_, sbg_ekf_quat_message_, sbg_ekf_euler_message_));
            }
          }
          else
          {
            if (areStampsClose(sbg_imu_short_message_.header.stamp, sbg_ekf_euler_message_.header.stamp))
            {
              odometry_pub_->publish(message_wrapper_.createRosOdoMessage(sbg_imu_short_message_, sbg_ekf_nav_message_, sbg_ekf_euler_message_));
            }
          }
        }
      }
      else if (imu_data_received_)
      {
        if (sbg_imu_message_.time_stamp == sbg_ekf_nav_message_.time_stamp)
        {
          /*
          * Odometry message can be generated from quaternion or euler angles.
          * Quaternion is prefered if they are available.
          */
          if (ekf_quat_received_)
          {
            if (sbg_imu_message_.time_stamp == sbg_ekf_quat_message_.time_stamp)
            {
              odometry_pub_->publish(message_wrapper_.createRosOdoMessage(sbg_imu_message_, sbg_ekf_nav_message_, sbg_ekf_quat_message_, sbg_ekf_euler_message_));
            }
          }
          else
          {
            if (sbg_imu_message_.time_stamp == sbg_ekf_euler_message_.time_stamp)
            {
              odometry_pub_->publish(message_wrapper_.createRosOdoMessage(sbg_imu_message_, sbg_ekf_nav_message_, sbg_ekf_euler_message_));
            }
          }
        }
      }
    }
  }
}

void MessagePublisher::publishMagData(const SbgEComLogUnion &ref_sbg_log)
{
  sbg_driver::msg::SbgMag sbg_mag_message;
  sbg_mag_message = message_wrapper_.createSbgMagMessage(ref_sbg_log.magData);

  if (sbg_mag_pub_)
  {
    sbg_mag_pub_->publish(sbg_mag_message);
  }
  if (mag_pub_)
  {
    mag_pub_->publish(message_wrapper_.createRosMagneticMessage(sbg_mag_message));
  }
}

void MessagePublisher::publishFluidPressureData(const SbgEComLogUnion &ref_sbg_log)
{
  sbg_driver::msg::SbgAirData sbg_air_data_message;
  sbg_air_data_message = message_wrapper_.createSbgAirDataMessage(ref_sbg_log.airData);

  if (sbg_air_data_pub_)
  {
    sbg_air_data_pub_->publish(sbg_air_data_message);
  }
  if (fluid_pub_)
  {
    fluid_pub_->publish(message_wrapper_.createRosFluidPressureMessage(sbg_air_data_message));
  }
}

void MessagePublisher::publishEkfNavigationData(const SbgEComLogUnion &ref_sbg_log)
{
  sbg_ekf_nav_message_ = message_wrapper_.createSbgEkfNavMessage(ref_sbg_log.ekfNavData);
  ekf_nav_received_    = true;

  if (sbg_ekf_nav_pub_)
  {
    sbg_ekf_nav_pub_->publish(sbg_ekf_nav_message_);
  }
  if (pos_ecef_pub_)
  {
    pos_ecef_pub_->publish(message_wrapper_.createRosPointStampedMessage(sbg_ekf_nav_message_));
  }
  processRosVelMessage();
}

void MessagePublisher::publishUtcData(const SbgEComLogUnion &ref_sbg_log)
{
  sbg_driver::msg::SbgUtcTime sbg_utc_message;

  sbg_utc_message = message_wrapper_.createSbgUtcTimeMessage(ref_sbg_log.utcData);

  if (sbg_utc_time_pub_)
  {
    sbg_utc_time_pub_->publish(sbg_utc_message);
  }
  if (utc_reference_pub_)
  {
    if (sbg_utc_message.clock_status.clock_utc_status != SBG_ECOM_UTC_STATUS_INVALID)
    {
      utc_reference_pub_->publish(message_wrapper_.createRosUtcTimeReferenceMessage(sbg_utc_message));
    }
  }
}

void MessagePublisher::publishGpsPosData(const SbgEComLogUnion &ref_sbg_log, SbgEComMsgId sbg_msg_id)
{
  sbg_driver::msg::SbgGpsPos sbg_gps_pos_message;

  sbg_gps_pos_message = message_wrapper_.createSbgGpsPosMessage(ref_sbg_log.gpsPosData);

  if (sbg_gps_pos_pub_)
  {
    sbg_gps_pos_pub_->publish(sbg_gps_pos_message);
  }
  if (nav_sat_fix_pub_)
  {
    nav_sat_fix_pub_->publish(message_wrapper_.createRosNavSatFixMessage(sbg_gps_pos_message));
  }
  if (nmea_gga_pub_ && sbg_msg_id == SBG_ECOM_LOG_GPS1_POS)
  {
    const nmea_msgs::msg::Sentence  nmea_gga_msg = message_wrapper_.createNmeaGGAMessageForNtrip(ref_sbg_log.gpsPosData);

    // Only publish if a valid NMEA GGA message has been generated
    if (nmea_gga_msg.sentence.size() > 0)
    {
      nmea_gga_pub_->publish(nmea_gga_msg);
    }
  }
}

//---------------------------------------------------------------------//
//- Operations                                                        -//
//---------------------------------------------------------------------//

void MessagePublisher::initPublishers(rclcpp::Node& ref_ros_node_handle, const ConfigStore &ref_config_store)
{
  message_wrapper_.setTimeReference(ref_config_store.getTimeReference());

  message_wrapper_.setFrameId(ref_config_store.getFrameId());
  message_wrapper_.setGpsFrameId(ref_config_store.getGpsFrameId());

  message_wrapper_.setUseEnu(ref_config_store.getUseEnu());

  message_wrapper_.setOdomEnable(ref_config_store.getOdomEnable());
  message_wrapper_.setOdomPublishTf(ref_config_store.getOdomPublishTf());
  message_wrapper_.setOdomFrameId(ref_config_store.getOdomFrameId());
  message_wrapper_.setOdomBaseFrameId(ref_config_store.getOdomBaseFrameId());
  message_wrapper_.setOdomInitFrameId(ref_config_store.getOdomInitFrameId());

  defineSbgPublishers(ref_ros_node_handle);

  if (ref_config_store.shouldPublishNmea())
  {
    nmea_gga_pub_ = ref_ros_node_handle.create_publisher<nmea_msgs::msg::Sentence>(ref_config_store.getNmeaFullTopic(), max_messages_);
  }

  if (ref_config_store.checkRosStandardMessages())
  {
    defineRosStandardPublishers(ref_ros_node_handle, ref_config_store.getOdomEnable(), ref_config_store.getUseEnu());
  }
}

void MessagePublisher::publish(SbgEComClass sbg_msg_class, SbgEComMsgId sbg_msg_id, const SbgEComLogUnion &ref_sbg_log)
{
  //
  // Publish the message with the corresponding publisher and SBG message ID.
  // For each log, check if the publisher has been initialized.
  //
  if (sbg_msg_class == SBG_ECOM_CLASS_LOG_ECOM_0)
  {
    switch (sbg_msg_id)
    {
      case SBG_ECOM_LOG_STATUS:

        if (sbg_status_pub_)
        {
          sbg_status_pub_->publish(message_wrapper_.createSbgStatusMessage(ref_sbg_log.statusData));
        }
        break;

      case SBG_ECOM_LOG_UTC_TIME:
        publishUtcData(ref_sbg_log);
        break;

      case SBG_ECOM_LOG_IMU_DATA:
        if (sbg_imu_data_pub_)
        {
          sbg_imu_message_    = message_wrapper_.createSbgImuDataMessage(ref_sbg_log.imuData);
          imu_data_received_  = true;
          sbg_imu_data_pub_->publish(sbg_imu_message_);
          processImuMessage();
        }
        break;

      case SBG_ECOM_LOG_MAG:
        publishMagData(ref_sbg_log);
        break;

      case SBG_ECOM_LOG_MAG_CALIB:
        if (sbg_mag_calib_pub_)
        {
          sbg_mag_calib_pub_->publish(message_wrapper_.createSbgMagCalibMessage(ref_sbg_log.magCalibData));
        }
        break;

      case SBG_ECOM_LOG_EKF_EULER:
        if (sbg_ekf_euler_pub_)
        {
          sbg_ekf_euler_message_  = message_wrapper_.createSbgEkfEulerMessage(ref_sbg_log.ekfEulerData);
          ekf_euler_received_     = true;
          sbg_ekf_euler_pub_->publish(sbg_ekf_euler_message_);
          processRosVelMessage();
          processRosOdoMessage();
        }
        break;

      case SBG_ECOM_LOG_EKF_QUAT:
        if (sbg_ekf_quat_pub_)
        {
          sbg_ekf_quat_message_ = message_wrapper_.createSbgEkfQuatMessage(ref_sbg_log.ekfQuatData);
          ekf_quat_received_    = true;
          sbg_ekf_quat_pub_->publish(sbg_ekf_quat_message_);
          processRosImuMessage();
          processRosVelMessage();
        }
        break;

      case SBG_ECOM_LOG_EKF_NAV:
        publishEkfNavigationData(ref_sbg_log);
        processRosOdoMessage();
        break;

      case SBG_ECOM_LOG_EKF_VEL_BODY:
        if (sbg_ekf_vel_body_pub_)
        {
          sbg_ekf_vel_body_pub_->publish(message_wrapper_.createSbgEkfVelBodyMessage(ref_sbg_log.ekfVelBody));
        }
        break;

      case SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY:
        if (sbg_ekf_rot_accel_body_pub_)
        {
          sbg_ekf_rot_accel_body_pub_->publish(message_wrapper_.createSbgEkfRotAccelMessage(ref_sbg_log.ekfRotAccel, true));
        }
        break;

      case SBG_ECOM_LOG_EKF_ROT_ACCEL_NED:
        if (sbg_ekf_rot_accel_ned_pub_)
        {
          sbg_ekf_rot_accel_ned_pub_->publish(message_wrapper_.createSbgEkfRotAccelMessage(ref_sbg_log.ekfRotAccel, false));
        }
        break;

      case SBG_ECOM_LOG_SHIP_MOTION:
        if (sbg_ship_motion_pub_)
        {
          sbg_ship_motion_pub_->publish(message_wrapper_.createSbgShipMotionMessage(ref_sbg_log.shipMotionData));
        }
        break;

      case SBG_ECOM_LOG_GPS1_VEL:
      case SBG_ECOM_LOG_GPS2_VEL:
        if (sbg_gps_vel_pub_)
        {
          sbg_gps_vel_pub_->publish(message_wrapper_.createSbgGpsVelMessage(ref_sbg_log.gpsVelData));
        }
        break;

      case SBG_ECOM_LOG_GPS1_POS:
      case SBG_ECOM_LOG_GPS2_POS:
        publishGpsPosData(ref_sbg_log, sbg_msg_id);
        break;

      case SBG_ECOM_LOG_GPS1_HDT:
      case SBG_ECOM_LOG_GPS2_HDT:
        if (sbg_gps_hdt_pub_)
        {
          sbg_gps_hdt_pub_->publish(message_wrapper_.createSbgGpsHdtMessage(ref_sbg_log.gpsHdtData));
        }
        break;

      case SBG_ECOM_LOG_GPS1_RAW:
      case SBG_ECOM_LOG_GPS2_RAW:
        if (sbg_gps_raw_pub_)
        {
          sbg_gps_raw_pub_->publish(message_wrapper_.createSbgGpsRawMessage(ref_sbg_log.gpsRawData));
        }
        break;

      case SBG_ECOM_LOG_ODO_VEL:
        if (sbg_odo_vel_pub_)
        {
          sbg_odo_vel_pub_->publish(message_wrapper_.createSbgOdoVelMessage(ref_sbg_log.odometerData));
        }
        break;

      case SBG_ECOM_LOG_EVENT_A:
        if (sbg_event_a_pub_)
        {
          sbg_event_a_pub_->publish(message_wrapper_.createSbgEventMessage(ref_sbg_log.eventMarker));
        }
        break;

      case SBG_ECOM_LOG_EVENT_B:
        if (sbg_event_b_pub_)
        {
          sbg_event_b_pub_->publish(message_wrapper_.createSbgEventMessage(ref_sbg_log.eventMarker));
        }
        break;

      case SBG_ECOM_LOG_EVENT_C:
        if (sbg_event_c_pub_)
        {
          sbg_event_c_pub_->publish(message_wrapper_.createSbgEventMessage(ref_sbg_log.eventMarker));
        }
        break;

      case SBG_ECOM_LOG_EVENT_D:
        if (sbg_event_d_pub_)
        {
          sbg_event_d_pub_->publish(message_wrapper_.createSbgEventMessage(ref_sbg_log.eventMarker));
        }
        break;

      case SBG_ECOM_LOG_EVENT_E:
        if (sbg_event_e_pub_)
        {
          sbg_event_e_pub_->publish(message_wrapper_.createSbgEventMessage(ref_sbg_log.eventMarker));
        }
        break;

      case SBG_ECOM_LOG_IMU_SHORT:
        if (sbg_imu_short_pub_)
        {
          sbg_imu_short_message_  = message_wrapper_.createSbgImuShortMessage(ref_sbg_log.imuShort);
          imu_short_received_     = true;
          sbg_imu_short_pub_->publish(sbg_imu_short_message_);
          processImuMessage();
        }
        break;

      case SBG_ECOM_LOG_AIR_DATA:
        publishFluidPressureData(ref_sbg_log);
        break;

      default:
        break;
    }
  }
  else if (sbg_msg_class == SBG_ECOM_CLASS_LOG_ECOM_1)
  {
    switch (sbg_msg_id)
    {
      default:
        break;
    }
  }
}

uint32_t MessagePublisher::getTimestamp(SbgEComClass sbg_msg_class, SbgEComMsgId sbg_msg_id, const SbgEComLogUnion &ref_sbg_log)
{
  uint32_t timestamp;

  if (sbg_msg_class == SBG_ECOM_CLASS_LOG_ECOM_0)
  {
    switch (sbg_msg_id)
    {
    case SBG_ECOM_LOG_STATUS:
      timestamp = ref_sbg_log.statusData.timeStamp;
      break;

    case SBG_ECOM_LOG_UTC_TIME:
      timestamp = ref_sbg_log.utcData.timeStamp;
      break;

    case SBG_ECOM_LOG_IMU_DATA:
      timestamp = ref_sbg_log.imuData.timeStamp;
      break;

    case SBG_ECOM_LOG_MAG:
      timestamp = ref_sbg_log.magData.timeStamp;
      break;

    case SBG_ECOM_LOG_MAG_CALIB:
      timestamp = ref_sbg_log.magCalibData.timeStamp;
      break;

    case SBG_ECOM_LOG_EKF_EULER:
      timestamp = ref_sbg_log.ekfEulerData.timeStamp;
      break;

    case SBG_ECOM_LOG_EKF_QUAT:
      timestamp = ref_sbg_log.ekfQuatData.timeStamp;
      break;

    case SBG_ECOM_LOG_EKF_NAV:
      timestamp = ref_sbg_log.ekfNavData.timeStamp;
      break;

    case SBG_ECOM_LOG_SHIP_MOTION:
    case SBG_ECOM_LOG_SHIP_MOTION_HP:
      timestamp = ref_sbg_log.shipMotionData.timeStamp;
      break;

    case SBG_ECOM_LOG_GPS1_VEL:
    case SBG_ECOM_LOG_GPS2_VEL:
      timestamp = ref_sbg_log.gpsVelData.timeStamp;
      break;

    case SBG_ECOM_LOG_GPS1_POS:
    case SBG_ECOM_LOG_GPS2_POS:
      timestamp = ref_sbg_log.gpsPosData.timeStamp;
      break;

    case SBG_ECOM_LOG_GPS1_HDT:
    case SBG_ECOM_LOG_GPS2_HDT:
      timestamp = ref_sbg_log.gpsHdtData.timeStamp;
      break;

    case SBG_ECOM_LOG_ODO_VEL:
      timestamp = ref_sbg_log.odometerData.timeStamp;
      break;

    case SBG_ECOM_LOG_EVENT_A:
    case SBG_ECOM_LOG_EVENT_B:
    case SBG_ECOM_LOG_EVENT_C:
    case SBG_ECOM_LOG_EVENT_D:
    case SBG_ECOM_LOG_EVENT_E:
    case SBG_ECOM_LOG_EVENT_OUT_A:
    case SBG_ECOM_LOG_EVENT_OUT_B:
      timestamp = ref_sbg_log.eventMarker.timeStamp;
      break;

    case SBG_ECOM_LOG_DVL_BOTTOM_TRACK:
    case SBG_ECOM_LOG_DVL_WATER_TRACK:
      timestamp = ref_sbg_log.dvlData.timeStamp;
      break;

    case SBG_ECOM_LOG_GPS1_RAW:
    case SBG_ECOM_LOG_GPS2_RAW:
      timestamp = 0;
      break;

    case SBG_ECOM_LOG_AIR_DATA:
      timestamp = ref_sbg_log.airData.timeStamp;
      break;

    case SBG_ECOM_LOG_USBL:
      timestamp = ref_sbg_log.usblData.timeStamp;
      break;

    case SBG_ECOM_LOG_IMU_SHORT:
      timestamp = ref_sbg_log.imuShort.timeStamp;
      break;

    case SBG_ECOM_LOG_DEPTH:
      timestamp = ref_sbg_log.depthData.timeStamp;
      break;

    case SBG_ECOM_LOG_DIAG:
      timestamp = ref_sbg_log.diagData.timestamp;
      break;

    case SBG_ECOM_LOG_RTCM_RAW:
      timestamp = 0;
      break;

    case SBG_ECOM_LOG_GPS1_SAT:
    case SBG_ECOM_LOG_GPS2_SAT:
      timestamp = ref_sbg_log.satGroupData.timeStamp;
      break;

    case SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY:
    case SBG_ECOM_LOG_EKF_ROT_ACCEL_NED:
      timestamp = ref_sbg_log.ekfRotAccel.timeStamp;
      break;

    case SBG_ECOM_LOG_EKF_VEL_BODY:
      timestamp = ref_sbg_log.ekfVelBody.timeStamp;
      break;
    }
  }
  else
  {
    timestamp = 0;
  }

  return timestamp;
}
