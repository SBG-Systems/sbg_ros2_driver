#include "message_publisher.h"

using sbg::MessagePublisher;

/*!
 * Class to publish all SBG-ROS messages to the corresponding publishers. 
 */
//---------------------------------------------------------------------//
//- Constructor                                                       -//
//---------------------------------------------------------------------//

MessagePublisher::MessagePublisher():
max_messages_(10)
{
}

//---------------------------------------------------------------------//
//- Private methods                                                   -//
//---------------------------------------------------------------------//

std::string MessagePublisher::getOutputTopicName(SbgEComMsgId sbg_message_id) const
{
  switch (sbg_message_id)
  {
    case SBG_ECOM_LOG_STATUS:
      return "sbg/status";

    case SBG_ECOM_LOG_UTC_TIME:
      return "sbg/utc_time";

    case SBG_ECOM_LOG_IMU_DATA:
      return "sbg/imu_data";

    case SBG_ECOM_LOG_MAG:
      return "sbg/mag";

    case SBG_ECOM_LOG_MAG_CALIB:
      return "sbg/mag_calib";

    case SBG_ECOM_LOG_EKF_EULER:
      return "sbg/ekf_euler";

    case SBG_ECOM_LOG_EKF_QUAT:
      return "sbg/ekf_quat";

    case SBG_ECOM_LOG_EKF_NAV:
      return "sbg/ekf_nav";

    case SBG_ECOM_LOG_EKF_VEL_BODY:
      return "sbg/ekf_vel_body";

    case SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY:
      return "sbg/ekf_rot_accel_body";

    case SBG_ECOM_LOG_EKF_ROT_ACCEL_NED:
      return "sbg/ekf_rot_accel_ned";

    case SBG_ECOM_LOG_SHIP_MOTION:
      return "sbg/ship_motion";

    case SBG_ECOM_LOG_GPS1_VEL:
      return "sbg/gps_vel";

    case SBG_ECOM_LOG_GPS1_POS:
      return "sbg/gps_pos";

    case SBG_ECOM_LOG_GPS1_HDT:
      return "sbg/gps_hdt";

    case SBG_ECOM_LOG_GPS1_RAW:
      return "sbg/gps_raw";

    case SBG_ECOM_LOG_ODO_VEL:
      return "sbg/odo_vel";

    case SBG_ECOM_LOG_EVENT_A:
      return "sbg/eventA";

    case SBG_ECOM_LOG_EVENT_B:
      return "sbg/eventB";

    case SBG_ECOM_LOG_EVENT_C:
      return "sbg/eventC";

    case SBG_ECOM_LOG_EVENT_D:
      return "sbg/eventD";

    case SBG_ECOM_LOG_EVENT_E:
      return "sbg/eventE";

    case SBG_ECOM_LOG_AIR_DATA:
      return "sbg/air_data";

    case SBG_ECOM_LOG_IMU_SHORT:
      return "sbg/imu_short";

    default:
      return "undefined";
  }
}

void MessagePublisher::initPublisher(rclcpp::Node& ref_ros_node_handle, SbgEComMsgId sbg_msg_id, SbgEComOutputMode output_conf, const std::string &ref_output_topic)
{
  //
  // Check if the publisher has to be initialized.
  //
  if (output_conf != SBG_ECOM_OUTPUT_MODE_DISABLED)
  {
    switch (sbg_msg_id)
    {
      case SBG_ECOM_LOG_STATUS:
        sbg_status_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgStatus>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_UTC_TIME:
        sbg_utc_time_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgUtcTime>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_IMU_DATA:
        sbg_imu_data_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgImuData>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_MAG:
        sbg_mag_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgMag>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_MAG_CALIB:
        sbg_mag_calib_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgMagCalib>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EKF_EULER:
        sbg_ekf_euler_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfEuler>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EKF_QUAT:
        sbg_ekf_quat_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfQuat>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EKF_NAV:
        sbg_ekf_nav_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfNav>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EKF_VEL_BODY:
        sbg_ekf_vel_body_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfVelBody>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY:
        sbg_ekf_rot_accel_body_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfRotAccel>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EKF_ROT_ACCEL_NED:
        sbg_ekf_rot_accel_ned_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEkfRotAccel>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_SHIP_MOTION:
        sbg_ship_motion_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgShipMotion>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_GPS1_VEL:
      case SBG_ECOM_LOG_GPS2_VEL:
        sbg_gps_vel_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgGpsVel>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_GPS1_POS:
      case SBG_ECOM_LOG_GPS2_POS:
        sbg_gps_pos_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgGpsPos>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_GPS1_HDT:
      case SBG_ECOM_LOG_GPS2_HDT:
        sbg_gps_hdt_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgGpsHdt>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_GPS1_RAW:
      case SBG_ECOM_LOG_GPS2_RAW:
        sbg_gps_raw_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgGpsRaw>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_ODO_VEL:
        sbg_odo_vel_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgOdoVel>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EVENT_A:
        sbg_event_a_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EVENT_B:
        sbg_event_b_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EVENT_C:
        sbg_event_c_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EVENT_D:
        sbg_event_d_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_EVENT_E:
        sbg_event_e_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgEvent>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_IMU_SHORT:
        sbg_imu_short_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgImuShort>(ref_output_topic, max_messages_);
        break;

      case SBG_ECOM_LOG_AIR_DATA:
        sbg_air_data_pub_ = ref_ros_node_handle.create_publisher<sbg_driver::msg::SbgAirData>(ref_output_topic, max_messages_);
        break;

      default:
        break;
    }
  }
}

