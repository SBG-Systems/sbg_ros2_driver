/*!
*  \file         rest_api_client.h
*  \author       SBG Systems
*  \date         29/07/2026
*
*  \brief        Access the device sbgInsRestApi over the sbgECom protocol.
*
*  Thin wrapper around the sbgEComCmdApiGet / sbgEComCmdApiPost commands, which
*  encapsulate the sbgInsRestApi in the sbgECom binary protocol. The same
*  interface is used for serial and UDP connections.
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

#ifndef SBG_ROS_REST_API_CLIENT_H
#define SBG_ROS_REST_API_CLIENT_H

// Standard headers
#include <string>

// SbgECom headers
#include <sbgEComLib.h>

namespace sbg
{
/*!
 * Class to send sbgInsRestApi requests to the device.
 */
class RestApiClient
{
public:

  /*!
   * Reply to a sbgInsRestApi request.
   */
  struct Reply
  {
    SbgErrorCode    error_code;   /*!< Transport level error code, SBG_NO_ERROR if the reply has been received. */
    uint16_t        status_code;  /*!< Reply status code, based on the HTTP status codes (200 is OK). */
    std::string     content;      /*!< Reply content, normally a JSON document. */
    bool            ok;           /*!< True if the request has been received and executed successfully. */
  };

private:

  SbgEComHandle&  ref_sbg_com_handle_;

  //---------------------------------------------------------------------//
  //- Private  methods                                                  -//
  //---------------------------------------------------------------------//

  /*!
   * Log the details of an unsuccessful reply.
   *
   * The sbgInsRestApi reports errors as a JSON document holding the status, title
   * and detail fields. The raw content is logged if it can't be parsed.
   *
   * \param[in] ref_path                    Path the request has been sent to.
   * \param[in] ref_reply                   Reply to log the error details of.
   */
  void logErrorDetails(const std::string& ref_path, const Reply& ref_reply) const;

public:

  //---------------------------------------------------------------------//
  //- Constructor                                                       -//
  //---------------------------------------------------------------------//

  /*!
   * Default constructor.
   *
   * \param[in] ref_sbg_com_handle          SBG communication handle.
   */
  explicit RestApiClient(SbgEComHandle &ref_sbg_com_handle);

  //---------------------------------------------------------------------//
  //- Operations                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Send a GET request to the device.
   *
   * \param[in] ref_path                    URI path component, for example api/v1/info.
   * \param[in] ref_query                   Query string, may be empty.
   * \return                                Reply from the device.
   */
  Reply get(const std::string& ref_path, const std::string& ref_query = "");

  /*!
   * Send a POST request to the device.
   *
   * \param[in] ref_path                    URI path component, for example api/v1/settings.
   * \param[in] ref_query                   Query string, may be empty.
   * \param[in] ref_body                    Request body, may be empty.
   * \return                                Reply from the device.
   */
  Reply post(const std::string& ref_path, const std::string& ref_query = "", const std::string& ref_body = "");

  /*!
   * Discard any data pending on the interface.
   *
   * Should be called before the first request when the device may already be
   * streaming logs, so that the reply is not preceded by a partial frame.
   */
  void purgeIncoming();
};
}

#endif // SBG_ROS_REST_API_CLIENT_H
