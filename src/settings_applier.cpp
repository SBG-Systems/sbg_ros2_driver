// File header
#include "settings_applier.h"

// Standard headers
#include <chrono>
#include <fstream>
#include <sstream>
#include <thread>

// ROS headers
#include <rclcpp/rclcpp.hpp>

// External headers
#include <nlohmann/json.hpp>

using sbg::SettingsApplier;

namespace
{
/*!
 * Time to wait for the device to boot after a reboot request.
 */
constexpr std::chrono::milliseconds  g_device_boot_time(500);

/*!
 * Read a whole file into a string.
 *
 * \param[in] ref_path                      Path to the file to read.
 * \return                                  File content.
 * \throw                                   Unable to open the file.
 */
std::string readFile(const std::string& ref_path)
{
  std::ifstream input_file(ref_path);

  if (!input_file)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG_DRIVER - [Settings] Unable to open the settings file " + ref_path);
  }

  std::ostringstream content;
  content << input_file.rdbuf();

  return content.str();
}
}

/*!
 * Class to apply a settings configuration to a device using the sbgInsRestApi.
 */
//---------------------------------------------------------------------//
//- Constructor                                                       -//
//---------------------------------------------------------------------//

SettingsApplier::SettingsApplier(RestApiClient &ref_client):
ref_client_(ref_client)
{
}

//---------------------------------------------------------------------//
//- Operations                                                        -//
//---------------------------------------------------------------------//

bool SettingsApplier::applySettingsFile(const std::string& ref_path)
{
  const std::string body = readFile(ref_path);

  //
  // Validate the JSON locally, so that a malformed file is reported against the file
  // rather than as a device error.
  //
  const auto settings = nlohmann::json::parse(body, nullptr, false);

  if (settings.is_discarded())
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG_DRIVER - [Settings] The settings file " + ref_path + " is not a valid JSON document");
  }

  RCLCPP_INFO(rclcpp::get_logger("Settings"), "SBG_DRIVER - [Settings] Applying %s to the device", ref_path.c_str());

  const auto reply = ref_client_.post("/api/v1/settings", "", body);

  if (!reply.ok)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG_DRIVER - [Settings] Unable to apply the settings file " + ref_path);
  }

  //
  // The device reports whether the applied settings require a reboot to take effect.
  //
  const auto reply_json  = nlohmann::json::parse(reply.content, nullptr, false);
  bool       need_reboot = false;

  if (!reply_json.is_discarded() && reply_json.is_object())
  {
    need_reboot = reply_json.value("needReboot", false);
  }
  else
  {
    RCLCPP_WARN(rclcpp::get_logger("Settings"), "SBG_DRIVER - [Settings] Unable to read needReboot from the device reply, assuming no reboot is needed.");
  }

  RCLCPP_INFO(rclcpp::get_logger("Settings"), "SBG_DRIVER - [Settings] Settings applied, reboot needed: %s", need_reboot ? "yes" : "no");

  return need_reboot;
}

void SettingsApplier::saveAndReboot()
{
  RCLCPP_INFO(rclcpp::get_logger("Settings"), "SBG_DRIVER - [Settings] Saving the settings to the device FLASH memory.");

  if (!ref_client_.post("/api/v1/settings/save").ok)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG_DRIVER - [Settings] Unable to save the settings to the device FLASH memory");
  }

  if (!ref_client_.post("/api/v1/system/reboot").ok)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG_DRIVER - [Settings] Unable to reboot the device");
  }

  RCLCPP_INFO(rclcpp::get_logger("Settings"), "SBG_DRIVER - [Settings] Device is rebooting...");
  std::this_thread::sleep_for(g_device_boot_time);
}
