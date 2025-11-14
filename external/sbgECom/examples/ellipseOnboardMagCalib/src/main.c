/*!
 * \file            main.c
 * \author          SBG Systems
 * \date            30/06/2014
 *
 * \brief           C example that demonstrates the onboard magnetic calibration procedure.
 *
 * The onboard magnetic calibration is done with the three following steps:
 *         - Acquire some magnetic field data
 *         - Compute a magnetic calibration
 *         - Apply the newly computed magnetic calibration
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
 * Returns the last char entered by the user just before the user presses the enter key.
 * 
 * \return                                  The key code entered by the user just before he has pressed the 'enter' key
 *                                          or -1 if only 'enter' has been pressed.
 */
static int32_t getUserChoice(void)
{
    int     userChoice = -1;
    
    //
    // Just drain getc input to only return the latest char just before the enter key is pressed
    //
    for (;;)
    {
        int     keyPressed;

        keyPressed = getc(stdin);

        if (keyPressed != '\n')
        {
            userChoice = keyPressed;
        }
        else if (keyPressed == '\n')
        {
            break;
        }
    }

    return userChoice;
}

/*!
 * Ask the user for a 2D or 3D calibration.
 *
 * \return                  The calibration mode to use.
 */
static SbgEComMagCalibMode askCalibrationMode(void)
{
    SbgEComMagCalibMode     mode;
    int32_t                 keyPressed;

    for (;;)
    {
        printf("Would you like to perform a 2D or 3D calibration?\n"
                "  1) For a 2D calibration\n"
                "  2) For a 3D calibration\n"
                "Select 1 or 2 and press enter: ");

        keyPressed = getUserChoice();

        if (keyPressed == '1')
        {
            mode = SBG_ECOM_MAG_CALIB_MODE_2D;
            break;
        }
        else if (keyPressed == '2')
        {
            mode = SBG_ECOM_MAG_CALIB_MODE_3D;
            break;
        }
        else
        {
            printf("Invalid choice, please retry.\n");
        }
    }

    return mode;
}

/*!
 * Display magnetic calibration results on the console.
 * 
 * \param[in]   mode                            Define which magnetic calibration type has been performed. It could be 3D or 2D
 * \param[in]   pMagCalibResults                Pointer on a read only magnetic calibration results structure.
 */
