// Standard headers
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

// sbgCommonLib headers
#include <sbgCommon.h>
#include <interfaces/sbgInterfaceSerial.h>

//----------------------------------------------------------------------//
//- Definitions                                                        -//
//----------------------------------------------------------------------//
#define SBG_IF_SERIAL_TX_BUFFER_SIZE            (4096u)                 /*!< Define the transmission buffer size for the serial port. */
#define SBG_IF_SERIAL_RX_BUFFER_SIZE            (4096u)                 /*!< Define the reception buffer size for the serial port. */

//----------------------------------------------------------------------//
//- Private methods declarations                                       -//
//----------------------------------------------------------------------//

/*!
 * Returns the right Unix baud rate const according to a baud rate value.
 *
 * \param[in] baudRate      The baud rate value (ie 115200).
 * \return                  The Unix baud rate constant.
 */
static uint32_t sbgInterfaceSerialGetBaudRateConst(uint32_t baudRate)
{
    uint32_t baudRateConst;

    //
    // Create the right baud rate value for Unix platforms
    //
    switch (baudRate)
    {
        case 9600:
            baudRateConst = B9600;
            break;
        case 19200:
            baudRateConst = B19200;
            break;
#ifdef B38400
        case 38400:
            baudRateConst = B38400;
            break;
#endif
#ifdef B57600
        case 57600:
            baudRateConst = B57600;
            break;
#endif
#ifdef B115200
        case 115200:
            baudRateConst = B115200;
            break;
#endif
#ifdef B230400
        case 230400:
            baudRateConst = B230400;
            break;
#endif
#ifdef B460800
        case 460800:
            baudRateConst = B460800;
            break;
#endif
#ifdef B921600
        case 921600:
            baudRateConst = B921600;
            break;
#endif
#ifdef B1000000
        case 1000000:
            baudRateConst = B1000000;
            break;
#endif
#ifdef B1152000
        case 1152000:
            baudRateConst = B1152000;
            break;
#endif
#ifdef B1500000
        case 1500000:
            baudRateConst = B1500000;
            break;
#endif
#ifdef B2000000
        case 2000000:
            baudRateConst = B2000000;
            break;
#endif
#ifdef B2500000
        case 2500000:
            baudRateConst = B2500000;
            break;
#endif
#ifdef B3000000
        case 3000000:
            baudRateConst = B3000000;
            break;
#endif
#ifdef B3500000
        case 3500000:
            baudRateConst = B3500000;
            break;
#endif
#ifdef B4000000
        case 4000000:
            baudRateConst = B4000000;
            break;
#endif
        default:
            baudRateConst = baudRate;
    }

    return baudRateConst;
}

/*!
 * Destroy an interface initialized using sbgInterfaceSerialCreate.
 * 
 * \param[in]   pInterface                      Valid handle on an initialized interface.
 * \return                                      SBG_NO_ERROR if the interface has been closed and released.
 */
static SbgErrorCode sbgInterfaceSerialDestroy(SbgInterface *pInterface)
{
    int *pSerialHandle;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);
        
    //
    // Test that we have a valid interface
    //
    if (pInterface)
    {
        //
        // Get the internal serial handle
        //
        pSerialHandle = (int *)pInterface->handle;
        
        //
        // Close the port com
        //
        close((*pSerialHandle));
        SBG_FREE(pSerialHandle);
        sbgInterfaceZeroInit(pInterface);

        return SBG_NO_ERROR;
    }
    else
    {
        return SBG_NULL_POINTER;
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
    SbgErrorCode                     errorCode;
    int                              fd;
    int                              result = 0;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);

    fd = *((int *)pInterface->handle);

    if ((result == 0) && (flags & SBG_IF_FLUSH_INPUT))
    {
        result = tcflush(fd, TCIFLUSH);

        if (result != 0)
        {
            SBG_LOG_ERROR(SBG_READ_ERROR, "unable to flush input, error: %s", strerror(errno));
        }
    }

    if ((result == 0) && (flags & SBG_IF_FLUSH_OUTPUT))
    {
        result = tcdrain(fd);

        if (result != 0)
        {
            SBG_LOG_ERROR(SBG_WRITE_ERROR, "unable to flush output, error: %s", strerror(errno));
        }
    }

    if (result == 0)
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
 * Change the serial interface baud rate immediately.
 * 
 * \param[in]   handle              Valid handle on an initialized interface.
 * \param[in]   baudRate            The new baudrate to apply in bps.
 * \return                          SBG_NO_ERROR if everything is OK
 */
