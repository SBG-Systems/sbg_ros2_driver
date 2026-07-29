// File header
#include "sbg_device.h"

// Standard headers
#include <iomanip>
#include <fstream>
#include <ctime>
#include <chrono>
#include <thread>

// External headers
#include <nlohmann/json.hpp>

// SbgECom headers
#include <version/sbgVersion.h>

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG
#include <commands/sbgEComCmdInterface.h>
#endif

// Project headers
#include "settings_applier.h"

using namespace std;
using sbg::SbgDevice;

// From ros_com/recorder
std::string timeToStr()
{
    std::stringstream msg;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto tm = *std::localtime(&now);
    msg << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S");
    return msg.str();
}

/*!
 * Class to handle a connected SBG device.
 */
//---------------------------------------------------------------------//
//- Constructor                                                       -//
//---------------------------------------------------------------------//

SbgDevice::SbgDevice(rclcpp::Node& ref_node_handle):
ref_node_(ref_node_handle),
rest_client_(com_handle_),
rest_api_supported_(false),
mag_calibration_ongoing_(false),
mag_calibration_done_(false),
mag_calib_results_{},
log_replay_last_timestamp_(0)
{
  loadParameters();
  connect();
}

SbgDevice::~SbgDevice()
{
  SbgErrorCode error_code;

  error_code = sbgEComClose(&com_handle_);

  if (error_code != SBG_NO_ERROR)
  {
    RCLCPP_ERROR(ref_node_.get_logger(), "Unable to close the SBG communication handle - %s.", sbgErrorCodeToString(error_code));
  }

  error_code = sbgInterfaceDestroy(&sbg_interface_);

  if (error_code != SBG_NO_ERROR)
  {
    RCLCPP_ERROR(ref_node_.get_logger(), "SBG DRIVER - Unable to close the communication interface.");
  }
}

//---------------------------------------------------------------------//
//- Private  methods                                                  -//
//---------------------------------------------------------------------//

SbgErrorCode SbgDevice::onLogReceivedCallback(SbgEComHandle* p_handle, SbgEComClass msg_class, SbgEComMsgId msg, const SbgEComLogUnion* p_log_data, void* p_user_arg)
{
  assert(p_user_arg);

  SBG_UNUSED_PARAMETER(p_handle);

  SbgDevice *p_sbg_device;
  p_sbg_device = (SbgDevice*)(p_user_arg);

  p_sbg_device->onLogReceived(msg_class, msg, *p_log_data);

  return SBG_NO_ERROR;
}

void SbgDevice::onLogReceived(SbgEComClass msg_class, SbgEComMsgId msg, const SbgEComLogUnion& ref_sbg_data)
{
  //
  // If Sbg driver is reading from file
  //
  if (config_store_.isInterfaceFile())
  {
    uint32_t time_to_sleep = 0;
    uint32_t timestamp = message_publisher_.getTimestamp(msg_class, msg, ref_sbg_data);

    //
    // GPS Raw logs don't contain timestamp as the other structures
    //
    if (timestamp > log_replay_last_timestamp_)
    {
      if (log_replay_last_timestamp_ != 0)
      {
        time_to_sleep = timestamp - log_replay_last_timestamp_;
      }
      log_replay_last_timestamp_ = timestamp;
    }

    std::this_thread::sleep_for(std::chrono::microseconds(time_to_sleep));
  }

  //
  // Publish the received SBG log.
  //
  message_publisher_.publish(msg_class, msg, ref_sbg_data);
}

void SbgDevice::loadParameters()
{
  //
  // Get the ROS private nodeHandle, where the parameters are loaded from the launch file.
  //
  rclcpp::NodeOptions node_opt;
  node_opt.automatically_declare_parameters_from_overrides(true);
  rclcpp::Node n_private("npv", "", node_opt);
  config_store_.loadFromRosNodeHandle(n_private);
}

