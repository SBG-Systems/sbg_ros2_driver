// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgEComIds.h>

// Local headers
#include "sbgEComLog.h"
#include "sbgEComLogAirData.h"
#include "sbgEComLogDepth.h"
#include "sbgEComLogDiag.h"
#include "sbgEComLogDvl.h"
#include "sbgEComLogEkf.h"
#include "sbgEComLogEkfRotAccel.h"
#include "sbgEComLogEvent.h"
#include "sbgEComLogGnssHdt.h"
#include "sbgEComLogGnssPos.h"
#include "sbgEComLogGnssVel.h"
#include "sbgEComLogImu.h"
#include "sbgEComLogMag.h"
#include "sbgEComLogMagCalib.h"
#include "sbgEComLogOdometer.h"
#include "sbgEComLogPtp.h"
#include "sbgEComLogRawData.h"
#include "sbgEComLogSat.h"
#include "sbgEComLogSessionInfo.h"
#include "sbgEComLogShipMotion.h"
#include "sbgEComLogStatus.h"
#include "sbgEComLogUsbl.h"
#include "sbgEComLogUtc.h"
#include "sbgEComLogVelocity.h"
#include "sbgEComLogVibMon.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogParse(SbgEComClass msgClass, SbgEComMsgId msgId, const void *pPayload, size_t payloadSize, SbgEComLogUnion *pLogData)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    SbgStreamBuffer     inputStream;

    assert(pPayload);
    assert(payloadSize > 0);
    assert(pLogData);

    //
    // Create an input stream buffer that points to the frame payload so we can easily parse it's content
    //
    sbgStreamBufferInitForRead(&inputStream, pPayload, payloadSize);

    //
    // Handle the different classes of messages differently
    //
    if (msgClass == SBG_ECOM_CLASS_LOG_ECOM_0)
    {
        //
        // Parse the incoming log according to its type
        //
        switch (msgId)
        {
        case SBG_ECOM_LOG_STATUS:
            errorCode = sbgEComLogStatusReadFromStream(&pLogData->statusData, &inputStream);
            break;
        case SBG_ECOM_LOG_IMU_DATA:
            errorCode = sbgEComLogImuLegacyReadFromStream(&pLogData->imuData, &inputStream);
            break;
        case SBG_ECOM_LOG_IMU_SHORT:
            errorCode = sbgEComLogImuShortReadFromStream(&pLogData->imuShort, &inputStream);
            break;
        case SBG_ECOM_LOG_EKF_EULER:
            errorCode = sbgEComLogEkfEulerReadFromStream(&pLogData->ekfEulerData, &inputStream);
            break;
        case SBG_ECOM_LOG_EKF_QUAT:
            errorCode = sbgEComLogEkfQuatReadFromStream(&pLogData->ekfQuatData, &inputStream);
            break;
        case SBG_ECOM_LOG_EKF_NAV:
            errorCode = sbgEComLogEkfNavReadFromStream(&pLogData->ekfNavData, &inputStream);
            break;
        case SBG_ECOM_LOG_EKF_VEL_BODY:
            errorCode = sbgEComLogEkfVelBodyReadFromStream(&pLogData->ekfVelBody, &inputStream);
            break;
        case SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY:
        case SBG_ECOM_LOG_EKF_ROT_ACCEL_NED:
            errorCode = sbgEComLogEkfRotAccelReadFromStream(&pLogData->ekfRotAccel, &inputStream);
            break;
        case SBG_ECOM_LOG_SHIP_MOTION:
        case SBG_ECOM_LOG_SHIP_MOTION_HP:
            errorCode = sbgEComLogShipMotionReadFromStream(&pLogData->shipMotionData, &inputStream);
            break;
        case SBG_ECOM_LOG_ODO_VEL:
            errorCode = sbgEComLogOdometerReadFromStream(&pLogData->odometerData, &inputStream);
            break;
        case SBG_ECOM_LOG_UTC_TIME:
            errorCode = sbgEComLogUtcReadFromStream(&pLogData->utcData, &inputStream);
            break;
        case SBG_ECOM_LOG_PTP_STATUS:
            errorCode = sbgEComLogPtpReadFromStream(&pLogData->ptpData, &inputStream);
            break;
        case SBG_ECOM_LOG_VIB_MON_FFT:
            errorCode = sbgEComLogVibMonFftReadFromStream(&pLogData->vibMonFft, &inputStream);
            break;
        case SBG_ECOM_LOG_VIB_MON_REPORT:
            errorCode = sbgEComLogVibMonReportReadFromStream(&pLogData->vibMonReport, &inputStream);
            break;
        case SBG_ECOM_LOG_GPS1_VEL:
        case SBG_ECOM_LOG_GPS2_VEL:
            errorCode = sbgEComLogGnssVelReadFromStream(&pLogData->gpsVelData, &inputStream);
            break;
        case SBG_ECOM_LOG_GPS1_POS:
        case SBG_ECOM_LOG_GPS2_POS:
            errorCode = sbgEComLogGnssPosReadFromStream(&pLogData->gpsPosData, &inputStream);
            break;
        case SBG_ECOM_LOG_GPS1_HDT:
        case SBG_ECOM_LOG_GPS2_HDT:
            errorCode = sbgEComLogGnssHdtReadFromStream(&pLogData->gpsHdtData, &inputStream);
            break;
        case SBG_ECOM_LOG_GPS1_RAW:
        case SBG_ECOM_LOG_GPS2_RAW:
            errorCode = sbgEComLogRawDataReadFromStream(&pLogData->gpsRawData, &inputStream);
            break;
        case SBG_ECOM_LOG_GPS1_SAT:
        case SBG_ECOM_LOG_GPS2_SAT:
            errorCode = sbgEComLogSatListReadFromStream(&pLogData->satGroupData, &inputStream);
            break;
        case SBG_ECOM_LOG_RTCM_RAW:
            errorCode = sbgEComLogRawDataReadFromStream(&pLogData->rtcmRawData, &inputStream);
            break;
        case SBG_ECOM_LOG_MAG:
            errorCode = sbgEComLogMagReadFromStream(&pLogData->magData, &inputStream);
            break;
        case SBG_ECOM_LOG_MAG_CALIB:
            errorCode = sbgEComLogMagCalibReadFromStream(&pLogData->magCalibData, &inputStream);
            break;
        case SBG_ECOM_LOG_DVL_BOTTOM_TRACK:
            errorCode = sbgEComLogDvlReadFromStream(&pLogData->dvlData, &inputStream);
            break;
        case SBG_ECOM_LOG_DVL_WATER_TRACK:
            errorCode = sbgEComLogDvlReadFromStream(&pLogData->dvlData, &inputStream);
            break;
        case SBG_ECOM_LOG_AIR_DATA:
            errorCode = sbgEComLogAirDataReadFromStream(&pLogData->airData, &inputStream);
            break;
        case SBG_ECOM_LOG_USBL:
            errorCode = sbgEComLogUsblReadFromStream(&pLogData->usblData, &inputStream);
            break;
        case SBG_ECOM_LOG_DEPTH:
            errorCode = sbgEComLogDepthReadFromStream(&pLogData->depthData, &inputStream);
            break;
        case SBG_ECOM_LOG_EVENT_A:
        case SBG_ECOM_LOG_EVENT_B:
        case SBG_ECOM_LOG_EVENT_C:
        case SBG_ECOM_LOG_EVENT_D:
        case SBG_ECOM_LOG_EVENT_E:
        case SBG_ECOM_LOG_EVENT_OUT_A:
        case SBG_ECOM_LOG_EVENT_OUT_B:
            errorCode = sbgEComLogEventReadFromStream(&pLogData->eventMarker, &inputStream);
            break;
        case SBG_ECOM_LOG_DIAG:
            errorCode = sbgEComLogDiagReadFromStream(&pLogData->diagData, &inputStream);
            break;
        case SBG_ECOM_LOG_SESSION_INFO:
            errorCode = sbgEComLogSessionInfoReadFromStream(&pLogData->sessionInfoData, &inputStream);
            break;
        case SBG_ECOM_LOG_VELOCITY_1:
            errorCode = sbgEComLogVelocityReadFromStream(&pLogData->velocityData, &inputStream);
            break;


        default:
            errorCode = SBG_ERROR;
        }
    }
    else if (msgClass == SBG_ECOM_CLASS_LOG_ECOM_1)
    {
        //
        // Parse the message depending on the message ID
        //
        switch ((SbgEComLog1)msgId)
        {
        case SBG_ECOM_LOG_FAST_IMU_DATA:
            errorCode = sbgEComLogImuFastLegacyReadFromStream(&pLogData->fastImuData, &inputStream);
            break;
        default:
            errorCode = SBG_ERROR;
        }
    }
    else
    {
        //
        // Unhandled message class
        //
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

void sbgEComLogCleanup(SbgEComLogUnion *pLogData, SbgEComClass msgClass, SbgEComMsgId msgId)
{
    assert(pLogData);

    SBG_UNUSED_PARAMETER(pLogData);
    SBG_UNUSED_PARAMETER(msgClass);
    SBG_UNUSED_PARAMETER(msgId);

    //
    // Nothing to do for now
    //
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParse(SbgEComClass msgClass, SbgEComMsgId msg, const void *pPayload, size_t payloadSize, SbgEComLogUnion *pLogData)
{
    return sbgEComLogParse(msgClass, msg, pPayload, payloadSize, pLogData);
}
