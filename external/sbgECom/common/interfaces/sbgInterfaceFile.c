// sbgCommnonLib headers
#include <sbgCommon.h>

// Local headers
#include "sbgInterface.h"
#include "sbgInterfaceFile.h"

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

/*!
 * Returns the interface FILE descriptor.
 * 
 * \param[in]   pInterface                              Interface instance.
 * \return                                              The associated FILE descriptor.
 */
static FILE *sbgInterfaceFileGetDesc(SbgInterface *pInterface)
{
    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_FILE);
    assert(pInterface->handle);

    return (FILE*)pInterface->handle;
}

/*!
 * Destroy the interface by closing the file descriptor.
 *
 * \param[in]   pInterface                              Interface instance.
 * \return                                              SBG_NO_ERROR if the interface has been closed successfully.
 */
static SbgErrorCode sbgInterfaceFileDestroy(SbgInterface *pInterface)
{
    FILE    *pInputFile;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_FILE);

    pInputFile = sbgInterfaceFileGetDesc(pInterface);

    fclose(pInputFile);
    sbgInterfaceZeroInit(pInterface);   

    return SBG_NO_ERROR;
}

/*!
 * Write some data the the file
 *
 * \param[in]   pInterface                              Valid handle on an initialized interface.
 * \param[in]   pBuffer                                 Pointer on an allocated buffer that contains the data to write
 * \param[in]   bytesToWrite                            Number of bytes we would like to write.
 * \return                                              SBG_NO_ERROR if all bytes have been written successfully.
 */
static SbgErrorCode sbgInterfaceFileWrite(SbgInterface *pInterface, const void *pBuffer, size_t bytesToWrite)
{
    FILE    *pOutputFile;
    size_t   bytesWritten;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_FILE);
    assert(pBuffer);

    pOutputFile = sbgInterfaceFileGetDesc(pInterface);

    //
    // Write the data and check if all bytes have been written
    //
    bytesWritten = fwrite(pBuffer, sizeof(uint8_t), bytesToWrite, pOutputFile);

    if (bytesWritten == bytesToWrite)
    {

        return SBG_NO_ERROR;
    }
    else
    {
        return SBG_WRITE_ERROR;
    }
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
static SbgErrorCode sbgInterfaceFileRead(SbgInterface *pInterface, void *pBuffer, size_t *pReadBytes, size_t bytesToRead)
{
    SbgErrorCode     errorCode = SBG_NO_ERROR;
    FILE            *pInputFile;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_FILE);
    assert(pBuffer);
    assert(pReadBytes);

    pInputFile = sbgInterfaceFileGetDesc(pInterface);

    //
    // Read some bytes from the file and check if an error has occurred
    //
    *pReadBytes = fread(pBuffer, sizeof(uint8_t), bytesToRead, pInputFile);

    if (*pReadBytes < bytesToRead)
    {
        //
        // Don't report an error if we have reached the end of the file to comply with sbgInterface specification
        //
        if (ferror(pInputFile) != 0)
        {
            errorCode = SBG_READ_ERROR;
            SBG_LOG_ERROR(errorCode, "File read error %u", ferror(pInputFile));
        }
    }

    return errorCode;
}

