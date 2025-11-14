/*!
 * \file            sbgEComLogGnssPos.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            09 May 2023
 *
 * \brief           GNSS position related logs.
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

#ifndef SBG_ECOM_LOG_GPS_POS_H
#define SBG_ECOM_LOG_GPS_POS_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Public definitions                                                 -//
//----------------------------------------------------------------------//

/*!
 * GNSS signals definitions
 */
#define SBG_ECOM_GNSS_POS_GPS_L1_USED       (0x00000001u << 12)             /*!< Set to 1 if GPS L1CA/L1P is used in solution. */
#define SBG_ECOM_GNSS_POS_GPS_L2_USED       (0x00000001u << 13)             /*!< Set to 1 if GPS L2P/L2C is used in solution. */
#define SBG_ECOM_GNSS_POS_GPS_L5_USED       (0x00000001u << 14)             /*!< Set to 1 if GPS L5 is used in solution. */

#define SBG_ECOM_GNSS_POS_GLO_L1_USED       (0x00000001u << 15)             /*!< Set to 1 if GLONASS L1CA is used in solution. */
#define SBG_ECOM_GNSS_POS_GLO_L2_USED       (0x00000001u << 16)             /*!< Set to 1 if GLONASS L2C/L2P is used in solution. */
#define SBG_ECOM_GNSS_POS_GLO_L3_USED       (0x00000001u << 17)             /*!< Set to 1 if GLONASS L3 is used in solution. */

#define SBG_ECOM_GNSS_POS_GAL_E1_USED       (0x00000001u << 18)             /*!< Set to 1 if Galileo E1 is used in solution. */
#define SBG_ECOM_GNSS_POS_GAL_E5A_USED      (0x00000001u << 19)             /*!< Set to 1 if Galileo E5a is used in solution. */
#define SBG_ECOM_GNSS_POS_GAL_E5B_USED      (0x00000001u << 20)             /*!< Set to 1 if Galileo E5b is used in solution. */
#define SBG_ECOM_GNSS_POS_GAL_E5ALT_USED    (0x00000001u << 21)             /*!< Set to 1 if Galileo E5 AltBoc is used in solution. */
#define SBG_ECOM_GNSS_POS_GAL_E6_USED       (0x00000001u << 22)             /*!< Set to 1 if Galileo E6 is used in solution. */

#define SBG_ECOM_GNSS_POS_BDS_B1_USED       (0x00000001u << 23)             /*!< Set to 1 if BeiDou B1 is used in solution. */
#define SBG_ECOM_GNSS_POS_BDS_B2_USED       (0x00000001u << 24)             /*!< Set to 1 if BeiDou B2 is used in solution. */
#define SBG_ECOM_GNSS_POS_BDS_B3_USED       (0x00000001u << 25)             /*!< Set to 1 if BeiDou B3 is used in solution. */

#define SBG_ECOM_GNSS_POS_QZSS_L1_USED      (0x00000001u << 26)             /*!< Set to 1 if QZSS L1CA is used in solution. */
#define SBG_ECOM_GNSS_POS_QZSS_L2_USED      (0x00000001u << 27)             /*!< Set to 1 if QZSS L2C is used in solution. */
#define SBG_ECOM_GNSS_POS_QZSS_L5_USED      (0x00000001u << 28)             /*!< Set to 1 if QZSS L5 is used in solution. */
#define SBG_ECOM_GNSS_POS_QZSS_L6_USED      (0x00000001u << 29)             /*!< Set to 1 if QZSS L6 is used in solution. */

/*!
 * GNSS position status definitions.
 */
typedef enum _SbgEComGnssPosStatus
{
    SBG_ECOM_GNSS_POS_STATUS_SOL_COMPUTED       = 0,                        /*!< A valid solution has been computed. */
    SBG_ECOM_GNSS_POS_STATUS_INSUFFICIENT_OBS   = 1,                        /*!< Not enough valid SV to compute a solution. */
    SBG_ECOM_GNSS_POS_STATUS_INTERNAL_ERROR     = 2,                        /*!< An internal error has occurred. */
    SBG_ECOM_GNSS_POS_STATUS_HEIGHT_LIMIT       = 3                         /*!< The height limit has been exceeded. */
} SbgEComGnssPosStatus;

