/*!
*  \file         message_publisher.h
*  \author       SBG Systems
*  \date         13/03/2020
*
*  \brief        Manage publishing of messages from logs.
*
*  \section CodeCopyright Copyright Notice
*  MIT License
*
*  Copyright (c) 2023 SBG Systems
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

#ifndef SBG_ROS_MESSAGE_PUBLISHER_H
#define SBG_ROS_MESSAGE_PUBLISHER_H

// Project headers
#include <config_store.h>
#include <message_wrapper.h>

namespace sbg
{
/*!
 * Class to publish all SBG-ROS messages to the corresponding publishers.
 */
class MessagePublisher
{
private:

  rclcpp::Publisher<sbg_driver::msg::SbgStatus, std::allocator<void>>::SharedPtr        sbg_status_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgUtcTime, std::allocator<void>>::SharedPtr       sbg_utc_time_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgImuData, std::allocator<void>>::SharedPtr       sbg_imu_data_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEkfEuler, std::allocator<void>>::SharedPtr      sbg_ekf_euler_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEkfQuat, std::allocator<void>>::SharedPtr       sbg_ekf_quat_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEkfNav, std::allocator<void>>::SharedPtr        sbg_ekf_nav_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEkfVelBody, std::allocator<void>>::SharedPtr    sbg_ekf_vel_body_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEkfRotAccel, std::allocator<void>>::SharedPtr   sbg_ekf_rot_accel_body_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEkfRotAccel, std::allocator<void>>::SharedPtr   sbg_ekf_rot_accel_ned_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgShipMotion, std::allocator<void>>::SharedPtr    sbg_ship_motion_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgMag, std::allocator<void>>::SharedPtr           sbg_mag_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgMagCalib, std::allocator<void>>::SharedPtr      sbg_mag_calib_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgGpsVel, std::allocator<void>>::SharedPtr        sbg_gps_vel_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgGpsPos, std::allocator<void>>::SharedPtr        sbg_gps_pos_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgGpsHdt, std::allocator<void>>::SharedPtr        sbg_gps_hdt_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgGpsRaw, std::allocator<void>>::SharedPtr        sbg_gps_raw_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgOdoVel, std::allocator<void>>::SharedPtr        sbg_odo_vel_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEvent, std::allocator<void>>::SharedPtr         sbg_event_a_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEvent, std::allocator<void>>::SharedPtr         sbg_event_b_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEvent, std::allocator<void>>::SharedPtr         sbg_event_c_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEvent, std::allocator<void>>::SharedPtr         sbg_event_d_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgEvent, std::allocator<void>>::SharedPtr         sbg_event_e_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgImuShort, std::allocator<void>>::SharedPtr      sbg_imu_short_pub_;
  rclcpp::Publisher<sbg_driver::msg::SbgAirData, std::allocator<void>>::SharedPtr       sbg_air_data_pub_;

  rclcpp::Publisher<sensor_msgs::msg::Imu, std::allocator<void>>::SharedPtr             imu_pub_;
  sbg_driver::msg::SbgImuData                                                           sbg_imu_message_;
  sbg_driver::msg::SbgEkfQuat                                                           sbg_ekf_quat_message_;
  sbg_driver::msg::SbgEkfNav                                                            sbg_ekf_nav_message_;
  sbg_driver::msg::SbgEkfEuler                                                          sbg_ekf_euler_message_;

  rclcpp::Publisher<sensor_msgs::msg::Temperature, std::allocator<void>>::SharedPtr     temp_pub_;
  rclcpp::Publisher<sensor_msgs::msg::MagneticField, std::allocator<void>>::SharedPtr   mag_pub_;
  rclcpp::Publisher<sensor_msgs::msg::FluidPressure, std::allocator<void>>::SharedPtr   fluid_pub_;
  rclcpp::Publisher<geometry_msgs::msg::PointStamped, std::allocator<void>>::SharedPtr  pos_ecef_pub_;
  rclcpp::Publisher<geometry_msgs::msg::TwistStamped, std::allocator<void>>::SharedPtr  velocity_pub_;
  rclcpp::Publisher<sensor_msgs::msg::TimeReference, std::allocator<void>>::SharedPtr   utc_reference_pub_;
  rclcpp::Publisher<sensor_msgs::msg::NavSatFix, std::allocator<void>>::SharedPtr       nav_sat_fix_pub_;
  rclcpp::Publisher<nav_msgs::msg::Odometry, std::allocator<void>>::SharedPtr           odometry_pub_;

