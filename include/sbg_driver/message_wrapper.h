/*!
*	\file         message_wrapper.h
*	\author       SBG Systems
*	\date         13/03/2020
*
*	\brief        Handle creation of messages.
*
*   Methods to create ROS messages from given data.
*
*	\section CodeCopyright Copyright Notice
*	MIT License
*
*	Copyright (c) 2020 SBG Systems
*
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	SOFTWARE.
*/

#ifndef SBG_ROS_MESSAGE_WRAPPER_H
#define SBG_ROS_MESSAGE_WRAPPER_H

// SbgECom headers
#include <sbgEComLib.h>
#include <sbgEComIds.h>

// Sbg header
#include <sbg_matrix3.h>
#include <config_store.h>

// ROS headers
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/temperature.hpp>
#include <sensor_msgs/msg/magnetic_field.hpp>
#include <sensor_msgs/msg/fluid_pressure.hpp>
#include <sensor_msgs/msg/time_reference.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <nav_msgs/msg/odometry.hpp>
#include <nmea_msgs/msg/sentence.hpp>

// SbgRos message headers
#include "sbg_driver/msg/sbg_status.hpp"
#include "sbg_driver/msg/sbg_utc_time.hpp"
#include "sbg_driver/msg/sbg_imu_data.hpp"
#include "sbg_driver/msg/sbg_ekf_euler.hpp"
#include "sbg_driver/msg/sbg_ekf_quat.hpp"
#include "sbg_driver/msg/sbg_ekf_nav.hpp"
#include "sbg_driver/msg/sbg_ship_motion.hpp"
#include "sbg_driver/msg/sbg_mag.hpp"
#include "sbg_driver/msg/sbg_mag_calib.hpp"
#include "sbg_driver/msg/sbg_gps_vel.hpp"
#include "sbg_driver/msg/sbg_gps_pos.hpp"
#include "sbg_driver/msg/sbg_gps_hdt.hpp"
#include "sbg_driver/msg/sbg_gps_raw.hpp"
#include "sbg_driver/msg/sbg_odo_vel.hpp"
#include "sbg_driver/msg/sbg_event.hpp"
#include "sbg_driver/msg/sbg_imu_short.hpp"
#include "sbg_driver/msg/sbg_air_data.hpp"

namespace sbg
{
typedef struct _UTM0
{
	double			easting;
	double			northing;
	double			altitude;
	int				zone;
} UTM0;

/*!
 * Class to wrap the SBG logs into ROS messages.
 */
class MessageWrapper : public rclcpp::Node
{
public:
  const int32_t                       DEFAULT_UTC_OFFSET  = 18;         /*!< Driver default GPS to UTC offset in seconds. */

  /*!
  * Standard NMEA GGA quality indicator value.
  */
  enum class NmeaGGAQuality: int32_t
  {
    INVALID           = 0,
    SINGLE            = 1,
    DGPS              = 2,
    PPS               = 3,
    RTK_FIXED         = 4,
    RTK_FLOAT         = 5,
    DEAD_RECKONING    = 6,
    STATIC_POSITION   = 7,
    SIMULATED         = 8,
  };

private:
  sbg_driver::msg::SbgUtcTime  	      m_last_sbg_utc_;
  bool                                m_first_valid_utc_;
  std::string                         m_frame_id_;
  bool                                m_use_enu_;
  TimeReference                       m_time_reference_;
  UTM0					              m_utm0_;

  bool                                m_odom_enable_;
  bool                                m_odom_publish_tf_;
  std::string                         m_odom_frame_id_;
  std::string                         m_odom_base_frame_id_;
  std::string                         m_odom_init_frame_id_;

  //---------------------------------------------------------------------//
  //- Internal methods                                                  -//
  //---------------------------------------------------------------------//

  /*!
   * Wrap an angle between [ -Pi ; Pi ] rad.
   *
   * \param[in] angle_rad			Angle in rad.
   * \return						Wrapped angle.
   */
  static float wrapAnglePi(float angle_rad);

