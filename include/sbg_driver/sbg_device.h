/*!
*  \file         sbg_device.h
*  \author       SBG Systems
*  \date         13/03/2020
*
*  \brief       Implement device connection / parsing.
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

#ifndef SBG_ROS_SBG_DEVICE_H
#define SBG_ROS_SBG_DEVICE_H

// Standard headers
#include <iostream>
#include <string>

// ROS headers
#include <std_srvs/srv/set_bool.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <rtcm_msgs/msg/message.hpp>

// Project headers
#include <config_applier.h>
#include <config_store.h>
#include <mag_calibration.h>
#include <message_publisher.h>
#include <rest_api_client.h>

namespace sbg
{
/*!
 * Class to handle a connected SBG device.
 */
class SbgDevice
{
private:

  //---------------------------------------------------------------------//
  //- Private variables                                                 -//
  //---------------------------------------------------------------------//

  SbgEComHandle                                             com_handle_;
  SbgInterface                                              sbg_interface_;
  rclcpp::Node&                                             ref_node_;
  MessagePublisher                                          message_publisher_;
  ConfigStore                                               config_store_;

  RestApiClient                                             rest_client_;
  bool                                                      rest_api_supported_;

  uint32_t                                                  rate_frequency_;

  bool                                                      mag_calibration_ongoing_;
  bool                                                      mag_calibration_done_;
  MagCalibResults                                           mag_calib_results_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr        calib_service_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr        calib_save_service_;

  rclcpp::Subscription<rtcm_msgs::msg::Message>::SharedPtr  rtcm_sub_;

  uint32_t                                                  log_replay_last_timestamp_;

  //---------------------------------------------------------------------//
  //- Private  methods                                                  -//
  //---------------------------------------------------------------------//

  /*!
   *  Callback definition called each time a new log is received.
   * 
   *  \param[in]  pHandle         Valid handle on the sbgECom instance that has called this callback.
   *  \param[in]  msg_class       Class of the message we have received
   *  \param[in]  msg             Message ID of the log received.
   *  \param[in]  p_log_data      Contains the received log data as an union.
   *  \param[in]  p_user_arg      Optional user supplied argument.
   *  \return                     SBG_NO_ERROR if the received log has been used successfully.
   */
  static SbgErrorCode onLogReceivedCallback(SbgEComHandle* p_handle, SbgEComClass msg_class, SbgEComMsgId msg, const SbgEComLogUnion* p_log_data, void* p_user_arg);

  /*!
   * Function to handle the received log.
   * 
   * \param[in]  msg_class        Class of the message we have received
   * \param[in]  msg              Message ID of the log received.
   * \param[in]  ref_sbg_data     Contains the received log data as an union.
   */
  void onLogReceived(SbgEComClass msg_class, SbgEComMsgId msg, const SbgEComLogUnion& ref_sbg_data);

  /*!
   * Load the parameters.
   */
  void loadParameters();

  /*!
   * Create the connection to the SBG device.
   * 
   * \throw                       Unable to connect to the SBG device.
   */
  void connect();

  /*!
   * Create the communication interface from the configuration and initialize the sbgECom protocol.
   *
   * \param[in] baud_rate         Serial baudrate to open the interface with. Ignored for the other interfaces.
   * \throw                       Unable to open the interface.
   */
  void openInterface(uint32_t baud_rate);

  /*!
   * Close then reopen the communication interface and the sbgECom protocol.
   *
   * Used after an operation that reboots the device, and to switch the serial baudrate.
   *
   * \param[in] baud_rate         Serial baudrate to reopen the interface with. Ignored for the other interfaces.
   * \throw                       Unable to reopen the interface.
   */
  void reopenInterface(uint32_t baud_rate);

  /*!
   * Read the device informations.
   *
   * The sbgInsRestApi api/v1/info endpoint is used first. If the connected device doesn't
   * support it, the deprecated sbgEComCmdGetInfo command is used instead and the error
   * code of that command is returned, so that the baudrate detection keeps working.
   *
   * \return                      SBG_NO_ERROR if reading device info succeeded.
   */
  SbgErrorCode readDeviceInfo();

  /*!
   * Read and log the device informations using the sbgInsRestApi.
   *
   * \return                      SBG_NO_ERROR if reading device info succeeded.
   */
  SbgErrorCode readDeviceInfoFromRestApi();

