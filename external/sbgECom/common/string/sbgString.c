// Standard headers
#include <ctype.h>
#include <inttypes.h>

// sbgCommonLib headers
#include <sbgCommon.h>

// Local headers
#include "sbgString.h"

//----------------------------------------------------------------------//
// Private functions                                                    //
//----------------------------------------------------------------------//

/*!
 * Reset the state of a string.
 *
 * The content of the string is reset to the empty string, using its internal
 * buffer for storage. This function doesn't check if the string buffer refers
 * to dynamically allocated memory, and it is therefore the responsibility of
 * the caller to handle that memory before calling this function.
 *
 * \param[in]   pString                     String.
 */
static void sbgStringReset(SbgString *pString)
{
    assert(pString);

    pString->internalBuffer[0] = '\0';

    pString->pBuffer    = pString->internalBuffer;
    pString->capacity   = sizeof(pString->internalBuffer);
    pString->length     = 0;
}

/*!
 * Count the number of leading zeros in an unsigned integer.
 *
 * \param[in]   value                       Integer value, must not be zero.
 * \return                                  Number of leading zeros.
 */
static size_t sbgStringCountLeadingZeros(size_t value)
{
    size_t                               result;

    assert(value != 0);

#if defined(__GNUC__) || defined(__clang__)
    result = __builtin_clzl((unsigned long)value);
#else
    size_t                               count;
    size_t                               tmp;

    tmp = value;
    count = 0;

    while (tmp != 0)
    {
        tmp >>= 1;
        count++;
    }

    result = (sizeof(size_t) * 8) - count;
#endif // defined(__GNUC__) || defined(__clang__)

    return result;
}

/*!
 * Compute the capacity required to store a string of the given size, in bytes.
 *
 * The size includes the terminating null character.
 *
 * The size must not be zero.
 *
 * \param[in]   pString                     String.
 * \param[in]   size                        Size, in bytes.
 * \param[out]  pCapacity                   Capacity, in bytes.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgStringComputeCapacity(const SbgString *pString, size_t size, size_t *pCapacity)
{
    SbgErrorCode                         errorCode;
    size_t                               nrLeadingZeros;

    assert(pString);

    SBG_UNUSED_PARAMETER(pString);

    //
    // The computed capacity is the power-of-two immediately strictly greater than the size.
    // In other words, it's the value with a single bit set where the index of that bit is one
    // more than the MSB bit of the size.
    //
    // If the size already requires all bits of the size_t type to be encoded, then there's no
    // extra bit available to encode the capacity.
    //
    // Values that would require more than half of the address space are very likely a bug,
    // so just refuse them.
    //

    nrLeadingZeros = sbgStringCountLeadingZeros(size);

    if (nrLeadingZeros != 0)
    {
        *pCapacity = (size_t)1 << ((sizeof(size_t) * 8) - sbgStringCountLeadingZeros(size));

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
        SBG_LOG_ERROR(errorCode, "unable to compute capacity");
    }

    return errorCode;
}

/*!
 * Resize the buffer of a string.
 *
 * This function only resizes the buffer, it doesn't guarantee the content is null-terminated.
 *
 * If an allocation error occurs, the string is left unchanged.
 *
 * If an allocation error occurs, and the new size is smaller than the current size, the
 * operation is considered successful.
 *
 * \param[in]   pString                     String.
 * \param[in]   size                        Required size, in bytes.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgStringResizeBuffer(SbgString *pString, size_t size)
{
    SbgErrorCode                         errorCode;

    assert(pString);
    assert(!pString->readOnly);

    if (pString->isStatic)
    {
        if (size <= pString->capacity)
        {
            errorCode = SBG_NO_ERROR;
        }
        else
        {
            errorCode = SBG_BUFFER_OVERFLOW;
            SBG_LOG_ERROR(errorCode, "insufficient static space for string: requested %zu, capacity %zu", size, pString->capacity);
        }
    }
    else
    {
        size_t                           capacity;

        //
        // XXX If a user performs frequent operations on a string that change its size
        // below and above the size of the internal buffer, the performance impact of
        // copying the content to and from the internal buffer would cause hysteresis.
        //
        // There is no special handling of this situation as it is assumed that the
        // internal buffer is small enough that the cost of the copy operation is
        // negligible.
        //

        errorCode = sbgStringComputeCapacity(pString, size, &capacity);

        if (errorCode == SBG_NO_ERROR)
        {
            if (capacity != pString->capacity)
            {
                if (capacity <= sizeof(pString->internalBuffer))
                {
                    if (pString->pBuffer != pString->internalBuffer)
                    {
                        memcpy(pString->internalBuffer, pString->pBuffer, capacity);
                        free(pString->pBuffer);

                        pString->pBuffer = pString->internalBuffer;
                        pString->capacity = sizeof(pString->internalBuffer);
                    }
                }
                else
                {
                    if (pString->pBuffer == pString->internalBuffer)
                    {
                        char            *pBuffer;

                        pBuffer = malloc(capacity);

                        if (pBuffer)
                        {
                            memcpy(pBuffer, pString->internalBuffer, pString->capacity);
                            pString->pBuffer = pBuffer;
                            pString->capacity = capacity;
                        }
                        else
                        {
                            errorCode = SBG_MALLOC_FAILED;
                            SBG_LOG_ERROR(errorCode, "unable to allocate buffer");
                        }
                    }
                    else
                    {
                        char            *pBuffer;

                        pBuffer = realloc(pString->pBuffer, capacity);

                        if (pBuffer)
                        {
                            pString->pBuffer = pBuffer;
                            pString->capacity = capacity;
                        }
                        else
                        {
                            if (capacity < pString->capacity)
                            {
                                errorCode = SBG_NO_ERROR;
                                SBG_LOG_WARNING(SBG_MALLOC_FAILED, "unable to reallocate buffer");
                            }
                            else
                            {
                                errorCode = SBG_MALLOC_FAILED;
                                SBG_LOG_ERROR(errorCode, "unable to reallocate buffer");
                            }
                        }
                    }
                }
            }
        }
    }

    return errorCode;
}

/*!
 * Get a pointer to a character in a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   index                       Index.
 * \return                                  Pointer to the character denoted by the index.
 */
