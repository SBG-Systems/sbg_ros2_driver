/*!
*  \file         sbg_utm.h
*  \author       SBG Systems
*  \date         25/08/2023
*
*  \brief        Handle creation of utm class.
*
*  Methods to create Utm from given data.
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

// STL headers
#include <array>

namespace sbg
{

/*!
 * Class for Utm related data
 */
class Utm final
{
  public:
    Utm() = default;
    Utm(double latitude, double longitude);

    void init(double latitude, double longitude);
    void clear();
    void reset(double latitude, double longitude);

    bool isInit() const;
    int getZoneNumber() const;
    double getMeridian() const;
    char getLetterDesignator() const;

    std::array<double, 2> computeEastingNorthing(double latitude, double longitude) const;

  private:

    /*!
     * Convert latitude and longitude to an UTM zone number.
     *
     * \param[in] latitude                Latitude, in degrees.
     * \param[in] longitude               Longitude, in degrees.
     * \return                            UTM zone number.
     */
    static int computeZoneNumber(double latitude, double longitude);

    /*!
     * Get UTM letter designator for the given latitude.
     * Originally written by Chuck Gantz- chuck.gantz@globalstar.com
     *
     * \param[in] latitude                Latitude, in degrees.
     * \return                            UTM letter designator.
     */
    static char computeLetterDesignator(double latitude);

    /*!
     * Compute UTM zone meridian from UTM zone number.
     *
     * \return                            Meridian angle, in degrees.
     */
    double computeMeridian() const;

    bool      is_init_ = false;
    double    meridian_{};
    int       zone_number_{};
    char      letter_designator_{};
};

}

#endif // SBG_ROS_UTM_H
