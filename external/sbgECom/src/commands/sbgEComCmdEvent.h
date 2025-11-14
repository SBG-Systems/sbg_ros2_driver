/*!
 * \file            sbgEComCmdEvent.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            11 June 2014
 *
 * \brief           Input/output event markers configuration commands.
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

#ifndef SBG_ECOM_CMD_EVENT_H
#define SBG_ECOM_CMD_EVENT_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Event definitions                                                  -//
//----------------------------------------------------------------------//

/*!
 * List of sync in signals available.
 */
typedef enum _SbgEComSyncInId
{
    SBG_ECOM_SYNC_IN_A = 0,                         /*!< Sync IN A */
    SBG_ECOM_SYNC_IN_B = 1,                         /*!< Sync IN B */
    SBG_ECOM_SYNC_IN_C = 2,                         /*!< Sync IN C */
    SBG_ECOM_SYNC_IN_D = 3                          /*!< Sync IN D */
} SbgEComSyncInId;

/*!
 * List of available sensitivities for sync in signals.
 */
typedef enum _SbgEComSyncInSensitivity
{
    SBG_ECOM_SYNC_IN_DISABLED       = 0,            /*!< This trigger is turned OFF. */
    SBG_ECOM_SYNC_IN_FALLING_EDGE   = 1,            /*!< The trigger will be activated by a falling edge. */
    SBG_ECOM_SYNC_IN_RISING_EDGE    = 2,            /*!< The trigger will be activated by a rising edge. */
    SBG_ECOM_SYNC_IN_BOTH_EDGES     = 3             /*!< The trigger is activated by a level change (rising or falling edge). */
} SbgEComSyncInSensitivity;

/*!
 * List of sync out signals available.
 */
typedef enum _SbgEComSyncOutId
{
    SBG_ECOM_SYNC_OUT_A = 0,                        /*!< Synchronization output A */
    SBG_ECOM_SYNC_OUT_B = 1                         /*!< Synchronization output B */
} SbgEComSyncOutId;

/*!
 * Logic and synchronization output types
 */
typedef enum _SbgEComSyncOutFunction
{
    SBG_ECOM_SYNC_OUT_MODE_DISABLED     = 0,        /*!< Output is disabled. */
    SBG_ECOM_SYNC_OUT_MODE_MAIN_LOOP    = 1,        /*!< Output is generated at  200Hz. */
    SBG_ECOM_SYNC_OUT_MODE_DIV_2        = 2,        /*!< Output is generated at  100Hz. */
    SBG_ECOM_SYNC_OUT_MODE_DIV_4        = 4,        /*!< Output is generated at   50Hz. */
    SBG_ECOM_SYNC_OUT_MODE_DIV_5        = 5,        /*!< Output is generated at   40Hz. (only ELLIPSE fmw v3) */
    SBG_ECOM_SYNC_OUT_MODE_DIV_8        = 8,        /*!< Output is generated at   25Hz. */
    SBG_ECOM_SYNC_OUT_MODE_DIV_10       = 10,       /*!< Output is generated at   20Hz. */
    SBG_ECOM_SYNC_OUT_MODE_DIV_20       = 20,       /*!< Output is generated at   10Hz. */
    SBG_ECOM_SYNC_OUT_MODE_DIV_40       = 40,       /*!< Output is generated at    5Hz. */
    SBG_ECOM_SYNC_OUT_MODE_DIV_100      = 100,      /*!< Output is generated at    2Hz. (only ELLIPSE fmw v3). */
    SBG_ECOM_SYNC_OUT_MODE_DIV_200      = 200,      /*!< Output is generated at    1Hz. */

    SBG_ECOM_SYNC_OUT_MODE_1_MS         = 1001,     /*!< Output is generated at 1000 Hz. (only ELLIPSE fmw v3) */
    SBG_ECOM_SYNC_OUT_MODE_2_MS         = 1002,     /*!< Output is generated at  500 Hz. (only ELLIPSE fmw v3) */
    SBG_ECOM_SYNC_OUT_MODE_4_MS         = 1004,     /*!< Output is generated at  250 Hz. (only ELLIPSE fmw v3) */
        
    SBG_ECOM_SYNC_OUT_MODE_PPS          = 10000,    /*!< Pulse Per Second. Same as SBG_ECOM_SYNC_OUT_MODE_DIV_200. */
    SBG_ECOM_SYNC_OUT_MODE_DIRECT_PPS   = 10100,    /*!< The internal GNSS PPS signal is directly routed to the Sync Out.
                                                         This mode is only valid for the following products:
                                                            - ELLIPSE-N with hardware revisions above 1.2.1.0
                                                            - ELLIPSE-N & ELLIPSE-D Generation 3 or above.
                                                         Polarity and duration parameters are ignored with this specific mode. */
} SbgEComSyncOutFunction;