  rclcpp::Publisher<nmea_msgs::msg::Sentence, std::allocator<void>>::SharedPtr          nmea_gga_pub_;

  MessageWrapper                                                                        message_wrapper_;
  uint32_t                                                                              max_messages_;
  std::string                                                                           frame_id_;

  //---------------------------------------------------------------------//
  //- Private methods                                                   -//
  //---------------------------------------------------------------------//

  /*!
   * Get the corresponding topic name output for the SBG output mode.
   *
   * \param[in] sbg_message_id          SBG message ID.
   * \return                            Output topic name.
   */
  std::string getOutputTopicName(SbgEComMsgId sbg_message_id) const;

  /*!
   * Initialize the publisher for the specified SBG Id, and the output configuration.
   *
   * \param[in] ref_ros_node_handle     Ros Node to advertise the publisher.
   * \param[in] sbg_msg_id              Id of the SBG message.
   * \param[in] output_conf             Output configuration.
   * \param[in] ref_output_topic        Output topic for the publisher.
   */
  void initPublisher(rclcpp::Node& ref_ros_node_handle, SbgEComMsgId sbg_msg_id, SbgEComOutputMode output_conf, const std::string &ref_output_topic);

  /*!
   * Define standard ROS publishers.
   *
   * \param[in] ref_ros_node_handle     Ros Node to advertise the publisher.
   * \param[in] odom_enable             If true, enable odometry messages.
   */
  void defineRosStandardPublishers(rclcpp::Node& ref_ros_node_handle, bool odom_enable, bool enu_enable);

  /*!
   * Publish a received SBG IMU log.
   *
   * \param[in] ref_sbg_log             SBG log.
   */
  void publishIMUData(const SbgEComLogUnion &ref_sbg_log);

  /*!
   * Process a ROS Velocity standard message.
   */
  void processRosVelMessage();

  /*!
   * Process a ROS IMU standard message.
   */
  void processRosImuMessage();

  /*!
   * Process a ROS odometry standard message.
   */
  void processRosOdoMessage();

  /*!
   * Publish a received SBG Magnetic log.
   *
   * \param[in] ref_sbg_log             SBG log.
   */
  void publishMagData(const SbgEComLogUnion &ref_sbg_log);

  /*!
   * Publish a received SBG Fluid pressure log.
   *
   * \param[in] ref_sbg_log             SBG log.
   */
  void publishFluidPressureData(const SbgEComLogUnion &ref_sbg_log);

  /*!
   * Publish a received SBG EkfNav log.
   *
   * \param[in] ref_sbg_log             SBG log.
   */
  void publishEkfNavigationData(const SbgEComLogUnion &ref_sbg_log);

  /*!
   * Publish a received SBG UTC log.
   *
   * \param[in] ref_sbg_log             SBG log.
   */
  void publishUtcData(const SbgEComLogUnion &ref_sbg_log);

  /*!
   * Publish a received SBG GpsPos log.
   *
   * \param[in] ref_sbg_log             SBG log.
   * \param[in] sbg_msg_id              Id of the SBG message.
   */
  void publishGpsPosData(const SbgEComLogUnion &ref_sbg_log, SbgEComMsgId sbg_msg_id);

public:

  //---------------------------------------------------------------------//
  //- Constructor                                                       -//
  //---------------------------------------------------------------------//

  /*!
   * Default constructor.
   */
  MessagePublisher();

  //---------------------------------------------------------------------//
  //- Operations                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Initialize the publishers for the output configuration.
   *
   * \param[in] ref_ros_node_handle     Ros Node to advertise the publisher.
   * \param[in] ref_config_store        Store configuration for the publishers.
   */
  void initPublishers(rclcpp::Node& ref_ros_node_handle, const ConfigStore &ref_config_store);

  /*!
   * Publish the received SbgLog if the corresponding publisher is defined.
   *
   * \param[in] sbg_msg_class           Class ID of the SBG message.
   * \param[in] sbg_msg_id              Id of the SBG message.
   * \param[in] ref_sbg_log             SBG binary log.
   */
  void publish(SbgEComClass sbg_msg_class, SbgEComMsgId sbg_msg_id, const SbgEComLogUnion &ref_sbg_log);
};
}

#endif // SBG_ROS_MESSAGE_PUBLISHER_H