/*!
 * GNSS position types definitions.
 */
typedef enum _SbgEComGnssPosType
{
    SBG_ECOM_GNSS_POS_TYPE_NO_SOLUTION          = 0,                        /*!< No valid solution available. */
    SBG_ECOM_GNSS_POS_TYPE_UNKNOWN              = 1,                        /*!< An unknown solution type has been computed. */
    SBG_ECOM_GNSS_POS_TYPE_SINGLE               = 2,                        /*!< Single point solution position. */
    SBG_ECOM_GNSS_POS_TYPE_PSRDIFF              = 3,                        /*!< Standard Pseudorange Differential Solution (DGPS). */
    SBG_ECOM_GNSS_POS_TYPE_SBAS                 = 4,                        /*!< SBAS satellite used for differential corrections. */
    SBG_ECOM_GNSS_POS_TYPE_OMNISTAR             = 5,                        /*!< Omnistar VBS Position (L1 sub-meter). */
    SBG_ECOM_GNSS_POS_TYPE_RTK_FLOAT            = 6,                        /*!< Floating RTK ambiguity solution (20 cms RTK). */
    SBG_ECOM_GNSS_POS_TYPE_RTK_INT              = 7,                        /*!< Integer RTK ambiguity solution (2 cms RTK). */
    SBG_ECOM_GNSS_POS_TYPE_PPP_FLOAT            = 8,                        /*!< Precise Point Positioning with float ambiguities. */
    SBG_ECOM_GNSS_POS_TYPE_PPP_INT              = 9,                        /*!< Precise Point Positioning with fixed ambiguities. */
    SBG_ECOM_GNSS_POS_TYPE_FIXED                = 10                        /*!< Fixed location solution position. */
} SbgEComGnssPosType;

/*!
 * GNSS interference monitoring and mitigation indicator.
 */
typedef enum _SbgEComGnssIfmStatus
{
    SBG_ECOM_GNSS_IFM_STATUS_ERROR              = 0,                        /*!< Interference monitoring system is in error and doesn't work. */
    SBG_ECOM_GNSS_IFM_STATUS_UNKNOWN            = 1,                        /*!< Interference monitoring is either disabled or not available. */
    SBG_ECOM_GNSS_IFM_STATUS_CLEAN              = 2,                        /*!< Interference are monitored and the environment is clean. */
    SBG_ECOM_GNSS_IFM_STATUS_MITIGATED          = 3,                        /*!< Interference are detected and mitigated, the PVT is OK. */
    SBG_ECOM_GNSS_IFM_STATUS_CRITICAL           = 4                         /*!< Interference are detected and couldn't be mitigated, the PVT is invalid. */
} SbgEComGnssIfmStatus;

/*!
 * GNSS spoofing monitoring and mitigation indicator.
 */
typedef enum _SbgEComGnssSpoofingStatus
{
    SBG_ECOM_GNSS_SPOOFING_STATUS_ERROR         = 0,                        /*!< Spoofing detection system is in error and doesn't work. */
    SBG_ECOM_GNSS_SPOOFING_STATUS_UNKNOWN       = 1,                        /*!< Spoofing detection is either disabled or not available. */
    SBG_ECOM_GNSS_SPOOFING_STATUS_CLEAN         = 2,                        /*!< Spoofing detection is enabled and not spoofer is detected. */
    SBG_ECOM_GNSS_SPOOFING_STATUS_SINGLE        = 3,                        /*!< Probable spoofing identified by one method only. */
    SBG_ECOM_GNSS_SPOOFING_STATUS_MULTIPLE      = 4                         /*!< Confirmed spoofing identified by several methods such as Galileo OSNMA + Built In. */
} SbgEComGnssSpoofingStatus;

/*!
 * GNSS Galileo OSNMA Status.
 */
