// Windows headers
#include <windows.h>

// sbgCommonLib headers
#include <sbgCommon.h>
#include <interfaces/sbgInterfaceSerial.h>

//----------------------------------------------------------------------//
//- Definitions                                                        -//
//----------------------------------------------------------------------//
#define SBG_IF_SERIAL_TX_BUFFER_SIZE            (4096u)                 /*!< Define the transmission buffer size for the serial port. */
#define SBG_IF_SERIAL_RX_BUFFER_SIZE            (4096u)                 /*!< Define the reception buffer size for the serial port. */

//----------------------------------------------------------------------//
//- Internal methods declarations                                      -//
//----------------------------------------------------------------------//

/*!
 * Returns the last error message for windows api calls.
 *
 * \param[out]  pErrorMsg                   Pointer on an allocated string that can stores the windows error message.
 * \param[in]   errorMsgSize                Error message buffer size in bytes.
 * \return                                  The last error number.
 */
static uint32_t sbgGetWindowsErrorMsg(char *pErrorMsg, size_t errorMsgSize)
{
    DWORD   dw = GetLastError(); 
    DWORD   numCharWritten;
    LPVOID  lpMsgBuf = NULL;

    assert(pErrorMsg);

    //
    // Get the error message
    //
    numCharWritten = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMsgBuf, 0, NULL);

    //
    // Test if a message has been correctly written
    //
    if (numCharWritten > 0)
    {
        //
        // Copy the error message
        //
        strcpy_s(pErrorMsg, errorMsgSize, lpMsgBuf);
    }
    else
    {
        pErrorMsg[0] = '\0';
    }

    //
    // Release the buffer
    //
    LocalFree(lpMsgBuf);

    return dw;
}

/*!
 * Returns the serial interface descriptor.
 *
 * \param[in]   pInterface                              Interface instance.
 * \return                                              The associated serial descriptor.
 */
static HANDLE *sbgInterfaceFileGetDesc(SbgInterface *pInterface)
{
    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);
    assert(pInterface->handle);

    return (HANDLE*)pInterface->handle;
}

/*!
 * Destroy and close the serial interface.
 *
 * \param[in]   pInterface                      Valid handle on an initialized interface.
 * \return                                      SBG_NO_ERROR if the interface has been closed and released.
 */
static SbgErrorCode sbgInterfaceSerialDestroy(SbgInterface *pInterface)
{
    HANDLE  pSerialDevice;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);
    
    pSerialDevice = sbgInterfaceFileGetDesc(pInterface);
    
    //
    // Close the port com and clear the interface
    //
    CloseHandle(pSerialDevice);
    sbgInterfaceZeroInit(pInterface);

    return SBG_NO_ERROR;
}


/*!
 * Try to write some data to an interface.
 *
 * \param[in]   pInterface                              Valid handle on an initialized interface.
 * \param[in]   pBuffer                                 Pointer on an allocated buffer that contains the data to write
 * \param[in]   bytesToWrite                            Number of bytes we would like to write.
 * \return                                              SBG_NO_ERROR if all bytes have been written successfully.
 */
static SbgErrorCode sbgInterfaceSerialWrite(SbgInterface *pInterface, const void *pBuffer, size_t bytesToWrite)
{
    DWORD    numBytesLeftToWrite;
    uint8_t *pCurrentBuffer;
    DWORD    numBytesWritten;
    HANDLE   pSerialDevice;
    char     errorMsg[256];

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);
    assert(pBuffer);
    
    pSerialDevice = sbgInterfaceFileGetDesc(pInterface);
        
    //
    // Write the whole buffer chunk by chunk
    //
    numBytesLeftToWrite     = (DWORD)bytesToWrite;
    pCurrentBuffer          = (uint8_t*)pBuffer;

    while (numBytesLeftToWrite > 0)
    {
        //
        // Write these bytes to the serial interface
        //
        if (WriteFile(pSerialDevice, pCurrentBuffer, numBytesLeftToWrite, (LPDWORD)&numBytesWritten, NULL) == false)
        {
            //
            // An error has occurred during the write
            //
            sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
            SBG_LOG_ERROR(SBG_WRITE_ERROR, "Write failed error: %s", errorMsg);
            return SBG_WRITE_ERROR;
        }

        //
        // Update the buffer pointer and the number of bytes to write
        //
        numBytesLeftToWrite -= (size_t)numBytesWritten;
        pCurrentBuffer      += numBytesWritten;
    }

    return SBG_NO_ERROR;
}

/*!
 * Try to read some data from an interface.
 *
 * \param[in]   pInterface                              Valid handle on an initialized interface.
 * \param[in]   pBuffer                                 Pointer on an allocated buffer that can hold at least bytesToRead bytes of data.
 * \param[out]  pReadBytes                              Pointer on an uint32_t used to return the number of read bytes.
 * \param[in]   bytesToRead                             Number of bytes we would like to read.
 * \return                                              SBG_NO_ERROR if no error occurs, please check the number of received bytes.
 */
