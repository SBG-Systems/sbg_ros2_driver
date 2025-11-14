/*!
 * \file            sbgEComCmdFeatures.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            19 March 2015
 *
 * \brief           Commands used to query supported device features.
 *
 * \copyright       Copyright (C) 2007-2024, SBG Systems SAS. All rights reserved.
 * \beginlicense    The MIT license
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * \endlicense
 */

#ifndef SBG_ECOM_CMD_FEATURES_H
#define SBG_ECOM_CMD_FEATURES_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Feature commands definitions                                       -//
//----------------------------------------------------------------------//

//
// Main sensor features
//
#define SBG_ECOM_SENSOR_FEATURE_IMU             (0x00000001 << 0)           /*!< The device can provide IMU data */
#define SBG_ECOM_SENSOR_FEATURE_AHRS            (0x00000001 << 1)           /*!< The device can provide orientation data    */
#define SBG_ECOM_SENSOR_FEATURE_NAVIGATION      (0x00000001 << 2)           /*!< The device can provide position and velocity data */
#define SBG_ECOM_SENSOR_FEATURE_SHIP_MOTION     (0x00000001 << 3)           /*!< The device can provide ship motion data output (heave) */

//
// GPS Signals bitmask defining every signal
//
#define SBG_ECOM_GNSS_SIGNAL_GPS_L1             (0x00000001 << 0)           /*!< The GNSS receiver tracks GPS L1 band. */
#define SBG_ECOM_GNSS_SIGNAL_GPS_L2             (0x00000001 << 1)           /*!< The GNSS receiver tracks GPS L2 band. */
#define SBG_ECOM_GNSS_SIGNAL_GPS_L5             (0x00000001 << 2)           /*!< The GNSS receiver tracks GPS L5 band. */
#define SBG_ECOM_GNSS_SIGNAL_GLONASS_L1         (0x00000001 << 3)           /*!< The GNSS receiver tracks GLONASS L1 band. */
#define SBG_ECOM_GNSS_SIGNAL_GLONASS_L2         (0x00000001 << 4)           /*!< The GNSS receiver tracks GLONASS L2 band. */
#define SBG_ECOM_GNSS_SIGNAL_BEIDOU_B1          (0x00000001 << 5)           /*!< The GNSS receiver tracks BEIDOU B1 band. */
#define SBG_ECOM_GNSS_SIGNAL_BEIDOU_B2          (0x00000001 << 6)           /*!< The GNSS receiver tracks BEIDOU B2 band. */
#define SBG_ECOM_GNSS_SIGNAL_BEIDOU_B3          (0x00000001 << 7)           /*!< The GNSS receiver tracks BEIDOU B3 band. */
#define SBG_ECOM_GNSS_SIGNAL_GALILEO_E1         (0x00000001 << 8)           /*!< The GNSS receiver tracks GALILEO E1 band. */
#define SBG_ECOM_GNSS_SIGNAL_GALILEO_E5         (0x00000001 << 9)           /*!< The GNSS receiver tracks GALILEO E5 band. */
#define SBG_ECOM_GNSS_SIGNAL_GALILEO_E6         (0x00000001 << 10)          /*!< The GNSS receiver tracks GALILEO E6 band. */
#define SBG_ECOM_GNSS_SIGNAL_QZSS               (0x00000001 << 11)          /*!< The GNSS receiver tracks QZSS signals */
#define SBG_ECOM_GNSS_SIGNAL_SBAS               (0x00000001 << 12)          /*!< The GNSS receiver tracks SBAS signals */
#define SBG_ECOM_GNSS_SIGNAL_L_BAND             (0x00000001 << 13)          /*!< The GNSS receiver tracks L-Band (for PPP services) */