  /*!
   * Wrap an angle between [ 0 ; 360 ] degree.
   *
   * \param[in] angle_deg			Angle in degree.
   * \return						Wrapped angle.
   */
  static float wrapAngle360(float angle_deg);

  /*!
   * Compute UTM zone meridian.
   *
   * \param[in] zone_number			UTM Zone number.
   * \return						Meridian angle, in degrees.
   */
  static double computeMeridian(int zone_number);

  /*!
   * Create a ROS message header.
   * 
   * \param[in] device_timestamp    SBG device timestamp (in microseconds).
   * \return                        ROS header message.
   */
  const std_msgs::msg::Header createRosHeader(uint32_t device_timestamp) const;

  /*!
   * Convert INS timestamp from a SBG device to UNIX timestamp.
   * 
   * \param[in] device_timestamp    SBG device timestamp (in microseconds).
   * \return                        ROS time.
   */
  const rclcpp::Time convertInsTimeToUnix(uint32_t device_timestamp) const;

  /*!
   * Create SBG-ROS Ekf status message.
   * 
   * \param[in] ekf_status          SBG Ekf status.
   * \return                        Ekf status message.
   */
  const sbg_driver::msg::SbgEkfStatus createEkfStatusMessage(uint32_t ekf_status) const;

  /*!
   * Create SBG-ROS GPS Position status message.
   * 
   * \param[in] ref_log_gps_pos     SBG GPS position log.
   * \return                        GPS Position status.
   */
  const sbg_driver::msg::SbgGpsPosStatus createGpsPosStatusMessage(const SbgLogGpsPos& ref_log_gps_pos) const;

  /*!
   * Create SBG-ROS GPS Velocity status message.
   * 
   * \param[in] ref_log_gps_vel     SBG GPS Velocity log.
   * \return                        GPS Velocity status.
   */
  const sbg_driver::msg::SbgGpsVelStatus createGpsVelStatusMessage(const SbgLogGpsVel& ref_log_gps_vel) const;

  /*!
   * Create a SBG-ROS IMU status message.
   * 
   * \param[in] sbg_imu_status      SBG IMU status.
   * \return                        IMU status message.
   */
  const sbg_driver::msg::SbgImuStatus createImuStatusMessage(uint16_t sbg_imu_status) const;

  /*!
   * Create a SBG-ROS Magnetometer status message.
   * 
   * \param[in] ref_log_mag         SBG Magnetometer log.
   * \return                        Magnetometer status message.
   */
  const sbg_driver::msg::SbgMagStatus createMagStatusMessage(const SbgLogMag& ref_log_mag) const;

  /*!
   * Create a SBG-ROS Ship motion status message.
   * 
   * \param[in] ref_log_ship_motion SBG Ship motion log.
   * \return                        ship motion status message.
   */
  const sbg_driver::msg::SbgShipMotionStatus createShipMotionStatusMessage(const SbgLogShipMotionData& ref_log_ship_motion) const;

  /*!
   * Create a SBG-ROS aiding status message.
   *
   * \param[in] ref_log_status      SBG status log.
   * \return                        Aiding status message.
   */
  const sbg_driver::msg::SbgStatusAiding createStatusAidingMessage(const SbgLogStatusData& ref_log_status) const;

  /*!
   * Create a SBG-ROS com status message.
   *
   * \param[in] ref_log_status      SBG status log.
   * \return                        Com status message.
   */
  const sbg_driver::msg::SbgStatusCom createStatusComMessage(const SbgLogStatusData& ref_log_status) const;

  /*!
   * Create a SBG-ROS general status message.
   *
   * \param[in] ref_log_status      SBG status log.
   * \return                        General status message.
   */
  const sbg_driver::msg::SbgStatusGeneral createStatusGeneralMessage(const SbgLogStatusData& ref_log_status) const;

