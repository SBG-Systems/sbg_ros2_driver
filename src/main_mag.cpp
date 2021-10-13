#include "sbg_device.h"

using sbg::SbgDevice;

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::Node::SharedPtr node_handle;

  try
  {
    RCLCPP_INFO(node_handle->get_logger(), "SBG DRIVER - Init node, load params and connect to the device");
    SbgDevice sbg_device(*node_handle);
    
    sbg_device.initDeviceForMagCalibration();
    
    rclcpp::spin(node_handle);

    return 0;
  }
  catch (std::exception const& refE)
  {
    RCLCPP_ERROR(node_handle->get_logger(), "SBG_DRIVER - [MagNode] Error - %s.", refE.what());
  }

  return 0;
}
