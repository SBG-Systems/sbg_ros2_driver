/*!
 * \file            imuDataMean.h
 * \author          SBG Systems
 * \date            March 14, 2023
 *
 * \brief           Implemented simple moving average filter for IMU data entries.
 *
 * \copyright       Copyright (C) 2007-2024, SBG Systems SAS. All rights reserved.
 * \beginlicense    Proprietary license
 *
 * This source code is intended for use only by SBG Systems SAS and
 * those that have explicit written permission to use it from
 * SBG Systems SAS.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * \endlicense
 */

#ifndef SBG_IMU_DATA_MEAN_H
#define SBG_IMU_DATA_MEAN_H

// STL headers
#include <array>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

namespace sbg
{
    //----------------------------------------------------------------------//
    //- CImuDataMean                                                       -//
    //----------------------------------------------------------------------//

    /*!
     * Helper used to accumulate and decimate IMU data.
     * The timestamp represents the latest added measurement.
     * The status represents the OR combinaison of all accumulated status.
     */
    class CImuDataMean
    {
    public:

        /*!
         * Default constructor.
         */
        CImuDataMean();
        
        /*!
         * Accumulate a IMU measurement and return the number of accumulated values.
         * 
         * \param[in]   imuData                         IMU data to accumulate.
         * \return                                      Number of accumulated data so far.
         */
        int32_t add(const SbgEComLogImuLegacy &imuData);
            
        /*!
         * Accumulate a IMU measurement and return the number of accumulated values.
         * 
         * \param[in]   imuData                         IMU data to accumulate.
         * \return                                      Number of accumulated data so far.
         */
        int32_t add(const SbgEComLogImuShort &imuData);
            
        /*!
         * Accumulate a IMU measurement and return the number of accumulated values.
         * 
         * \param[in]   imuData                         IMU data to accumulate.
         * \return                                      Number of accumulated data so far.
         */
        int32_t add(const SbgEComLogImuFastLegacy &imuData);

        /*!
         * Reset the accumulators.
         */
        void reset();
            
        /*!
         * Returns the latest timestamp value.
         * 
         * \return                                      timestamp in us.
         */
        uint32_t getTimeStamp() const;
            
        /*!
         * Returns the accumulated status.
         * 
         * \return                                      status bitmask.
         */
        uint32_t getStatus() const;
            
        /*!
         * Returns the mean accelerometer value.
         * 
         * \param[in]   idx                             Axis index from 0 to 2.
         * \return                                      Mean accelerometer value in m.s^-2.
         */
        double getAccelerometer(size_t idx) const;
            
        /*!
         * Returns the mean rotation rate value.
         * 
         * \param[in]   idx                             Axis index from 0 to 2.
         * \return                                      Mean gyroscope value in rad.s^-1.
         */
        double getGyroscope(size_t idx) const;

        /*!
         * Returns the mean rotation rate value.
         * 
         * \param[in]   idx                             Axis index from 0 to 2.
         * \return                                      Mean gyroscope value in deg.s^-1.
         */
        double getGyroscopeDeg(size_t idx) const;

        /*!
         * Returns the mean temperature value.
         * 
         * \return                                      Mean temperature in degC.
         */
        double getTemperature() const;

    private:
        uint32_t                    m_timeStamp;                    /*!< Latest added measurement timestamp in us. */
        uint32_t                    m_status;                       /*!< Status bitmask. */
        std::array<double, 3>       m_accelerometer;                /*!< X,Y,Z accelerometer accumulator in m.s^-2 */
        std::array<double, 3>       m_gyroscope;                    /*!< X,y,Z rotation rate accumulator in rad.s^-1 */
        double                      m_temperature;                  /*!< Temperature accumulator in degC. */
        int32_t                     m_numAccValues;                 /*!< Number of accumulated values. */
    };
};

#endif // SBG_IMU_DATA_MEAN_H