  /*!
   * Create a SBG-ROS UTC time status message.
   * 
   * \param[in] ref_log_utc         SBG UTC data log.
   * \return                        UTC time status message.
   */
  const sbg_driver::msg::SbgUtcTimeStatus createUtcStatusMessage(const SbgLogUtcData& ref_log_utc) const;

  /*!
   * Get the number of days in the year.
   *
   * \param[in] year                Year to get the number of days.
   * \return                        Number of days in the year.
   */
  uint32_t getNumberOfDaysInYear(uint16_t year) const;

  /*!
   * Get the number of days of the month index.
   * 
   * \param[in] year                Year.
   * \param[in] month_index         Month index [1..12].
   * \return                        Number of days in the month.
   */
  uint32_t getNumberOfDaysInMonth(uint16_t year, uint8_t month_index) const;

  /*!
   * Check if the given year is a leap year.
   * 
   * \param[in] year                Year to check.
   * \return                        True if the year is a leap year.
   */
  bool isLeapYear(uint16_t year) const;

  /*!
   * Convert the UTC time to an Unix time.
   * 
   * \param[in] ref_sbg_utc_msg     UTC message.
   * \return                        Converted Epoch time (in s).
   */
  const rclcpp::Time convertUtcTimeToUnix(const sbg_driver::msg::SbgUtcTime& ref_sbg_utc_msg) const;
  
  /*!
   * Returns the GPS to UTC leap second offset: GPS_Time = UTC_Tme + utcOffset
   *
   * WARNING: The leap second is computed from the latest received SbgUtcTime message if any.
   *          If no SbgUtcTime message has been received, a default driver current value is used.
   * 
   * \return                        Offset in seconds to apply to UTC time to get GPS time.
   */
  int32_t getUtcOffset() const;

  /*!
   * Create a SBG-ROS air data status message.
   * 
   * \param[in] ref_sbg_air_data    SBG AirData log.
   * \return                        SBG-ROS air data status message.
   */
  const sbg_driver::msg::SbgAirDataStatus createAirDataStatusMessage(const SbgLogAirData& ref_sbg_air_data) const;
 
  /*!
   * Create a ROS standard TwistStamped message.
   *
   * \param[in] body_vel            SBG Body velocity vector.
   * \param[in] ref_sbg_air_data    SBG IMU message.
   * \return                        SBG TwistStamped message.
   */
  const geometry_msgs::msg::TwistStamped createRosTwistStampedMessage(const sbg::SbgVector3f& body_vel, const sbg_driver::msg::SbgImuData& ref_sbg_imu_msg) const;

  /*!
   * Fill a transformation.
   *
   * \param[in] ref_parent_frame_id     Parent frame ID.
   * \param[in] ref_child_frame_id      Child frame ID.
   * \param[in] ref_pose                Pose.
   * \param[out] ref_transform_stamped  Stamped transformation.
   */
  void fillTransform(const std::string &ref_parent_frame_id, const std::string &ref_child_frame_id, const geometry_msgs::msg::Pose &ref_pose, geometry_msgs::msg::TransformStamped &ref_transform_stamped);

  /*!
   * Get UTM letter designator for the given latitude.
   *
   * \param[in] Lat                     Latitude, in degrees.
   * \return                            UTM letter designator.
   */
  char UTMLetterDesignator(double Lat);

  /*!
   * Set UTM initial position.
   *
   * \param[in] Lat                     Latitude, in degrees.
   * \param[in] Long                    Longitude, in degrees.
   * \param[in] altitude                Altitude, in meters.
   */
  void initUTM(double Lat, double Long, double altitude);

  /*!
   * Convert latitude and longitude to a position relative to UTM initial position.
   *
   * \param[in] Lat                     Latitude, in degrees.
   * \param[in] Long                    Longitude, in degrees.
   * \param[in] zoneNumber              UTM zone number.
   * \param[out] UTMNorthing            UTM northing, in meters.
   * \param[out] UTMEasting             UTM easting, in meters.
   */
  void LLtoUTM(double Lat, double Long, int zoneNumber, double &UTMNorthing, double &UTMEasting) const;