typedef enum _SbgEComGnssOsnmaStatus
{
    SBG_ECOM_GNSS_OSNMA_STATUS_ERROR            = 0,                        /*!< OSNMA is in error state and doesn't work. */
    SBG_ECOM_GNSS_OSNMA_STATUS_DISABLED         = 1,                        /*!< OSNMA is either disabled or not available. */
    SBG_ECOM_GNSS_OSNMA_STATUS_INITIALIZING     = 2,                        /*!< OSNMA initialization in progress. */
    SBG_ECOM_GNSS_OSNMA_STATUS_WAITING_NTP      = 3,                        /*!< OSNMA strict is used and is waiting to receive valid time from NTP server. */
    SBG_ECOM_GNSS_OSNMA_STATUS_VALID            = 4,                        /*!< OSNMA is actively authenticating Galileo signals and not spoofing is detected. */
    SBG_ECOM_GNSS_OSNMA_STATUS_SPOOFED          = 5                         /*!< OSNMA has detected spoofing on Galileo signals. */
} SbgEComGnssOsnmaStatus;

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Structure that stores data for the SBG_ECOM_LOG_GPS#_POS message.
 */
typedef struct _SbgEComLogGnssPos
{
    uint32_t        timeStamp;              /*!< Time in us since the sensor power up. */
    uint32_t        status;                 /*!< GPS position status, type and bitmask. */
    uint32_t        timeOfWeek;             /*!< GPS time of week in ms. */
    double          latitude;               /*!< Latitude in degrees, positive north. */
    double          longitude;              /*!< Longitude in degrees, positive east. */
    double          altitude;               /*!< Altitude above Mean Sea Level in meters. */
    float           undulation;             /*!< Altitude difference between the geoid and the Ellipsoid in meters (Height above Ellipsoid = altitude + undulation). */
    float           latitudeAccuracy;       /*!< 1 sigma latitude accuracy in meters (0 to 9999). */
    float           longitudeAccuracy;      /*!< 1 sigma longitude accuracy in meters (0 to 9999). */
    float           altitudeAccuracy;       /*!< 1 sigma altitude accuracy in meters (0 to 9999). */
    uint8_t         numSvUsed;              /*!< Number of space vehicles used to compute the solution - set to 0xFF if not available. (added in 1.4) */
    uint16_t        baseStationId;          /*!< Base station id for differential corrections (0-4095) - set to 0xFFFF if differential are not used or not available. (added in 1.4). */
    uint16_t        differentialAge;        /*!< Differential correction age in 0.01 seconds - set to 0xFFFF if differential are not used or not available. (added in 1.4). */  
    uint8_t         numSvTracked;           /*!< Number of space vehicles tracked by the GNSS - set to 0xFF if not available. (added in 4.0) */
    uint32_t        statusExt;              /*!< Additional status for interference, spoofing and OSNMA (added in 4.0). */
} SbgEComLogGnssPos;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Zero initialize the message struct.
 * 
 * \param[out]  pLogData                    Structure instance to zero init.
 */
void sbgEComLogGnssPosZeroInit(SbgEComLogGnssPos *pLogData);

/*!
 * Parse data for the SBG_ECOM_LOG_GPS#_POS message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogGnssPosReadFromStream(SbgEComLogGnssPos *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_GPS#_POS message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogGnssPosWriteToStream(const SbgEComLogGnssPos *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Set the GNSS position solution status.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   status              The solution status to set.
 */
void sbgEComLogGnssPosSetStatus(SbgEComLogGnssPos *pLogData, SbgEComGnssPosStatus status);

/*!
 * Returns the GNSS position solution status.
 * 
 * Note: Method doesn't follow standard naming conventions because of legacy sbgEComLogGnssPosGetStatus method.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          The solution status.
 */
SbgEComGnssPosStatus sbgEComLogGnssPosGetStatus(const SbgEComLogGnssPos *pLogData);

/*!
 * Set the GNSS position solution type.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   posType             The solution type to set.
 */
void sbgEComLogGnssPosSetType(SbgEComLogGnssPos *pLogData, SbgEComGnssPosType posType);

/*!
 * Returns the GNSS position solution type.
 * 
 * Note: Method doesn't follow standard naming conventions because of legacy sbgEComLogGnssPosGetStatus method.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          The solution type.
 */
SbgEComGnssPosType sbgEComLogGnssPosGetType(const SbgEComLogGnssPos *pLogData);

/*!
 * Set the GNSS signals used in solution bitmask.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   signalMask          Bitmak of signals used in solution to set.
 */
void sbgEComLogGnssPosSetSignalsUsed(SbgEComLogGnssPos *pLogData, uint32_t signalMask);

