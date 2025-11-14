/*!
 * \file            sbgEComDefsGnss.h
 * \ingroup         main
 * \author          SBG Systems
 * \date            20 September 2022
 *
 * \brief           Common enumeration and definitions for RAW GNSS data
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

#ifndef SBG_ECOM_DEFS_GNSS_H
#define SBG_ECOM_DEFS_GNSS_H

// sbgCommonLib headers
#include <sbgCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Enumeration definitions                                            -//
//----------------------------------------------------------------------//

/*!
 * Signal IDs.
 *
 * These are on-the-wire values.
 */
typedef enum _SbgEComSignalId
{
    SBG_ECOM_SIGNAL_ID_UNKNOWN                  = 0,

    //
    // GPS constellation (10 to 39)
    //
    SBG_ECOM_SIGNAL_ID_GPS_L1C_DP               = 10,
    SBG_ECOM_SIGNAL_ID_GPS_L1C_D                = 11,
    SBG_ECOM_SIGNAL_ID_GPS_L1C_P                = 12,
    SBG_ECOM_SIGNAL_ID_GPS_L1_W                 = 13,
    SBG_ECOM_SIGNAL_ID_GPS_L1_CA                = 14,
    SBG_ECOM_SIGNAL_ID_GPS_L1P                  = 15,
    SBG_ECOM_SIGNAL_ID_GPS_L1_PY                = 16,
    SBG_ECOM_SIGNAL_ID_GPS_L1M                  = 17,
    SBG_ECOM_SIGNAL_ID_GPS_L2C_ML               = 18,
    SBG_ECOM_SIGNAL_ID_GPS_L2C_L                = 19,
    SBG_ECOM_SIGNAL_ID_GPS_L2_SEMICL            = 20,
    SBG_ECOM_SIGNAL_ID_GPS_L2_W                 = 21,
    SBG_ECOM_SIGNAL_ID_GPS_L2_CA                = 22,
    SBG_ECOM_SIGNAL_ID_GPS_L2C_M                = 23,
    SBG_ECOM_SIGNAL_ID_GPS_L2_PY                = 24,
    SBG_ECOM_SIGNAL_ID_GPS_L2M                  = 25,
    SBG_ECOM_SIGNAL_ID_GPS_L2P                  = 26,
    SBG_ECOM_SIGNAL_ID_GPS_L5_IQ                = 27,
    SBG_ECOM_SIGNAL_ID_GPS_L5_I                 = 28,
    SBG_ECOM_SIGNAL_ID_GPS_L5_Q                 = 29,

    //
    // GLONASS constellation (40 to 59)
    //
    SBG_ECOM_SIGNAL_ID_GLONASS_G1_P             = 40,
    SBG_ECOM_SIGNAL_ID_GLONASS_G1_CA            = 41,
    SBG_ECOM_SIGNAL_ID_GLONASS_G2_P             = 42,
    SBG_ECOM_SIGNAL_ID_GLONASS_G2_CA            = 43,
    SBG_ECOM_SIGNAL_ID_GLONASS_G3_I             = 44,
    SBG_ECOM_SIGNAL_ID_GLONASS_G3_Q             = 45,
    SBG_ECOM_SIGNAL_ID_GLONASS_G3_IQ            = 46,

    //
    // Galileo constellation (60 to 99)
    //
    SBG_ECOM_SIGNAL_ID_GALILEO_E1_BC            = 60,
    SBG_ECOM_SIGNAL_ID_GALILEO_E1_C             = 61,
    SBG_ECOM_SIGNAL_ID_GALILEO_E1_B             = 62,
    SBG_ECOM_SIGNAL_ID_GALILEO_E1_A             = 63,
    SBG_ECOM_SIGNAL_ID_GALILEO_E1_ABC           = 64,
    SBG_ECOM_SIGNAL_ID_GALILEO_E5B_IQ           = 65,
    SBG_ECOM_SIGNAL_ID_GALILEO_E5B_I            = 66,
    SBG_ECOM_SIGNAL_ID_GALILEO_E5B_Q            = 67,
    SBG_ECOM_SIGNAL_ID_GALILEO_E5A_IQ           = 68,
    SBG_ECOM_SIGNAL_ID_GALILEO_E5A_I            = 69,
    SBG_ECOM_SIGNAL_ID_GALILEO_E5A_Q            = 70,
    SBG_ECOM_SIGNAL_ID_GALILEO_E5_IQ            = 71,
    SBG_ECOM_SIGNAL_ID_GALILEO_E5_I             = 72,
    SBG_ECOM_SIGNAL_ID_GALILEO_E5_Q             = 73,
    SBG_ECOM_SIGNAL_ID_GALILEO_E6_BC            = 74,
    SBG_ECOM_SIGNAL_ID_GALILEO_E6_C             = 75,
    SBG_ECOM_SIGNAL_ID_GALILEO_E6_B             = 76,
    SBG_ECOM_SIGNAL_ID_GALILEO_E6_ABC           = 77,
    SBG_ECOM_SIGNAL_ID_GALILEO_E6_A             = 78,

    //
    // BeiDou constellation (100 to 149)
    //
    SBG_ECOM_SIGNAL_ID_BEIDOU_B1IQ              = 100,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B1I               = 101,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B1Q               = 102,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_P             = 103,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_DP            = 104,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_D             = 105,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_P             = 106,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_DP            = 107,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_D             = 108,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2IQ              = 109,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2I               = 110,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_P             = 111,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_DP            = 112,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_D             = 113,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2Q               = 114,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_P             = 115,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_DP            = 116,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_D             = 117,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_P            = 118,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_DP           = 119,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_D            = 120,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B3IQ              = 121,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B3I               = 122,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B3Q               = 123,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_D             = 124,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_P             = 125,
    SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_DP            = 126,

    //
    // QZSS constellation (150 to 179)
    //
    SBG_ECOM_SIGNAL_ID_QZSS_L1C_DP              = 150,
    SBG_ECOM_SIGNAL_ID_QZSS_L1C_D               = 151,
    SBG_ECOM_SIGNAL_ID_QZSS_L1C_P               = 152,
    SBG_ECOM_SIGNAL_ID_QZSS_L1_CA               = 153,
    SBG_ECOM_SIGNAL_ID_QZSS_L1_SAIF             = 154,
    SBG_ECOM_SIGNAL_ID_QZSS_L1_SB               = 155,
    SBG_ECOM_SIGNAL_ID_QZSS_L2C_ML              = 156,
    SBG_ECOM_SIGNAL_ID_QZSS_L2C_L               = 157,
    SBG_ECOM_SIGNAL_ID_QZSS_L2C_M               = 158,
    SBG_ECOM_SIGNAL_ID_QZSS_L5_IQ               = 159,
    SBG_ECOM_SIGNAL_ID_QZSS_L5_I                = 160,
    SBG_ECOM_SIGNAL_ID_QZSS_L5_Q                = 161,
    SBG_ECOM_SIGNAL_ID_QZSS_L5S_IQ              = 162,
    SBG_ECOM_SIGNAL_ID_QZSS_L5S_I               = 163,
    SBG_ECOM_SIGNAL_ID_QZSS_L5S_Q               = 164,
    SBG_ECOM_SIGNAL_ID_QZSS_L6_P                = 165,
    SBG_ECOM_SIGNAL_ID_QZSS_L6_DP               = 166,
    SBG_ECOM_SIGNAL_ID_QZSS_L6_D                = 167,
    SBG_ECOM_SIGNAL_ID_QZSS_L6_E                = 168,
    SBG_ECOM_SIGNAL_ID_QZSS_L6_DE               = 169,

    //
    // SBAS system (180 to 199)
    //
    SBG_ECOM_SIGNAL_ID_SBAS_L1_CA               = 180,
    SBG_ECOM_SIGNAL_ID_SBAS_L5_I                = 181,
    SBG_ECOM_SIGNAL_ID_SBAS_L5_Q                = 182,
    SBG_ECOM_SIGNAL_ID_SBAS_L5_IQ               = 183,

    //
    // IRNSS / NAVIC constellation (200 to 219)
    //
    SBG_ECOM_SIGNAL_ID_IRNSS_L5_A               = 200,
    SBG_ECOM_SIGNAL_ID_IRNSS_L5_B               = 201,
    SBG_ECOM_SIGNAL_ID_IRNSS_L5_C               = 202,
    SBG_ECOM_SIGNAL_ID_IRNSS_L5_BC              = 203,
    SBG_ECOM_SIGNAL_ID_IRNSS_S9_A               = 204,
    SBG_ECOM_SIGNAL_ID_IRNSS_S9_B               = 205,
    SBG_ECOM_SIGNAL_ID_IRNSS_S9_C               = 206,
    SBG_ECOM_SIGNAL_ID_IRNSS_S9_BC              = 207,

    //
    // L-Band system (220 to 230)
    //
    SBG_ECOM_SIGNAL_ID_LBAND                    = 220,
} SbgEComSignalId;

