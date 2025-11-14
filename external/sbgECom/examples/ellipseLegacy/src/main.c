/*!
 * \file            main.c
 * \author          SBG Systems
 * \date            28/03/2014
 *
 * \brief           C example that showcase ELLIPSE configuration and log parsing.
 *
 * This small example demonstrates how to initialize the sbgECom library
 * to read data from an Ellipse using callbacks.
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

// sbgCommonLib headers
#include <sbgCommon.h>
#include <version/sbgVersion.h>

// sbgECom headers
#include <sbgEComLib.h>

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

/*!
 * Callback definition called each time a new log is received.
 * 
 * \param[in]   pHandle                                 Valid handle on the sbgECom instance that has called this callback.
 * \param[in]   msgClass                                Class of the message we have received
 * \param[in]   msg                                     Message ID of the log received.
 * \param[in]   pLogData                                Contains the received log data as an union.
 * \param[in]   pUserArg                                Optional user supplied argument.
 * \return                                              SBG_NO_ERROR if the received log has been used successfully.
 */
static SbgErrorCode onLogReceived(SbgEComHandle *pHandle, SbgEComClass msgClass, SbgEComMsgId msg, const SbgEComLogUnion *pLogData, void *pUserArg)
{
    assert(pLogData);

    SBG_UNUSED_PARAMETER(pHandle);
    SBG_UNUSED_PARAMETER(pUserArg);

    if (msgClass == SBG_ECOM_CLASS_LOG_ECOM_0)
    {
        //
        // Handle separately each received data according to the log ID
        //
        switch (msg)
        {
        case SBG_ECOM_LOG_EKF_EULER:
            //
            // Simply display euler angles in real time
            //
            printf("Euler Angles: %3.1f\t%3.1f\t%3.1f\tStd Dev:%3.1f\t%3.1f\t%3.1f   \r",
                sbgRadToDegf(pLogData->ekfEulerData.euler[0]),          sbgRadToDegf(pLogData->ekfEulerData.euler[1]),          sbgRadToDegf(pLogData->ekfEulerData.euler[2]),
                sbgRadToDegf(pLogData->ekfEulerData.eulerStdDev[0]),    sbgRadToDegf(pLogData->ekfEulerData.eulerStdDev[1]),    sbgRadToDegf(pLogData->ekfEulerData.eulerStdDev[2]));
            break;
        default:
            break;
        }
    }

    return SBG_NO_ERROR;
}

/*!
 * Get and print product info.
 *
 * \param[in]   pECom                   SbgECom instance.
 * \return                              SBG_NO_ERROR if successful.
 */
static SbgErrorCode getAndPrintProductInfo(SbgEComHandle *pECom)
{
    SbgErrorCode                    errorCode;
    SbgEComDeviceInfo               deviceInfo;

    assert(pECom);

    //
    // Get device information
    //
    errorCode = sbgEComCmdGetInfo(pECom, &deviceInfo);

    //
    // Display device information if no error
    //
    if (errorCode == SBG_NO_ERROR)
    {
        char    calibVersionStr[32];
        char    hwRevisionStr[32];
        char    fmwVersionStr[32];      

        sbgVersionToStringEncoded(deviceInfo.calibationRev, calibVersionStr, sizeof(calibVersionStr));
        sbgVersionToStringEncoded(deviceInfo.hardwareRev, hwRevisionStr, sizeof(hwRevisionStr));
        sbgVersionToStringEncoded(deviceInfo.firmwareRev, fmwVersionStr, sizeof(fmwVersionStr));

        printf("      Serial Number: %09"PRIu32"\n",    deviceInfo.serialNumber);
        printf("       Product Code: %s\n",             deviceInfo.productCode);
        printf("  Hardware Revision: %s\n",             hwRevisionStr);
        printf("   Firmware Version: %s\n",             fmwVersionStr);
        printf("     Calib. Version: %s\n",             calibVersionStr);
        printf("\n");
    }
    else
    {
        SBG_LOG_WARNING(errorCode, "Unable to retrieve device information");
    }

    return errorCode;
}