/*!
 * Returns true if the signal(s) are used in the solution.
 * 
 * Only returns true if all signals set in signalsMask are used in solution.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   signalsMask         Bitmask of signals to check if there are used in solution.
 * \return                          true if the signal(s) are used in solution.
 */
bool sbgEComLogGnssPosSignalsAreUsed(const SbgEComLogGnssPos *pLogData, uint32_t signalsMask);

/*!
 * Returns true if the numSvTracked field is valid.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          true if the numSvTracked field is valid or false otherwise.
 */
bool sbgEComLogGnssPosNumSvTrackedIsValid(const SbgEComLogGnssPos *pLogData);

/*!
 * Returns true if the numSvUsed field is valid.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          true if the numSvUsed field is valid or false otherwise.
 */
bool sbgEComLogGnssPosNumSvUsedIsValid(const SbgEComLogGnssPos *pLogData);

/*!
 * Returns true if the baseStationId field is valid.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          true if the baseStationId field is valid or false otherwise.
 */
bool sbgEComLogGnssPosBaseStationIdIsValid(const SbgEComLogGnssPos *pLogData);

/*!
 * Returns true if the differentialAge field is valid.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          true if the differentialAge field is valid or false otherwise.
 */
bool sbgEComLogGnssPosDifferentialAgeIsValid(const SbgEComLogGnssPos *pLogData);

/*!
 * Set the differential correction age in seconds.
 * 
 * If differential correction age is greater than 655.340 seconds or NaN
 * the field is set to invalid/not available.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   differentialAge     Differential correction age in seconds (>= 0).
 */
void sbgEComLogGnssPosSetDifferentialAge(SbgEComLogGnssPos *pLogData, float differentialAge);

/*!
 * Returns the differential correction age in seconds.
 * 
 * If there is no valid differential age, returns NaN.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          Differential correction age in seconds.
 */
float sbgEComLogGnssPosGetDifferentialAge(const SbgEComLogGnssPos *pLogData);

/*!
 * Set the GNSS interference monitoring and mitigation status.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   status              The status to set.
 */
void sbgEComLogGnssPosSetIfmStatus(SbgEComLogGnssPos *pLogData, SbgEComGnssIfmStatus status);

/*!
 * Returns the GNSS position solution status.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          The interference monitoring and mitigation status.
 */
SbgEComGnssIfmStatus sbgEComLogGnssPosGetIfmStatus(const SbgEComLogGnssPos *pLogData);

/*!
 * Set the GNSS spoofing status.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   status              The status to set.
 */
void sbgEComLogGnssPosSetSpoofingStatus(SbgEComLogGnssPos *pLogData, SbgEComGnssSpoofingStatus status);

/*!
 * Returns the GNSS spoofing status.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          The spoofing status.
 */
SbgEComGnssSpoofingStatus sbgEComLogGnssPosGetSpoofingStatus(const SbgEComLogGnssPos *pLogData);

/*!
 * Set the GNSS Galileo OSNMA status.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   status              The status to set.
 */
void sbgEComLogGnssPosSetOsnmaStatus(SbgEComLogGnssPos *pLogData, SbgEComGnssOsnmaStatus status);

/*!
 * Returns the GNSS Galileo OSNMA status.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          The Galileo OSNMA status.
 */
SbgEComGnssOsnmaStatus sbgEComLogGnssPosGetOsnmaStatus(const SbgEComLogGnssPos *pLogData);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

