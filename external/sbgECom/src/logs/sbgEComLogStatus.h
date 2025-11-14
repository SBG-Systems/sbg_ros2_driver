/*!
 * \file            sbgEComLogStatus.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            03 April 2013
 *
 * \brief           Parse logs used to report device status.
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

#ifndef SBG_ECOM_LOG_STATUS_H
#define SBG_ECOM_LOG_STATUS_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- General status definitions                                         -//
//----------------------------------------------------------------------//
#define SBG_ECOM_GENERAL_MAIN_POWER_OK      (0x0001u << 0)              /*!< Set to 1 when main power supply is OK. */
#define SBG_ECOM_GENERAL_IMU_POWER_OK       (0x0001u << 1)              /*!< Set to 1 when IMU power supply is OK. */
#define SBG_ECOM_GENERAL_GPS_POWER_OK       (0x0001u << 2)              /*!< Set to 1 when GPS power supply is OK. */
#define SBG_ECOM_GENERAL_SETTINGS_OK        (0x0001u << 3)              /*!< Set to 1 if settings where correctly loaded. */
#define SBG_ECOM_GENERAL_TEMPERATURE_OK     (0x0001u << 4)              /*!< Set to 1 when temperature is within specified limits. */
#define SBG_ECOM_GENERAL_DATALOGGER_OK      (0x0001u << 5)              /*!< Set to 1 when the datalogger is working correctly. */
#define SBG_ECOM_GENERAL_CPU_OK             (0x0001u << 6)              /*!< Set to 1 if the CPU headroom is correct.*/

//----------------------------------------------------------------------//
//- Communication status definitions                                   -//
//----------------------------------------------------------------------//

/*!
 * Communication status bit mask definitions (comStatus).
 */
#define SBG_ECOM_PORTA_VALID                (0x00000001u << 0)          /*!< Set to 0 in case of low level communication error. */
#define SBG_ECOM_PORTB_VALID                (0x00000001u << 1)          /*!< Set to 0 in case of low level communication error. */
#define SBG_ECOM_PORTC_VALID                (0x00000001u << 2)          /*!< Set to 0 in case of low level communication error. */
#define SBG_ECOM_PORTD_VALID                (0x00000001u << 3)          /*!< Set to 0 in case of low level communication error. */
#define SBG_ECOM_PORTE_VALID                (0x00000001u << 4)          /*!< Set to 0 in case of low level communication error. */

#define SBG_ECOM_PORTA_RX_OK                (0x00000001u << 5)          /*!< Set to 0 in case of error on PORT A input. */
#define SBG_ECOM_PORTA_TX_OK                (0x00000001u << 6)          /*!< Set to 0 in case of error on PORT A output. */
#define SBG_ECOM_PORTB_RX_OK                (0x00000001u << 7)          /*!< Set to 0 in case of error on PORT B input. */
#define SBG_ECOM_PORTB_TX_OK                (0x00000001u << 8)          /*!< Set to 0 in case of error on PORT B output. */
#define SBG_ECOM_PORTC_RX_OK                (0x00000001u << 9)          /*!< Set to 0 in case of error on PORT C input. */
#define SBG_ECOM_PORTC_TX_OK                (0x00000001u << 10)         /*!< Set to 0 in case of error on PORT C output. */
#define SBG_ECOM_PORTD_RX_OK                (0x00000001u << 11)         /*!< Set to 0 in case of error on PORT D input. */
#define SBG_ECOM_PORTD_TX_OK                (0x00000001u << 12)         /*!< Set to 0 in case of error on PORT D input. */
#define SBG_ECOM_PORTE_RX_OK                (0x00000001u << 13)         /*!< Set to 0 in case of error on PORT E input. */
#define SBG_ECOM_PORTE_TX_OK                (0x00000001u << 14)         /*!< Set to 0 in case of error on PORT D input. */