/*!
 * Execute the ellipseMinimal example given an opened and valid interface.
 * 
 * \param[in]   pInterface                          Interface used to communicate with the device.
 * \return                                          SBG_NO_ERROR if successful.
 */
static SbgErrorCode ellipseMinimalProcess(SbgInterface *pInterface)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComHandle           comHandle;
        
    assert(pInterface);

    //
    // Create the sbgECom library and associate it with the created interfaces
    //
    errorCode = sbgEComInit(&comHandle, pInterface);

    //
    // Test that the sbgECom has been initialized
    //
    if (errorCode == SBG_NO_ERROR)
    {
        //
        // Welcome message
        //
        printf("Welcome to the ELLIPSE minimal example.\n");
        printf("sbgECom version %s\n\n", SBG_E_COM_VERSION_STR);

        //
        // Query and display produce info, don't stop if there is an error
        //
        getAndPrintProductInfo(&comHandle);

        //
        // Showcase how to configure some output logs to 25 Hz, don't stop if there is an error
        //
        errorCode = sbgEComCmdOutputSetConf(&comHandle, SBG_ECOM_OUTPUT_PORT_A, SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_IMU_DATA, SBG_ECOM_OUTPUT_MODE_DIV_8);

        if (errorCode != SBG_NO_ERROR)
        {
            SBG_LOG_WARNING(errorCode, "Unable to configure SBG_ECOM_LOG_IMU_DATA log");
        }

        errorCode = sbgEComCmdOutputSetConf(&comHandle, SBG_ECOM_OUTPUT_PORT_A, SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EKF_EULER, SBG_ECOM_OUTPUT_MODE_DIV_8);

        if (errorCode != SBG_NO_ERROR)
        {
            SBG_LOG_WARNING(errorCode, "Unable to configure SBG_ECOM_LOG_EKF_EULER log");
        }

        //
        // Define callbacks for received data and display header
        //
        sbgEComSetReceiveLogCallback(&comHandle, onLogReceived, NULL);
        printf("Euler Angles display with estimated standard deviation - degrees\n");

        //
        // Loop until the user exist
        //
        while (1)
        {
            //
            // Try to read a frame
            //
            errorCode = sbgEComHandle(&comHandle);

            //
            // Test if we have to release some CPU (no frame received)
            //
            if (errorCode == SBG_NOT_READY)
            {
                //
                // Release CPU
                //
                sbgSleep(1);
            }
            else
            {
                SBG_LOG_ERROR(errorCode, "Unable to process incoming sbgECom logs");
            }
        }

        //
        // Close the sbgECom library
        //
        sbgEComClose(&comHandle);
    }
    else
    {
        SBG_LOG_ERROR(errorCode, "Unable to initialize the sbgECom library");
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//  Main program                                                        //
//----------------------------------------------------------------------//

/*!
 * Program entry point usage: ellipseMinimal COM1 921600
 * 
 * \param[in]   argc                    Number of input arguments.
 * \param[in]   argv                    Input arguments as an array of strings.
 * \return                              EXIT_SUCCESS if successful.
 */
int main(int argc, char** argv)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    SbgInterface        sbgInterface;
    int                 exitCode;

    SBG_UNUSED_PARAMETER(argc);
    SBG_UNUSED_PARAMETER(argv);

    if (argc == 3)
    {
        //
        // Create a serial interface to communicate with the PULSE
        //
        errorCode = sbgInterfaceSerialCreate(&sbgInterface, argv[1], atoi(argv[2]));

        if (errorCode == SBG_NO_ERROR)
        {
            errorCode = ellipseMinimalProcess(&sbgInterface);

            if (errorCode == SBG_NO_ERROR)
            {
                exitCode = EXIT_SUCCESS;
            }
            else
            {
                exitCode = EXIT_FAILURE;
            }

            sbgInterfaceDestroy(&sbgInterface);
        }
        else
        {
            SBG_LOG_ERROR(errorCode, "unable to open serial interface");
            exitCode = EXIT_FAILURE;
        }
    }
    else
    {
        printf("Invalid input arguments, usage: ellipseMinimal SERIAL_DEVICE SERIAL_BAUDRATE\n");
        exitCode = EXIT_FAILURE;
    }
    
    return exitCode;
}
