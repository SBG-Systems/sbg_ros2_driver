/*!
*  \file         settings_applier.h
*  \author       SBG Systems
*  \date         29/07/2026
*
*  \brief        Apply device settings using the sbgInsRestApi.
*
*  The settings are provided as a sbgInsRestApi JSON document, as exported by the
*  device web interface or by the sbgEComApi command line tool, and are sent to
*  the api/v1/settings endpoint.
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

#ifndef SBG_ROS_SETTINGS_APPLIER_H
#define SBG_ROS_SETTINGS_APPLIER_H

// Standard headers
#include <string>

// Project headers
#include "rest_api_client.h"

namespace sbg
{
/*!
 * Class to apply a settings configuration to a device using the sbgInsRestApi.
 */
class SettingsApplier
{
private:

  RestApiClient&  ref_client_;

public:

  //---------------------------------------------------------------------//
  //- Constructor                                                       -//
  //---------------------------------------------------------------------//

  /*!
   * Default constructor.
   *
   * \param[in] ref_client                  sbgInsRestApi client.
   */
  explicit SettingsApplier(RestApiClient &ref_client);

  //---------------------------------------------------------------------//
  //- Operations                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Apply a sbgInsRestApi JSON settings file to the device.
   *
   * The file content is sent as is to the api/v1/settings endpoint, so it may
   * hold either the full settings or only the fields to update.
   *
   * \param[in] ref_path                    Path to the JSON settings file.
   * \return                                True if the device has to be rebooted for the settings to take effect.
   * \throw                                 Unable to read the file, or to apply the settings.
   */
  bool applySettingsFile(const std::string& ref_path);

  /*!
   * Save the current settings to the device FLASH memory and reboot it.
   *
   * The communication interface is left closed by the device reboot, the caller
   * is responsible for reopening it.
   *
   * \throw                                 Unable to save the settings or to reboot the device.
   */
  void saveAndReboot();
};
}

#endif // SBG_ROS_SETTINGS_APPLIER_H
