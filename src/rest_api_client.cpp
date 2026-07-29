// File header
#include "rest_api_client.h"

// ROS headers
#include <rclcpp/rclcpp.hpp>

// External headers
#include <nlohmann/json.hpp>

// SbgECom headers
#include <commands/sbgEComCmdApi.h>

using sbg::RestApiClient;

namespace
{
/*!
 * RAII holder for a sbgInsRestApi reply.
 *
 * The reply has to be constructed before being filled and destroyed before the
 * next attempt to receive data, either logs or command replies.
 */
class ReplyGuard
{
public:

  ReplyGuard()
  {
    sbgEComCmdApiReplyConstruct(&reply_);
  }

  ~ReplyGuard()
  {
    sbgEComCmdApiReplyDestroy(&reply_);
  }

  ReplyGuard(const ReplyGuard&) = delete;
  ReplyGuard& operator=(const ReplyGuard&) = delete;

  SbgEComCmdApiReply *get()
  {
    return &reply_;
  }

  /*!
   * Copy the reply into an owning structure, so that it survives the guard.
   *
   * \param[in] error_code                  Error code returned by the request.
   * \return                                Owning copy of the reply.
   */
  RestApiClient::Reply toReply(SbgErrorCode error_code) const
  {
    RestApiClient::Reply reply;

    reply.error_code  = error_code;
    reply.status_code = reply_.statusCode;
    reply.content     = (reply_.pContent ? reply_.pContent : "");
    reply.ok          = (error_code == SBG_NO_ERROR) && sbgEComCmdApiReplySuccessful(&reply_);

    return reply;
  }

private:

  SbgEComCmdApiReply  reply_;
};
}

/*!
 * Class to send sbgInsRestApi requests to the device.
 */
//---------------------------------------------------------------------//
//- Constructor                                                       -//
//---------------------------------------------------------------------//

RestApiClient::RestApiClient(SbgEComHandle &ref_sbg_com_handle):
ref_sbg_com_handle_(ref_sbg_com_handle)
{
}

//---------------------------------------------------------------------//
//- Private  methods                                                  -//
//---------------------------------------------------------------------//

void RestApiClient::logErrorDetails(const std::string& ref_path, const Reply& ref_reply) const
{
  const auto logger = rclcpp::get_logger("RestApi");

  if (ref_reply.error_code != SBG_NO_ERROR)
  {
    RCLCPP_ERROR(logger, "SBG_DRIVER - [RestApi] %s failed - %s", ref_path.c_str(), sbgErrorCodeToString(ref_reply.error_code));
    return;
  }

  //
  // The device reports errors as a JSON document, fall back to the raw content if it can't be parsed.
  //
  const auto json = nlohmann::json::parse(ref_reply.content, nullptr, false);

  if (json.is_discarded() || !json.is_object())
  {
    RCLCPP_ERROR(logger, "SBG_DRIVER - [RestApi] %s returned status %u - %s", ref_path.c_str(), ref_reply.status_code, ref_reply.content.c_str());
    return;
  }

  //
  // Read the fields defensively, this is already an error path and a malformed reply should
  // not hide the original error.
  //
  const auto readString = [&json](const char *p_key) -> std::string
  {
    const auto it = json.find(p_key);

    return ((it != json.end()) && it->is_string()) ? it->get<std::string>() : std::string();
  };

  RCLCPP_ERROR(logger, "SBG_DRIVER - [RestApi] %s returned status %u - %s - %s", ref_path.c_str(), ref_reply.status_code, readString("title").c_str(), readString("detail").c_str());
}

//---------------------------------------------------------------------//
//- Operations                                                        -//
//---------------------------------------------------------------------//

RestApiClient::Reply RestApiClient::get(const std::string& ref_path, const std::string& ref_query)
{
  ReplyGuard    guard;
  SbgErrorCode  error_code;

  error_code = sbgEComCmdApiGet(&ref_sbg_com_handle_, ref_path.c_str(), ref_query.empty() ? nullptr : ref_query.c_str(), guard.get());

  Reply reply = guard.toReply(error_code);

  if (!reply.ok)
  {
    logErrorDetails(ref_path, reply);
  }

  return reply;
}

RestApiClient::Reply RestApiClient::post(const std::string& ref_path, const std::string& ref_query, const std::string& ref_body)
{
  ReplyGuard    guard;
  SbgErrorCode  error_code;

  error_code = sbgEComCmdApiPost(&ref_sbg_com_handle_, ref_path.c_str(), ref_query.empty() ? nullptr : ref_query.c_str(), ref_body.empty() ? nullptr : ref_body.c_str(), guard.get());

  Reply reply = guard.toReply(error_code);

  if (!reply.ok)
  {
    logErrorDetails(ref_path, reply);
  }

  return reply;
}

void RestApiClient::purgeIncoming()
{
  sbgEComPurgeIncoming(&ref_sbg_com_handle_);
}