  /*!
   * Convert SbgEComGpsPosType enum to NmeaGGAQuality enum
   *
   * \param[in] sbgGpsType              SbgECom GPS type
   * \return                            NMEA GPS type
   */
  static NmeaGGAQuality convertSbgGpsTypeToNmeaGpsType(SbgEComGpsPosType sbgGpsType);

public:

  //---------------------------------------------------------------------//
  //- Transform broadcasters                                            -//
  //---------------------------------------------------------------------//

  std::shared_ptr<tf2_ros::TransformBroadcaster> m_tf_broadcaster_;
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> m_static_tf_broadcaster_;

  //---------------------------------------------------------------------//
  //- Constructor                                                       -//
  //---------------------------------------------------------------------//

  /*!
   * Default constructor.
   */
  MessageWrapper();

  //---------------------------------------------------------------------//
  //- Parameters                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Set the time reference.
   * 
   * \param[in] time_reference    Time reference.
   */
  void setTimeReference(TimeReference time_reference);

  /*!
   * Set Frame ID.
   *
   * \param[in]  frame_id      Frame ID.
   */
  void setFrameId(const std::string &frame_id);

  /*!
   * Set use ENU.
   *
   * \param[in]  enu          If true publish data in the ENU convention.
   */
  void setUseEnu(bool enu);

  /*!
   * Set odom enable.
   *
   * \param[in] odom_enable		 If true enable odometry.
   */
   void setOdomEnable(bool odom_enable);

  /*!
   * Set odom publish_tf.
   *
   * \param[in] publish_tf		 If true publish odometry transforms.
   */
   void setOdomPublishTf(bool publish_tf);

  /*!
   * Set the odometry frame ID.
   *
   * \param[in] ref_frame_id     Odometry frame ID.
   */
  void setOdomFrameId(const std::string &ref_frame_id);

  /*!
   * Set the odometry base frame ID.
   *
   * \param[in] ref_frame_id     Odometry base frame ID.
   */
  void setOdomBaseFrameId(const std::string &ref_frame_id);

  /*!
   * Set the odometry init frame ID.
   *
   * \param[in] ref_frame_id     Odometry init frame ID.
   */
  void setOdomInitFrameId(const std::string &ref_frame_id);

  //---------------------------------------------------------------------//
  //- Operations                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Create a SBG-ROS Ekf Euler message.
   * 
   * \param[in] ref_log_ekf_euler   SBG Ekf Euler log.
   * \return                        Ekf Euler message.
   */
  const sbg_driver::msg::SbgEkfEuler createSbgEkfEulerMessage(const SbgLogEkfEulerData& ref_log_ekf_euler) const;

  /*!
   * Create a SBG-ROS Ekf Navigation message.
   * 
   * \param[in] ref_log_ekf_nav     SBG Ekf Navigation log.
   * \return                        Ekf Navigation message.
   */
  const sbg_driver::msg::SbgEkfNav createSbgEkfNavMessage(const SbgLogEkfNavData& ref_log_ekf_nav) const;

  /*!
   * Create a SBG-ROS Ekf Quaternion message.
   * 
   * \param[in] ref_log_ekf_quat    SBG Ekf Quaternion log.
   * \return                        Ekf Quaternion message.
   */
  const sbg_driver::msg::SbgEkfQuat createSbgEkfQuatMessage(const SbgLogEkfQuatData& ref_log_ekf_quat) const;

  /*!
   * Create a SBG-ROS event message.
   * 
   * \param[in] ref_log_event       SBG event log.
   * \return                        Event message.
   */
  const sbg_driver::msg::SbgEvent createSbgEventMessage(const SbgLogEvent& ref_log_event) const;

  /*!
   * Create SBG-ROS GPS-HDT message.
   * 
   * \param[in] ref_log_gps_hdt     SBG GPS HDT log.
   * \return                        GPS HDT message.
   */
  const sbg_driver::msg::SbgGpsHdt createSbgGpsHdtMessage(const SbgLogGpsHdt& ref_log_gps_hdt) const;

