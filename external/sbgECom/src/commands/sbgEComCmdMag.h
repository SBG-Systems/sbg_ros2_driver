/*!
 * \file            sbgEComCmdMag.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            11 June 2014
 *
 * \brief           Magnetometer aiding module configuration & on-board magnetic calibration commands.
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

#ifndef SBG_ECOM_CMD_MAG_H
#define SBG_ECOM_CMD_MAG_H

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
//- Magnetometer definitions                                           -//
//----------------------------------------------------------------------//

/*!
 * Defines the algorithm mode used for compensating magnetometers against soft and hard iron effects.
 *
 * The 2D calibration mode can only be used by the INS filter when roll and pitch are minimal (± 5°)
 * as it assumes a near-horizontal orientation of the sensor.
 * 
 * The 3D calibration mode, in contrast, provides a valid magnetic field compensation for any sensor orientation,
 * thereby improving the overall quality and reliability of the magnetic heading data.
 */
typedef enum _SbgEComMagCalibMode
{
    SBG_ECOM_MAG_CALIB_MODE_2D          = 1,                    /*!< Calibration using 2D mode, requiring at least a full 360° and motions less than  5° */
    SBG_ECOM_MAG_CALIB_MODE_3D          = 2                     /*!< Calibration using 3D mode, accurate in all orientations but needs motions greater than ± 30° */
} SbgEComMagCalibMode;

/*!
 * Define the expected motions bandwidth during the magnetic field compensation acquisition.
 * 
 * \note This parameter is deprecated and has no effect for ELLIPSE firmware v3.0 and above.  
 * For ELLIPSE firmware v2.x and before, please use SBG_ECOM_MAG_CALIB_HIGH_BW.
 */
typedef enum _SbgEComMagCalibBandwidth
{
    SBG_ECOM_MAG_CALIB_LOW_BW       = 0,                        /*!< Not used anymore. */
    SBG_ECOM_MAG_CALIB_MEDIUM_BW    = 1,                        /*!< Not used anymore. */
    SBG_ECOM_MAG_CALIB_HIGH_BW      = 2                         /*!< Not used anymore. */
} SbgEComMagCalibBandwidth;

/*!
 * General quality indicator of an on-board magnetic calibration.
 */
typedef enum _SbgEComMagCalibQuality
{
    SBG_ECOM_MAG_CALIB_QUAL_OPTIMAL = 0,                        /*!< All acquired points fit very well on a unit sphere after the calibration. */
    SBG_ECOM_MAG_CALIB_QUAL_GOOD    = 1,                        /*!< Small deviations of the magnetic field norm have been detected. The magnetic calibration should although provide accurate heading. */
    SBG_ECOM_MAG_CALIB_QUAL_POOR    = 2,                        /*!< Large deviations of the magnetic field norm have been detected. It may come from external magnetic distortions during the calibration. */
    SBG_ECOM_MAG_CALIB_QUAL_INVALID = 3                         /*!< No valid magnetic calibration has been computed. It could comes from too much magnetic disturbances, insufficient or invalid motions. */
} SbgEComMagCalibQuality;

/*!
 * Confidence indicator on results of an on-bard magnetic calibration.
 */
typedef enum _SbgEComMagCalibConfidence
{
    SBG_ECOM_MAG_CALIB_TRUST_HIGH   = 0,                        /*!< Reported quality indicator can be trusted as enough remarkable magnetic field points have been acquired. */
    SBG_ECOM_MAG_CALIB_TRUST_MEDIUM = 1,                        /*!< Few remarkable magnetic field points have been used to compute the magnetic calibration leading to a medium confidence in reported quality indicators. */
    SBG_ECOM_MAG_CALIB_TRUST_LOW    = 2                         /*!< Even if the quality indicator could report an excellent calibration,
                                                                     The data set used to compute the magnetic calibration was not meaningful enough to compute meaningful quality indicators.
                                                                     This calibration should be used carefully. */
} SbgEComMagCalibConfidence;

/*!
 * Status bit masks used to report advanced information on the on-board magnetic calibration.
 */