/*!
 * Make an interface flush all pending input or output data.
 *
 * If flags include SBG_IF_FLUSH_INPUT, all pending input data is discarded.
 * If flags include SBG_IF_FLUSH_OUTPUT, the function blocks until all output data has been written out.
 *
 * \param[in]   pInterface                              Valid handle on an initialized interface.
 * \param[in]   flags                                   Combination of the SBG_IF_FLUSH_INPUT and SBG_IF_FLUSH_OUTPUT flags.
 * \return                                              SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgInterfaceFileFlush(SbgInterface *pInterface, uint32_t flags)
{
    SbgErrorCode     errorCode;
    FILE            *pInputFile;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_FILE);

    pInputFile = sbgInterfaceFileGetDesc(pInterface);

    if ((pInterface->pReadFunc  && (flags & SBG_IF_FLUSH_INPUT)) ||
        (pInterface->pWriteFunc && (flags & SBG_IF_FLUSH_OUTPUT)))
    {
        int          ret;

        ret = fflush(pInputFile);

        if (ret == 0)
        {
            errorCode = SBG_NO_ERROR;
        }
        else
        {
            errorCode = SBG_ERROR;
        }
    }
    else
    {
        errorCode = SBG_NO_ERROR;
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API SbgErrorCode sbgInterfaceFileOpen(SbgInterface *pInterface, const char *filePath)
{
    SbgErrorCode     errorCode = SBG_NO_ERROR;
    FILE            *pInputFile;
    
    assert(pInterface);
    assert(filePath);

    //
    // Always call the underlying zero init method to make sure we can correctly handle SbgInterface evolutions
    //
    sbgInterfaceZeroInit(pInterface);

    //
    // Try to open the file
    //
    pInputFile = fopen(filePath, "rb");

    //
    // Test if the input file has been opened
    //
    if (pInputFile)
    {
        //
        // Define base interface members
        //
        pInterface->handle      = pInputFile;
        pInterface->type        = SBG_IF_TYPE_FILE;

        //
        // Define the interface name
        //
        sbgInterfaceNameSet(pInterface, filePath);
                
        //
        // Define all specialized members
        //
        pInterface->pDestroyFunc    = sbgInterfaceFileDestroy;
        pInterface->pReadFunc       = sbgInterfaceFileRead;
        pInterface->pWriteFunc      = NULL;
        pInterface->pFlushFunc      = sbgInterfaceFileFlush;
    }
    else
    {
        //
        // Unable to open the input file
        //
        errorCode = SBG_INVALID_PARAMETER;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgInterfaceFileWriteOpen(SbgInterface *pInterface, const char *filePath)
{
    SbgErrorCode     errorCode = SBG_NO_ERROR;
    FILE            *pInputFile;

    assert(pInterface);
    assert(filePath);

    //
    // Try to open the file
    //
    pInputFile = fopen(filePath, "wb");

    //
    // Test if the input file has been opened
    //
    if (pInputFile)
    {
        //
        // Define base interface members
        //
        pInterface->handle = pInputFile;
        pInterface->type = SBG_IF_TYPE_FILE;

        //
        // Define the interface name
        //
        sbgInterfaceNameSet(pInterface, filePath);

        //
        // Define all specialized members
        //
        pInterface->pDestroyFunc    = sbgInterfaceFileDestroy;
        pInterface->pReadFunc       = NULL;
        pInterface->pWriteFunc      = sbgInterfaceFileWrite;
        pInterface->pFlushFunc      = sbgInterfaceFileFlush;
    }
    else
    {
        //
        // Unable to open the input file
        //
        errorCode = SBG_INVALID_PARAMETER;
    }

    return errorCode;
}

SBG_COMMON_LIB_API size_t sbgInterfaceFileGetSize(SbgInterface *pInterface)
{
    FILE    *pInputFile;
    long     cursorPos;
    long     fileSize;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_FILE);

    pInputFile = sbgInterfaceFileGetDesc(pInterface);

    //
    // Compute the file size
    //
    cursorPos = ftell(pInputFile);
    fseek(pInputFile, 0, SEEK_END);
    fileSize = ftell(pInputFile);
    fseek(pInputFile, cursorPos, SEEK_SET);
    
    return (size_t)fileSize;
}

SBG_COMMON_LIB_API size_t sbgInterfaceFileGetCursor(const SbgInterface *pInterface)
{
    FILE    *pInputFile;

    assert(pInterface);
    assert(pInterface->type == SBG_IF_TYPE_FILE);

    pInputFile = sbgInterfaceFileGetDesc((SbgInterface*)pInterface);

    return (size_t)ftell(pInputFile);
}
