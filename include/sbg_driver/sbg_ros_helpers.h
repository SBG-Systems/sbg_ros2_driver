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

namespace sbg::helpers
{
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
}

#endif // #ifndef SBG_ROS_ROS_HELPERS_H

