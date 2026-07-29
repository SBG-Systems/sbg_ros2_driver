// File header
#include "mag_calibration.h"

// Standard headers
#include <algorithm>
#include <map>

// ROS headers
#include <rclcpp/rclcpp.hpp>

// External headers
#include <nlohmann/json.hpp>

// Project headers
#include "settings_applier.h"

using sbg::MagCalibration;
using sbg::MagCalibResults;

namespace
{
/*!
 * Get the logger used by the magnetic calibration.
 */
rclcpp::Logger getLogger()
{
  return rclcpp::get_logger("MagCalib");
}

/*!
 * Get the sbgInsRestApi calibration mode string for a calibration mode.
 *
 * \param[in] mode                          Calibration mode.
 * \return                                  Calibration mode as expected by the sbgInsRestApi.
 */
const char *getRestApiCalibModeStr(SbgEComMagCalibMode mode)
{
  return (mode == SBG_ECOM_MAG_CALIB_MODE_2D) ? "2d" : "3d";
}

/*!
 * Read a fixed size float array from a JSON node.
 *
 * \template  N                             Number of elements to read.
 * \param[in] ref_json                      JSON node to read the array from.
 * \param[in] p_key                         Key of the array to read.
 * \param[out] ref_values                   Read values, left untouched if the array is missing or malformed.
 */
template <size_t N>
void readFloatArray(const nlohmann::json& ref_json, const char *p_key, std::array<float, N>& ref_values)
{
  const auto it = ref_json.find(p_key);

  if ((it == ref_json.end()) || !it->is_array() || (it->size() != N))
  {
    RCLCPP_WARN(getLogger(), "SBG DRIVER [Mag Calib] - '%s' is missing or malformed in the device reply.", p_key);
    return;
  }

  for (size_t i = 0; i < N; i++)
  {
    ref_values[i] = it->at(i).get<float>();
  }
}

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG

//
// The sbgInsRestApi reports the calibration quality and trust as strings. These maps
// render the legacy enums the same way, so that the results can be displayed and
// exported without knowing which path produced them.
//
const std::map<SbgEComMagCalibQuality, std::string> g_mag_calib_quality = {
  {SBG_ECOM_MAG_CALIB_QUAL_OPTIMAL, "optimal"},
  {SBG_ECOM_MAG_CALIB_QUAL_GOOD,    "good"},
  {SBG_ECOM_MAG_CALIB_QUAL_POOR,    "poor"},
  {SBG_ECOM_MAG_CALIB_QUAL_INVALID, "invalid"}};

const std::map<SbgEComMagCalibConfidence, std::string> g_mag_calib_confidence = {
  {SBG_ECOM_MAG_CALIB_TRUST_HIGH,   "high"},
  {SBG_ECOM_MAG_CALIB_TRUST_MEDIUM, "medium"},
  {SBG_ECOM_MAG_CALIB_TRUST_LOW,    "low"}};

/*!
 * Look up an enum in a map, returning a placeholder for unknown values.
 *
 * \template  T                             Enum type to look up.
 * \param[in] ref_map                       Map to look the value up in.
 * \param[in] value                         Value to look up.
 * \return                                  String for the value, "unknown" if not in the map.
 */
template <typename T>
std::string lookUp(const std::map<T, std::string>& ref_map, T value)
{
  const auto it = ref_map.find(value);

  return (it != ref_map.end()) ? it->second : std::string("unknown");
}

#endif // SBG_USE_DEPRECATED_ECOM_CONFIG
}

/*!
 * Class to run the on-board magnetic field calibration procedure.
 */
//---------------------------------------------------------------------//
//- Constructor                                                       -//
//---------------------------------------------------------------------//

MagCalibration::MagCalibration(RestApiClient &ref_client, SbgEComHandle &ref_sbg_com_handle, bool rest_api_supported):
ref_client_(ref_client),
ref_sbg_com_handle_(ref_sbg_com_handle),
rest_api_supported_(rest_api_supported)
{
}

//---------------------------------------------------------------------//
//- Private  methods                                                  -//
//---------------------------------------------------------------------//

void MagCalibration::checkLegacySupport() const
{
#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG
  RCLCPP_WARN(getLogger(), "SBG DRIVER [Mag Calib] - The connected device doesn't support the sbgInsRestApi, falling back to the deprecated sbgECom magnetic calibration commands.");
#else
  rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG DRIVER [Mag Calib] - The connected device doesn't support the sbgInsRestApi. Rebuild the driver with -DSBG_USE_DEPRECATED_ECOM_CONFIG=ON to use the deprecated sbgECom magnetic calibration commands.");
#endif
}