#define SBG_ECOM_MAG_CALIB_NOT_ENOUGH_POINTS    (0x0001u << 0)  /*!< Not enough valid magnetic points have been acquired. */
#define SBG_ECOM_MAG_CALIB_TOO_MUCH_DISTORTIONS (0x0001u << 1)  /*!< Unable to compute a magnetic calibration due to magnetic interferences or incorrect data set distribution. */
#define SBG_ECOM_MAG_CALIB_X_MOTION_ISSUE       (0x0001u << 2)  /*!< For a 3D calibration: not enough motion on X axis. For a 2D calibration; too much motion on X axis. */
#define SBG_ECOM_MAG_CALIB_Y_MOTION_ISSUE       (0x0001u << 3)  /*!< For a 3D calibration: not enough motion on Y axis. For a 2D calibration; too much motion on Y axis. */
#define SBG_ECOM_MAG_CALIB_Z_MOTION_ISSUE       (0x0001u << 4)  /*!< For a 3D or 2D calibration: not enough motion on Z axis. */
#define SBG_ECOM_MAG_CALIB_ALIGNMENT_ISSUE      (0x0001u << 5)  /*!< For a 3D calibration: the alignment between the magnetometers and the inertial frame seems to be invalid. */

/*!
 * Defines the magnetometers aiding error model and protocol configuration.
 */
typedef enum _SbgEComMagModelsStdIds
{
    SBG_ECOM_MAG_MODEL_INTERNAL_NORMAL          = 201,          /*!< Internal magnetometer - implements world leading  magnetic rejection and interference mitigation solutions. */
    SBG_ECOM_MAG_MODEL_INTERNAL_RESERVED        = 202,          /*!< Internal magnetometer - only used for backward compatibility with ELLIPSE firmware v2.x */
    SBG_ECOM_MAG_MODEL_ECOM_NORMAL              = 203,          /*!< External magnetometer - sbgECom protocol, implements world leading  magnetic rejection and interference mitigation solutions. */
} SbgEComMagModelsStdId;

//----------------------------------------------------------------------//
//- Magnetometer configuration                                         -//
//----------------------------------------------------------------------//

/*!
 * Holds all necessary information for Magnetometer module data rejection.
 */
typedef struct _SbgEComMagRejectionConf
{
    SbgEComRejectionMode    magneticField;                  /*!< Rejection mode for magnetic field. */
} SbgEComMagRejectionConf;

/*!
 * Helper structure to retrieve on-board magnetic calibration results.
 */
typedef struct _SbgEComMagCalibResults
{
    SbgEComMagCalibQuality      quality;                    /*!< General magnetic calibration quality indicator. */
    SbgEComMagCalibConfidence   confidence;                 /*!< Confidence indicator that should be read to interpret the quality indicator. */
    uint16_t                    advancedStatus;             /*!< Set of bit masks used to report advanced information on the magnetic calibration status.*/

    float                       beforeMeanError;            /*!< Mean magnetic field norm error observed before calibration. */
    float                       beforeStdError;             /*!< Standard deviation of the magnetic field norm error observed before calibration. */
    float                       beforeMaxError;             /*!< Maximum magnetic field norm error observed before calibration. */

    float                       afterMeanError;             /*!< Mean magnetic field norm error observed after calibration. */
    float                       afterStdError;              /*!< Standard deviation of the magnetic field norm error observed after calibration. */
    float                       afterMaxError;              /*!< Maximum magnetic field norm error observed after calibration. */

    float                       meanAccuracy;               /*!< Mean expected heading accuracy in radians. */
    float                       stdAccuracy;                /*!< Standard deviation of the expected heading accuracy in radians. */
    float                       maxAccuracy;                /*!< Maximum expected heading accuracy in radians. */

    uint16_t                    numPoints;                  /*!< Number of magnetic field points stored internally and used to compute the magnetic calibration. */
    uint16_t                    maxNumPoints;               /*!< Maximum number of magnetic field points that can be stored internally. */
    float                       offset[3];                  /*!< Computed Hard Iron correction vector offset. */
    float                       matrix[9];                  /*!< Computed Hard & Soft Iron correction matrix. */
} SbgEComMagCalibResults;

//----------------------------------------------------------------------//
//- Magnetometer commands                                              -//
//----------------------------------------------------------------------//

/*!
 * Set magnetometer error model id.
 *
 * \param[in]   pHandle                     A valid sbgECom handle
 * \param[in]   modelId                     Magnetometer model id to set
 * \return                                  SBG_NO_ERROR if the command has been executed successfully
 */
