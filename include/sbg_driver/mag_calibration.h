/*!
*  \file         mag_calibration.h
*  \author       SBG Systems
*  \date         29/07/2026
*
*  \brief        On-board magnetic field calibration procedure.
*
*  The procedure is executed with the sbgInsRestApi. For devices that don't
*  support it, such as ELLIPSE firmware v2 and before, the legacy sbgECom
*  magnetic calibration commands are used as a fallback. That fallback is only
*  available when the driver is built with SBG_USE_DEPRECATED_ECOM_CONFIG.
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

#ifndef SBG_ROS_MAG_CALIBRATION_H
#define SBG_ROS_MAG_CALIBRATION_H

// Standard headers
#include <array>
#include <string>

// SbgECom headers
#include <sbgEComLib.h>
#include <commands/sbgEComCmdMag.h>

// Project headers
#include "rest_api_client.h"

namespace sbg
{
/*!
 * Results of an on-board magnetic field calibration.
 *
 * The sbgInsRestApi and the legacy sbgECom command don't report the results in the
 * same form, the sbgInsRestApi using strings and explicit flags where the command
 * uses enums and a status bitmask. This structure holds the union of both, so that
 * the results can be displayed and exported the same way whatever the source.
 */
struct MagCalibResults
{
  std::string           quality;              /*!< General calibration quality indicator. */
  std::string           trust;                /*!< Confidence indicator on the quality indicator. */

  uint16_t              num_points_used;      /*!< Number of magnetic field points used to compute the calibration. */
  uint16_t              num_points_total;     /*!< Number of magnetic field points acquired. Same as num_points_used for the legacy command, which doesn't report both. */
  uint16_t              max_num_points;       /*!< Maximum number of magnetic field points that can be stored by the device. */

  bool                  is_2d;                /*!< True if the calibration has been computed in 2D mode. */

  bool                  enough_points;        /*!< True if enough valid magnetic points have been acquired. */
  bool                  roll_motion_valid;    /*!< True if the roll motion during the acquisition was suitable for the calibration mode. */
  bool                  pitch_motion_valid;   /*!< True if the pitch motion during the acquisition was suitable for the calibration mode. */
  bool                  yaw_motion_valid;     /*!< True if the yaw motion during the acquisition was suitable for the calibration mode. */
  bool                  distortion_issue;     /*!< True if magnetic interferences prevented the calibration. Only reported by the legacy command. */
  bool                  alignment_issue;      /*!< True if the magnetometers to inertial frame alignment seems invalid. */

  float                 before_mean_error;    /*!< Mean magnetic field norm error observed before calibration. */
  float                 before_std_error;     /*!< Standard deviation of the magnetic field norm error observed before calibration. */
  float                 before_max_error;     /*!< Maximum magnetic field norm error observed before calibration. */

  float                 after_mean_error;     /*!< Mean magnetic field norm error observed after calibration. */
  float                 after_std_error;      /*!< Standard deviation of the magnetic field norm error observed after calibration. */
  float                 after_max_error;      /*!< Maximum magnetic field norm error observed after calibration. */

  float                 mean_accuracy;        /*!< Mean expected heading accuracy (in radians). */
  float                 std_accuracy;         /*!< Standard deviation of the expected heading accuracy (in radians). */
  float                 max_accuracy;         /*!< Maximum expected heading accuracy (in radians). */

  std::array<float, 3>  hard_iron;            /*!< Computed hard iron correction vector. */
  std::array<float, 9>  soft_iron;            /*!< Computed hard and soft iron correction matrix. */
};

/*!
 * Class to run the on-board magnetic field calibration procedure.
 */
class MagCalibration
{
private:

  RestApiClient&  ref_client_;
  SbgEComHandle&  ref_sbg_com_handle_;
  bool            rest_api_supported_;

  //---------------------------------------------------------------------//
  //- Private  methods                                                  -//
  //---------------------------------------------------------------------//

  /*!
   * Check that the legacy sbgECom magnetic calibration commands can be used.
   *
   * \throw                                 The device doesn't support the sbgInsRestApi and the legacy commands are not built in.
   */
  void checkLegacySupport() const;