#define SBG_ECOM_ETH0_VALID                 (0x00000001u << 15)         /*!< Set to 0 in case of error on ETH0. */
#define SBG_ECOM_ETH1_VALID                 (0x00000001u << 16)         /*!< Set to 0 in case of error on ETH1. */
#define SBG_ECOM_ETH2_VALID                 (0x00000001u << 17)         /*!< Set to 0 in case of error on ETH2. */
#define SBG_ECOM_ETH3_VALID                 (0x00000001u << 18)         /*!< Set to 0 in case of error on ETH3. */
#define SBG_ECOM_ETH4_VALID                 (0x00000001u << 19)         /*!< Set to 0 in case of error on ETH4. */

#define SBG_ECOM_CAN_VALID                  (0x00000001u << 25)         /*!< Set to 0 in case of low level communication error. */
#define SBG_ECOM_CAN_RX_OK                  (0x00000001u << 26)         /*!< Set to 0 in case of error on CAN Bus input buffer. */
#define SBG_ECOM_CAN_TX_OK                  (0x00000001u << 27)         /*!< Set to 0 in case of error on CAN Bus output buffer. */

/*!
 * Communication status for the CAN Bus.
 */
typedef enum _SbgEComCanBusStatus
{
    SBG_ECOM_CAN_BUS_OFF                    = 0,                        /*!< CAN bus is OFF (either not enabled or disabled due to too much errors). */
    SBG_ECOM_CAN_BUS_TX_RX_ERR              = 1,                        /*!< CAN bus transmitter or receiver has error. */
    SBG_ECOM_CAN_BUS_OK                     = 2,                        /*!< CAN Bus is OK. */
    SBG_ECOM_CAN_BUS_ERROR                  = 3                         /*!< CAN Bus has a general error such as busy bus. */
} SbgEComCanBusStatus;

/*!
 * Second communication status bit mask definitions (comStatus2).
 */
#define SBG_ECOM_COM2_ETH0_RX_OK                (0x0001u << 0)          /*!< Set to 0 in case of error on ETH0 input. */
#define SBG_ECOM_COM2_ETH0_TX_OK                (0x0001u << 1)          /*!< Set to 0 in case of error on ETH0 output. */
#define SBG_ECOM_COM2_ETH1_RX_OK                (0x0001u << 2)          /*!< Set to 0 in case of error on ETH1 input. */
#define SBG_ECOM_COM2_ETH1_TX_OK                (0x0001u << 3)          /*!< Set to 0 in case of error on ETH1 output. */
#define SBG_ECOM_COM2_ETH2_RX_OK                (0x0001u << 4)          /*!< Set to 0 in case of error on ETH2 input. */
#define SBG_ECOM_COM2_ETH2_TX_OK                (0x0001u << 5)          /*!< Set to 0 in case of error on ETH2 output. */
#define SBG_ECOM_COM2_ETH3_RX_OK                (0x0001u << 6)          /*!< Set to 0 in case of error on ETH3 input. */
#define SBG_ECOM_COM2_ETH3_TX_OK                (0x0001u << 7)          /*!< Set to 0 in case of error on ETH3 output. */
#define SBG_ECOM_COM2_ETH4_RX_OK                (0x0001u << 8)          /*!< Set to 0 in case of error on ETH4 input. */
#define SBG_ECOM_COM2_ETH4_TX_OK                (0x0001u << 9)          /*!< Set to 0 in case of error on ETH4 output. */

