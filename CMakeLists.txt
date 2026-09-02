cmake_minimum_required(VERSION 3.5)
project(sbg_driver)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(rtcm_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_msgs REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(nmea_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(CURL REQUIRED)

## Build the deprecated sbgECom command device configuration path.
## Only needed for devices that don't support the sbgInsRestApi, such as ELLIPSE firmware
## v2 and before. This option will default to OFF then be removed in a future release.
option(SBG_USE_DEPRECATED_ECOM_CONFIG "Build the deprecated sbgECom command device configuration path" ON)

################################################
## Declare ROS messages, services and actions ##
################################################

## Generate messages in the 'msg' folder
set(USED_LIBRARIES
  rclcpp::rclcpp
  ${sensor_msgs_TARGETS}
  ${std_msgs_TARGETS}
  ${std_srvs_TARGETS}
  ${geometry_msgs_TARGETS}
  ${nav_msgs_TARGETS}
  ${rtcm_msgs_TARGETS}
  tf2::tf2
  ${tf2_msgs_TARGETS}
  ${tf2_geometry_msgs_TARGETS}
  ${nmea_msgs_TARGETS}
  nlohmann_json::nlohmann_json
  CURL::libcurl
)

set(msg_files
  "msg/SbgGpsVelStatus.msg"
  "msg/SbgGpsPosStatus.msg"
  "msg/SbgMagStatus.msg"
  "msg/SbgShipMotionStatus.msg"
  "msg/SbgUtcTimeStatus.msg"
  "msg/SbgAirData.msg"
  "msg/SbgAirDataStatus.msg"
  "msg/SbgEkfEuler.msg"
  "msg/SbgGpsPos.msg"
  "msg/SbgMag.msg"
  "msg/SbgStatusCom.msg"
  "msg/SbgEkfNav.msg"
  "msg/SbgGpsRaw.msg"
  "msg/SbgOdoVel.msg"
  "msg/SbgStatusGeneral.msg"
  "msg/SbgEkfQuat.msg"
  "msg/SbgGpsVel.msg"
  "msg/SbgStatus.msg"
  "msg/SbgEvent.msg"
  "msg/SbgImuStatus.msg"
  "msg/SbgImuData.msg"
  "msg/SbgImuShort.msg"
  "msg/SbgShipMotion.msg"
  "msg/SbgUtcTime.msg"
  "msg/SbgGpsHdt.msg"
  "msg/SbgMagCalib.msg"
  "msg/SbgStatusAiding.msg"
  "msg/SbgEkfStatus.msg"
  "msg/SbgEkfVelBody.msg"
  "msg/SbgEkfRotAccel.msg"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES
    std_msgs
    geometry_msgs
    builtin_interfaces
)

ament_export_dependencies(rosidl_default_runtime)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
add_subdirectory(external/sbgECom)

include_directories(
  ${PROJECT_SOURCE_DIR}/include/sbg_driver/
  ${PROJECT_SOURCE_DIR}/external/sbgECom/common
  ${PROJECT_SOURCE_DIR}/external/sbgECom/src
)

## Define common resources.
set(SBG_COMMON_RESOURCES
  src/mag_calibration.cpp
  src/message_publisher.cpp
  src/message_wrapper.cpp
  src/config_store.cpp
  src/rest_api_client.cpp
  src/sbg_device.cpp
  src/sbg_utm.cpp
  src/sbg_ros_helpers.cpp
  src/settings_applier.cpp
)

if(SBG_USE_DEPRECATED_ECOM_CONFIG)
  list(APPEND SBG_COMMON_RESOURCES src/config_applier.cpp)
endif()

rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")

## Declare the driver library, shared by the executables and the tests
add_library(sbg_driver_core STATIC ${SBG_COMMON_RESOURCES})
add_dependencies(sbg_driver_core ${PROJECT_NAME})

## Specify libraries to link a library or executable target against
target_link_libraries(sbg_driver_core PUBLIC sbgECom)
target_link_libraries(sbg_driver_core PUBLIC ${USED_LIBRARIES})
target_link_libraries(sbg_driver_core PUBLIC "${cpp_typesupport_target}")

if(SBG_USE_DEPRECATED_ECOM_CONFIG)
  target_compile_definitions(sbg_driver_core PUBLIC SBG_USE_DEPRECATED_ECOM_CONFIG)
endif()

set_property(TARGET sbg_driver_core PROPERTY CXX_STANDARD 14)

## Declare a C++ executable
add_executable(sbg_device src/main.cpp)
target_link_libraries(sbg_device sbg_driver_core)

add_executable(sbg_device_mag src/main_mag.cpp)
target_link_libraries(sbg_device_mag sbg_driver_core)

set_property(TARGET sbg_device PROPERTY CXX_STANDARD 14)
set_property(TARGET sbg_device_mag PROPERTY CXX_STANDARD 14)

## The warning policy only applies to the driver, not to the bundled sbgECom
if(WIN32)
target_compile_options(sbg_driver_core PRIVATE)
target_compile_options(sbg_device PRIVATE)
target_compile_options(sbg_device_mag PRIVATE)
else()
target_compile_options(sbg_driver_core PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(sbg_device PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(sbg_device_mag PRIVATE -Wall -Wextra -Wpedantic)
endif()

#############
## Install ##
#############

## Mark executables and/or libraries for installation
install(TARGETS sbg_device sbg_device_mag
  DESTINATION lib/${PROJECT_NAME}
)

## Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION include/${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h"
  PATTERN ".svn" EXCLUDE
  PATTERN ".git" EXCLUDE
)

# Mark other files for installation (e.g. launch and bag files, etc.)
install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
  PATTERN ".git" EXCLUDE)

install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
  PATTERN ".git" EXCLUDE)

# Install URDF and meshes
install(DIRECTORY urdf/
  DESTINATION share/${PROJECT_NAME}/urdf
)

install(DIRECTORY meshes/
  DESTINATION share/${PROJECT_NAME}/meshes
)

###################################
## ament specific configuration  ##
###################################

ament_export_dependencies(
    rclcpp
    sensor_msgs
    std_msgs
    std_srvs
    geometry_msgs
    nav_msgs
    rtcm_msgs
    tf2_ros
    tf2_msgs
    tf2_geometry_msgs
    nmea_msgs
    nlohmann_json
    rosidl_default_runtime
)

ament_export_include_directories(
    include
)

#############
## Testing ##
#############

## ament_uncrustify, ament_cpplint and ament_copyright are not enabled yet: the driver
## sources predate the ament code style and turning them on requires a dedicated
## reformatting change.
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_index_cpp REQUIRED)
  find_package(launch_testing_ament_cmake REQUIRED)

  ## external/sbgECom is bundled upstream code and is not part of the ROS driver's lint scope.
  ## The linters match exclusions per file, so the whole subtree is listed explicitly.
  file(GLOB_RECURSE _external_files "${CMAKE_CURRENT_SOURCE_DIR}/external/*")
  set(AMENT_LINT_AUTO_FILE_EXCLUDE ${_external_files})

  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(test_sbg_ros_helpers test/test_sbg_ros_helpers.cpp)
  target_link_libraries(test_sbg_ros_helpers sbg_driver_core)

  ament_add_gtest(test_sbg_utm test/test_sbg_utm.cpp)
  target_link_libraries(test_sbg_utm sbg_driver_core)

  ament_add_gtest(test_sbg_stream_buffer test/test_sbg_stream_buffer.cpp)
  target_link_libraries(test_sbg_stream_buffer sbgECom)

  ament_add_gtest(test_message_wrapper test/test_message_wrapper.cpp)
  target_link_libraries(test_message_wrapper sbg_driver_core)

  ament_add_gtest(test_config_store test/test_config_store.cpp)
  target_link_libraries(test_config_store sbg_driver_core)

  ## Check that launch, config, urdf and meshes resources are actually installed
  ament_add_gtest(test_install_resources test/test_install_resources.cpp)
  target_link_libraries(test_install_resources ament_index_cpp::ament_index_cpp)

  ## The integration test replays a recorded sbgECom stream through the sbg_device
  ## executable. Its resources are installed so that the test resolves them from the
  ## install space rather than from the source tree.
  install(DIRECTORY test/config test/data
    DESTINATION share/${PROJECT_NAME}/test
  )

  add_launch_test(test/test_replay_launch.py TIMEOUT 240)
endif()

ament_package()
