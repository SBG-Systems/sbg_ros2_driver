// File header
#include <config_store.h>

// Test framework
#include <gtest/gtest.h>

// ROS headers
#include <rclcpp/rclcpp.hpp>

// STL headers
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
  class ConfigStoreTest : public ::testing::Test
  {
  protected:
    static void SetUpTestSuite()
    {
      rclcpp::init(0, nullptr);
    }

    static void TearDownTestSuite()
    {
      rclcpp::shutdown();
    }

    /*!
     * Create a node exposing the given parameters, as a launch file would do.
     */
    static std::shared_ptr<rclcpp::Node> createNode(const std::vector<rclcpp::Parameter> &ref_parameters)
    {
      rclcpp::NodeOptions options;

      options.automatically_declare_parameters_from_overrides(true);
      options.parameter_overrides(ref_parameters);

      return std::make_shared<rclcpp::Node>("config_store_test", options);
    }
  };
}

TEST_F(ConfigStoreTest, serialConfigurationIsLoaded)
{
  const auto node = createNode({
    rclcpp::Parameter("uartConf.portName", "/dev/ttyUSB0"),
    rclcpp::Parameter("uartConf.baudRate", 921600),
    rclcpp::Parameter("driver.frequency", 200),
    rclcpp::Parameter("output.use_enu", true),
    rclcpp::Parameter("output.frame_id", "imu_link"),
    rclcpp::Parameter("output.gps_frame_id", "gnss_link"),
    rclcpp::Parameter("output.time_reference", "ins_unix"),
  });

  sbg::ConfigStore config_store;

  config_store.loadFromRosNodeHandle(*node);

  EXPECT_TRUE(config_store.isInterfaceSerial());
  EXPECT_FALSE(config_store.isInterfaceUdp());
  EXPECT_FALSE(config_store.isInterfaceFile());

  EXPECT_EQ(config_store.getUartPortName(), "/dev/ttyUSB0");
  EXPECT_EQ(config_store.getBaudRate(), 921600u);
  EXPECT_EQ(config_store.getReadingRateFrequency(), 200u);

  EXPECT_TRUE(config_store.getUseEnu());
  EXPECT_EQ(config_store.getFrameId(), "imu_link");
  EXPECT_EQ(config_store.getGpsFrameId(), "gnss_link");
  EXPECT_EQ(config_store.getTimeReference(), sbg::TimeReference::INS_UNIX);
}

TEST_F(ConfigStoreTest, udpConfigurationIsLoaded)
{
  const auto node = createNode({
    rclcpp::Parameter("ipConf.ipAddress", "192.168.1.1"),
    rclcpp::Parameter("ipConf.out_port", 1234),
    rclcpp::Parameter("ipConf.in_port", 5678),
  });

  sbg::ConfigStore config_store;

  config_store.loadFromRosNodeHandle(*node);

  EXPECT_TRUE(config_store.isInterfaceUdp());
  EXPECT_FALSE(config_store.isInterfaceSerial());

  EXPECT_EQ(config_store.getOutputPortAddress(), 1234u);
  EXPECT_EQ(config_store.getInputPortAddress(), 5678u);
}

TEST_F(ConfigStoreTest, fileConfigurationIsLoaded)
{
  const auto node = createNode({
    rclcpp::Parameter("fileConf.path", "sbg_log.bin"),
  });

  sbg::ConfigStore config_store;

  config_store.loadFromRosNodeHandle(*node);

  EXPECT_TRUE(config_store.isInterfaceFile());
  EXPECT_EQ(config_store.getFile(), "sbg_log.bin");
}