static char * sbgStringGetCharPtr(const SbgString *pString, size_t index)
{
    assert(pString);
    assert(index < pString->length);

    return &pString->pBuffer[index];
}

/*!
 * Check if the start and end indexes of a substring are valid.
 *
 * \param[in]   pString                     String.
 * \param[in]   startIndex                  Start index of the substring.
 * \param[in]   endIndex                    End index of the substring.
 * \return                                  True if these indexes are valid.
 */
static bool sbgStringIndexesAreValid(const SbgString *pString, size_t startIndex, size_t endIndex)
{
    assert(pString);

    return ((startIndex <= endIndex) && (endIndex <= (pString->length)));
}

/*!
 * Assign a buffer of characters to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pBuffer                     Buffer.
 * \param[in]   length                      Length of the buffer, in bytes.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgStringAssignBuffer(SbgString *pString, const char *pBuffer, size_t length)
{
    assert(pString);
    assert(pBuffer);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            memcpy(pString->pBuffer, pBuffer, length) ;
            pString->pBuffer[length] = '\0';
            pString->length = length;
        }
    }

    return pString->errorCode;
}

/*!
 * Move a buffer of characters to a string.
 *
 * If moving is impossible, this function performs a copy.
 *
 * The buffer must have been allocated using a standard C allocation function,
 * such as malloc or strdup.
 *
 * On return, if successful, the buffer is owned by the string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pBuffer                     Buffer.
 * \param[in]   length                      Length of the buffer, in bytes.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgStringMoveBuffer(SbgString *pString, char *pBuffer, size_t length)
{
    assert(pString);
    assert(pBuffer);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        if (pString->isStatic)
        {
            SbgErrorCode                         errorCode;

            errorCode = sbgStringAssignBuffer(pString, pBuffer, length);

            if (errorCode == SBG_NO_ERROR)
            {
                free(pBuffer);
            }
        }
        else
        {
            if (pString->pBuffer != pString->internalBuffer)
            {
                free(pString->pBuffer);
            }

            pString->pBuffer    = pBuffer;
            pString->capacity   = length + 1;
            pString->length     = length;
        }
    }

    return pString->errorCode;
}

/*!
 * Remove all spacing characters at the beginning of a string.
 *
 * This function doesn't resize the string buffer.
 *
 * \param[in]   pString                     String.
 */
