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
#include <curl/curl.h>
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

/*!
 * RAII holder for a libcurl easy handle and its associated mime structure.
 */
class CurlGuard
{
public:

  CurlGuard():
  p_handle_(curl_easy_init()),
  p_mime_(nullptr)
  {
    if (p_handle_)
    {
      p_mime_ = curl_mime_init(p_handle_);
    }
  }

  ~CurlGuard()
  {
    curl_mime_free(p_mime_);
    curl_easy_cleanup(p_handle_);
  }

  CurlGuard(const CurlGuard&) = delete;
  CurlGuard& operator=(const CurlGuard&) = delete;

  CURL *getHandle() const
  {
    return p_handle_;
  }

  curl_mime *getMime() const
  {
    return p_mime_;
  }

private:

  CURL        *p_handle_;
  curl_mime   *p_mime_;
};

/*!
 * Append the data received by libcurl to a string.
 *
 * \param[in] p_data                        Received data.
 * \param[in] size                          Size of a data item, in bytes.
 * \param[in] nmemb                         Number of data items.
 * \param[in] p_user_data                   String to append the data to.
 * \return                                  Number of bytes handled.
 */
size_t appendReplyContent(char *p_data, size_t size, size_t nmemb, void *p_user_data)
{
  static_cast<std::string*>(p_user_data)->append(p_data, size * nmemb);

  return size * nmemb;
}

/*!
 * Upload a settings document to the HTTP server of the device.
 *
 * The document is sent as a multipart/form-data file, the only form accepted by the
 * api/v1/settings/import endpoint over HTTP.
 *
 * \param[in] ref_host                      Host name or IP address of the device.
 * \param[in] ref_file_name                 Name to send the document under.
 * \param[in] ref_body                      Settings document to upload.
 * \throw                                   Unable to reach the device, or the device rejected the document.
 */
void uploadSettings(const std::string& ref_host, const std::string& ref_file_name, const std::string& ref_body)
{
  const std::string url = "http://" + ref_host + "/api/v1/settings/import";
  CurlGuard         curl;
  std::string       reply_content;
  long              status_code = 0;

  if (!curl.getHandle() || !curl.getMime())
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG_DRIVER - [Settings] Unable to initialize the HTTP client");
  }

  curl_mimepart *p_part = curl_mime_addpart(curl.getMime());

  curl_mime_name(p_part, "file");
  curl_mime_filename(p_part, ref_file_name.c_str());
  curl_mime_type(p_part, "application/octet-stream");
  curl_mime_data(p_part, ref_body.c_str(), ref_body.size());

  curl_easy_setopt(curl.getHandle(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl.getHandle(), CURLOPT_MIMEPOST, curl.getMime());
  curl_easy_setopt(curl.getHandle(), CURLOPT_WRITEFUNCTION, appendReplyContent);
  curl_easy_setopt(curl.getHandle(), CURLOPT_WRITEDATA, &reply_content);
  curl_easy_setopt(curl.getHandle(), CURLOPT_CONNECTTIMEOUT, 5L);
  curl_easy_setopt(curl.getHandle(), CURLOPT_TIMEOUT, 60L);

  const CURLcode curl_code = curl_easy_perform(curl.getHandle());

  if (curl_code != CURLE_OK)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG_DRIVER - [Settings] " + url + " failed - " + curl_easy_strerror(curl_code));
  }

  curl_easy_getinfo(curl.getHandle(), CURLINFO_RESPONSE_CODE, &status_code);

  if ((status_code < 200) || (status_code >= 300))
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG_DRIVER - [Settings] " + url + " returned status " + std::to_string(status_code) + " - " + reply_content);
  }
}
}

/*!
 * Class to apply a settings configuration to a device using the sbgInsRestApi.
 */
//---------------------------------------------------------------------//
//- Constructor                                                       -//
//---------------------------------------------------------------------//

SettingsApplier::SettingsApplier(RestApiClient &ref_client, const std::string &ref_http_host):
ref_client_(ref_client),
http_host_(ref_http_host)
{
}

//---------------------------------------------------------------------//
//- Operations                                                        -//
//---------------------------------------------------------------------//

void SettingsApplier::applySettingsFile(const std::string& ref_path)
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

  //
  // The endpoint only accepts a file upload over HTTP, while the sbgECom API tunnel carries
  // the document as the request body.
  //
  if (http_host_.empty())
  {
    if (!ref_client_.post("/api/v1/settings/import", "", body).ok)
    {
      rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG_DRIVER - [Settings] Unable to apply the settings file " + ref_path);
    }
  }
  else
  {
    uploadSettings(http_host_, ref_path.substr(ref_path.find_last_of("/\\") + 1), body);
  }

  //
  // The device saves the imported settings to its FLASH memory and reboots on its own.
  //
  RCLCPP_INFO(rclcpp::get_logger("Settings"), "SBG_DRIVER - [Settings] Settings imported, device is rebooting...");
  std::this_thread::sleep_for(g_device_boot_time);
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
