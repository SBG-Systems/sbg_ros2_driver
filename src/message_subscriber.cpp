#include "message_subscriber.h"

// ROS headers
#include <rclcpp/rclcpp.hpp>

// SBG headers
#include <interfaces/sbgInterface.h>

using sbg::MessageSubscriber;

/*!
 * Class to subscribe to all corresponding topics.
 */
//---------------------------------------------------------------------//
//- Constructor                                                       -//
//---------------------------------------------------------------------//

MessageSubscriber::MessageSubscriber():
m_max_messages_(10)
{
}

//---------------------------------------------------------------------//
//- Private methods                                                   -//
//---------------------------------------------------------------------//

void MessageSubscriber::readRosRtcmMessage(SbgInterface &sbg_interface, const mavros_msgs::msg::RTCM::SharedPtr msg) const
{
    auto rtcm_data = msg->data;
    auto error_code = sbgInterfaceWrite(&sbg_interface, rtcm_data.data(), rtcm_data.size());
    if (error_code != SBG_NO_ERROR)
    {
        char error_str[256];

        sbgEComErrorToString(error_code, error_str);
        SBG_LOG_ERROR(SBG_ERROR, "Failed to sent RTCM data to device: %s", error_str);
    }

}

//---------------------------------------------------------------------//
//- Operations                                                        -//
//---------------------------------------------------------------------//

void MessageSubscriber::initTopicSubscriptions(rclcpp::Node& ref_ros_node_handle,
                                               SbgInterface &sbg_interface,
                                               const ConfigStore &ref_config_store)
{
    auto rtcm_cb = [&](const mavros_msgs::msg::RTCM::SharedPtr msg) -> void {
        this->readRosRtcmMessage(sbg_interface, msg);
    };

    if (ref_config_store.shouldListenRtcm())
    {
        m_rtcm_sub_ = ref_ros_node_handle.create_subscription<mavros_msgs::msg::RTCM>(
                ref_config_store.getRtcmFullTopic(), m_max_messages_, rtcm_cb);
    }
}
