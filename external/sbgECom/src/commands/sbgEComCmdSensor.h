/*!
 * \file            sbgEComCmdSensor.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            11 June 2014
 *
 * \brief           Motion profile, aiding assignment & sensor installation commands.
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

#ifndef SBG_ECOM_CMD_SENSOR_H
#define SBG_ECOM_CMD_SENSOR_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Sensor definitions                                                 -//
//----------------------------------------------------------------------//

/*!
 * Ports available for the modules.
 */
typedef enum _SbgEComModulePortAssignment
{
    SBG_ECOM_MODULE_PORT_A                      = 0,        /*!< Module connected on PORT_A. */
    SBG_ECOM_MODULE_PORT_B                      = 1,        /*!< Module connected on PORT_B. */
    SBG_ECOM_MODULE_PORT_C                      = 2,        /*!< Module connected on PORT_C. */
    SBG_ECOM_MODULE_PORT_D                      = 3,        /*!< Module connected on PORT_D. */
    SBG_ECOM_MODULE_PORT_E                      = 4,        /*!< Module connected on PORT_E. */
    SBG_ECOM_MODULE_INTERNAL                    = 5,        /*!< Module is connected internally. */
    SBG_ECOM_MODULE_DISABLED                    = 0xFF      /*!< Module is disabled. */
} SbgEComModulePortAssignment;

/*!
 * Synchronization signals available for the modules.
 */
typedef enum _SbgEComModuleSyncAssignment
{
    SBG_ECOM_MODULE_SYNC_DISABLED               = 0,        /*!< Module is disabled. */
    SBG_ECOM_MODULE_SYNC_IN_A                   = 1,        /*!< Synchronization is done using SYNC_IN_A pin. */
    SBG_ECOM_MODULE_SYNC_IN_B                   = 2,        /*!< Synchronization is done using SYNC_IN_B pin. */
    SBG_ECOM_MODULE_SYNC_IN_C                   = 3,        /*!< Synchronization is done using SYNC_IN_C pin. */
    SBG_ECOM_MODULE_SYNC_IN_D                   = 4,        /*!< Synchronization is done using SYNC_IN_D pin. */
    SBG_ECOM_MODULE_SYNC_INTERNAL               = 5,        /*!< Synchronization is internal. */
    SBG_ECOM_MODULE_SYNC_OUT_A                  = 6,        /*!< Synchronization signal is output on SYNC_OUT_A. */
    SBG_ECOM_MODULE_SYNC_OUT_B                  = 7,        /*!< Synchronization signal is output on SYNC_OUT_B. */
} SbgEComModuleSyncAssignment;

/*!
 * List of configurations available for the odometer.
 */
typedef enum _SbgEComOdometerPinAssignment
{
    SBG_ECOM_MODULE_ODO_DISABLED                = 0,        /*!< Odometer is disabled. */
    SBG_ECOM_MODULE_ODO_A                       = 1,        /*!< Odometer connected only to ODO_A (unidirectional).. */
    SBG_ECOM_MODULE_ODO_A_B                     = 2,        /*!< Odometer connected to both ODO_A (signal A) and ODO_B (Signal B or direction) for bidirectional odometer.. */
    SBG_ECOM_MODULE_ODO_CAN                     = 3,        /*!< Vehicle odometer using CAN (OBD-II). */
} SbgEComOdometerPinAssignment;

/*!
 * This enum defines the different motion profile IDs available in standard
 */
typedef enum _SbgEComMotionProfileStdIds
{
    SBG_ECOM_MOTION_PROFILE_GENERAL_PURPOSE     = 1,        /*!< Should be used as a default when other profiles do not apply */
    SBG_ECOM_MOTION_PROFILE_AUTOMOTIVE          = 2,        /*!< Dedicated to car applications with strict lateral velocity constraints. */
    SBG_ECOM_MOTION_PROFILE_MARINE              = 3,        /*!< Used in marine and underwater applications */
    SBG_ECOM_MOTION_PROFILE_AIRPLANE            = 4,        /*!< For fixed wings aircraft */
    SBG_ECOM_MOTION_PROFILE_HELICOPTER          = 5,        /*!< For rotary wing aircraft */
    SBG_ECOM_MOTION_PROFILE_PEDESTRIAN          = 6,        /*!< Pedestrian applications using foot odometer (only for ELLIPSE firmware v3.x and above). */
    SBG_ECOM_MOTION_PROFILE_UAV_ROTARY_WING     = 7,        /*!< For rotary wing UAVs that have low dynamics */
    SBG_ECOM_MOTION_PROFILE_HEAVY_MACHINERY     = 8,        /*!< For vibrating applications with low dynamics and no specific travel direction */
    SBG_ECOM_MOTION_PROFILE_STATIC              = 9,        /*!< Static motion profile that delivers stable results for 27/7 operations. */
    SBG_ECOM_MOTION_PROFILE_TRUCK               = 10,       /*!< Truck applications with medium lateral velocity constraints (only for ELLIPSE firmware v3.x and above). */
    SBG_ECOM_MOTION_PROFILE_RAILWAY             = 11,       /*!< Train applications with relaxed lateral velocity constraints (only for ELLIPSE firmware v3.x and above). */
    SBG_ECOM_MOTION_PROFILE_OFF_ROAD_VEHICLE    = 12,       /*!< Off road vehicle such as Jeeps, expect slide slip and drift (only for ELLIPSE firmware v3.x and above). */
    SBG_ECOM_MOTION_PROFILE_UNDERWATER          = 13        /*!< Marine underwater with low dynamics such as for ROV (only for ELLIPSE firmware v3.x and above). */
} SbgEComMotionProfileStdIds;

