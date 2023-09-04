/*!
*  \file         sbg_position.h
*  \author       SBG Systems
*  \date         04/09/2023
*
*  \brief        Handle creation of position class.
*
*  Methods to create Position from given data.
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

#ifndef SBG_DRIVER_SBG_POSITION_H
#define SBG_DRIVER_SBG_POSITION_H

#include <sbg_utm.h>

namespace sbg
{
  /*!
   * Class to describe a position using Utm and altitude.
   */
  class Position
  {
    public:

      //---------------------------------------------------------------------//
      //- Constructor                                                       -//
      //---------------------------------------------------------------------//

      /*!
       * Default constructor.
       */
      Position() = default;

      /*!
       * Constructor.
       *
       * \param[in] latitude                  Latitude in degree.
       * \param[in] longitude                 Longitude in degree.
       * \param[in] altitude                  Altitude is meter.
       */
      Position(double latitude, double longitude, double altitude);

      //---------------------------------------------------------------------//
      //- Parameters                                                        -//
      //---------------------------------------------------------------------//

      /*!
       * Returns if class is initialized.
       *
       * \return        True if class is initialized.
       */
      bool isInit() const;

      /*!
       * Get UTM parameters
       *
       * \return        Utm structure.
       */
      const Utm &getUtm() const;

      /*!
       * Get altitude in meter
       *
       * \return        Altitude.
       */
      double getAltitude() const;

      //---------------------------------------------------------------------//
      //- Operations                                                        -//
      //---------------------------------------------------------------------//

      /*!
       * Initialize position.
       *
       * \param[in] latitude                  Latitude in degree.
       * \param[in] longitude                 Longitude in degree.
       * \param[in] altitude                  Altitude in meter.
       */
      void init(double latitude, double longitude, double altitude);

      /*!
       * Clear values and set them to zero / false.
       */
      void clear();

    private:
      bool is_init_ = false;
      Utm utm_position_{};
      double altitude_{};
  };
}

#endif //SBG_DRIVER_SBG_POSITION_H