void MessagePublisher::defineRosStandardPublishers(rclcpp::Node& ref_ros_node_handle, bool odom_enable, bool enu_enable)
{
  if (!enu_enable)
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] Driver is configured in NED frame convention, ROS standard message are disabled.");
    return;
  }

  if (sbg_imu_data_pub_ && sbg_ekf_quat_pub_)
  {
    imu_pub_ = ref_ros_node_handle.create_publisher<sensor_msgs::msg::Imu>("imu/data", max_messages_);
  }
  else
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] SBG Imu and/or Quat output are not configured, the standard IMU can not be defined.");
  }

  if (sbg_imu_data_pub_)
  {
    temp_pub_ = ref_ros_node_handle.create_publisher<sensor_msgs::msg::Temperature>("imu/temp", max_messages_);
  }
  else
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] SBG Imu data output are not configured, the standard Temperature publisher can not be defined.");
  }

  if (sbg_mag_pub_)
  {
    mag_pub_ = ref_ros_node_handle.create_publisher<sensor_msgs::msg::MagneticField>("imu/mag", max_messages_);
  }
  else
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] SBG Mag data output are not configured, the standard Magnetic publisher can not be defined.");
  }

  //
  // We need either Euler or quat angles, and we must have Nav and IMU data to
  // compute Body and angular velocity.
  //
  if ((sbg_ekf_euler_pub_ || sbg_ekf_quat_pub_) && sbg_ekf_nav_pub_ && sbg_imu_data_pub_)
  {
    velocity_pub_ = ref_ros_node_handle.create_publisher<geometry_msgs::msg::TwistStamped>("imu/velocity", max_messages_);
  }
  else
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] SBG Imu, Nav or Angles data outputs are not configured, the standard Velocity publisher can not be defined.");
  }

  if (sbg_air_data_pub_)
  {
    fluid_pub_ = ref_ros_node_handle.create_publisher<sensor_msgs::msg::FluidPressure>("imu/pres", max_messages_);
  }
  else
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] SBG AirData output are not configured, the standard FluidPressure publisher can not be defined.");
  }

  if (sbg_ekf_nav_pub_)
  {
    pos_ecef_pub_ = ref_ros_node_handle.create_publisher<geometry_msgs::msg::PointStamped>("imu/pos_ecef", max_messages_);
  }
  else
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] SBG Ekf data output are not configured, the standard ECEF position publisher can not be defined.");
  }

  if (sbg_utc_time_pub_)
  {
    utc_reference_pub_ = ref_ros_node_handle.create_publisher<sensor_msgs::msg::TimeReference>("imu/utc_ref", max_messages_);
  }
  else
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] SBG Utc data output are not configured, the UTC time reference publisher can not be defined.");
  }

  if (sbg_gps_pos_pub_)
  {
    nav_sat_fix_pub_ = ref_ros_node_handle.create_publisher<sensor_msgs::msg::NavSatFix>("imu/nav_sat_fix", max_messages_);
  }
  else
  {
    RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] SBG GPS Pos data output are not configured, the NavSatFix publisher can not be defined.");
  }

  if (odom_enable)
  {
    if (sbg_imu_data_pub_ && sbg_ekf_nav_pub_ && (sbg_ekf_euler_pub_ || sbg_ekf_quat_pub_))
    {
      odometry_pub_ = ref_ros_node_handle.create_publisher<nav_msgs::msg::Odometry>("imu/odometry", max_messages_);
    }
    else
    {
      RCLCPP_WARN(ref_ros_node_handle.get_logger(), "SBG_DRIVER - [Publisher] SBG IMU, NAV and Quaternion (or Euler) outputs are not configured, the odometry publisher can not be defined.");
    }
  }
}

