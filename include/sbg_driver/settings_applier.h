/*!
*  \file         settings_applier.h
*  \author       SBG Systems
*  \date         29/07/2026
*
*  \brief        Apply device settings using the sbgInsRestApi.
*
*  The settings are provided as a sbgInsRestApi JSON export document, as exported by
*  the device web interface or by the sbgEComApi command line tool, and are sent to
*  the api/v1/settings/import endpoint, either through the sbgECom API tunnel or as
*  an HTTP file upload for devices reached over Ethernet.
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
  std::string     http_host_;

public:

  //---------------------------------------------------------------------//
  //- Constructor                                                       -//
  //---------------------------------------------------------------------//

  /*!
   * Default constructor.
   *
   * \param[in] ref_client                  sbgInsRestApi client.
   * \param[in] ref_http_host               Host name or IP address of the device HTTP server, used to
   *                                        import the settings. If empty, the settings are imported
   *                                        through the sbgECom API tunnel.
   */
  explicit SettingsApplier(RestApiClient &ref_client, const std::string &ref_http_host = "");

  //---------------------------------------------------------------------//
  //- Operations                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Apply a sbgInsRestApi JSON settings file to the device.
   *
   * The file content is sent as is to the api/v1/settings/import endpoint, which
   * migrates the document to the firmware of the connected device. It must be a
   * complete export document, its settings object may hold either the full
   * settings or only the fields to update.
   *
   * The document is uploaded to the device HTTP server when a host has been given to
   * the constructor, and sent through the sbgECom API tunnel otherwise.
   *
   * The device saves the imported settings to its FLASH memory and reboots, which
   * leaves the communication interface closed, the caller is responsible for
   * reopening it.
   *
   * \param[in] ref_path                    Path to the JSON settings file.
   * \throw                                 Unable to read the file, or to apply the settings.
   */
  void applySettingsFile(const std::string& ref_path);

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