TEST_F(ConfigStoreTest, optionalParametersFallBackToTheirDefaults)
{
  const auto node = createNode({
    rclcpp::Parameter("uartConf.portName", "/dev/ttyUSB0"),
  });

  sbg::ConfigStore config_store;

  config_store.loadFromRosNodeHandle(*node);

  EXPECT_EQ(config_store.getBaudRate(), 0u);
  EXPECT_TRUE(config_store.getFallbackBaudRates().empty());
  EXPECT_EQ(config_store.getReadingRateFrequency(), 400u);

  //
  // Without an explicit convention the driver keeps the NED frames.
  //
  EXPECT_FALSE(config_store.getUseEnu());
  EXPECT_EQ(config_store.getFrameId(), "imu_link_ned");
  EXPECT_EQ(config_store.getTimeReference(), sbg::TimeReference::ROS);

  //
  // The GNSS frame falls back to the IMU frame to keep the previous behavior.
  //
  EXPECT_EQ(config_store.getGpsFrameId(), config_store.getFrameId());

  EXPECT_FALSE(config_store.getOdomEnable());
  EXPECT_FALSE(config_store.getOdomPublishTf());
  EXPECT_EQ(config_store.getOdomFrameId(), "odom");
  EXPECT_EQ(config_store.getOdomBaseFrameId(), "base_link");
  EXPECT_EQ(config_store.getOdomInitFrameId(), "map");

  EXPECT_FALSE(config_store.shouldSubscribeToRtcm());
  EXPECT_EQ(config_store.getRtcmFullTopic(), "ntrip_client/rtcm");
  EXPECT_FALSE(config_store.shouldPublishNmea());
  EXPECT_EQ(config_store.getNmeaFullTopic(), "ntrip_client/nmea");

  EXPECT_FALSE(config_store.checkConfigWithRos());
  EXPECT_FALSE(config_store.checkRosStandardMessages());
  EXPECT_TRUE(config_store.getInsSettingsFile().empty());
  EXPECT_FALSE(config_store.hasLegacyInsParameters());
}

TEST_F(ConfigStoreTest, rtcmAndNmeaTopicsAreBuiltFromNamespaceAndName)
{
  const auto node = createNode({
    rclcpp::Parameter("uartConf.portName", "/dev/ttyUSB0"),
    rclcpp::Parameter("rtcm.subscribe", true),
    rclcpp::Parameter("rtcm.topic_name", "rtcm_stream"),
    rclcpp::Parameter("rtcm.namespace", "ntrip"),
    rclcpp::Parameter("nmea.publish", true),
    rclcpp::Parameter("nmea.topic_name", "gga"),
    rclcpp::Parameter("nmea.namespace", "ntrip"),
  });

  sbg::ConfigStore config_store;

  config_store.loadFromRosNodeHandle(*node);

  EXPECT_TRUE(config_store.shouldSubscribeToRtcm());
  EXPECT_EQ(config_store.getRtcmFullTopic(), "ntrip/rtcm_stream");

  EXPECT_TRUE(config_store.shouldPublishNmea());
  EXPECT_EQ(config_store.getNmeaFullTopic(), "ntrip/gga");
}

TEST_F(ConfigStoreTest, missingCommunicationInterfaceIsRejected)
{
  const auto node = createNode({
    rclcpp::Parameter("driver.frequency", 200),
  });

  sbg::ConfigStore config_store;

  EXPECT_ANY_THROW(config_store.loadFromRosNodeHandle(*node));
}

TEST_F(ConfigStoreTest, unknownTimeReferenceIsRejected)
{
  const auto node = createNode({
    rclcpp::Parameter("uartConf.portName", "/dev/ttyUSB0"),
    rclcpp::Parameter("output.time_reference", "gps"),
  });

  sbg::ConfigStore config_store;

  EXPECT_ANY_THROW(config_store.loadFromRosNodeHandle(*node));
}

TEST_F(ConfigStoreTest, legacyInsParametersAreDetected)
{
  const auto node = createNode({
    rclcpp::Parameter("uartConf.portName", "/dev/ttyUSB0"),
    rclcpp::Parameter("sensorParameters.motionProfile", 1),
  });

  sbg::ConfigStore config_store;

  config_store.loadFromRosNodeHandle(*node);

  EXPECT_TRUE(config_store.hasLegacyInsParameters());
}