/*!
 * Constellation IDs.
 *
 * All values must be strictly lower than 16.
 *
 * These are on-the-wire values.
 */
typedef enum _SbgEComConstellationId
{
    SBG_ECOM_CONSTELLATION_ID_UNKNOWN           = 0,
    SBG_ECOM_CONSTELLATION_ID_GPS               = 1,
    SBG_ECOM_CONSTELLATION_ID_GLONASS           = 2,
    SBG_ECOM_CONSTELLATION_ID_GALILEO           = 3,
    SBG_ECOM_CONSTELLATION_ID_BEIDOU            = 4,
    SBG_ECOM_CONSTELLATION_ID_QZSS              = 5,
    SBG_ECOM_CONSTELLATION_ID_SBAS              = 6,
    SBG_ECOM_CONSTELLATION_ID_IRNSS             = 7,
    SBG_ECOM_CONSTELLATION_ID_LBAND             = 8,
} SbgEComConstellationId;

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

/*!
 * Returns a constellation given a signal ID
 * 
 * \param[in]   signalId                    Signal ID value.
 * \return                                  Constellation this signal belongs to.
 */
SbgEComConstellationId sbgEComGetConstellationFromSignalId(SbgEComSignalId signalId);

/*!
 * Check if a value belongs to SbgEComSignalId enum.
 * 
 * \note: SBG_ECOM_SIGNAL_ID_UNKNOWN is considered to be a valid enum value.
 *
 * \param[in]   signalId                    Signal ID value.
 * \return                                  true if the value is valid
 */
bool sbgEComSignalIdIsValid(uint8_t signalId);

/*!
 * Get a signal ID as a read only C string.
 * 
 * \param[in]   signalId                    Signal ID value.
 * \return                                  Signal ID as a read only C string.
 */
const char *sbgEComSignalToStr(SbgEComSignalId signalId);

/*!
 * Check if a value belongs to SbgEComConstellationId enum.
 *
 * \param[in]   constellationId             constellation ID value.
 * \return                                  true if the value is valid
 */
bool sbgEComConstellationIdIsValid(uint8_t constellationId);

/*!
 * Get a constellation ID as a read only C string.
 * 
 * \param[in]   constellationId             Constellation ID value.
 * \return                                  Constellation ID as a read only C string.
 */
const char *sbgEComConstellationToStr(SbgEComConstellationId constellationId);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_DEFS_GNSS_H