static SbgErrorCode sbgInterfaceSerialChangeBaudrate(SbgInterface *pInterface, uint32_t baudRate)
{
    int             hSerialHandle;
    struct termios  options;
    uint32_t        baudRateConst;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);

    //
    // Get the internal serial handle
    //
    hSerialHandle = *((int*)pInterface->handle);
        
    //
    // Get the baud rate const for our Unix platform
    //
    baudRateConst = sbgInterfaceSerialGetBaudRateConst(baudRate);
        
    //
    // Retrieve current options
    //
    if (tcgetattr(hSerialHandle, &options) != -1)
    {   
        //
        // Set both input and output baud
        //
        if ( (cfsetispeed(&options, baudRateConst) == -1)  || (cfsetospeed(&options, baudRateConst) == -1) )
        {
            SBG_LOG_ERROR(SBG_ERROR, "unable to change interface speed");
            return SBG_ERROR;
        }
            
        //
        // Define options
        //
        if (tcsetattr(hSerialHandle, TCSADRAIN, &options) != -1)
        {
            return SBG_NO_ERROR;
        }
        else
        {
            SBG_LOG_ERROR(SBG_ERROR, "tcsetattr has failed");
            return SBG_ERROR;
        }
    }
    else
    {
        SBG_LOG_ERROR(SBG_ERROR, "tcgetattr has failed");
        return SBG_ERROR;
    }
}

//----------------------------------------------------------------------//
//- Internal interfaces write/read implementations                     -//
//----------------------------------------------------------------------//

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
    size_t          numBytesLeftToWrite = bytesToWrite;
    uint8_t        *pCurrentBuffer = (uint8_t*)pBuffer;
    ssize_t         numBytesWritten;
    int             hSerialHandle;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);
    assert(pBuffer);
    
    //
    // Get the internal serial handle
    //
    hSerialHandle = *((int*)pInterface->handle);

    //
    // Write the whole buffer
    //
    while (numBytesLeftToWrite > 0)
    {
        //
        // Write these bytes to the serial interface
        //
        numBytesWritten = write(hSerialHandle, pCurrentBuffer, numBytesLeftToWrite);
            
        //
        // Test the there is no error
        //  
        if (numBytesWritten == -1)
        {
            if (errno == EAGAIN)
            {
                sbgSleep(1);
            }
            else
            {
                SBG_LOG_ERROR(SBG_WRITE_ERROR, "unable to write to the device: %s", strerror(errno));
                return SBG_WRITE_ERROR;
            }
        }
        else
        {
            //
            // Update the buffer pointer and the number of bytes to write
            //
            numBytesLeftToWrite -= (size_t)numBytesWritten;
            pCurrentBuffer += (size_t)numBytesWritten;
        }
    }

    return SBG_NO_ERROR;
}

/*!
 * Try to read some data from an interface.
 * 
 * \param[in]   pInterface                              Valid handle on an initialized interface.
 * \param[in]   pBuffer                                 Pointer on an allocated buffer that can hold at least bytesToRead bytes of data.
 * \param[out]  pReadBytes                              Pointer on an uint32 used to return the number of read bytes.
 * \param[in]   bytesToRead                             Number of bytes we would like to read.
 * \return                                              SBG_NO_ERROR if no error occurs, please check the number of received bytes.
 */