  /*!
   * Read and log the device informations using the deprecated sbgEComCmdGetInfo command.
   *
   * \return                      SBG_NO_ERROR if reading device info succeeded.
   */
  SbgErrorCode readDeviceInfoFromEComCommand();

  /*!
   * Find the baudrate currently configured on the device.
   *
   * \return                      SBG_NO_ERROR if the current device baudrate was found.
   * \throw                       Unable to read the device information.
   */
  SbgErrorCode findCurrentDeviceBaudrate();

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG

  /*!
   * Use the current baudrate to set the baudrate configured in the config file.
   *
   * \deprecated Set the device baudrate with the sbgInsRestApi, from the settings file
   *             referenced by the ins.settingsFile parameter.
   *
   * \throw                       Unable to read the device information.
   */
  void setDeviceBaudrate();

#endif // SBG_USE_DEPRECATED_ECOM_CONFIG

  /*!
   * Get the SBG version as a string.
   * 
   * \param[in] sbg_version_enc   SBG version encoded.
   * \return                      String version decoded.
   */
  std::string getVersionAsString(uint32_t sbg_version_enc) const;

  /*!
   * Initialize the publishers according to the configuration.
   */
  void initPublishers();

  /*!
   * Initialize the subscribers according to the configuration.
   */
  void initSubscribers();

  /*!
   * Configure the connected SBG device.
   * This function will configure the device if the config file allows it.
   * It will log warning for unavailable parameters for the connected device.
   * 
   * \throw                       Unable to configure the connected device.
   */
  void configure();

  /*!
   * Process the magnetometer calibration.
   * 
   * \param[in] ref_ros_request   ROS service request.
   * \param[in] ref_ros_response  ROS service response.
   * \return                      Return true if the calibration process has been succesfull.
   */
  bool processMagCalibration(const std::shared_ptr<std_srvs::srv::Trigger::Request> ref_ros_request, std::shared_ptr<std_srvs::srv::Trigger::Response> ref_ros_response);

  /*!
   * Save the magnetometer calibration.
   * 
   * \param[in] ref_ros_request   ROS service request.
   * \param[in] ref_ros_response  ROS service response.
   * \return                      Return true if the calibration has been saved.
   */
  bool saveMagCalibration(const std::shared_ptr<std_srvs::srv::Trigger::Request> ref_ros_request, std::shared_ptr<std_srvs::srv::Trigger::Response> ref_ros_response);

  /*!
   * Start the magnetometer calibration process.
   * 
   * \return                      True if the calibration process has started successfully.
   */
  bool startMagCalibration();

  /*!
   * End the magnetometer calibration process.
   * 
   * \return                      True if the calibration process has ended successfully.
   */
  bool endMagCalibration();

  /*!
   * Upload the magnetometers calibration results to the device.
   * 
   * \return                      True if the magnetometers calibration has been successfully uploaded to the device.
   */
  bool uploadMagCalibrationToDevice();

  /*!
   * Display magnetometers calibration status result.
   */
  void displayMagCalibrationStatusResult() const;

  /*!
   * Export magnetometers calibration results.
   */
  void exportMagCalibrationResults() const;

  /*!
   * Handler for subscription to RTCM topic.
   *
   * \param[in] msg             ROS RTCM message.
   */
  void writeRtcmMessageToDevice(const rtcm_msgs::msg::Message::SharedPtr msg);

public:

  //---------------------------------------------------------------------//
  //- Constructor                                                       -//
  //---------------------------------------------------------------------//

  /*!
   * Default constructor.
   * 
   * \param[in] ref_node_handle   ROS Node.
   */
  SbgDevice(rclcpp::Node& ref_node_handle);

  /*!
   * Default destructor.
   */
  ~SbgDevice();

  //---------------------------------------------------------------------//
  //- Parameters                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Get the frequency to update the main rate loop for device handling.
   * 
   * \return                      Device frequency to read the logs (in Hz).
   */
  uint32_t getUpdateFrequency() const;

  //---------------------------------------------------------------------//
  //- Public  methods                                                   -//
  //---------------------------------------------------------------------//

  /*!
   * Initialize the SBG device for receiving data.
   * 
   * \throw                       Unable to initialize the SBG device.
   */
  void initDeviceForReceivingData();

  /*!
   * Initialize the device for magnetometers calibration.
   */
  void initDeviceForMagCalibration();

  /*!
   * Periodic handle of the connected SBG device.
   */
  void periodicHandle();
};
}

#endif // SBG_ROS_SBG_DEVICE_H
