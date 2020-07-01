cmake_minimum_required(VERSION 3.5)
project(sbg_driver)

#
#find_package(catkin REQUIRED COMPONENTS
#  rclcpp
#  sensor_msgs
#  std_msgs
#  std_srvs
#  geometry_msgs
#  message_generation
#)


find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(Boost REQUIRED)

################################################
## Declare ROS messages, services and actions ##
################################################

## Generate messages in the 'msg' folder

set (USED_LIBRARIES
  rclcpp
  sensor_msgs
  std_msgs
  std_srvs
  geometry_msgs
)

set (msg_files
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
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES
    std_msgs
    geometry_msgs
    builtin_interfaces
)

ament_export_dependencies(rosidl_default_runtime)


## Generate added messages and services with any dependencies listed here
#generate_messages(DEPENDENCIES
#  std_msgs
#  geometry_msgs
#)


###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
add_subdirectory(external/sbgECom)

include_directories(
  #${catkin_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include/sbg_driver/
  ${PROJECT_SOURCE_DIR}/external/sbgECom/common
  ${PROJECT_SOURCE_DIR}/external/sbgECom/src
)

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(sbg ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Define common resources.
set (SBG_COMMON_RESOURCES
  src/config_applier.cpp
  src/message_publisher.cpp
  src/message_wrapper.cpp
  src/config_store.cpp
  src/sbg_device.cpp
)

## Declare a C++ executable
add_executable(sbg_device ${SBG_COMMON_RESOURCES} src/main.cpp)
#add_dependencies(sbg_device ${sbg_driver_EXPORTED_TARGETS})
add_dependencies(sbg_device ${PROJECT_NAME})
target_compile_options(sbg_device PRIVATE -Wall -Wextra)

add_executable(sbg_device_mag ${SBG_COMMON_RESOURCES} src/main_mag.cpp)
#add_dependencies(sbg_device_mag ${sbg_driver_EXPORTED_TARGETS})
add_dependencies(sbg_device_mag ${PROJECT_NAME})
target_compile_options(sbg_device_mag PRIVATE -Wall -Wextra)

## Specify libraries to link a library or executable target against
target_link_libraries(sbg_device ${catkin_LIBRARIES} sbgECom)
target_link_libraries(sbg_device_mag ${catkin_LIBRARIES} sbgECom)

ament_target_dependencies(sbg_device ${USED_LIBRARIES}) 
ament_target_dependencies(sbg_device_mag ${USED_LIBRARIES})

rosidl_target_interfaces(sbg_device ${PROJECT_NAME} "rosidl_typesupport_cpp")
rosidl_target_interfaces(sbg_device_mag ${PROJECT_NAME} "rosidl_typesupport_cpp")

set_property(TARGET sbg_device PROPERTY CXX_STANDARD 14)
set_property(TARGET sbg_device_mag PROPERTY CXX_STANDARD 14)

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executables and/or libraries for installation
install(TARGETS sbg_device sbg_device_mag
   DESTINATION lib/${PROJECT_NAME}
)

## Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION include/${PROJECT_NAME}  #${CATKIN_PACKAGE_INCLUDE_DESTINATION}
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


###################################
## catkin specific configuration ##
###################################

#catkin_package(
#  INCLUDE_DIRS src
#  #LIBRARIES ${PROJECT_NAME} 
#  CATKIN_DEPENDS 
#    message_runtime 
#    rclcpp
#    sensor_msgs
#    std_msgs
#    std_srvs
#    geometry_msgs
#)

ament_export_dependencies(
    rclcpp
    sensor_msgs
    std_msgs
    std_srvs
    geometry_msgs
    rosidl_default_runtime
)

ament_export_include_directories(
    include
)

ament_package()

#############
## Testing ##
#############