static SbgErrorCode sbgInterfaceSerialRead(SbgInterface *pInterface, void *pBuffer, size_t *pReadBytes, size_t bytesToRead)
{
    HANDLE  pSerialDevice;
    char    errorMsg[256];
    DWORD   bytesRead;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);
    assert(pBuffer);
    assert(pReadBytes);

    pSerialDevice = sbgInterfaceFileGetDesc(pInterface);
    
    //
    // Read some bytes on the serial buffer
    //
    if (ReadFile(pSerialDevice, pBuffer, (DWORD)bytesToRead, (LPDWORD)&bytesRead, NULL) == true)
    {
        //
        //  Update the number of bytes read
        //
        (*pReadBytes) = (size_t)bytesRead;

        return SBG_NO_ERROR;
    }
    else
    {
        *pReadBytes = (size_t)bytesRead;

        //
        // Unable to read some bytes
        //
        sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
        SBG_LOG_ERROR(SBG_READ_ERROR, "Read failed: %s", errorMsg);
        return SBG_READ_ERROR;
    }
}

/*!
 * Make an interface flush pending input and/or output data.
 *
 * If flags include SBG_IF_FLUSH_INPUT, all pending input data is discarded.
 * If flags include SBG_IF_FLUSH_OUTPUT, the function blocks until all output data has been written out.
 *
 * \param[in]   pInterface                              Interface instance.
 * \param[in]   flags                                   Combination of the SBG_IF_FLUSH_INPUT and SBG_IF_FLUSH_OUTPUT flags.
 * \return                                              SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgInterfaceSerialFlush(SbgInterface *pInterface, uint32_t flags)
{
    SbgErrorCode                         errorCode;
    HANDLE                               pSerialDevice;
    BOOL                                 result = TRUE;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);

    pSerialDevice = sbgInterfaceFileGetDesc(pInterface);

    if (result && (flags & SBG_IF_FLUSH_INPUT))
    {
        uint8_t                          buffer[256];
        size_t                           nrBytes;

        //
        // XXX The PurgeComm function is too unreliable. Instead, perform the flush by manually
        // draining all input data.
        //
        do
        {
            errorCode = sbgInterfaceSerialRead(pInterface, buffer, &nrBytes, sizeof(buffer));
        }
        while ((errorCode == SBG_NO_ERROR) && (nrBytes != 0));

        if (errorCode != SBG_NO_ERROR)
        {
            result = FALSE;
        }
    }

    if (result && (flags & SBG_IF_FLUSH_OUTPUT))
    {
        result = FlushFileBuffers(pSerialDevice);

        if (!result)
        {
            char                         errorMsg[256];

            sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
            SBG_LOG_ERROR(SBG_WRITE_ERROR, "unable to flush output, error:%s", errorMsg);
        }
    }

    if (result == TRUE)
    {
        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

/*!
 * Change the serial interface baudrate and flush the serial port to avoid having trash.
 *
 * \param[in]   pInterface          Valid handle on an initialized interface.
 * \param[in]   baudRate            The new baudrate to apply in bps.
 * \return                          SBG_NO_ERROR if everything is OK
 */
static SbgErrorCode sbgInterfaceSerialSetSpeed(SbgInterface *pInterface, uint32_t baudRate)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;
    HANDLE          pSerialDevice;
    DCB             comState;
    char            errorMsg[256];

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);

    pSerialDevice = sbgInterfaceFileGetDesc(pInterface);
    
    //
    // Try to retreive current com state
    //
    if (GetCommState(pSerialDevice, &comState))
    {
        comState.BaudRate = baudRate;

        //
        // Send the new configuration
        if (SetCommState(pSerialDevice, &comState))
        {
            //
            // Flush the serial interface
            //
            errorCode = sbgInterfaceFlush(pInterface, SBG_IF_FLUSH_ALL);
        }
        else
        {
            errorCode = SBG_ERROR;
            sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
            SBG_LOG_ERROR(errorCode, "Unable to set com state: %s", errorMsg);

        }
    }
    else
    {
        errorCode = SBG_ERROR;
        sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
        SBG_LOG_ERROR(errorCode, "Unable to retrieve com state: %s", errorMsg);
    }

    return errorCode;
}

/*!
 * Returns the current interface baud rate in bps (bit per second)
 *
 * WARNING: The method will returns zero if not applicable for a type of interface
 *
 * \param[in]   pInterface                              Interface instance.
 * \return                                              The current interface baud rate in bps or zero if not applicable.
 */