SbgErrorCode sbgEComCmdMagSetModelId(SbgEComHandle *pHandle, SbgEComMagModelsStdId modelId);

/*!
 * Retrieve magnetometer error model id
 *
 * \param[in]   pHandle                     A valid sbgECom handle
 * \param[out]  pModelId                    Retrieved magnetometer model id
 * \return                                  SBG_NO_ERROR if the command has been executed successfully
 */
SbgErrorCode sbgEComCmdMagGetModelId(SbgEComHandle *pHandle, SbgEComMagModelsStdId *pModelId);

/*!
 * Retrieve the rejection configuration of the magnetometer module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pRejectConf                 Pointer to a SbgEComMagRejectionConf struct to hold rejection configuration of the magnetometer module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdMagGetRejection(SbgEComHandle *pHandle, SbgEComMagRejectionConf *pRejectConf);

/*!
 * Set the rejection configuration of the magnetometer module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pRejectConf                 Pointer to a SbgEComMagRejectionConf struct holding rejection configuration for the magnetometer module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdMagSetRejection(SbgEComHandle *pHandle, const SbgEComMagRejectionConf *pRejectConf);

/*!
 * Set the magnetometers soft and hard iron compensation parameters (Firmware v2.x and earlier).
 * 
 * \note: This command should ONLY be used for ELLIPSE firmware v2 and below.  
 * Please use sbgEComCmdMagSetCalibData2 for ELLIPSE firmware v3 and after.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pOffset                     Magnetometers calibration offset vector (array of 3 floats).
 * \param[in]   pMatrix                     Magnetometers calibration 3x3 matrix  (array of 9 floats).
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdMagSetCalibData(SbgEComHandle *pHandle, const float *pOffset, const float *pMatrix);

/*!
 * Set the magnetometers soft and hard iron compensation parameters (Firmware v3.x and later).
 * 
 * With the introduction of ELLIPSE firmware version 3, the soft/hard iron compensation algorithm mode information
 * is provided to refine magnetometer aiding within the Inertial Navigation System (INS) filter.
 *
 * \note: Firmware versions 2 and earlier do not support this feature and 'sbgEComCmdMagSetCalibData' method should be used instead.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pOffset                     Magnetometers calibration offset vector (array of 3 floats).
 * \param[in]   pMatrix                     Magnetometers calibration 3x3 matrix (array of 9 floats).
 * \param[in]   mode                        Defines if the magnetic calibration is a 2D or 3D one.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdMagSetCalibData2(SbgEComHandle *pHandle, const float *pOffset, const float *pMatrix, SbgEComMagCalibMode mode);

//----------------------------------------------------------------------//
//- Magnetometer on-board calibration commands                         -//
//----------------------------------------------------------------------//

/*!
 * Start the magnetic calibration process.
 *
 * As soon as this command is sent, the device will start logging magnetic field data internally.
 * This set of data will be used later by the magnetic calibration algorithms to map the surrounding magnetic field.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   mode                        Define which magnetic calibration type to perform. It could be 3D or 2D.
 * \param[in]   bandwidth                   Not used anymore for ELLIPSE firmware v3.0, this value has no effect.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdMagStartCalib(SbgEComHandle *pHandle, SbgEComMagCalibMode mode, SbgEComMagCalibBandwidth bandwidth);

/*!
 * This command computes a magnetic calibration solution based on the magnetic field logged since the last call to the command SBG_ECOM_CMD_START_MAG_CALIB (15).
 *
 * As soon as the computations are done, the device will answer with quality indicators, status flags and if possible a valid magnetic calibration matrix and offset.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pCalibResults               Pointer on a SbgEComMagCalibResults structure that can hold on-board magnetic calibration results and status.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdMagComputeCalib(SbgEComHandle *pHandle, SbgEComMagCalibResults *pCalibResults);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

#ifdef SBG_ECOM_USE_DEPRECATED_MACROS
    #define SBG_ECOM_MAG_MODEL_NORMAL               (SBG_ECOM_MAG_MODEL_INTERNAL_NORMAL)
    #define SBG_ECOM_MAG_MODEL_NOISY_MAG_TOLERANT   (SBG_ECOM_MAG_MODEL_INTERNAL_RESERVED)
#endif


#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_MAG_H