static void displayMagCalibResults(SbgEComMagCalibMode mode, const SbgEComMagCalibResults *pMagCalibResults)
{
    //
    // Display the magnetic calibration results
    //
    printf("\n======== Magnetic calibration report ========\n");

    //
    // Convert the quality indicator to human readable output
    //
    switch (pMagCalibResults->quality)
    {
    case SBG_ECOM_MAG_CALIB_QUAL_OPTIMAL:
        printf("Quality:\t\toptimal\n");
        break;
    case SBG_ECOM_MAG_CALIB_QUAL_GOOD:
        printf("Quality:\t\tgood\n");
        break;
    case SBG_ECOM_MAG_CALIB_QUAL_POOR:
        printf("Quality:\t\tpoor\n");
        break;
    default:
        printf("Quality:\t\tundefined\n");
    }

    //
    // Convert the confidence indicator to human readable output
    //
    switch (pMagCalibResults->confidence)
    {
    case SBG_ECOM_MAG_CALIB_TRUST_HIGH:
        printf("Confidence:\t\thigh\n");
        break;
    case SBG_ECOM_MAG_CALIB_TRUST_MEDIUM:
        printf("Confidence:\t\tmedium\n");
        break;
    case SBG_ECOM_MAG_CALIB_TRUST_LOW:
        printf("Confidence:\t\tlow\n");
        break;
    default:
        printf("Confidence:\t\tundefined\n");
    }

    //
    // Print advanced status
    //
    printf("Advanced Status:\n");
    if (pMagCalibResults->advancedStatus & SBG_ECOM_MAG_CALIB_NOT_ENOUGH_POINTS)
    {
        printf("\t- Not enough valid points. Maybe you are moving too fast.\n");
    }
    if (pMagCalibResults->advancedStatus & SBG_ECOM_MAG_CALIB_TOO_MUCH_DISTORTIONS)
    {
        printf("\t- Unable to find a calibration solution. Maybe there are too much non static distortions.\n");
    }
    if (pMagCalibResults->advancedStatus & SBG_ECOM_MAG_CALIB_ALIGNMENT_ISSUE)
    {
        printf("\t- The magnetic calibration has troubles to correct the magnetometers and inertial frame alignment.\n");
    }

    //
    // Test if we have a 2D or 3D calibration mode
    //
    if (mode == SBG_ECOM_MAG_CALIB_MODE_2D)
    {
        //
        // In 2D mode, a X or Y motion issue means we have too much motion
        //
        if (pMagCalibResults->advancedStatus & SBG_ECOM_MAG_CALIB_X_MOTION_ISSUE)
        {
            printf("\t- Too much roll motion for a 2D magnetic calibration.\n");
        }
        if (pMagCalibResults->advancedStatus & SBG_ECOM_MAG_CALIB_Y_MOTION_ISSUE)
        {
            printf("\t- Too much pitch motion for a 2D magnetic calibration.\n");
        }
    }
    else
    {
        //
        // In 3D mode, a X or Y motion issue means we have not enough motion
        //
        if (pMagCalibResults->advancedStatus & SBG_ECOM_MAG_CALIB_X_MOTION_ISSUE)
        {
            printf("\t- Not enough roll motion for a 3D magnetic calibration.\n");
        }
        if (pMagCalibResults->advancedStatus & SBG_ECOM_MAG_CALIB_Y_MOTION_ISSUE)
        {
            printf("\t- Not enough pitch motion for a 3D magnetic calibration.\n");
        }
    }

    //
    // Test if we had enough yaw motion to compute a calibration
    //
    if (pMagCalibResults->advancedStatus & SBG_ECOM_MAG_CALIB_Z_MOTION_ISSUE)
    {
        //
        // Test if we are in
        printf("\t- Not enough yaw motion to compute a valid magnetic calibration.\n");
    }

    //
    // Display the number of points used to compute the magnetic calibration
    //
    printf("\n");
    printf("Used Points:\t%"PRIu16"\n", pMagCalibResults->numPoints);
    printf("Max Points:\t%"PRIu16"\n",  pMagCalibResults->maxNumPoints);

    //
    // Display magnetic field deviation errors
    //
    printf( "\n"
            "---------------------------------------------\n"
            "- Magnetic field deviation report           -\n"
            "---------------------------------------------\n");

    printf("\t\tMean\tStd\tMax\n");
    printf("Before\t\t%0.2f\t%0.2f\t%0.2f\n", pMagCalibResults->beforeMeanError, pMagCalibResults->beforeStdError, pMagCalibResults->beforeMaxError);
    printf("After\t\t%0.2f\t%0.2f\t%0.2f\n", pMagCalibResults->afterMeanError, pMagCalibResults->afterStdError, pMagCalibResults->afterMaxError);
    printf("Accuracy (deg)\t%0.2f\t%0.2f\t%0.2f\n", sbgRadToDegf(pMagCalibResults->meanAccuracy), sbgRadToDegf(pMagCalibResults->stdAccuracy), sbgRadToDegf(pMagCalibResults->maxAccuracy));
    printf("\n================ END  REPORT ================\n");
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
 * Compute the magnetic calibration and display the results to the user.
 *
 * The new magnetic calibration can be applied on the device if apply is set to true.
 * Computing a magnetic calibration don't reset the list of acquired points.
 * 
 * \param[in]   pHandle                 sbgEComHandle instance
 * \param[in]   mode                    2D or 3D magnetic calibration mode.
 * \param[in]   apply                   Set to true to compute and then apply the magnetic calibration
 *                                      Set to false to just compute and display the magnetic calibration results.
 * \return                              SBG_NO_ERROR if a valid magnetic calibration has been computed (and optionally applied).
 */
static SbgErrorCode computeMagneticCalibration(SbgEComHandle *pHandle, SbgEComMagCalibMode mode, bool apply)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComMagCalibResults  magCalibResults;

    assert(pHandle);

    //
    // Try to compute a magnetic calibration and get the results
    //
    errorCode = sbgEComCmdMagComputeCalib(pHandle, &magCalibResults);

    if (errorCode == SBG_NO_ERROR)
    {
        //
        // Test if the device has computed a valid magnetic calibration
        //
        if (magCalibResults.quality != SBG_ECOM_MAG_CALIB_QUAL_INVALID)
        {
            if (apply)
            {
                //
                // Send the new magnetic calibration data (for ELLIPSE firmware v3 and above)
                //
                errorCode = sbgEComCmdMagSetCalibData2(pHandle, magCalibResults.offset, magCalibResults.matrix, mode);

                if (errorCode == SBG_NO_ERROR)
                {
                    //
                    // Magnetic calibration applied, save and reboot device to use it
                    //
                    errorCode = sbgEComCmdSettingsAction(pHandle, SBG_ECOM_SAVE_SETTINGS);

                    if (errorCode == SBG_NO_ERROR)
                    {
                        //
                        // Successfully saved & applied magnetic calibration, display results
                        //
                        printf("The magnetic calibration has been saved and applied.\n");
                        displayMagCalibResults(mode, &magCalibResults);
                    }
                    else
                    {
                        SBG_LOG_ERROR(errorCode, "Unable to save new magnetic calibration");
                    }
                }
                else
                {
                    SBG_LOG_ERROR(errorCode, "Unable to apply new magnetic calibration.");
                }
            }
            else
            {
                //
                // Display the magnetic calibration status
                //
                printf("A new magnetic calibration solution has been computed but not applied.\n");
                displayMagCalibResults(mode, &magCalibResults);
            }
        }
        else
        {
            SBG_LOG_ERROR(errorCode, "Computed magnetic calibration is invalid. Please retry.");
        }
    }
    else
    {
        SBG_LOG_ERROR(errorCode, "Unable to compute magnetic calibration.");
    }

    return errorCode;
}

