// File header
#include <ament_index_cpp/get_package_share_directory.hpp>

// Test framework
#include <gtest/gtest.h>

// STL headers
#include <fstream>
#include <string>

namespace
{
  /*!
   * Check that a resource has been installed in the package share directory.
   */
  void expectInstalled(const std::string &ref_relative_path)
  {
    const std::string path = ament_index_cpp::get_package_share_directory("sbg_driver") + "/" + ref_relative_path;
    const std::ifstream file(path);

    EXPECT_TRUE(file.good()) << "missing installed resource: " << path;
  }
}

TEST(InstallResources, launchFilesAreInstalled)
{
  expectInstalled("launch/sbg_device_launch.py");
  expectInstalled("launch/sbg_device_mag_calibration_launch.py");
}

TEST(InstallResources, configurationFilesAreInstalled)
{
  expectInstalled("config/sbg_device_uart_default.yaml");
  expectInstalled("config/sbg_device_udp_default.yaml");
  expectInstalled("config/sbg_device_file_default.yaml");

  //
  // The config directory is installed recursively, examples and INS settings included.
  //
  expectInstalled("config/example/ellipse_A_default.yaml");
  expectInstalled("config/settings/ellipse_example.json");
}

TEST(InstallResources, urdfAndMeshesAreInstalled)
{
  expectInstalled("urdf/ELLIPSE-A.urdf");
  expectInstalled("urdf/ELLIPSE-E.urdf");

  expectInstalled("meshes/ELLIPSE-A.stl");
  expectInstalled("meshes/ELLIPSE-E.stl");
}