void MagCalibration::startWithRestApi(SbgEComMagCalibMode mode)
{
  nlohmann::json body;

  body["mode"] = getRestApiCalibModeStr(mode);

  if (!ref_client_.post("/api/v1/magnetometer/calibration/start", "", body.dump()).ok)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG DRIVER [Mag Calib] - Unable to start the magnetic field acquisition");
  }
}

MagCalibResults MagCalibration::computeWithRestApi()
{
  const auto reply = ref_client_.get("/api/v1/magnetometer/calibration");

  if (!reply.ok)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG DRIVER [Mag Calib] - Unable to compute the magnetic calibration");
  }

  const auto json = nlohmann::json::parse(reply.content, nullptr, false);

  if (json.is_discarded() || !json.is_object())
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG DRIVER [Mag Calib] - The magnetic calibration results are not a valid JSON document");
  }

  MagCalibResults results{};

  results.quality             = json.value("quality", std::string("unknown"));
  results.trust               = json.value("trust", std::string("unknown"));

  results.num_points_used     = json.value<uint16_t>("numPointsUsed", 0);
  results.num_points_total    = json.value<uint16_t>("numPointsTotal", 0);
  results.max_num_points      = json.value<uint16_t>("maxNumPoints", 0);

  results.is_2d               = json.value("is2dCalibration", false);

  results.enough_points       = json.value("enoughPts", false);
  results.roll_motion_valid   = json.value("rollMotionValid", false);
  results.pitch_motion_valid  = json.value("pitchMotionValid", false);
  results.yaw_motion_valid    = json.value("yawMotionValid", false);

  //
  // The sbgInsRestApi doesn't report magnetic distortions as a dedicated flag, they are
  // reflected by the quality indicator instead.
  //
  results.distortion_issue    = false;
  results.alignment_issue     = !json.value("alignmentEnoughPts", true) || !json.value("alignmentKeptEnoughPts", true);

  results.before_mean_error   = json.value("beforeMeanError", 0.0f);
  results.before_std_error    = json.value("beforeStdError", 0.0f);
  results.before_max_error    = json.value("beforeMaxError", 0.0f);

  results.after_mean_error    = json.value("afterMeanError", 0.0f);
  results.after_std_error     = json.value("afterStdError", 0.0f);
  results.after_max_error     = json.value("afterMaxError", 0.0f);

  results.mean_accuracy       = json.value("meanAccuracy", 0.0f);
  results.std_accuracy        = json.value("stdAccuracy", 0.0f);
  results.max_accuracy        = json.value("maxAccuracy", 0.0f);

  readFloatArray(json, "hardIronCorrection", results.hard_iron);
  readFloatArray(json, "softIronCorrection", results.soft_iron);

  return results;
}

void MagCalibration::applyWithRestApi(const MagCalibResults& ref_results)
{
  nlohmann::json body;

  body["hardIronCorrection"] = ref_results.hard_iron;
  body["softIronCorrection"] = ref_results.soft_iron;
  body["calibrationMode"]    = ref_results.is_2d ? "2d" : "3d";

  if (!ref_client_.post("/api/v1/settings/aiding/magnetometer", "", body.dump()).ok)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG DRIVER [Mag Calib] - Unable to apply the magnetic calibration to the device");
  }

  SettingsApplier settings_applier(ref_client_);

  settings_applier.saveAndReboot();
}

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG

void MagCalibration::startWithEComCommand(SbgEComMagCalibMode mode, SbgEComMagCalibBandwidth bandwidth)
{
  const SbgErrorCode error_code = sbgEComCmdMagStartCalib(&ref_sbg_com_handle_, mode, bandwidth);

  if (error_code != SBG_NO_ERROR)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG DRIVER [Mag Calib] - Unable to start the magnetic field acquisition : " + std::string(sbgErrorCodeToString(error_code)));
  }
}