//----------------------------------------------------------------------//
//- Aiding status definitions                                          -//
//----------------------------------------------------------------------//
#define SBG_ECOM_AIDING_GPS1_POS_RECV           (0x00000001u << 0)      /*!< Set to 1 when valid GPS 1 position data is received. */
#define SBG_ECOM_AIDING_GPS1_VEL_RECV           (0x00000001u << 1)      /*!< Set to 1 when valid GPS 1 velocity data is received. */
#define SBG_ECOM_AIDING_GPS1_HDT_RECV           (0x00000001u << 2)      /*!< Set to 1 when valid GPS 1 true heading data is received. */
#define SBG_ECOM_AIDING_GPS1_UTC_RECV           (0x00000001u << 3)      /*!< Set to 1 when valid GPS 1 UTC time data is received. */
#define SBG_ECOM_AIDING_GPS2_POS_RECV           (0x00000001u << 4)      /*!< Set to 1 when valid GPS 2 position data is received. */
#define SBG_ECOM_AIDING_GPS2_VEL_RECV           (0x00000001u << 5)      /*!< Set to 1 when valid GPS 2 velocity data is received. */
#define SBG_ECOM_AIDING_GPS2_HDT_RECV           (0x00000001u << 6)      /*!< Set to 1 when valid GPS 2 true heading data is received. */
#define SBG_ECOM_AIDING_GPS2_UTC_RECV           (0x00000001u << 7)      /*!< Set to 1 when valid GPS 2 UTC time data is received. */
#define SBG_ECOM_AIDING_MAG_RECV                (0x00000001u << 8)      /*!< Set to 1 when valid Magnetometer data is received. */
#define SBG_ECOM_AIDING_ODO_RECV                (0x00000001u << 9)      /*!< Set to 1 when Odometer pulse is received. */
#define SBG_ECOM_AIDING_DVL_RECV                (0x00000001u << 10)     /*!< Set to 1 when valid DVL data is received. */
#define SBG_ECOM_AIDING_USBL_RECV               (0x00000001u << 11)     /*!< Set to 1 when valid USBL data is received. */
#define SBG_ECOM_AIDING_DEPTH_RECV              (0x00000001u << 12)     /*!< Set to 1 when valid Depth Log data is received. */
#define SBG_ECOM_AIDING_AIR_DATA_RECV           (0x00000001u << 13)     /*!< Set to 1 when valid Air Data (altitude and/or true airspeed) is received. */
#define SBG_ECOM_AIDING_VEL1_RECV               (0x00000001u << 14)     /*!< Set to 1 when valid generic velocity 1 data is received. */

//----------------------------------------------------------------------//
//- Status definitions                                                 -//
//----------------------------------------------------------------------//

/*!
 * Stores global status data.
 */
typedef struct _SbgEComLogStatus
{
    uint32_t    timeStamp;                                              /*!< Time in us since the sensor power up. */
    uint16_t    generalStatus;                                          /*!< General status bitmask and enum. */
    uint32_t    comStatus;                                              /*!< Communication status bitmask and enum. */
    uint16_t    comStatus2;                                             /*!< Second communication status bitmask and enum (used since v4.0). */
    uint32_t    aidingStatus;                                           /*!< Aiding equipments status bitmask and enum. */
    uint32_t    reserved2;                                              /*!< Reserved status field for future use. */
    uint16_t    reserved3;                                              /*!< Reserved status field for future use. */
    uint32_t    uptime;                                                 /*!< System uptime in seconds - 0 if N/A (added v1.7). */
    uint8_t     cpuUsage;                                               /*!< Main CPU usage in percent - 0xFF if N/A (added v5.0). */
} SbgEComLogStatus;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_STATUS message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogStatusReadFromStream(SbgEComLogStatus *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_STATUS message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogStatusWriteToStream(const SbgEComLogStatus *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public getters & helpers                                           -//
//----------------------------------------------------------------------//

/*!
 * Defines the CAN bus status.
 *
 * \param[out]  pLogData                    Log status instance.
 * \param[in]   status                      CAN bus status to set.
 */
void sbgEComLogStatusSetCanBusStatus(SbgEComLogStatus *pLogData, SbgEComCanBusStatus status);

/*!
 * Returns the CAN bus status.
 *
 * \param[in]   pLogData                    Log status instance.
 * \return                                  CAN bus status.
 */
SbgEComCanBusStatus sbgEComLogStatusGetCanBusStatus(const SbgEComLogStatus *pLogData);

/*!
 * Returns true if the CPU usage in percent is available.
 *
 * \param[in]   pLogData                    Log status instance.
 * \return                                  true if available.
 */
bool sbgEComLogStatusIsCpuUsageAvailable(const SbgEComLogStatus *pLogData);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogStatus SbgLogStatusData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseStatusData(SbgStreamBuffer *pStreamBuffer, SbgEComLogStatus *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteStatusData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogStatus *pLogData));

SBG_DEPRECATED(SbgEComCanBusStatus sbgEComLogStatusGetCanStatus(uint32_t status));
SBG_DEPRECATED(uint32_t sbgEComLogStatusBuildCommunicationStatus(SbgEComCanBusStatus canStatus, uint32_t masks));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_STATUS_H
