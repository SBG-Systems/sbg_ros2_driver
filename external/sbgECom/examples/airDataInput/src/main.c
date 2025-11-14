/*!
 * \file            main.c
 * \author          SBG Systems
 * \date            28/03/2014
 *
 * \brief           Send AirData aiding mesurements to an ELLIPSE and read back data
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

// sbgECom headers
#include <sbgEComLib.h>

//----------------------------------------------------------------------//
//- Private methids                                                    -//
//----------------------------------------------------------------------//

/*!
 * Generate a random float number between min and max.
 *
 * \param[in]   randMin                             Minimum random value to generate.
 * \param[in]   randMax                             Maximum random value to generate.
 * \return                                          Float randome value between min and max.
 */
static float airDataInputRandFloat(float randMin, float randMax)
{
    assert(randMin <= randMax);

    if (randMin == randMax)
    {
        return randMin;
    }
    else
    {
        return (randMax - randMin) * ((float)rand() / RAND_MAX) + randMin;
    }
}

/*!
 * Send an AirData packet to the device with random values.
 *
 * \param[in]   pHandle                             SbgECom handle
 * \return                                          SBG_NO_ERROR if the packet has been sent successfully.
 */
static SbgErrorCode airDataInputSendOneLog(SbgEComHandle *pHandle)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    SbgEComLogAirData   airDataLog;
    uint8_t             outputBuffer[64];
    SbgStreamBuffer     outputStream;

    assert(pHandle);

    //
    // Create a random AirData struct
    //
    memset(&airDataLog, 0x00, sizeof(airDataLog));

    //
    // We consider a fixed delay of 10 ms
    //
    airDataLog.timeStamp = 10000;
    airDataLog.status |= SBG_ECOM_AIR_DATA_TIME_IS_DELAY;

    //
    // We create a random altitude between 0 to 8000 meters
    //
    airDataLog.altitude = airDataInputRandFloat(0.0f, 8000.0f);
    airDataLog.status |= SBG_ECOM_AIR_DATA_ALTITUDE_VALID;

    //
    // We create a random airspeed between 0 to 12 m.s^-1
    //
    airDataLog.trueAirspeed = airDataInputRandFloat(0.0f, 12.0f);
    airDataLog.status |= SBG_ECOM_AIR_DATA_AIRPSEED_VALID;

    //
    // Write the payload
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    errorCode = sbgEComLogAirDataWriteToStream(&airDataLog, &outputStream);

    if (errorCode == SBG_NO_ERROR)
    {
        //
        // Send the sbgECom log to the device
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_AIR_DATA, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        if (errorCode != SBG_NO_ERROR)
        {
            SBG_LOG_ERROR(errorCode, "Unable to send the AirData log");
        }
    }
    else
    {
        SBG_LOG_ERROR(errorCode, "Unable to write the AirData payload.");
    }

    return errorCode;
}

/*!
 * Execute the airDataInput example given an opened and valid interface.
 * 
 * \param[in]   pInterface                          Interface used to communicate with the device.
 * \return                                          SBG_NO_ERROR if successful.
 */
static SbgErrorCode airDataInputProcess(SbgInterface *pInterface)
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
        printf("Welcome to the AirDataInput example.\n");
        printf("sbgECom version %s\n\n", SBG_E_COM_VERSION_STR);

        //
        // Loop until the user kill the process
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
                // Send one AirData log
                //
                if (airDataInputSendOneLog(&comHandle) == SBG_NO_ERROR)
                {
                    SBG_LOG_DEBUG("Airdata log sent!");
                }
                else
                {
                    SBG_LOG_WARNING(errorCode, "Unable to send AirData log");
                }

                //
                // Wait for 100 ms to only send AirData at 10 Hz
                //
                sbgSleep(100);
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
 * Program entry point usage: airDataInput COM1 921600
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
            errorCode = airDataInputProcess(&sbgInterface);

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
        printf("Invalid input arguments, usage: airDataInput SERIAL_DEVICE SERIAL_BAUDRATE\n");
        exitCode = EXIT_FAILURE;
    }
    
    return exitCode;
}