#ifdef SBG_ECOM_USE_DEPRECATED_MACROS
    #define SBG_ECOM_GPS_POS_GPS_L1_USED        (0x00000001u << 12)
    #define SBG_ECOM_GPS_POS_GPS_L2_USED        (0x00000001u << 13)
    #define SBG_ECOM_GPS_POS_GPS_L5_USED        (0x00000001u << 14)

    #define SBG_ECOM_GPS_POS_GLO_L1_USED        (0x00000001u << 15)
    #define SBG_ECOM_GPS_POS_GLO_L2_USED        (0x00000001u << 16)
    #define SBG_ECOM_GPS_POS_GLO_L3_USED        (0x00000001u << 17)

    #define SBG_ECOM_GPS_POS_GAL_E1_USED        (0x00000001u << 18)
    #define SBG_ECOM_GPS_POS_GAL_E5A_USED       (0x00000001u << 19)
    #define SBG_ECOM_GPS_POS_GAL_E5B_USED       (0x00000001u << 20)
    #define SBG_ECOM_GPS_POS_GAL_E5ALT_USED     (0x00000001u << 21)
    #define SBG_ECOM_GPS_POS_GAL_E6_USED        (0x00000001u << 22)

    #define SBG_ECOM_GPS_POS_BDS_B1_USED        (0x00000001u << 23)
    #define SBG_ECOM_GPS_POS_BDS_B2_USED        (0x00000001u << 24)
    #define SBG_ECOM_GPS_POS_BDS_B3_USED        (0x00000001u << 25)

    #define SBG_ECOM_GPS_POS_QZSS_L1_USED       (0x00000001u << 26)
    #define SBG_ECOM_GPS_POS_QZSS_L2_USED       (0x00000001u << 27)
    #define SBG_ECOM_GPS_POS_QZSS_L5_USED       (0x00000001u << 28)
    #define SBG_ECOM_GPS_POS_QZSS_L6_USED       (0x00000001u << 29)

    #define SBG_ECOM_POS_SOL_COMPUTED           (SBG_ECOM_GNSS_POS_STATUS_SOL_COMPUTED)
    #define SBG_ECOM_POS_INSUFFICIENT_OBS       (SBG_ECOM_GNSS_POS_STATUS_INSUFFICIENT_OBS)
    #define SBG_ECOM_POS_INTERNAL_ERROR         (SBG_ECOM_GNSS_POS_STATUS_INTERNAL_ERROR)
    #define SBG_ECOM_POS_HEIGHT_LIMIT           (SBG_ECOM_GNSS_POS_STATUS_HEIGHT_LIMIT)
        
    #define SBG_ECOM_POS_NO_SOLUTION            (SBG_ECOM_GNSS_POS_TYPE_NO_SOLUTION)
    #define SBG_ECOM_POS_UNKNOWN_TYPE           (SBG_ECOM_GNSS_POS_TYPE_UNKNOWN)
    #define SBG_ECOM_POS_SINGLE                 (SBG_ECOM_GNSS_POS_TYPE_SINGLE)
    #define SBG_ECOM_POS_PSRDIFF                (SBG_ECOM_GNSS_POS_TYPE_PSRDIFF)
    #define SBG_ECOM_POS_SBAS                   (SBG_ECOM_GNSS_POS_TYPE_SBAS)
    #define SBG_ECOM_POS_OMNISTAR               (SBG_ECOM_GNSS_POS_TYPE_OMNISTAR)
    #define SBG_ECOM_POS_RTK_FLOAT              (SBG_ECOM_GNSS_POS_TYPE_RTK_FLOAT)
    #define SBG_ECOM_POS_RTK_INT                (SBG_ECOM_GNSS_POS_TYPE_RTK_INT)
    #define SBG_ECOM_POS_PPP_FLOAT              (SBG_ECOM_GNSS_POS_TYPE_PPP_FLOAT)  
    #define SBG_ECOM_POS_PPP_INT                (SBG_ECOM_GNSS_POS_TYPE_PPP_INT)
    #define SBG_ECOM_POS_FIXED                  (SBG_ECOM_GNSS_POS_TYPE_FIXED)
#endif

SBG_DEPRECATED_TYPEDEF(typedef enum _SbgEComGnssPosStatus   SbgEComGpsPosStatus);
SBG_DEPRECATED_TYPEDEF(typedef enum _SbgEComGnssPosType     SbgEComGpsPosType);
SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogGnssPos    SbgLogGpsPos);

SBG_DEPRECATED(uint32_t sbgEComLogGpsPosBuildStatus(SbgEComGnssPosStatus status, SbgEComGnssPosType type, uint32_t masks));
SBG_DEPRECATED(SbgEComGnssPosStatus sbgEComLogGpsPosGetStatus(uint32_t status));
SBG_DEPRECATED(SbgEComGnssPosType sbgEComLogGpsPosGetType(uint32_t status));

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseGpsPosData(SbgStreamBuffer *pStreamBuffer, SbgEComLogGnssPos *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteGpsPosData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogGnssPos *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_GNSS_POS_H