MagCalibResults MagCalibration::computeWithEComCommand(SbgEComMagCalibMode mode)
{
  SbgEComMagCalibResults  sbg_results;
  const SbgErrorCode      error_code = sbgEComCmdMagComputeCalib(&ref_sbg_com_handle_, &sbg_results);

  if (error_code != SBG_NO_ERROR)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG DRIVER [Mag Calib] - Unable to compute the magnetic calibration : " + std::string(sbgErrorCodeToString(error_code)));
  }

  MagCalibResults results{};

  results.quality             = lookUp(g_mag_calib_quality, sbg_results.quality);
  results.trust               = lookUp(g_mag_calib_confidence, sbg_results.confidence);

  //
  // The legacy command only reports the number of points used to compute the calibration.
  //
  results.num_points_used     = sbg_results.numPoints;
  results.num_points_total    = sbg_results.numPoints;
  results.max_num_points      = sbg_results.maxNumPoints;

  //
  // The legacy command doesn't report the calibration mode, it is taken from the mode the
  // acquisition has been started with.
  //
  results.is_2d               = (mode == SBG_ECOM_MAG_CALIB_MODE_2D);

  results.enough_points       = (sbg_results.advancedStatus & SBG_ECOM_MAG_CALIB_NOT_ENOUGH_POINTS) == 0;
  results.roll_motion_valid   = (sbg_results.advancedStatus & SBG_ECOM_MAG_CALIB_X_MOTION_ISSUE) == 0;
  results.pitch_motion_valid  = (sbg_results.advancedStatus & SBG_ECOM_MAG_CALIB_Y_MOTION_ISSUE) == 0;
  results.yaw_motion_valid    = (sbg_results.advancedStatus & SBG_ECOM_MAG_CALIB_Z_MOTION_ISSUE) == 0;
  results.distortion_issue    = (sbg_results.advancedStatus & SBG_ECOM_MAG_CALIB_TOO_MUCH_DISTORTIONS) != 0;
  results.alignment_issue     = (sbg_results.advancedStatus & SBG_ECOM_MAG_CALIB_ALIGNMENT_ISSUE) != 0;

  results.before_mean_error   = sbg_results.beforeMeanError;
  results.before_std_error    = sbg_results.beforeStdError;
  results.before_max_error    = sbg_results.beforeMaxError;

  results.after_mean_error    = sbg_results.afterMeanError;
  results.after_std_error     = sbg_results.afterStdError;
  results.after_max_error     = sbg_results.afterMaxError;

  results.mean_accuracy       = sbg_results.meanAccuracy;
  results.std_accuracy        = sbg_results.stdAccuracy;
  results.max_accuracy        = sbg_results.maxAccuracy;

  std::copy(sbg_results.offset, sbg_results.offset + results.hard_iron.size(), results.hard_iron.begin());
  std::copy(sbg_results.matrix, sbg_results.matrix + results.soft_iron.size(), results.soft_iron.begin());

  return results;
}

void MagCalibration::applyWithEComCommand(const MagCalibResults& ref_results)
{
  SbgErrorCode error_code;

  error_code = sbgEComCmdMagSetCalibData(&ref_sbg_com_handle_, ref_results.hard_iron.data(), ref_results.soft_iron.data());

  if (error_code != SBG_NO_ERROR)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG DRIVER [Mag Calib] - Unable to apply the magnetic calibration to the device : " + std::string(sbgErrorCodeToString(error_code)));
  }

  error_code = sbgEComCmdSettingsAction(&ref_sbg_com_handle_, SBG_ECOM_SAVE_SETTINGS);

  if (error_code != SBG_NO_ERROR)
  {
    rclcpp::exceptions::throw_from_rcl_error(RMW_RET_ERROR, "SBG DRIVER [Mag Calib] - Unable to save the magnetic calibration to the device : " + std::string(sbgErrorCodeToString(error_code)));
  }

  RCLCPP_INFO(getLogger(), "SBG DRIVER [Mag Calib] - Settings saved and device rebooted.");
}

#endif // SBG_USE_DEPRECATED_ECOM_CONFIG

//---------------------------------------------------------------------//
//- Operations                                                        -//
//---------------------------------------------------------------------//

void MagCalibration::start(SbgEComMagCalibMode mode, SbgEComMagCalibBandwidth bandwidth)
{
  if (rest_api_supported_)
  {
    //
    // The sbgInsRestApi has no bandwidth parameter, it is not used anymore by the devices
    // supporting it.
    //
    if (bandwidth != SBG_ECOM_MAG_CALIB_HIGH_BW)
    {
      RCLCPP_DEBUG(getLogger(), "SBG DRIVER [Mag Calib] - The calibration bandwidth is ignored, the sbgInsRestApi has no such parameter.");
    }

    startWithRestApi(mode);
  }
  else
  {
    checkLegacySupport();

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG
    startWithEComCommand(mode, bandwidth);
#endif
  }
}

MagCalibResults MagCalibration::compute(SbgEComMagCalibMode mode)
{
  if (rest_api_supported_)
  {
    return computeWithRestApi();
  }

  checkLegacySupport();

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG
  return computeWithEComCommand(mode);
#else
  SBG_UNUSED_PARAMETER(mode);

  return MagCalibResults{};
#endif
}

void MagCalibration::apply(const MagCalibResults& ref_results)
{
  if (rest_api_supported_)
  {
    applyWithRestApi(ref_results);
  }
  else
  {
    checkLegacySupport();

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG
    applyWithEComCommand(ref_results);
#endif
  }
}