//
// GPS capabilities
//
#define SBG_ECOM_GNSS_FEATURE_DUAL_ANT          (0x00000001 << 0)           /*!< The GNSS receiver provides a dual antenna heading */
#define SBG_ECOM_GNSS_FEATURE_RTK_LIMITED       (0x00000001 << 1)           /*!< The GNSS receiver has limited RTK accuracy (eg. Trimble RTK 30/30)  */
#define SBG_ECOM_GNSS_FEATURE_RTK               (0x00000001 << 2)           /*!< The GNSS receiver provides full RTK accuracy */
#define SBG_ECOM_GNSS_FEATURE_PPP               (0x00000001 << 3)           /*!< The GNSS receiver provides PPP computations */
#define SBG_ECOM_GNSS_FEATURE_RAW_DATA          (0x00000001 << 4)           /*!< The GNSS receiver provides RAW data output */
#define SBG_ECOM_GNSS_FEATURE_RAIM              (0x00000001 << 5)           /*!< The GNSS receiver provides Receiver Autonomous Integrity Monitoring */
#define SBG_ECOM_GNSS_FEATURE_HIGH_SPEED        (0x00000001 << 6)           /*!< The GNSS receiver has no high speed limitation (> 515m/s) */
#define SBG_ECOM_GNSS_FEATURE_IM                (0x00000001 << 7)           /*!< The GNSS receiver is able to detect and mitigate RF interferences. */
#define SBG_ECOM_GNSS_FEATURE_OSNMA             (0x00000001 << 8)           /*!< The GNSS receiver implements Galileo OSNMA spoofing detection. */
#define SBG_ECOM_GNSS_FEATURE_CLAS              (0x00000001 << 9)           /*!< The GNSS receiver implements Centimeter-Level Augmentation Service. */

//----------------------------------------------------------------------//
//- Feature commands types definition                                  -//
//----------------------------------------------------------------------//

/*!
 * This enum defines the different types of internal GNSS receiver that can provide specific features.
 * Note External type is considered as not handled by the feature system
 */
typedef enum _SbgEComGnssType
{
    SBG_ECOM_GNSS_TYPE_DISABLED         = 0,    /*!< GNSS module disabled */
    SBG_ECOM_GNSS_TYPE_EXTERNAL         = 1,    /*!< External GNSS module (all features are unknown) */
    SBG_ECOM_GNSS_TYPE_UBX_MAX_M8       = 2,    /*!< Ublox MAX-M8 module */
    SBG_ECOM_GNSS_TYPE_NOV_OEM615       = 3,    /*!< Novatel OEM615 device */
    SBG_ECOM_GNSS_TYPE_NOV_OEM615_DUAL  = 4,    /*!< Two Novatel OEM615 devices for dual antenna */
    SBG_ECOM_GNSS_TYPE_NOV_OEM617D      = 5,    /*!< Novatel OEM617D device */
    SBG_ECOM_GNSS_TYPE_SEP_AX4          = 6,    /*!< Septentrio AsteRx-m4 */
    SBG_ECOM_GNSS_TYPE_SEP_AXM2A        = 7,    /*!< Septentrio AsteRx-m2a */
    SBG_ECOM_GNSS_TYPE_UBX_F9P          = 8,    /*!< Ublox ZED-F9P module */
} SbgEComGnssType;

/*!
 * This structure contains all the information provided by the SBG_ECOM_CMD_GET_FEATURES command
 */
typedef struct _SbgEComFeatures
{
    uint32_t            sensorFeaturesMask;         /*!< The different measurement capabilities of this unit */
    SbgEComGnssType     gnssType;                   /*!< The type of GNSS receiver used (brand and model) */
    uint8_t             gnssUpdateRate;             /*!< The actual GNSS update rate */
    uint32_t            gnssSignalsMask;            /*!< GNSS receiver signals tracking */
    uint32_t            gnssFeaturesMask;           /*!< GNSS receiver computation and output features */
    char                gnssProductCode[32];        /*!< String containing the GNSS receiver product code ("\0" if unknown) */
    char                gnssSerialNumber[32];       /*!< String containing the GNSS receiver serial number ("\0" if unknown) */
    char                gnssFirmwareVersion[32];    /*!< String containing the GNSS receiver firmware version ("\0" if unknown) */
} SbgEComFeatures;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Retrieve the device and embedded GPS receiver features.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pFeatures                   A pointer to a structure to hold features.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGetFeatures(SbgEComHandle *pHandle, SbgEComFeatures *pFeatures);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_FEATURES_H