void MessagePublisher::publishIMUData(const SbgEComLogUnion &ref_sbg_log)
{
  if (sbg_imu_data_pub_)
  {
    sbg_imu_message_ = message_wrapper_.createSbgImuDataMessage(ref_sbg_log.imuData);
    sbg_imu_data_pub_->publish(sbg_imu_message_);
  }
  if (temp_pub_)
  {
    temp_pub_->publish(message_wrapper_.createRosTemperatureMessage(sbg_imu_message_));
  }

  processRosImuMessage();
  processRosVelMessage();
  processRosOdoMessage();
}

void MessagePublisher::processRosVelMessage()
{
  if (velocity_pub_)
  {
    if (sbg_ekf_quat_pub_)
    {
      velocity_pub_->publish(message_wrapper_.createRosTwistStampedMessage(sbg_ekf_quat_message_, sbg_ekf_nav_message_, sbg_imu_message_));
    }
    else if (sbg_ekf_euler_pub_)
    {
      velocity_pub_->publish(message_wrapper_.createRosTwistStampedMessage(sbg_ekf_euler_message_, sbg_ekf_nav_message_, sbg_imu_message_));
    }
  }
}

void MessagePublisher::processRosImuMessage()
{
  if (imu_pub_)
  {
    if (sbg_imu_message_.time_stamp == sbg_ekf_quat_message_.time_stamp)
    {
      imu_pub_->publish(message_wrapper_.createRosImuMessage(sbg_imu_message_, sbg_ekf_quat_message_));
    }
  }
}

void MessagePublisher::processRosOdoMessage()
{
  if (odometry_pub_)
  {
    if (sbg_ekf_nav_message_.status.position_valid)
    {
      if (sbg_imu_message_.time_stamp == sbg_ekf_nav_message_.time_stamp)
      {
        /*
         * Odometry message can be generated from quaternion or euler angles.
         * Quaternion is prefered if they are available.
         */
        if (sbg_ekf_quat_pub_)
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
  //
  // Initialize all the publishers with the defined SBG output from the config store.
  //
  const std::vector<ConfigStore::SbgLogOutput> &ref_output_modes = ref_config_store.getOutputModes();

  message_wrapper_.setTimeReference(ref_config_store.getTimeReference());

  message_wrapper_.setFrameId(ref_config_store.getFrameId());

  message_wrapper_.setUseEnu(ref_config_store.getUseEnu());

  message_wrapper_.setOdomEnable(ref_config_store.getOdomEnable());
  message_wrapper_.setOdomPublishTf(ref_config_store.getOdomPublishTf());
  message_wrapper_.setOdomFrameId(ref_config_store.getOdomFrameId());
  message_wrapper_.setOdomBaseFrameId(ref_config_store.getOdomBaseFrameId());
  message_wrapper_.setOdomInitFrameId(ref_config_store.getOdomInitFrameId());

  for (const ConfigStore::SbgLogOutput &ref_output : ref_output_modes)
  {
    initPublisher(ref_ros_node_handle, ref_output.message_id, ref_output.output_mode, getOutputTopicName(ref_output.message_id));
  }

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
        publishIMUData(ref_sbg_log);
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
          sbg_ekf_euler_message_ = message_wrapper_.createSbgEkfEulerMessage(ref_sbg_log.ekfEulerData);
          sbg_ekf_euler_pub_->publish(sbg_ekf_euler_message_);
          processRosVelMessage();
          processRosOdoMessage();
        }
        break;

      case SBG_ECOM_LOG_EKF_QUAT:
        if (sbg_ekf_quat_pub_)
        {
          sbg_ekf_quat_message_ = message_wrapper_.createSbgEkfQuatMessage(ref_sbg_log.ekfQuatData);
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
          sbg_ekf_rot_accel_body_pub_->publish(message_wrapper_.createSbgEkfRotAccelMessage(ref_sbg_log.ekfRotAccel));
        }
        break;

      case SBG_ECOM_LOG_EKF_ROT_ACCEL_NED:
        if (sbg_ekf_rot_accel_ned_pub_)
        {
          sbg_ekf_rot_accel_ned_pub_->publish(message_wrapper_.createSbgEkfRotAccelMessage(ref_sbg_log.ekfRotAccel));
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
          sbg_imu_short_pub_->publish(message_wrapper_.createSbgImuShortMessage(ref_sbg_log.imuShort));
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