  /*!
   * Create a SBG-ROS GPS-Position message.
   * 
   * \param[in] ref_log_gps_pos     SBG GPS Position log.
   * \return                        GPS Position message.
   */
  const sbg_driver::msg::SbgGpsPos createSbgGpsPosMessage(const SbgLogGpsPos& ref_log_gps_pos) const;

  /*!
   * Create a SBG-ROS GPS raw message.
   * 
   * \param[in] ref_log_gps_raw     SBG GPS raw log.
   * \return                        GPS raw message.
   */
  const sbg_driver::msg::SbgGpsRaw createSbgGpsRawMessage(const SbgLogGpsRaw& ref_log_gps_raw) const;

  /*!
   * Create a SBG-ROS GPS Velocity message.
   * 
   * \param[in] ref_log_gps_vel     SBG GPS Velocity log.
   * \return                        GPS Velocity message.
   */
  const sbg_driver::msg::SbgGpsVel createSbgGpsVelMessage(const SbgLogGpsVel& ref_log_gps_vel) const;

  /*!
   * Create a SBG-ROS Imu data message.
   * 
   * \param[in] ref_log_imu_data    SBG Imu data log.
   * \return                        Imu data message.
   */
  const sbg_driver::msg::SbgImuData createSbgImuDataMessage(const SbgLogImuData& ref_log_imu_data) const;

  /*!
   * Create a SBG-ROS Magnetometer message.
   * 
   * \param[in] ref_log_mag         SBG Magnetometer log.
   * \return                        Magnetometer message.
   */
  const sbg_driver::msg::SbgMag createSbgMagMessage(const SbgLogMag& ref_log_mag) const;

  /*!
   * Create a SBG-ROS Magnetometer calibration message.
   * 
   * \param[in] ref_log_mag_calib   SBG Magnetometer calibration log.
   * \return                        Magnetometer calibration message.
   */
  const sbg_driver::msg::SbgMagCalib createSbgMagCalibMessage(const SbgLogMagCalib& ref_log_mag_calib) const;

  /*!
   * Create a SBG-ROS Odometer velocity message.
   * 
   * \param[in] ref_log_odo         SBG Odometer log.
   * \return                        Odometer message.
   */
  const sbg_driver::msg::SbgOdoVel createSbgOdoVelMessage(const SbgLogOdometerData& ref_log_odo) const;

  /*!
   * Create a SBG-ROS Shipmotion message.
   * 
   * \param[in] ref_log_ship_motion SBG Ship motion log.
   * \return                        Ship motion message.
   */
  const sbg_driver::msg::SbgShipMotion createSbgShipMotionMessage(const SbgLogShipMotionData& ref_log_ship_motion) const;

  /*!
   * Create a SBG-ROS status message from a SBG status log.
   *
   * \param[in] ref_log_status      SBG status log.
   * \return                        Status message.
   */
  const sbg_driver::msg::SbgStatus createSbgStatusMessage(const SbgLogStatusData& ref_log_status) const;

  /*!
   * Create a SBG-ROS UTC time message from a SBG UTC log.
   *
   * \param[in] ref_log_utc         SBG UTC log.
   * \return                        UTC time message.                  
   */
  const sbg_driver::msg::SbgUtcTime createSbgUtcTimeMessage(const SbgLogUtcData& ref_log_utc);

  /*!
   * Create a SBG-ROS Air data message from a SBG log.
   * 
   * \param[in] ref_air_data_log    SBG AirData log.
   * \return                        SBG-ROS airData message.
   */
  const sbg_driver::msg::SbgAirData createSbgAirDataMessage(const SbgLogAirData& ref_air_data_log) const;