  /*!
   * Start a magnetic field acquisition using the sbgInsRestApi.
   *
   * \param[in] mode                        Calibration mode to use.
   * \throw                                 Unable to start the acquisition.
   */
  void startWithRestApi(SbgEComMagCalibMode mode);

  /*!
   * Compute the magnetic calibration using the sbgInsRestApi.
   *
   * \return                                Calibration results.
   * \throw                                 Unable to compute the calibration.
   */
  MagCalibResults computeWithRestApi();

  /*!
   * Apply and save a magnetic calibration using the sbgInsRestApi.
   *
   * \param[in] ref_results                 Calibration results to apply.
   * \throw                                 Unable to apply or save the calibration.
   */
  void applyWithRestApi(const MagCalibResults& ref_results);

#ifdef SBG_USE_DEPRECATED_ECOM_CONFIG

  /*!
   * Start a magnetic field acquisition using the deprecated sbgECom command.
   *
   * \param[in] mode                        Calibration mode to use.
   * \param[in] bandwidth                   Expected motions bandwidth during the acquisition.
   * \throw                                 Unable to start the acquisition.
   */
  void startWithEComCommand(SbgEComMagCalibMode mode, SbgEComMagCalibBandwidth bandwidth);

  /*!
   * Compute the magnetic calibration using the deprecated sbgECom command.
   *
   * \param[in] mode                        Calibration mode the acquisition has been started with.
   * \return                                Calibration results.
   * \throw                                 Unable to compute the calibration.
   */
  MagCalibResults computeWithEComCommand(SbgEComMagCalibMode mode);

  /*!
   * Apply and save a magnetic calibration using the deprecated sbgECom commands.
   *
   * \param[in] ref_results                 Calibration results to apply.
   * \throw                                 Unable to apply or save the calibration.
   */
  void applyWithEComCommand(const MagCalibResults& ref_results);

#endif // SBG_USE_DEPRECATED_ECOM_CONFIG

public:

  //---------------------------------------------------------------------//
  //- Constructor                                                       -//
  //---------------------------------------------------------------------//

  /*!
   * Default constructor.
   *
   * \param[in] ref_client                  sbgInsRestApi client.
   * \param[in] ref_sbg_com_handle          SBG communication handle, used by the legacy fallback.
   * \param[in] rest_api_supported          True if the connected device supports the sbgInsRestApi.
   */
  MagCalibration(RestApiClient &ref_client, SbgEComHandle &ref_sbg_com_handle, bool rest_api_supported);

  //---------------------------------------------------------------------//
  //- Operations                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Clear any previously acquired data and start a new magnetic field acquisition.
   *
   * \param[in] mode                        Calibration mode to use.
   * \param[in] bandwidth                   Expected motions bandwidth. Only used by the legacy fallback, the sbgInsRestApi has no such parameter.
   * \throw                                 Unable to start the acquisition.
   */
  void start(SbgEComMagCalibMode mode, SbgEComMagCalibBandwidth bandwidth);

  /*!
   * Compute the magnetic calibration from the acquired magnetic field data.
   *
   * The acquisition is not cleared, so the calibration can be computed again after
   * acquiring more data.
   *
   * \param[in] mode                        Calibration mode the acquisition has been started with.
   *                                        Only used by the legacy fallback, which doesn't report
   *                                        the mode it has been run with.
   * \return                                Calibration results.
   * \throw                                 Unable to compute the calibration.
   */
  MagCalibResults compute(SbgEComMagCalibMode mode);

  /*!
   * Apply a magnetic calibration to the device and save it to the FLASH memory.
   *
   * The device is rebooted so that the new calibration takes effect, the caller is
   * responsible for reopening the communication interface.
   *
   * \param[in] ref_results                 Calibration results to apply.
   * \throw                                 Unable to apply or save the calibration.
   */
  void apply(const MagCalibResults& ref_results);
};
}

#endif // SBG_ROS_MAG_CALIBRATION_H