static bool sbgStringTrimLeftCommon(SbgString *pString)
{
    size_t                               index;
    bool                                 trimmed;

    assert(pString);
    assert(!pString->readOnly);

    index = pString->length;

    for (size_t i = 0; i < pString->length; i++)
    {
        char                            *pCharPtr;

        pCharPtr = sbgStringGetCharPtr(pString, i);

        if (!isspace((unsigned char)*pCharPtr))
        {
            index = i;
            break;
        }
    }

    if (index != 0)
    {
        size_t                           length;

        length = pString->length - index;

        memmove(pString->pBuffer, &pString->pBuffer[index], length + 1);
        pString->length = length;

        trimmed = true;
    }
    else
    {
        trimmed = false;
    }

    return trimmed;
}

/*!
 * Remove all spacing characters at the end of a string.
 *
 * This function doesn't resize the string buffer.
 *
 * \param[in]   pString                     String.
 */
static bool sbgStringTrimRightCommon(SbgString *pString)
{
    size_t                               index;
    bool                                 trimmed;

    assert(pString);
    assert(!pString->readOnly);

    index = pString->length;

    for (size_t i = 0; i < pString->length; i++)
    {
        char                            *pCharPtr;

        pCharPtr = sbgStringGetCharPtr(pString, pString->length - i - 1);

        if (!isspace((unsigned char)*pCharPtr))
        {
            index = i;
            break;
        }
    }

    if (index != 0)
    {
        size_t                           length;

        length = pString->length - index;

        pString->pBuffer[length] = '\0';
        pString->length = length;

        trimmed = true;
    }
    else
    {
        trimmed = false;
    }

    return trimmed;
}

/*!
 * Compare a string to a buffer of characters, ignoring case differences.
 *
 * \param[in]   pString                     String.
 * \param[in]   pBuffer                     Buffer.
 * \param[in]   length                      Length of the buffer, in bytes.
 * \return                                  The return value is 0 if the string value is equal to the buffer value,
 *                                          less than 0 if the string value is less than the buffer value,
 *                                          greater than 0 if the string value is greater than the buffer value.
 */
static int32_t sbgStringCompareIgnoreCaseCommon(const SbgString *pString, const char *pBuffer, size_t length)
{
    int32_t                              result;
    size_t                               minLength;

    assert(pString);
    assert(pBuffer);

    result = 0;

    if (length > pString->length)
    {
        minLength = pString->length;
    }
    else
    {
        minLength = length;
    }

    for (size_t i = 0; i <= minLength; i++)
    {
        unsigned char                    c1;
        unsigned char                    c2;

        c1 = (unsigned char)tolower((unsigned char)pString->pBuffer[i]);
        c2 = (unsigned char)tolower((unsigned char)pBuffer[i]);

        if (c1 != c2)
        {
            result = c1 - c2;
            break;
        }
    }

    return result;
}