/*!
 * Process.
 *
 * \param[in]   pInterface              Interface.
 * \return                              SBG_NO_ERROR if successful.
 */
static SbgErrorCode ellipseOnBoardMagCalibProcess(SbgInterface *pInterface)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComHandle           comHandle;
    SbgEComMagCalibMode     mode;
    int                     keyPressed;

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
        printf("Welcome to the ELLIPSE on-board magnetic calibration example.\n");
        printf("sbgECom version %s\n\n", SBG_E_COM_VERSION_STR);

        //
        // Get device information and print them
        //
        getAndPrintProductInfo(&comHandle);

        //
        // Ask the user if he wants a 2D or 3D calibration
        //
        mode = askCalibrationMode();

        //
        // Flush the serial interface and sbgECom work buffer as the program flow has been interrupted
        //
        sbgEComPurgeIncoming(&comHandle);
            
        //
        // Start / reset the acquisition of magnetic field data
        // Each time this command is called, the device is prepared to acquire a new set of magnetic field data
        // You have to specify here if the magnetic field data acquisition will be used to compute a 2D or 3D calibration
        //
        errorCode = sbgEComCmdMagStartCalib(&comHandle, mode, SBG_ECOM_MAG_CALIB_HIGH_BW);

        //
        // Make sure that the magnetic calibration has started
        //
        if (errorCode == SBG_NO_ERROR)
        {
            for (;;)
            {
                //
                // The device is now acquiring some magnetic field data.
                // Wait for a user input before computing the magnetic calibration
                //
                if (mode == SBG_ECOM_MAG_CALIB_MODE_3D)
                {
                    printf("\n\nThe device is acquiring magnetic field data for a 3D calibration........\n");
                }
                else
                {
                    printf("\n\nThe device is acquiring magnetic field data for a 2D calibration.........\n");
                }

                printf( "Please rotate the device slowly...\n"
                        "\n"
                        "You can compute as many magnetic calibration as you want without loosing already acquired points:\n"
                        "  1) Compute a magnetic calibration but don't apply it\n"
                        "  2) Compute and apply a magnetic calibration then save and reboot the device\n"
                        "  3) Stop the current acquisition and quit\n"
                        "\n"
                        "Please enter your choice 1, 2 or 3 and press enter : ");

                //
                // Wait for user choice
                //
                keyPressed = getUserChoice();

                //
                // Flush the serial interface and sbgECom work buffer as the program flow has been interrupted
                //
                sbgEComPurgeIncoming(&comHandle);

                if (keyPressed == '1')
                {
                    computeMagneticCalibration(&comHandle, mode, false);
                }
                else if (keyPressed == '2')
                {
                    computeMagneticCalibration(&comHandle, mode, true);
                    break;
                }
                else if (keyPressed == '3')
                {
                    break;
                }
            }
        }
        else
        {
            SBG_LOG_ERROR(errorCode, "Unable to start the on-board magnetic calibration");
        }

        //
        // Close the sbgECom library
        //
        sbgEComClose(&comHandle);
    }
    else
    {
        SBG_LOG_ERROR(errorCode, "Unable to initialize the sbgECom library.");
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//  Main program                                                        //
//----------------------------------------------------------------------//

/*!
 * Program entry point usage: ellipseOnboardMagCalib COM1 921600
 * 
 * \param[in]   argc                    Number of input arguments.
 * \param[in]   argv                    Input arguments as an array of strings.
 * \return                              EXIT_SUCCESS if successful.
 */
int main(int argc, char **argv)
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
            errorCode = ellipseOnBoardMagCalibProcess(&sbgInterface);

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
            SBG_LOG_ERROR(errorCode, "Unable to open serial interface");
            exitCode = EXIT_FAILURE;
        }
    }
    else
    {
        SBG_LOG_ERROR(SBG_ERROR, "Invalid input arguments, usage: ellipseOnboardMagCalib SERIAL_DEVICE SERIAL_BAUDRATE");
        exitCode = EXIT_FAILURE;
    }
    
    return exitCode;
}