void SbgDevice::openInterface(uint32_t baud_rate)
{
  SbgErrorCode error_code;
  error_code = SBG_NO_ERROR;

  //
  // Initialize the communication interface from the config store, then initialize the sbgECom protocol to communicate with the device.
  //
  if (config_store_.isInterfaceSerial())
  {
    RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - serial interface %s at %d bps", config_store_.getUartPortName().c_str(), baud_rate);
    error_code = sbgInterfaceSerialCreate(&sbg_interface_, config_store_.getUartPortName().c_str(), baud_rate);
  }
  else if (config_store_.isInterfaceUdp())
  {
    char ip[16];
    sbgNetworkIpToString(config_store_.getIpAddress(), ip, sizeof(ip));
    RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - UDP interface %s %d->%d", ip, config_store_.getInputPortAddress(), config_store_.getOutputPortAddress());
    error_code = sbgInterfaceUdpCreate(&sbg_interface_, config_store_.getIpAddress(), config_store_.getInputPortAddress(), config_store_.getOutputPortAddress());
  }
  else if (config_store_.isInterfaceFile())
  {
    error_code = sbgInterfaceFileOpen(&sbg_interface_, config_store_.getFile().c_str());
  }
  else
  {
    rclcpp::exceptions::throw_from_rcl_error(RCL_RET_ERROR, "Invalid interface type for the SBG device.");
  }

  if (error_code != SBG_NO_ERROR)
  {
    rclcpp::exceptions::throw_from_rcl_error(RCL_RET_ERROR, "SBG_DRIVER - [Init] Unable to initialize the interface - " + std::string(sbgErrorCodeToString(error_code)));
  }

  error_code = sbgEComInit(&com_handle_, &sbg_interface_);

  if (error_code != SBG_NO_ERROR)
  {
    rclcpp::exceptions::throw_from_rcl_error(RCL_RET_ERROR, "SBG_DRIVER - [Init] Unable to initialize the SbgECom protocol - " + std::string(sbgErrorCodeToString(error_code)));
  }
}

void SbgDevice::reopenInterface(uint32_t baud_rate)
{
  sbgEComClose(&com_handle_);
  sbgInterfaceDestroy(&sbg_interface_);

  openInterface(baud_rate);
}

void SbgDevice::connect()
{
  SbgErrorCode error_code;
  error_code = SBG_NO_ERROR;

  openInterface(config_store_.getBaudRate());

  if (!config_store_.isInterfaceFile())
  {
    error_code = readDeviceInfo();
  }

  if (error_code == SBG_NO_ERROR)
  {
    return;
  }

  if (error_code == SBG_TIME_OUT && config_store_.isInterfaceSerial())
  {
    // readDeviceInfo() not successful - error could be that the baudrate configured on the device
    // is different from the one configured in the config file. Retry with different baudrates.
    error_code = findCurrentDeviceBaudrate();

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG
    if (error_code == SBG_NO_ERROR && config_store_.checkConfigWithRos())
    {
      setDeviceBaudrate();
    }
#endif
  }
}

SbgErrorCode SbgDevice::findCurrentDeviceBaudrate()
{
  SbgErrorCode         error_code = SBG_ERROR;
  std::vector<int64_t> baudrates  = config_store_.getFallbackBaudRates();

  for (const auto br : baudrates)
  {
    RCLCPP_INFO(ref_node_.get_logger(), "Not successful with %d bps, trying with %" PRIi64 " bps", config_store_.getBaudRate(), br);

    // Baudrate should be an uint32 but rclcpp only support int64 vectors
    if (br < 0 || br > UINT32_MAX)
    {
      continue;
    }

    reopenInterface(static_cast<uint32_t>(br));

    error_code = readDeviceInfo();

    if (error_code == SBG_NO_ERROR)
    {
      // current device baud rate found
      break;
    }
  }

  return error_code;
}

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG

void SbgDevice::setDeviceBaudrate()
{
  SbgErrorCode         error_code;
  SbgEComInterfaceConf com_conf;

  error_code = sbgEComCmdInterfaceGetUartConf(&com_handle_, SBG_ECOM_IF_COM_A, &com_conf);

  if (error_code != SBG_NO_ERROR)
  {
    rclcpp::exceptions::throw_from_rcl_error(RCL_RET_ERROR, "SBG_DRIVER - [Reconfig] Unable to get config of device - " + std::string(sbgErrorCodeToString(error_code)));
  }

  com_conf.baudRate = config_store_.getBaudRate();
  error_code        = sbgEComCmdInterfaceSetUartConf(&com_handle_, SBG_ECOM_IF_COM_A, &com_conf);

  if (error_code != SBG_NO_ERROR)
  {
    rclcpp::exceptions::throw_from_rcl_error(RCL_RET_ERROR, "SBG_DRIVER - [Reconfig] Unable to set new baudrate of device - " + std::string(sbgErrorCodeToString(error_code)));
  }

  error_code = sbgEComCmdSettingsAction(&com_handle_, SBG_ECOM_SAVE_SETTINGS);

  if (error_code != SBG_NO_ERROR)
  {
    rclcpp::exceptions::throw_from_rcl_error(RCL_RET_ERROR, "SBG_DRIVER - [Reconfig] Unable to save settings on device - " + std::string(sbgErrorCodeToString(error_code)));
  }

  reopenInterface(config_store_.getBaudRate());

  RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - successfully reconfigured baudrate to %d", config_store_.getBaudRate());
}