static uint32_t sbgInterfaceSerialGetSpeed(const SbgInterface *pInterface)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;
    HANDLE          pSerialDevice;
    DCB             comState;
    char            errorMsg[256];

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);

    pSerialDevice = sbgInterfaceFileGetDesc((SbgInterface*)pInterface);
    
    if (GetCommState(pSerialDevice, &comState))
    {
        return comState.BaudRate;
    }
    else
    {
        errorCode = SBG_ERROR;
        sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
        SBG_LOG_ERROR(errorCode, "Unable to retrieve com state: %s", errorMsg);
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API SbgErrorCode sbgInterfaceSerialCreate(SbgInterface *pInterface, const char *deviceName, uint32_t baudRate)
{
    char            errorMsg[256];
    char            comPortPath[32];
    COMMTIMEOUTS    comTimeOut;
    DCB             comState;
    uint32_t        deviceNum;
    HANDLE          hSerialDevice;

    assert(pInterface);
    assert(deviceName);

    //
    // Always call the underlying zero init method to make sure we can correctly handle SbgInterface evolutions
    //
    sbgInterfaceZeroInit(pInterface);

    //
    // Extract device number
    //
    if (sscanf_s(deviceName, "COM%i", &deviceNum) == 1)
    {
        //
        // Build the com port path
        //
        sprintf_s(comPortPath, 32, "\\\\.\\COM%i", deviceNum);

        //
        // Init the com port
        //
        hSerialDevice = CreateFileA(comPortPath, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        //
        // Test that the port has been initialized
        //
        if (hSerialDevice != INVALID_HANDLE_VALUE)
        {
            //
            // Purge the com port
            //
            if (PurgeComm(hSerialDevice, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
            {
                //
                // Retrieve current com state and com timeout
                //
                if ( (GetCommState(hSerialDevice, &comState)) && (GetCommTimeouts(hSerialDevice, &comTimeOut)) )
                {
                    //
                    // Define common attributes
                    //
                    comState.BaudRate= baudRate;
                    comState.Parity= NOPARITY;
                    comState.ByteSize= 8;
                    comState.StopBits= ONESTOPBIT;

                    //
                    // Disable flow control
                    //
                    comState.fDsrSensitivity = false;
                    comState.fOutxCtsFlow = false;
                    comState.fOutxDsrFlow = false;
                    comState.fOutX = false;
                    comState.fInX = false;

                    //
                    // Define timeout attributes (0 ms read timeout)
                    //
                    comTimeOut.ReadIntervalTimeout = MAXDWORD;
                    comTimeOut.ReadTotalTimeoutMultiplier = 0;
                    comTimeOut.ReadTotalTimeoutConstant = 0;

                    comTimeOut.WriteTotalTimeoutConstant = 0;
                    comTimeOut.WriteTotalTimeoutMultiplier = 0;
                        
                    //
                    // Configure the com port
                    //
                    if ( (SetCommState(hSerialDevice, &comState)) && (SetCommTimeouts(hSerialDevice, &comTimeOut)) )
                    {
                        //
                        // Define the COM port buffer size
                        //
                        if (SetupComm(hSerialDevice, SBG_IF_SERIAL_RX_BUFFER_SIZE, SBG_IF_SERIAL_TX_BUFFER_SIZE))
                        {
                            //
                            // Define base interface members
                            //
                            pInterface->handle  = hSerialDevice;
                            pInterface->type    = SBG_IF_TYPE_SERIAL;

                            //
                            // Define the interface name
                            //
                            sbgInterfaceNameSet(pInterface, deviceName);

                            //
                            // Define all overloaded members
                            //
                            pInterface->pDestroyFunc    = sbgInterfaceSerialDestroy;
                            pInterface->pReadFunc       = sbgInterfaceSerialRead;
                            pInterface->pWriteFunc      = sbgInterfaceSerialWrite;
                            pInterface->pFlushFunc      = sbgInterfaceSerialFlush;
                            pInterface->pSetSpeedFunc   = sbgInterfaceSerialSetSpeed;
                            pInterface->pGetSpeedFunc   = sbgInterfaceSerialGetSpeed;

                            //
                            // Purge the communication
                            //
                            return sbgInterfaceSerialFlush(pInterface, SBG_IF_FLUSH_ALL);
                        }
                        else
                        {
                            sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
                            SBG_LOG_ERROR(SBG_ERROR, "Unable to define buffer size: %s", errorMsg);
                        }
                    }
                    else
                    {
                        sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
                        SBG_LOG_ERROR(SBG_ERROR, "Unable to set com state and/or timeout: %s", errorMsg);
                    }
                }
                else
                {
                    sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
                    SBG_LOG_ERROR(SBG_ERROR, "Unable to retrieve com state and/or timeout: %s", errorMsg);
                }
            }
            else
            {
                sbgGetWindowsErrorMsg(errorMsg, sizeof(errorMsg));
                SBG_LOG_ERROR(SBG_ERROR, "Unable to purge com port %i: %s", deviceNum, errorMsg);
            }

            //
            // Close the port com
            // An error occurred while setting up the serial port, close it.
            //
            CloseHandle(hSerialDevice);
        }

        return SBG_ERROR;
    }
    else
    {
        //
        // Invalid device name
        //
        return SBG_INVALID_PARAMETER;
    }
}