static SbgErrorCode sbgInterfaceSerialRead(SbgInterface *pInterface, void *pBuffer, size_t *pReadBytes, size_t bytesToRead)
{
    SbgErrorCode    errorCode;
    int             hSerialHandle;
    ssize_t         numBytesRead;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_SERIAL);
    assert(pBuffer);
    assert(pReadBytes);
    
    //
    // Get the internal serial handle
    //
    hSerialHandle = *((int*)pInterface->handle);
        
    //
    // Read our buffer
    //
    numBytesRead = read(hSerialHandle, pBuffer, bytesToRead);
        
    //
    // Check if the read operation was successful
    //
    if (numBytesRead >= 0)
    {
        errorCode = SBG_NO_ERROR;
    }
    else
    {
        if (errno == EAGAIN)
        {
            errorCode = SBG_NO_ERROR;
        }
        else
        {
            errorCode = SBG_READ_ERROR;
        }

        numBytesRead = 0;
    }
        
    //
    // If we can, returns the number of read bytes
    //
    *pReadBytes = (size_t)numBytesRead;
        
    return errorCode;
}

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgInterfaceSerialCreate(SbgInterface *pInterface, const char *deviceName, uint32_t baudRate)
{
    int             *pSerialHandle;
    struct termios   options;
    uint32_t         baudRateConst;

    assert(pInterface);
    assert(deviceName);

    //
    // Always call the underlying zero init method to make sure we can correctly handle SbgInterface evolutions
    //
    sbgInterfaceZeroInit(pInterface);

    //
    // Get our baud rate const for our Unix platform
    //
    baudRateConst = sbgInterfaceSerialGetBaudRateConst(baudRate);
            
    //
    //  Allocate the serial handle
    //
    pSerialHandle = (int*)malloc(sizeof(int));

    //
    // Init the com port
    //
    (*pSerialHandle) = open(deviceName, O_RDWR | O_NOCTTY | O_NDELAY);
            
    //
    // Test that the port has been initialized
    //
    if ((*pSerialHandle) != -1)
    {
        //
        // Don't block on read call if no data are available
        //
        if (fcntl((*pSerialHandle), F_SETFL, O_NONBLOCK) != -1)
        {
            //
            // Retrieve current options
            //
            if (tcgetattr((*pSerialHandle), &options) != -1)
            {
                //
                // Define com port options
                //
                options.c_cflag |=  (CLOCAL | CREAD);       // Enable the receiver and set local mode...
                options.c_cflag &= ~(PARENB|CSTOPB|CSIZE);  // No parity, 1 stop bit, mask character size bits
                options.c_cflag |= CS8;                     // Select 8 data bits
                options.c_cflag &= ~CRTSCTS;                // Disable Hardware flow control

                //
                // Disable software flow control
                //
                options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);

                //
                // We would like raw input
                //
                options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG /*| IEXTEN | ECHONL*/);
                options.c_oflag &= ~OPOST;

                //
                // Set our timeout to 0
                //
                options.c_cc[VMIN]     = 0;
                options.c_cc[VTIME]    = 1;

                //
                // Set both input and output baud
                //
                if ( (cfsetispeed(&options, baudRateConst) != -1)  && (cfsetospeed(&options, baudRateConst) != -1) )
                {
                    //
                    // Define options
                    //
                    if (tcsetattr((*pSerialHandle), TCSANOW, &options) != -1)
                    {                               
                        //
                        // The serial port is ready so create a new serial interface
                        //
                        pInterface->handle = (void*)pSerialHandle;
                        pInterface->type = SBG_IF_TYPE_SERIAL;

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
                        pInterface->pSetSpeedFunc   = sbgInterfaceSerialChangeBaudrate;

                        //
                        // Purge the communication
                        //
                        return sbgInterfaceSerialFlush(pInterface, SBG_IF_FLUSH_ALL);
                    }
                    else
                    {
                        SBG_LOG_ERROR(SBG_ERROR, "tcsetattr has failed");
                    }
                }
                else
                {
                    SBG_LOG_ERROR(SBG_ERROR, "unable to change interface speed");
                }
            }
            else
            {
                SBG_LOG_ERROR(SBG_ERROR, "tcgetattr has failed");
            }
        }
        else
        {
            SBG_LOG_ERROR(SBG_ERROR, "fcntl has failed");
        }
    }
    else
    {
        SBG_LOG_ERROR(SBG_ERROR, "unable to open the interface: %s", deviceName);
    }
            
    //
    //  Release the allocated serial handle
    //
    SBG_FREE(pSerialHandle);

    return SBG_ERROR;
}