#endif // SBG_USE_DEPRECATED_ECOM_CONFIG

SbgErrorCode SbgDevice::readDeviceInfo()
{
  //
  // The device information is read with the sbgInsRestApi, which also tells whether the
  // connected device supports it at all. The deprecated command is used as a fallback for
  // the devices that don't, such as ELLIPSE firmware v2 and before.
  //
  // The error code of the fallback is returned so that connect() can still detect a
  // baudrate mismatch from a SBG_TIME_OUT.
  //
  rest_api_supported_ = (readDeviceInfoFromRestApi() == SBG_NO_ERROR);

  if (rest_api_supported_)
  {
    return SBG_NO_ERROR;
  }

  return readDeviceInfoFromEComCommand();
}

SbgErrorCode SbgDevice::readDeviceInfoFromRestApi()
{
  //
  // The device may already be streaming logs, discard them so that the reply is not
  // preceded by a partial frame.
  //
  rest_client_.purgeIncoming();

  const auto reply = rest_client_.get("/api/v1/info");

  if (!reply.ok)
  {
    return (reply.error_code != SBG_NO_ERROR) ? reply.error_code : SBG_ERROR;
  }

  const auto info = nlohmann::json::parse(reply.content, nullptr, false);

  if (info.is_discarded() || !info.is_object())
  {
    RCLCPP_ERROR(ref_node_.get_logger(), "SBG_DRIVER - The device information is not a valid JSON document");
    return SBG_ERROR;
  }

  //
  // The "mnfVersion" field was named "calibVersion" in the initial sbgInsRestApi implementation.
  //
  const auto mnf_version = info.contains("mnfVersion") ? info.value("mnfVersion", std::string()) : info.value("calibVersion", std::string());

  RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - productCode = %s", info.value("productCode", std::string()).c_str());
  RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - serialNumber = %s", info.value("serialNumber", std::string()).c_str());
  RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - hardwareRev = %s", info.value("hwRevision", std::string()).c_str());
  RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - mnfVersion = %s", mnf_version.c_str());
  RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - firmwareRev = %s", info.value("fmwVersion", std::string()).c_str());
  RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - bootloaderRev = %s", info.value("btVersion", std::string()).c_str());

  return SBG_NO_ERROR;
}

SbgErrorCode SbgDevice::readDeviceInfoFromEComCommand()
{
  SbgEComDeviceInfo device_info;
  SbgErrorCode      error_code;

  error_code = sbgEComCmdGetInfo(&com_handle_, &device_info);

  if (error_code == SBG_NO_ERROR)
  {
    RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - productCode = %s", device_info.productCode);
    RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - serialNumber = %u", device_info.serialNumber);

    RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - calibationRev = %s", getVersionAsString(device_info.calibationRev).c_str());
    RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - calibrationDate = %u / %u / %u", device_info.calibrationDay, device_info.calibrationMonth, device_info.calibrationYear);

    RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - hardwareRev = %s", getVersionAsString(device_info.hardwareRev).c_str());
    RCLCPP_INFO(ref_node_.get_logger(), "SBG_DRIVER - firmwareRev = %s", getVersionAsString(device_info.firmwareRev).c_str());
  }
  else
  {
    RCLCPP_ERROR(ref_node_.get_logger(), "Unable to get the device Info : %s", sbgErrorCodeToString(error_code));
  }

  return error_code;
}

std::string SbgDevice::getVersionAsString(uint32_t sbg_version_enc) const
{
  char version[32];
  sbgVersionToStringEncoded(sbg_version_enc, version, 32);

  return std::string(version);
}