/*!
 * Logic output polarity
 */
typedef enum _SbgEComSyncOutPolarity
{
    SBG_ECOM_SYNC_OUT_FALLING_EDGE  = 0,            /*!< The output pin will generate a falling edge*/
    SBG_ECOM_SYNC_OUT_RISING_EDGE   = 1,            /*!< The output pin will generate a rising edge */
    SBG_ECOM_SYNC_OUT_TOGGLE        = 2,            /*!< The pulse is a level change */
} SbgEComSyncOutPolarity;

//----------------------------------------------------------------------//
//- Event configurations                                               -//
//----------------------------------------------------------------------//

/*!
 * Helper structure for sync in configuration.
 */
typedef struct _SbgEComSyncInConf
{
    SbgEComSyncInSensitivity    sensitivity;    /*!< Sensitivity of the sync in. */
    int32_t                     delay;          /*!< Delay to take into account for the sync in. (in ns)*/
} SbgEComSyncInConf;

/*!
 * Helper structure for sync out configuration.
 */
typedef struct _SbgEComSyncOutConf
{
    SbgEComSyncOutFunction      outputFunction; /*!< Output function of the sync out pin */
    SbgEComSyncOutPolarity      polarity;       /*!< Polarity of the sync out. */
    uint32_t                    duration;       /*!< Pulse width for the sync out (in ns). */
} SbgEComSyncOutConf;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 *  Retrieve the configuration of a Sync In.
 * 
 *  \param[in]  pHandle                     A valid sbgECom handle.
 *  \param[in]  syncInId                    The id of the sync whose configuration is to be retrieved.
 *  \param[out] pConf                       Pointer to a SbgEComSyncInConf to contain the current configuration of the sync in.
 *  \return                                 SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSyncInGetConf(SbgEComHandle *pHandle, SbgEComSyncInId syncInId, SbgEComSyncInConf *pConf);

/*!
 *  Set the configuration of a Sync In.
 * 
 *  \param[in]  pHandle                     A valid sbgECom handle.
 *  \param[in]  syncInId                    The id of the sync whose configuration is to be set.
 *  \param[in]  pConf                       Pointer to a SbgEComSyncInConf that contains the new configuration for the sync in.
 *  \return                                 SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSyncInSetConf(SbgEComHandle *pHandle, SbgEComSyncInId syncInId, const SbgEComSyncInConf *pConf);

/*!
 *  Retrieve the configuration of a Sync Out.
 * 
 *  \param[in]  pHandle                     A valid sbgECom handle.
 *  \param[in]  syncOutId                   The id of the sync whose configuration is to be retrieved.
 *  \param[out] pConf                       Pointer to a SbgEComSyncOutConf to contain the current configuration of the sync out.
 *  \return                                 SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSyncOutGetConf(SbgEComHandle *pHandle, SbgEComSyncOutId syncOutId, SbgEComSyncOutConf *pConf);

/*!
 *  Set the configuration of a Sync Out.
 * 
 *  \param[in]  pHandle                     A valid sbgECom handle.
 *  \param[in]  syncOutId                   The id of the sync whose configuration is to be set.
 *  \param[in]  pConf                       Pointer to a SbgEComSyncOutConf that contains the new configuration for the sync Out.
 *  \return                                 SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSyncOutSetConf(SbgEComHandle *pHandle, SbgEComSyncOutId syncOutId, const SbgEComSyncOutConf *pConf);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_EVENT_H