//----------------------------------------------------------------------//
// Public functions                                                     //
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API void sbgStringConstructEmpty(SbgString *pString)
{
    sbgStringReset(pString);

    pString->readOnly = false;
    pString->isStatic = false;

    pString->errorCode = SBG_NO_ERROR;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringConstruct(SbgString *pString, const SbgString *pSourceString)
{
    sbgStringConstructEmpty(pString);

    if (pSourceString)
    {
        sbgStringAssign(pString, pSourceString);
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringConstructCString(SbgString *pString, const char *pCString)
{
    sbgStringConstructEmpty(pString);
    return sbgStringAssignCString(pString, pCString);
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringConstructVF(SbgString *pString, const char *pFormat, va_list args)
{
    sbgStringConstructEmpty(pString);
    return sbgStringAssignVF(pString, pFormat, args);
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringConstructF(SbgString *pString, const char *pFormat, ...)
{
    SbgErrorCode                         errorCode;
    va_list                              args;

    sbgStringConstructEmpty(pString);

    va_start(args, pFormat);
    errorCode = sbgStringAssignVF(pString, pFormat, args);
    va_end(args);

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringConstructStatic(SbgString *pString, const char *pCString, char *pBuffer, size_t size)
{
    assert(pString);
    assert(pBuffer);

    pString->pBuffer = pBuffer;
    pString->capacity = size;

    pString->readOnly = false;
    pString->isStatic = true;

    pString->errorCode = SBG_NO_ERROR;

    if (pCString)
    {
        sbgStringAssignCString(pString, pCString);
    }
    else
    {
        pString->length = strlen(pBuffer);
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API void sbgStringConstructReadOnly(SbgString *pString, const char *pCString)
{
    assert(pString);
    assert(pCString);

    pString->pBuffer = (char *)pCString;
    pString->length = strlen(pCString);
    pString->capacity = pString->length + 1;

    pString->readOnly = true;
    pString->isStatic = true;

    pString->errorCode = SBG_NO_ERROR;
}

SBG_COMMON_LIB_API void sbgStringDestroy(SbgString *pString)
{
    assert(pString);

    if (!pString->isStatic && (pString->pBuffer != pString->internalBuffer))
    {
        free(pString->pBuffer);
    }
}

SBG_COMMON_LIB_API int32_t sbgStringCompare(const SbgString *pString, const SbgString *pOtherString)
{
    assert(pString);
    assert(pOtherString);

    return (int32_t)strcmp(pString->pBuffer, pOtherString->pBuffer);
}

SBG_COMMON_LIB_API int32_t sbgStringCompareCString(const SbgString *pString, const char *pCString)
{
    assert(pString);
    assert(pCString);

    return (int32_t)strcmp(pString->pBuffer, pCString);
}

SBG_COMMON_LIB_API int32_t sbgStringCompareIgnoreCase(const SbgString *pString, const SbgString *pOtherString)
{
    assert(pOtherString);

    return sbgStringCompareIgnoreCaseCommon(pString, pOtherString->pBuffer, pOtherString->length);
}

SBG_COMMON_LIB_API int32_t sbgStringCompareIgnoreCaseCString(const SbgString *pString, const char *pCString)
{
    assert(pCString);

    return sbgStringCompareIgnoreCaseCommon(pString, pCString, strlen(pCString));
}

SBG_COMMON_LIB_API size_t sbgStringHash(const SbgString *pString)
{
    size_t                               hash;

    assert(pString);

    //
    // The algorithm is a simple polynomial accumulation :
    // s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1].
    //
    // See java.lang.String.hashCode().
    //

    hash = 0;

    for (size_t i = 0; i < pString->length; i++)
    {
        char                            *pCharPtr;

        pCharPtr = sbgStringGetCharPtr(pString, i);

        hash = ((hash << 5) - hash) + *pCharPtr;
    }

    return hash;
}

SBG_COMMON_LIB_API size_t sbgStringGetLength(const SbgString *pString)
{
    assert(pString);

    return pString->length;
}

SBG_COMMON_LIB_API const char *sbgStringGetCString(const SbgString *pString)
{
    assert(pString);

    return pString->pBuffer;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringCharAt(const SbgString *pString, size_t index, char *pChar)
{
    SbgErrorCode                         errorCode;

    assert(pString);
    assert(pChar);

    if (index < pString->length)
    {
        char                            *pCharPtr;

        pCharPtr    = sbgStringGetCharPtr(pString, index);
        *pChar      = *pCharPtr;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
    }

    return errorCode;
}

SBG_COMMON_LIB_API size_t sbgStringFindCString(const SbgString *pString, const char *pCString)
{
    char    *pChar;

    assert(pString);
    assert(pCString);

    pChar = strstr(pString->pBuffer, pCString);

    if (pChar)
    {
        return (size_t)(pChar - pString->pBuffer);
    }
    else
    {
        return SIZE_MAX;
    }
}

SBG_COMMON_LIB_API size_t sbgStringFind(const SbgString *pString, const SbgString *pOtherString)
{
    assert(pOtherString);

    return sbgStringFindCString(pString, pOtherString->pBuffer);
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringSubstring(const SbgString *pString, size_t startIndex, size_t endIndex, SbgString *pSubstring)
{
    SbgErrorCode                         errorCode;

    if (sbgStringIndexesAreValid(pString, startIndex, endIndex))
    {
        size_t                           length;

        length = endIndex - startIndex;

        errorCode = sbgStringAssignBuffer(pSubstring, &pString->pBuffer[startIndex], length);
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
    }

    return errorCode;
}

SBG_COMMON_LIB_API bool sbgStringStartsWith(const SbgString *pString, const char *pCString)
{
    size_t                               cStringLength;
    bool                                 match;

    assert(pString);
    assert(pCString);

    cStringLength = strlen(pCString);

    if (cStringLength <= pString->length)
    {
        int                              result;

        result = memcmp(pString->pBuffer, pCString, cStringLength);

        if (result == 0)
        {
            match = true;
        }
        else
        {
            match = false;
        }
    }
    else
    {
        match = false;
    }

    return match;
}

SBG_COMMON_LIB_API bool sbgStringEndsWith(const SbgString *pString, const char *pCString)
{
    size_t                               cStringLength;
    bool                                 match;

    assert(pString);
    assert(pCString);

    cStringLength = strlen(pCString);

    if (cStringLength <= pString->length)
    {
        size_t                           endIndex;
        int                              result;

        endIndex = pString->length - cStringLength;

        result = memcmp(&pString->pBuffer[endIndex], pCString, cStringLength);

        if (result == 0)
        {
            match = true;
        }
        else
        {
            match = false;
        }
    }
    else
    {
        match = false;
    }

    return match;
}

//----------------------------------------------------------------------//
//- Modification methods                                               -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API SbgErrorCode sbgStringGetLastError(const SbgString *pString)
{
    assert(pString);

    return pString->errorCode;
}

SBG_COMMON_LIB_API void sbgStringClearLastError(SbgString *pString)
{
    assert(pString);

    pString->errorCode = SBG_NO_ERROR;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringSetCharAt(SbgString *pString, size_t index, char c)
{
    SbgErrorCode                         errorCode;

    assert(pString);

    if ((index < pString->length) && (c != '\0'))
    {
        char                            *pCharPtr;

        pCharPtr    = sbgStringGetCharPtr(pString, index);
        *pCharPtr   = c;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringAppend(SbgString *pString, const SbgString *pAppendString)
{
    assert(pString);
    assert(pAppendString);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;

        length = pString->length + pAppendString->length;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            memcpy(&pString->pBuffer[pString->length], pAppendString->pBuffer, pAppendString->length + 1);
            pString->length = length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringAppendCString(SbgString *pString, const char *pCString)
{
    assert(pString);
    assert(pCString);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           cStringLength;
        size_t                           length;

        cStringLength = strlen(pCString);
        length = pString->length + cStringLength;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            memcpy(&pString->pBuffer[pString->length], pCString, cStringLength + 1);
            pString->length = length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringAppendVF(SbgString *pString, const char *pFormat, va_list args)
{
    assert(pString);
    assert(!pString->readOnly);
    assert(pFormat);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        va_list                          argsCopy;
        int                              result;
        size_t                           remainingSize;
        size_t                           newLength;

        remainingSize = pString->capacity - pString->length;

        va_copy(argsCopy, args);
        result = vsnprintf(&pString->pBuffer[pString->length], remainingSize, pFormat, argsCopy);
        assert(result >= 0);
        va_end(argsCopy);

        newLength = pString->length + (size_t)result;

        if ((size_t)result < remainingSize)
        {
            pString->length = newLength;
        }
        else
        {
            pString->errorCode = sbgStringResizeBuffer(pString, newLength + 1);

            if (pString->errorCode == SBG_NO_ERROR)
            {
                remainingSize = pString->capacity - pString->length;
                vsnprintf(&pString->pBuffer[pString->length], remainingSize, pFormat, args);
                pString->length = newLength;
            }
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringAppendF(SbgString *pString, const char *pFormat, ...)
{
    assert(pString);
    assert(pFormat);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        va_list                          args;

        va_start(args, pFormat);
        sbgStringAppendVF(pString, pFormat, args);
        va_end(args);
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringAssign(SbgString *pString, const SbgString *pAssignString)
{
    assert(pAssignString);

    return sbgStringAssignBuffer(pString, pAssignString->pBuffer, pAssignString->length);
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringAssignCString(SbgString *pString, const char *pCString)
{
    assert(pCString);

    return sbgStringAssignBuffer(pString, pCString, strlen(pCString));
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringAssignVF(SbgString *pString, const char *pFormat, va_list args)
{
    assert(pString);
    assert(pFormat);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        va_list                          argsCopy;
        int                              result;

        va_copy(argsCopy, args);
        result = vsnprintf(pString->pBuffer, pString->capacity, pFormat, argsCopy);
        assert(result >= 0);
        va_end(argsCopy);

        if ((size_t)result < pString->capacity)
        {
            pString->length = (size_t)result;
        }
        else
        {
            pString->errorCode = sbgStringResizeBuffer(pString, (size_t)result + 1);

            if (pString->errorCode == SBG_NO_ERROR)
            {
                vsnprintf(pString->pBuffer, pString->capacity, pFormat, args);
                pString->length = (size_t)result;
            }
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringAssignF(SbgString *pString, const char *pFormat, ...)
{
    assert(pString);
    assert(pFormat);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        va_list                          args;

        va_start(args, pFormat);
        sbgStringAssignVF(pString, pFormat, args);
        va_end(args);
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringMove(SbgString *pString, SbgString *pSourceString)
{
    SbgErrorCode                         errorCode;

    assert(pSourceString);

    errorCode = pSourceString->errorCode;

    if (errorCode == SBG_NO_ERROR)
    {
        if (pSourceString->isStatic || (pSourceString->pBuffer == pSourceString->internalBuffer))
        {
            errorCode = sbgStringAssign(pString, pSourceString);

            if (errorCode == SBG_NO_ERROR)
            {
                sbgStringClear(pSourceString);
            }
        }
        else
        {
            errorCode = sbgStringMoveBuffer(pString, pSourceString->pBuffer, pSourceString->length);

            if (errorCode == SBG_NO_ERROR)
            {
                sbgStringReset(pSourceString);
            }
        }
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringMoveCString(SbgString *pString, char *pCString)
{
    assert(pCString);

    return sbgStringMoveBuffer(pString, pCString, strlen(pCString));
}

SBG_COMMON_LIB_API char * sbgStringExtract(SbgString *pString)
{
    char                                *pBuffer = NULL;

    assert(pString);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        if (pString->isStatic || (pString->pBuffer == pString->internalBuffer))
        {
            size_t                               size;

            size    = pString->length + 1;
            pBuffer = malloc(size);

            if (pBuffer)
            {
                memcpy(pBuffer, pString->pBuffer, pString->length + 1);

                sbgStringClear(pString);
            }
        }
        else
        {
            pBuffer = pString->pBuffer;

            sbgStringReset(pString);
        }
    }

    return pBuffer;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringClear(SbgString *pString)
{
    assert(pString);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        sbgStringAssignCString(pString, "");
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToUpperCase(SbgString *pString)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        for (size_t i = 0; i < pString->length; i++)
        {
            pString->pBuffer[i] = (char)toupper((unsigned char)pString->pBuffer[i]);
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToLowerCase(SbgString *pString)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        for (size_t i = 0; i < pString->length; i++)
        {
            pString->pBuffer[i] = (char)tolower((unsigned char)pString->pBuffer[i]);
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringTrimLeft(SbgString *pString)
{
    assert(pString);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        bool                             trimmed;

        trimmed = sbgStringTrimLeftCommon(pString);

        if (trimmed)
        {
            sbgStringResizeBuffer(pString, pString->length + 1);
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringTrimRight(SbgString *pString)
{
    assert(pString);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        bool                             trimmed;

        trimmed = sbgStringTrimRightCommon(pString);

        if (trimmed)
        {
            sbgStringResizeBuffer(pString, pString->length + 1);
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringTrim(SbgString *pString)
{
    assert(pString);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        bool                             trimmedLeft;
        bool                             trimmedRight;

        trimmedLeft     = sbgStringTrimLeftCommon(pString);
        trimmedRight    = sbgStringTrimRightCommon(pString);

        if (trimmedLeft || trimmedRight)
        {
            sbgStringResizeBuffer(pString, pString->length + 1);
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API void sbgStringTruncate(SbgString *pString, size_t length)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        if (length < pString->length)
        {
            pString->pBuffer[length] = '\0';
            pString->length = length;

            sbgStringResizeBuffer(pString, length + 1);
        }
    }
}

//----------------------------------------------------------------------//
//- Conversion methods                                                 -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromInt8(SbgString *pString, int8_t value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%" PRId8, value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%" PRId8, value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToInt8(const SbgString *pString, int8_t *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    int8_t                               value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%" SCNd8, &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromUint8(SbgString *pString, uint8_t value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%" PRIu8, value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%" PRIu8, value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToUint8(const SbgString *pString, uint8_t *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    uint8_t                              value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%" SCNu8, &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromInt16(SbgString *pString, int16_t value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%" PRId16, value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%" PRId16, value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToInt16(const SbgString *pString, int16_t *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    int16_t                              value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%" SCNd16, &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromUint16(SbgString *pString, uint16_t value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%" PRIu16, value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%" PRIu16, value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToUint16(const SbgString *pString, uint16_t *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    uint16_t                             value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%" SCNu16, &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromInt32(SbgString *pString, int32_t value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%" PRId32, value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%" PRId32, value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToInt32(const SbgString *pString, int32_t *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    int32_t                              value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%" SCNd32, &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromUint32(SbgString *pString, uint32_t value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%" PRIu32, value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%" PRIu32, value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToUint32(const SbgString *pString, uint32_t *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    uint32_t                             value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%" SCNu32, &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromInt64(SbgString *pString, int64_t value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%" PRId64, value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%" PRId64, value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToInt64(const SbgString *pString, int64_t *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    int64_t                              value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%" SCNd64, &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromUint64(SbgString *pString, uint64_t value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%" PRIu64, value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%" PRIu64, value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToUint64(const SbgString *pString, uint64_t *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    uint64_t                             value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%" SCNu64, &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromFloat(SbgString *pString, float value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%f", value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%f", value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToFloat(const SbgString *pString, float *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    float                                value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%f", &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromDouble(SbgString *pString, double value)
{
    assert(pString);
    assert(!pString->readOnly);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           length;
        size_t                           oldCapacity;
        int                              result;

        result = snprintf(pString->pBuffer, pString->capacity, "%lf", value);

        assert(result >= 0);

        length = (size_t)result;

        oldCapacity = pString->capacity;

        pString->errorCode = sbgStringResizeBuffer(pString, length + 1);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            if (length >= oldCapacity)
            {
                snprintf(pString->pBuffer, pString->capacity, "%lf", value);
            }

            pString->length = (size_t)length;
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToDouble(const SbgString *pString, double *pValue)
{
    SbgErrorCode                         errorCode;
    int                                  result;
    double                               value;

    assert(pString);

    result = sscanf(pString->pBuffer, "%lf", &value);

    if (result == 1)
    {
        *pValue = value;

        errorCode = SBG_NO_ERROR;
    }
    else
    {
        errorCode = SBG_ERROR;
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringFromStreamBuffer(SbgString *pString, SbgStreamBuffer *pStream)
{
    assert(pString);

    if (pString->errorCode == SBG_NO_ERROR)
    {
        size_t                           size;

        size = sbgStreamBufferReadSizeT32(pStream);

        pString->errorCode = sbgStreamBufferGetLastError(pStream);

        if (pString->errorCode == SBG_NO_ERROR)
        {
            pString->errorCode = sbgStringResizeBuffer(pString, size);

            if (pString->errorCode == SBG_NO_ERROR)
            {
                pString->errorCode = sbgStreamBufferReadBuffer(pStream, pString->pBuffer, size);

                if (pString->errorCode == SBG_NO_ERROR)
                {
                    pString->length = size - 1;
                }
            }
        }
    }

    return pString->errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringToStreamBuffer(const SbgString *pString, SbgStreamBuffer *pStream)
{
    SbgErrorCode                         errorCode;

    assert(pString);

    errorCode = sbgStreamBufferWriteSizeT32(pStream, pString->length + 1);

    if (errorCode == SBG_NO_ERROR)
    {
        errorCode = sbgStreamBufferWriteBuffer(pStream, pString->pBuffer, pString->length + 1);
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Iterator methods                                                   -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API void sbgStringIteratorConstruct(SbgStringIterator *pIterator, const SbgString *pString)
{
    assert(pString);
    assert(pIterator);

    pIterator->pCursor = pString->pBuffer;
}

SBG_COMMON_LIB_API SbgErrorCode sbgStringIteratorWalk(SbgStringIterator *pIterator, const char *pSeparators, bool skipEmptyTokens, SbgString *pToken)
{
    SbgErrorCode                         errorCode;

    assert(pIterator);
    assert(pSeparators);
    assert(pToken);

    if (pIterator->pCursor)
    {
        size_t                           length;
        char                            *pSeparator;
        char                            *pNext;

        pSeparator = strpbrk(pIterator->pCursor, pSeparators);

        if (pSeparator)
        {
            length = pSeparator - pIterator->pCursor;

            if (skipEmptyTokens)
            {
                size_t                   nrSeparators;

                nrSeparators    = strspn(pSeparator, pSeparators);
                pNext           = &pSeparator[nrSeparators];
            }
            else
            {
                pNext           = &pSeparator[1];
            }
        }
        else
        {
            length  = strlen(pIterator->pCursor);
            pNext   = NULL;
        }

        errorCode = sbgStringAssignBuffer(pToken, pIterator->pCursor, length);

        if (errorCode == SBG_NO_ERROR)
        {
            pIterator->pCursor = pNext;
        }
    }
    else
    {
        errorCode = SBG_NOT_READY;
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Legacy functions                                                   -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API SbgErrorCode sbgStringCopy(char *pDestination, const char *pSource, size_t destMaxSize)
{
    size_t  srcLength;

    assert(pDestination);
    assert(pSource);
    assert(destMaxSize > 0);

    //
    // Get the source string length and add the NULL char
    //
    srcLength = strlen(pSource) + sizeof(char);

    //
    // Make sure the string fits in the provided buffer
    //
    if (srcLength <= destMaxSize)
    {
        //
        // We can use safely strcpy
        //
        strcpy(pDestination, pSource);

        return SBG_NO_ERROR;
    }
    else
    {
        //
        // The destination buffer isn't big enough to hold the provided source string
        // Don't forget to save one byte for the NULL terminated char
        //
        strncpy(pDestination, pSource, destMaxSize-sizeof(char));

        //
        // Append the NULL terminating char
        //
        pDestination[destMaxSize-1] = '\0';

        //
        // We have a buffer overflow
        //
        return SBG_BUFFER_OVERFLOW;
    }

}

SBG_COMMON_LIB_API const char *sbgStringFirstValidChar(const char *pInputStr)
{
    const char *pCurrentStr = pInputStr;

    assert(pInputStr);

    //
    // Skip any space or tab chars from the beginning of the string
    //
    while ((*pCurrentStr != '\0') && isspace((unsigned char)*pCurrentStr) )
    {
        pCurrentStr += sizeof(char);
    }

    return pCurrentStr;
}