void SbgDevice::initPublishers()
{
  message_publisher_.initPublishers(ref_node_, config_store_);

  rate_frequency_ = config_store_.getReadingRateFrequency();
}

void SbgDevice::initSubscribers()
{
  if (config_store_.shouldSubscribeToRtcm())
  {
    auto rtcm_cb = [&](const rtcm_msgs::msg::Message::SharedPtr msg) -> void {
        this->writeRtcmMessageToDevice(msg);
    };

    rtcm_sub_ = ref_node_.create_subscription<rtcm_msgs::msg::Message>(config_store_.getRtcmFullTopic(), 10, rtcm_cb);
  }
}

void SbgDevice::configure()
{
  if (!config_store_.checkConfigWithRos())
  {
    return;
  }

  if (config_store_.isInterfaceFile())
  {
    RCLCPP_WARN(ref_node_.get_logger(), "SBG_DRIVER - [Config] confWithRos is enabled but the driver is replaying a log file, there is no device to configure.");
    return;
  }

  if (!config_store_.getInsSettingsFile().empty())
  {
    if (!rest_api_supported_)
    {
      rclcpp::exceptions::throw_from_rcl_error(RCL_RET_ERROR, "SBG_DRIVER - [Config] The connected device doesn't support the sbgInsRestApi, the ins.settingsFile parameter can't be applied.");
    }

    SettingsApplier settings_applier(rest_client_);

    if (settings_applier.applySettingsFile(config_store_.getInsSettingsFile()))
    {
      settings_applier.saveAndReboot();
      reopenInterface(config_store_.getBaudRate());
      readDeviceInfo();
    }

    return;
  }

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG
  if (config_store_.hasLegacyInsParameters())
  {
    RCLCPP_WARN(ref_node_.get_logger(), "SBG_DRIVER - [Config] Configuring the device from yaml parameters is deprecated and will be removed in a future release.");
    RCLCPP_WARN(ref_node_.get_logger(), "SBG_DRIVER - [Config] Export the device settings as a sbgInsRestApi JSON file and reference it with the ins.settingsFile parameter instead.");

    ConfigApplier configApplier(com_handle_);
    configApplier.applyConfiguration(config_store_);

    return;
  }
#endif

  RCLCPP_WARN(ref_node_.get_logger(), "SBG_DRIVER - [Config] confWithRos is enabled but the ins.settingsFile parameter is empty, the device settings are left untouched.");
}

bool SbgDevice::processMagCalibration(const std::shared_ptr<std_srvs::srv::Trigger::Request> ref_ros_request, std::shared_ptr<std_srvs::srv::Trigger::Response> ref_ros_response)
{
  SBG_UNUSED_PARAMETER(ref_ros_request);

  if (mag_calibration_ongoing_)
  {
    if (endMagCalibration())
    {
      ref_ros_response->success = true;
      ref_ros_response->message = "Magnetometer calibration is finished. See the output console to get calibration informations.";
    }
    else
    {
      ref_ros_response->success = false;
      ref_ros_response->message = "Unable to end the calibration.";
    }

    mag_calibration_ongoing_  = false;
    mag_calibration_done_     = true;
  }
  else
  {
    if (startMagCalibration())
    {
      ref_ros_response->success = true;
      ref_ros_response->message = "Magnetometer calibration process started.";
    }
    else
    {
      ref_ros_response->success = false;
      ref_ros_response->message = "Unable to start magnetometers calibration.";
    }

    mag_calibration_ongoing_ = true;
  }

  return ref_ros_response->success;
}

bool SbgDevice::saveMagCalibration(const std::shared_ptr<std_srvs::srv::Trigger::Request> ref_ros_request, std::shared_ptr<std_srvs::srv::Trigger::Response> ref_ros_response)
{
  SBG_UNUSED_PARAMETER(ref_ros_request);

  if (mag_calibration_ongoing_)
  {
    ref_ros_response->success = false;
    ref_ros_response->message = "Magnetometer calibration process is still ongoing, finish it before trying to save it.";
  }
  else if (mag_calibration_done_)
  {
    if (uploadMagCalibrationToDevice())
    {
      ref_ros_response->success = true;
      ref_ros_response->message = "Magnetometer calibration has been uploaded to the device.";
    }
    else
    {
      ref_ros_response->success = false;
      ref_ros_response->message = "Magnetometer calibration has not been uploaded to the device.";
    }
  }
  else
  {
    ref_ros_response->success = false;
    ref_ros_response->message = "No magnetometer calibration has been done.";
  }

  return ref_ros_response->success;
}

