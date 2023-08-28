/*!
*	\file         sbg_helpers.h
*	\author       SBG Systems
*	\date         25/08/2023
*
*	\brief        Helpers for various tasks.
*
*   Helpers for various tasks.
*
*	\section CodeCopyright Copyright Notice
*	MIT License
*
*	Copyright (c) 2023 SBG Systems
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

#ifndef SBG_ROS_ROS_HELPERS_H
#define SBG_ROS_ROS_HELPERS_H

// SbgECom headers
#include <sbgEComLib.h>

// STL headers
#include <cstdint>

namespace sbg::helpers
{
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

  /*!
   * Convert latitude and longitude to a position relative to UTM initial position.
   *
   * \param[in] latitude                Latitude, in degrees.
   * \param[in] longitude               Longitude, in degrees.
   * \param[in] zone_number             UTM zone number.
   * \param[out] utm_northing           UTM northing, in meters.
   * \param[out] utm_easting            UTM easting, in meters.
   */
  void LLtoUTM(double latitude, double longitude, int zone_number, double &utm_northing, double &utm_easting);

  /*!
   * Get UTM letter designator for the given latitude.
   *
   * \param[in] latitude                Latitude, in degrees.
   * \return                            UTM letter designator.
   */
  char UTMLetterDesignator(double latitude);

  /*!
   * Wrap an angle between [ -Pi ; Pi ] rad.
   *
   * \param[in] angle_rad			Angle in rad.
   * \return						Wrapped angle.
   */
  float wrapAnglePi(float angle_rad);

  /*!
   * Wrap an angle between [ 0 ; 360 ] degree.
   *
   * \param[in] angle_deg			Angle in degree.
   * \return						Wrapped angle.
   */
  float wrapAngle360(float angle_deg);

  /*!
   * Compute UTM zone meridian.
   *
   * \param[in] zone_number			UTM Zone number.
   * \return						Meridian angle, in degrees.
   */
  double computeMeridian(int zone_number);

  /*!
   * Get the number of days in the year.
   *
   * \param[in] year                Year to get the number of days.
   * \return                        Number of days in the year.
   */
  uint32_t getNumberOfDaysInYear(uint16_t year);

  /*!
   * Get the number of days of the month index.
   *
   * \param[in] year                Year.
   * \param[in] month_index         Month index [1..12].
   * \return                        Number of days in the month.
   */
  uint32_t getNumberOfDaysInMonth(uint16_t year, uint8_t month_index);

  /*!
   * Check if the given year is a leap year.
   *
   * \param[in] year                Year to check.
   * \return                        True if the year is a leap year.
   */
  bool isLeapYear(uint16_t year);

  /*!
   * Returns the GPS to UTC leap second offset: GPS_Time = UTC_Tme + utcOffset
   *
   * WARNING: The leap second is computed from the latest received SbgUtcTime message if any.
   *          If no SbgUtcTime message has been received, a default driver current value is used.
   *
   * \param[in] first_valid_utc     First valid utc.
   * \param[in] gps_tow             Current GPS time of the week.
   * \param[in] sec                 Current second.
   * \return                        Offset in seconds to apply to UTC time to get GPS time.
   */
  int32_t getUtcOffset(bool first_valid_utc, uint32_t gps_tow, uint8_t sec);

  /*!
   * Convert SbgEComGpsPosType enum to NmeaGGAQuality enum
   *
   * \param[in] sbg_gps_type            SbgECom GPS type
   * \return                            NMEA GPS type
   */
  NmeaGGAQuality convertSbgGpsTypeToNmeaGpsType(SbgEComGpsPosType sbg_gps_type);
}

#endif // #ifndef SBG_ROS_ROS_HELPERS_H

