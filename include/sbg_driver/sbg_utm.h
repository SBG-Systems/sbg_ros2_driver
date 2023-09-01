/*!
*  \file         sbg_utm.h
*  \author       SBG Systems
*  \date         25/08/2023
*
*  \brief        Handle creation of utm class.
*
*  Methods to create UTM from given data.
*
*  \section CodeCopyright Copyright Notice
*  MIT License
*
*  Copyright (c) 2023 SBG Systems
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
*/

#ifndef SBG_ROS_UTM_H
#define SBG_ROS_UTM_H

namespace sbg
{

/*!
 * Class to wrap UTM computation.
 */
class SbgUtm final
{
public:
  //---------------------------------------------------------------------//
  //- Constructor                                                       -//
  //---------------------------------------------------------------------//

  SbgUtm() = default;

  //---------------------------------------------------------------------//
  //- Parameters                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Get UTM easting.
   *
   * \return                            Easting, in degrees.
   */
  double getEasting() const;

  /*!
   * Get UTM northing.
   *
   * \return                            Northing, in degrees.
   */
  double getNorthing() const;

  /*!
   * Get UTM altitude.
   *
   * \return                            Altitude, in meters.
   */
  double getAltitude() const;

  /*!
   * Get UTM zone.
   *
   * \return                            Zone number.
   */
  int getZoneNumber() const;

  //---------------------------------------------------------------------//
  //- Operations                                                        -//
  //---------------------------------------------------------------------//

  /*!
   * Set UTM initial position.
   *
   * \param[in] latitude                Latitude, in degrees.
   * \param[in] longitude               Longitude, in degrees.
   * \param[in] altitude                Altitude, in meters.
   */
  void init(double latitude, double longitude, double altitude);

private:
  double    easting_ = 0.0;
  double    northing_ = 0.0;
  double    altitude_ = 0.0;
  int       zone_ = 0;
};

}

#endif // SBG_ROS_UTM_H