//----------------------------------------------------------------------//
//- Event configurations                                               -//
//----------------------------------------------------------------------//

/*!
 * Helper structure for module assignments
 */
typedef struct _SbgEComAidingAssignConf
{
    SbgEComModulePortAssignment     gps1Port;           /*!< GNSS module port assignment. */
    SbgEComModuleSyncAssignment     gps1Sync;           /*!< GNSS module sync assignment. */
    SbgEComModulePortAssignment     dvlPort;            /*!< Port on which the DVL is connected */
    SbgEComModuleSyncAssignment     dvlSync;            /*!< Optional sync signal that could be used to timestamp the DVL data. */
    SbgEComModulePortAssignment     rtcmPort;           /*!< RTCM input port assignment for IGNG-N DGPS. */
    SbgEComModulePortAssignment     airDataPort;        /*!< Port on which Air Data aiding is connected. */
    SbgEComOdometerPinAssignment    odometerPinsConf;   /*!< Odometer module pin assignment. */
} SbgEComAidingAssignConf;

/*!
 * Helper structure for sensor alignment details
 */
typedef struct _SbgEComSensorAlignmentInfo
{
    SbgEComAxisDirection    axisDirectionX;         /*!< Sensor X axis direction in vehicle */
    SbgEComAxisDirection    axisDirectionY;         /*!< Sensor Y axis direction in vehicle */
    float                   misRoll;                /*!< Roll angle fine misalignment in rad */
    float                   misPitch;               /*!< Pitch angle fine misalignment in rad */
    float                   misYaw;                 /*!< Yaw angle fine misalignment in rad */
} SbgEComSensorAlignmentInfo;

/*!
 * Helper structure for sensor Initial condition details
 */
typedef struct _SbgEComInitConditionConf
{
    double              latitude;                   /*!< Initial latitude in °, positive north */
    double              longitude;                  /*!< Initial longitude in °, positive east */
    double              altitude;                   /*!< Initial height above ellipsoid in meters, positive up */
    uint16_t            year;                       /*!< Initial Year */
    uint8_t             month;                      /*!< Initial month */
    uint8_t             day;                        /*!< Initial day */
} SbgEComInitConditionConf;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Set the motion profile id used to tune the Kalman Filter to a specific application
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   modelId                     Motion profile id to set
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSensorSetMotionProfileId(SbgEComHandle *pHandle, SbgEComMotionProfileStdIds modelId);

/*!
 * Retrieve the motion profile id.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pModelId                    Retrieved motion profile id
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSensorGetMotionProfileId(SbgEComHandle *pHandle, SbgEComMotionProfileStdIds *pModelId);

/*!
 * Retrieve the initial conditions settings.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pConf                       Pointer to a SbgEComInitConditionConf to contain the current initial conditions settings.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSensorGetInitCondition(SbgEComHandle *pHandle, SbgEComInitConditionConf *pConf);

/*!
 * Set the initial condition configuration.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pConf                       Pointer to a SbgEComInitConditionConf containing the new initial condition configuration.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSensorSetInitCondition(SbgEComHandle *pHandle, const SbgEComInitConditionConf *pConf);

/*!
 * Retrieve the assignment of the aiding sensors.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pConf                       Pointer to a SbgEComAidingAssignConf to contain the current assignment of the aiding sensors.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSensorGetAidingAssignment(SbgEComHandle *pHandle, SbgEComAidingAssignConf *pConf);

/*!
 * Set the assignment of the aiding sensors.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pConf                       Pointer to a SbgEComAidingAssignConf containing the new assignment of the aiding sensors.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSensorSetAidingAssignment(SbgEComHandle *pHandle, const SbgEComAidingAssignConf *pConf);

/*!
 * Retrieve the alignment and lever arm configuration of the sensor.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pAlignConf                  Pointer to a SbgEComSensorAlignmentInfo struct to hold alignment configuration of the sensor.
 * \param[out]  pLeverArm                   Pointer to a table to contain lever arm X, Y, Z components in meters.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSensorGetAlignmentAndLeverArm(SbgEComHandle *pHandle, SbgEComSensorAlignmentInfo *pAlignConf, float *pLeverArm);

/*!
 * Set the alignment and lever arm configuration of the sensor.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pAlignConf                  Pointer to a SbgEComSensorAlignmentInfo struct holding alignment configuration for the sensor.
 * \param[in]   pLeverArm                   Pointer to a table containing lever arm X, Y, Z components in meters.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSensorSetAlignmentAndLeverArm(SbgEComHandle *pHandle, const SbgEComSensorAlignmentInfo *pAlignConf, const float *pLeverArm);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_SENSOR_H