  /*!
   * Create a SBG-ROS Short Imu message.
   * 
   * \param[in] ref_short_imu_log   SBG Imu short log.
   * \return                        SBG-ROS Imu short message.
   */
  const sbg_driver::msg::SbgImuShort createSbgImuShortMessage(const SbgLogImuShort& ref_short_imu_log) const;

  /*!
   * Create a ROS standard IMU message from SBG messages.
   * 
   * \param[in] ref_sbg_imu_msg     SBG-ROS IMU message.
   * \param[in] ref_sbg_quat_msg    SBG_ROS Quaternion message.
   * \return                        ROS standard IMU message.
   */
  const sensor_msgs::msg::Imu createRosImuMessage(const sbg_driver::msg::SbgImuData& ref_sbg_imu_msg, const sbg_driver::msg::SbgEkfQuat& ref_sbg_quat_msg) const;

  /*!
   * Create a ROS standard odometry message from SBG messages.
   *
   * \param[in] ref_sbg_imu_msg         SBG-ROS IMU message.
   * \param[in] ref_sbg_ekf_nav_msg     SBG-ROS Ekf Nav message.
   * \param[in] ref_sbg_ekf_quat_msg    SBG-ROS Ekf Quaternion message.
   * \param[in] ref_sbg_ekf_euler_msg   SBG-ROS Ekf Euler message.
   * \return                            ROS standard odometry message.
   */
  const nav_msgs::msg::Odometry createRosOdoMessage(const sbg_driver::msg::SbgImuData &ref_sbg_imu_msg, const sbg_driver::msg::SbgEkfNav &ref_sbg_ekf_nav_msg, const sbg_driver::msg::SbgEkfQuat &ref_sbg_ekf_quat_msg, const sbg_driver::msg::SbgEkfEuler &ref_sbg_ekf_euler_msg);

  /*!
   * Create a ROS standard odometry message from SBG messages.
   *
   * \param[in] ref_sbg_imu_msg         SBG-ROS IMU message.
   * \param[in] ref_sbg_ekf_nav_msg     SBG-ROS Ekf Nav message.
   * \param[in] ref_sbg_ekf_euler_msg   SBG-ROS Ekf Euler message.
   * \return                            ROS standard odometry message.
   */
  const nav_msgs::msg::Odometry createRosOdoMessage(const sbg_driver::msg::SbgImuData &ref_sbg_imu_msg, const sbg_driver::msg::SbgEkfNav &ref_sbg_ekf_nav_msg, const sbg_driver::msg::SbgEkfEuler &ref_sbg_ekf_euler_msg);

  /*!
   * Create a ROS standard odometry message from SBG messages and tf2 quaternion.
   *
   * \param[in] ref_sbg_imu_msg         SBG-ROS IMU message.
   * \param[in] ref_sbg_ekf_nav_msg     SBG-ROS Ekf Nav message.
   * \param[in] orientation             Orientation as a Tf2 quaternion.
   * \param[in] ref_sbg_ekf_euler_msg   SBG-ROS Ekf Euler message.
   * \return                            ROS standard odometry message.
   */
  const nav_msgs::msg::Odometry createRosOdoMessage(const sbg_driver::msg::SbgImuData &ref_sbg_imu_msg, const sbg_driver::msg::SbgEkfNav &ref_sbg_ekf_nav_msg, const tf2::Quaternion &ref_orientation, const sbg_driver::msg::SbgEkfEuler &ref_sbg_ekf_euler_msg);

  /*!
   * Create a ROS standard Temperature message from SBG message.
   * 
   * \param[in] ref_sbg_imu_msg     SBG-ROS IMU message.
   * \return                        ROS standard Temperature message.
   */
  const sensor_msgs::msg::Temperature createRosTemperatureMessage(const sbg_driver::msg::SbgImuData& ref_sbg_imu_msg) const;

  /*!
   * Create a ROS standard MagneticField message from SBG message.
   * 
   * \param[in] ref_sbg_mag_msg     SBG-ROS Mag message.
   * \return                        ROS standard Mag message.
   */
  const sensor_msgs::msg::MagneticField createRosMagneticMessage(const sbg_driver::msg::SbgMag& ref_sbg_mag_msg) const;