bool SbgDevice::startMagCalibration()
{
  const SbgEComMagCalibMode mag_calib_mode = config_store_.getMagnetometerCalibMode();

  try
  {
    MagCalibration mag_calibration(rest_client_, com_handle_, rest_api_supported_);

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG
    mag_calibration.start(mag_calib_mode, config_store_.getMagnetometerCalibBandwidth());
#else
    mag_calibration.start(mag_calib_mode, SBG_ECOM_MAG_CALIB_HIGH_BW);
#endif
  }
  catch (std::exception const& refE)
  {
    RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Unable to start the magnetometer calibration : %s", refE.what());
    return false;
  }

  RCLCPP_INFO(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Start calibration");
  RCLCPP_INFO(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Mode : %s", (mag_calib_mode == SBG_ECOM_MAG_CALIB_MODE_2D) ? "2D" : "3D");

  return true;
}

bool SbgDevice::endMagCalibration()
{
  try
  {
    MagCalibration mag_calibration(rest_client_, com_handle_, rest_api_supported_);

    mag_calib_results_ = mag_calibration.compute(config_store_.getMagnetometerCalibMode());
  }
  catch (std::exception const& refE)
  {
    RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Unable to compute the magnetometer calibration results : %s", refE.what());
    return false;
  }

  displayMagCalibrationStatusResult();
  exportMagCalibrationResults();

  return true;
}

bool SbgDevice::uploadMagCalibrationToDevice()
{
  if (mag_calib_results_.quality == "invalid")
  {
    RCLCPP_ERROR(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - The calibration was invalid, it can't be uploaded on the device.");
    return false;
  }

  try
  {
    MagCalibration mag_calibration(rest_client_, com_handle_, rest_api_supported_);

    RCLCPP_INFO(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Saving data to the device");
    mag_calibration.apply(mag_calib_results_);

    //
    // Saving the calibration reboots the device, the interface has to be reopened to keep
    // communicating with it.
    //
    reopenInterface(config_store_.getBaudRate());
  }
  catch (std::exception const& refE)
  {
    RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Unable to set the magnetometers calibration data to the device : %s", refE.what());
    return false;
  }

  return true;
}

void SbgDevice::displayMagCalibrationStatusResult() const
{
  RCLCPP_INFO(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Quality of the calibration %s", mag_calib_results_.quality.c_str());
  RCLCPP_INFO(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Calibration results confidence %s", mag_calib_results_.trust.c_str());

  //
  // Check the magnetometers calibration status and display the warnings.
  //
  if (!mag_calib_results_.enough_points)
  {
    RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Not enough valid points. Maybe you are moving too fast");
  }
  if (mag_calib_results_.distortion_issue)
  {
    RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Unable to find a calibration solution. Maybe there are too much non static distortions");
  }
  if (mag_calib_results_.alignment_issue)
  {
    RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - The magnetic calibration has troubles to correct the magnetometers and inertial frame alignment");
  }
  if (mag_calib_results_.is_2d)
  {
    if (!mag_calib_results_.roll_motion_valid)
    {
      RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Too much roll motion for a 2D magnetic calibration");
    }
    if (!mag_calib_results_.pitch_motion_valid)
    {
      RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Too much pitch motion for a 2D magnetic calibration");
    }
  }
  else
  {
    if (!mag_calib_results_.roll_motion_valid)
    {
      RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Not enough roll motion for a 3D magnetic calibration");
    }
    if (!mag_calib_results_.pitch_motion_valid)
    {
      RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Not enough pitch motion for a 3D magnetic calibration.");
    }
  }
  if (!mag_calib_results_.yaw_motion_valid)
  {
    RCLCPP_WARN(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Not enough yaw motion to compute a valid magnetic calibration");
  }
}

void SbgDevice::exportMagCalibrationResults() const
{
  ostringstream             mag_results_stream;
  string                    output_filename;

  mag_results_stream << "SBG DRIVER [Mag Calib]" << endl;
  mag_results_stream << "======= Parameters =======" << endl;
  mag_results_stream << "* CALIB_MODE = " << (mag_calib_results_.is_2d ? "Mode 2D" : "Mode 3D") << endl;

  mag_results_stream << "======= Results =======" << endl;
  mag_results_stream << "Quality: " << mag_calib_results_.quality << endl;
  mag_results_stream << "Confidence: " << mag_calib_results_.trust << endl;
  mag_results_stream << "======= Infos =======" << endl;
  mag_results_stream << "* Used points : " << mag_calib_results_.num_points_used << "/" << mag_calib_results_.max_num_points << endl;
  mag_results_stream << "* Mean, Std, Max" << endl;
  mag_results_stream << "[Before]\t" << mag_calib_results_.before_mean_error << "\t" << mag_calib_results_.before_std_error << "\t" << mag_calib_results_.before_max_error << endl;
  mag_results_stream << "[After]\t" << mag_calib_results_.after_mean_error << "\t" << mag_calib_results_.after_std_error << "\t" << mag_calib_results_.after_max_error << endl;
  mag_results_stream << "[Accuracy]\t" << sbgRadToDegf(mag_calib_results_.mean_accuracy) << "\t" << sbgRadToDegf(mag_calib_results_.std_accuracy) << "\t" << sbgRadToDegf(mag_calib_results_.max_accuracy) << endl;
  mag_results_stream << "* Offset\t" << mag_calib_results_.hard_iron[0] << "\t" << mag_calib_results_.hard_iron[1] << "\t" << mag_calib_results_.hard_iron[2] << endl;

  mag_results_stream << "* Matrix" << endl;
  mag_results_stream << mag_calib_results_.soft_iron[0] << "\t" << mag_calib_results_.soft_iron[1] << "\t" << mag_calib_results_.soft_iron[2] << endl;
  mag_results_stream << mag_calib_results_.soft_iron[3] << "\t" << mag_calib_results_.soft_iron[4] << "\t" << mag_calib_results_.soft_iron[5] << endl;
  mag_results_stream << mag_calib_results_.soft_iron[6] << "\t" << mag_calib_results_.soft_iron[7] << "\t" << mag_calib_results_.soft_iron[8] << endl;

  output_filename = "mag_calib_" + timeToStr() + ".txt";
  ofstream output_file(output_filename);
  output_file << mag_results_stream.str();
  output_file.close();

  RCLCPP_INFO(ref_node_.get_logger(), "%s", mag_results_stream.str().c_str());
  RCLCPP_INFO(ref_node_.get_logger(), "SBG DRIVER [Mag Calib] - Magnetometers calibration results saved to file %s", output_filename.c_str());
}

void SbgDevice::writeRtcmMessageToDevice(const rtcm_msgs::msg::Message::SharedPtr msg)
{
  auto rtcm_data = msg->message;
  auto error_code = sbgInterfaceWrite(&sbg_interface_, rtcm_data.data(), rtcm_data.size());

  if (error_code != SBG_NO_ERROR)
  {
    char error_str[256];

    sbgEComErrorToString(error_code, error_str);
    SBG_LOG_ERROR(SBG_ERROR, "Failed to sent RTCM data to device: %s", error_str);
  }
}

//---------------------------------------------------------------------//
//- Parameters                                                        -//
//---------------------------------------------------------------------//

uint32_t SbgDevice::getUpdateFrequency() const
{
  return rate_frequency_;
}

//---------------------------------------------------------------------//
//- Public  methods                                                   -//
//---------------------------------------------------------------------//

void SbgDevice::initDeviceForReceivingData()
{
  initPublishers();
  configure();

  sbgEComSetReceiveLogCallback(&com_handle_, onLogReceivedCallback, this);

  initSubscribers();
}

void SbgDevice::initDeviceForMagCalibration()
{
  calib_service_      = ref_node_.create_service<std_srvs::srv::Trigger>("sbg/mag_calibration", std::bind(&SbgDevice::processMagCalibration, this, std::placeholders::_1, std::placeholders::_2));
  calib_save_service_ = ref_node_.create_service<std_srvs::srv::Trigger>("sbg/mag_calibration_save", std::bind(&SbgDevice::saveMagCalibration, this, std::placeholders::_1, std::placeholders::_2));

  RCLCPP_INFO(ref_node_.get_logger(), "SBG DRIVER [Init] - SBG device is initialized for magnetometers calibration.");
}

void SbgDevice::periodicHandle()
{
  sbgEComHandle(&com_handle_);
}