  /*!
   * Create a ROS standard TwistStamped message from SBG messages.
   * 
   * \param[in] ref_sbg_ekf_euler_msg   SBG-ROS Ekf Euler message.
   * \param[in] ref_sbg_ekf_nav_msg     SBG-ROS Ekf Nav message.
   * \param[in] ref_sbg_imu_msg     SBG-ROS IMU message.
   * \return                        ROS standard TwistStamped message.
   */
  const geometry_msgs::msg::TwistStamped createRosTwistStampedMessage(const sbg_driver::msg::SbgEkfEuler& ref_sbg_ekf_euler_msg, const sbg_driver::msg::SbgEkfNav& ref_sbg_ekf_nav_msg, const sbg_driver::msg::SbgImuData& ref_sbg_imu_msg) const;

  /*!
   * Create a ROS standard TwistStamped message from SBG messages.
   *
   * \param[in] ref_sbg_ekf_quat_msg    SBG-ROS Ekf Quaternion message.
   * \param[in] ref_sbg_ekf_nav_msg     SBG-ROS Ekf Nav message.
   * \param[in] ref_sbg_imu_msg         SBG-ROS IMU message.
   * \return                            ROS standard TwistStamped message.
   */
  const geometry_msgs::msg::TwistStamped createRosTwistStampedMessage(const sbg_driver::msg::SbgEkfQuat& ref_sbg_ekf_vel_msg, const sbg_driver::msg::SbgEkfNav& ref_sbg_ekf_nav_msg, const sbg_driver::msg::SbgImuData& ref_sbg_imu_msg) const;

  /*!
   * Create a ROS standard PointStamped message from SBG messages.
   * 
   * \param[in] ref_sbg_ekf_msg     SBG-ROS EkfNav message.
   * \return                        ROS standard PointStamped message (ECEF).
   */
  const geometry_msgs::msg::PointStamped createRosPointStampedMessage(const sbg_driver::msg::SbgEkfNav& ref_sbg_ekf_msg) const;

  /*!
   * Create a ROS standard timeReference message for a UTC time.
   * 
   * \param[in] ref_sbg_utc_msg     SBG-ROS UTC message.
   * \return                        ROS standard timeReference message.
   */
  const sensor_msgs::msg::TimeReference createRosUtcTimeReferenceMessage(const sbg_driver::msg::SbgUtcTime& ref_sbg_utc_msg) const;

  /*!
   * Create a ROS standard NavSatFix message from a Gps message.
   * 
   * \param[in] ref_sbg_gps_msg     SBG-ROS GPS position message.
   * \return                        ROS standard NavSatFix message.
   */
  const sensor_msgs::msg::NavSatFix createRosNavSatFixMessage(const sbg_driver::msg::SbgGpsPos& ref_sbg_gps_msg) const;

  /*!
   * Create a ROS standard FluidPressure message.
   * 
   * \param[in] ref_sbg_air_msg     SBG-ROS AirData message.
   * \return                        ROS standard fluid pressure message.
   */
  const sensor_msgs::msg::FluidPressure createRosFluidPressureMessage(const sbg_driver::msg::SbgAirData& ref_sbg_air_msg) const;

  /*!
   * Create a ROS NMEA GGA message especially designed to support NTRIP VRS operations.
   *
   * This message is limited to 80 chars and only sent at up to 1 Hz to maximize VRS
   * providers compatibility.
   * 
   * WARNING: Don't use this GGA message for navigation purposes as the accuracy is limited.
   *
   * \param[in] ref_log_gps_pos     SBG GPS Position log.
   * \return                        ROS NMEA GGA message.
   */
    const nmea_msgs::msg::Sentence createNmeaGGAMessageForNtrip(const SbgLogGpsPos& ref_log_gps_pos) const;
};
}

#endif // SBG_ROS_MESSAGE_WRAPPER_H
