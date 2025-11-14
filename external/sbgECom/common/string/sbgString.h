/*!
 * \file            sbgString.h
 * \ingroup         common
 * \author          SBG Systems
 * \date            March 20, 2020
 *
 * \brief           Character string.
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

#ifndef SBG_STRING_H
#define SBG_STRING_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

//----------------------------------------------------------------------//
//- Header (open extern C block)                                       -//
//----------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Minimum size of the internal buffer, in bytes.
 *
 * This constant must be strictly positive in order to guarantee that no errors can occur
 * during the construction of an empty string.
 */
#define SBG_STRING_INTERNAL_BUFFER_MIN_SIZE                 (4)

#if SBG_STRING_INTERNAL_BUFFER_MIN_SIZE == 0
#error "invalid minimum internal buffer size"
#endif

/*!
 * Default size of the internal buffer, in bytes.
 */
#define SBG_STRING_INTERNAL_BUFFER_DEFAULT_SIZE             (16)

#ifndef SBG_CONFIG_STRING_INTERNAL_BUFFER_SIZE
#define SBG_CONFIG_STRING_INTERNAL_BUFFER_SIZE              SBG_STRING_INTERNAL_BUFFER_DEFAULT_SIZE
#elif SBG_CONFIG_STRING_INTERNAL_BUFFER_SIZE < SBG_STRING_INTERNAL_BUFFER_MIN_SIZE
#error "invalid internal buffer size"
#endif // SBG_CONFIG_STRING_INTERNAL_BUFFER_SIZE

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

/*!
 * String.
 *
 * The buffer contains a C null-terminated string. The null-terminating character is not
 * considered to be part of the string content.
 *
 * Attempts to modify a read-only string lead to undefined behavior.
 *
 * Attempts to modify a string while the last modification operation error is set are canceled
 * and return the last error code.
 */
typedef struct _SbgString
{
    /*!
     * Internal buffer.
     *
     * The internal buffer is used for small strings to avoid dynamic allocation.
     */
    char                                 internalBuffer[SBG_CONFIG_STRING_INTERNAL_BUFFER_SIZE];

    char                                *pBuffer;                                   /*!< Buffer. */
    size_t                               capacity;                                  /*!< Capacity of the buffer, in bytes. */
    size_t                               length;                                    /*!< Length of the string (not including the terminating null character), in bytes. */

    bool                                 readOnly;                                  /*!< True if the string is read-only. */
    bool                                 isStatic;                                  /*!< True if the buffer is statically allocated. */

    SbgErrorCode                         errorCode;                                 /*!< Error code of the last modification operation. */
} SbgString;

/*!
 * String iterator.
 */
typedef struct _SbgStringIterator
{
    const char                          *pCursor;                                   /*!< Cursor. */
} SbgStringIterator;

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

/*!
 * String constructor.
 *
 * The rationale for this constructor is to guarantee that no errors may occur on the
 * construction of empty strings.
 *
 * \param[in]   pString                     String.
 */
SBG_COMMON_LIB_API void sbgStringConstructEmpty(SbgString *pString);

/*!
 * String constructor.
 *
 * The string content is initialized from the given source string. If that string is NULL,
 * the string is initialized with an empty content.
 *
 * \param[in]   pString                     String.
 * \param[in]   pSourceString               Source string, may be NULL.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringConstruct(SbgString *pString, const SbgString *pSourceString);

/*!
 * String constructor.
 *
 * The string content is initialized from the given C null-terminated string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringConstructCString(SbgString *pString, const char *pCString);

/*!
 * String constructor.
 *
 * The string content is initialized from formatted data from a variable argument list.
 *
 * \param[in]   pString                     String.
 * \param[in]   pFormat                     Format.
 * \param[in]   args                        Variable argument list.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringConstructVF(SbgString *pString, const char *pFormat, va_list args) SBG_CHECK_FORMAT(printf, 2, 0);

/*!
 * String constructor.
 *
 * The string content is initialized from formatted data.
 *
 * \param[in]   pString                     String.
 * \param[in]   pFormat                     Format.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringConstructF(SbgString *pString, const char *pFormat, ...) SBG_CHECK_FORMAT(printf, 2, 3);

/*!
 * String constructor.
 *
 * The string content is initialized from the given C null-terminated string.
 *
 * If the C null-terminated string is NULL then the string contains the content of the given
 * buffer before construction. In this case the content of the given buffer must be a C
 * null-terminated string.
 *
 * The given buffer is owned by the string and must not be modified while the string is in use.
 * After destroying the string, the buffer contains the content of the string immediately before
 * destruction.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string, may be NULL.
 * \param[in]   pBuffer                     Buffer.
 * \param[in]   size                        Buffer size, in bytes.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringConstructStatic(SbgString *pString, const char *pCString, char *pBuffer, size_t size);

/*!
 * String constructor.
 *
 * The string cannot be modified.
 *
 * The given C null-terminated string provides both the statically allocated buffer
 * for the string, as well as its content. It is owned by the string and must not be modified.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 */
SBG_COMMON_LIB_API void sbgStringConstructReadOnly(SbgString *pString, const char *pCString);

/*!
 * String destructor.
 *
 * \param[in]   pString                     String.
 */
SBG_COMMON_LIB_API void sbgStringDestroy(SbgString *pString);

/*!
 * Compare a string to another string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pOtherString                Other string.
 * \return                                  The return value is 0 if both strings are equal,
 *                                          less than 0 if the string value is less than the other string value,
 *                                          greater than 0 if the string value is greater than the other string value.
 */
SBG_COMMON_LIB_API int32_t sbgStringCompare(const SbgString *pString, const SbgString *pOtherString);

/*!
 * Compare a string to a C null-terminated string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 * \return                                  The return value is 0 if the string is equal to the C null-terminated string,
 *                                          less than 0 if the string value is less than the C null-terminated string value,
 *                                          greater than 0 if the string value is greater than the C null-terminated string value.
 */
SBG_COMMON_LIB_API int32_t sbgStringCompareCString(const SbgString *pString, const char *pCString);

/*!
 * Compare a string to another string, ignoring case differences.
 *
 * \param[in]   pString                     String.
 * \param[in]   pOtherString                Other string.
 * \return                                  The return value is 0 if both strings are equal,
 *                                          less than 0 if the string value is less than the other string value,
 *                                          greater than 0 if the string value is greater than the other string value.
 */
SBG_COMMON_LIB_API int32_t sbgStringCompareIgnoreCase(const SbgString *pString, const SbgString *pOtherString);

/*!
 * Compare a string to a C null-terminated string, ignoring case differences.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 * \return                                  The return value is 0 if the string is equal to the C null-terminated string,
 *                                          less than 0 if the string value is less than the C null-terminated string value,
 *                                          greater than 0 if the string value is greater than the C null-terminated string value.
 */
SBG_COMMON_LIB_API int32_t sbgStringCompareIgnoreCaseCString(const SbgString *pString, const char *pCString);

/*!
 * Compute the hash value of a string.
 *
 * \param[in]   pString                     String.
 * \return                                  Hash value.
 */
SBG_COMMON_LIB_API size_t sbgStringHash(const SbgString *pString);

/*!
 * Get the length of a string (not including the terminating null character), in bytes.
 *
 * \param[in]   pString                     String.
 * \return                                  Length, in bytes.
 */
SBG_COMMON_LIB_API size_t sbgStringGetLength(const SbgString *pString);

/*!
 * Get a C null-terminated string version of a string.
 *
 * The pointer returned remains valid until the next modification operation on the string.
 *
 * \param[in]   pString                     String.
 * \return                                  C null-terminated string.
 */
SBG_COMMON_LIB_API const char *sbgStringGetCString(const SbgString *pString);

/*!
 * Get a character in a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   index                       Index.
 * \param[out]  pChar                       Character.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringCharAt(const SbgString *pString, size_t index, char *pChar);

/*!
 * Get the index of the first occurrence of a string in a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pStringToFind               String to find.
 * \return                                  Index of the first occurrence or SIZE_MAX if not found.
 */
SBG_COMMON_LIB_API size_t sbgStringFind(const SbgString *pString, const SbgString *pStringToFind);

/*!
 * Get the index of the first occurrence of the given C null-terminated string in a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 * \return                                  Index of the first occurrence or SIZE_MAX if not found.
 */
SBG_COMMON_LIB_API size_t sbgStringFindCString(const SbgString *pString, const char *pCString);

/*!
 * Get a substring from a string.
 *
 * The substring must be constructed before calling this function. If an error occurs, the last error
 * of the substring is set by this function.
 *
 * \param[in]   pString                     String.
 * \param[in]   startIndex                  Start index.
 * \param[in]   endIndex                    End index.
 * \param[out]  pSubstring                  Substring.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringSubstring(const SbgString *pString, size_t startIndex, size_t endIndex, SbgString *pSubstring);

/*!
 * Check if a C null-terminated string is at the beginning of a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 * \return                                  True if the C null-terminated string is at the beginning.
 */
SBG_COMMON_LIB_API bool sbgStringStartsWith(const SbgString *pString, const char *pCString);

/*!
 * Check if a C null-terminated string is at the end of a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 * \return                                  True if the C null-terminated string is at the end.
*/
SBG_COMMON_LIB_API bool sbgStringEndsWith(const SbgString *pString, const char *pCString);

//----------------------------------------------------------------------//
//- Modification methods                                               -//
//----------------------------------------------------------------------//

/*!
 * Get the last error of a modification operation on a string.
 *
 * \param[in]   pString                     String.
 * \return                                  Last modification error.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringGetLastError(const SbgString *pString);

/*!
 * Clear the last modification operation error of a string.
 *
 * \param[in]   pString                     String.
 */
SBG_COMMON_LIB_API void sbgStringClearLastError(SbgString *pString);

/*!
 * Set a character in a string.
 *
 * The given index must be strictly lower than the string length. The given character
 * must not be a null character.
 *
 * \param[in]   pString                     String.
 * \param[in]   index                       Index.
 * \param[in]   c                           Character.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringSetCharAt(SbgString *pString, size_t index, char c);

/*!
 * Append a string to another string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pAppendString               String to append.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringAppend(SbgString *pString, const SbgString *pAppendString);

/*!
 * Append a C null-terminated string to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringAppendCString(SbgString *pString, const char *pCString);

/*!
 * Append formatted data from a variable argument list to a string.
 *
 * The string format, as well as the variable argument list, are the same as for vprintf.
 *
 * \param[in]   pString                     String.
 * \param[in]   pFormat                     Format.
 * \param[in]   args                        Variable argument list.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringAppendVF(SbgString *pString, const char *pFormat, va_list args) SBG_CHECK_FORMAT(printf, 2, 0);

/*!
 * Append formatted data to a string.
 *
 * The string format, as well as the variable arguments, are the same as for printf.
 *
 * \param[in]   pString                     String.
 * \param[in]   pFormat                     Format.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringAppendF(SbgString *pString, const char *pFormat, ...) SBG_CHECK_FORMAT(printf, 2, 3);

/*!
 * Assign a string to another string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pAssignString               String to assign.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringAssign(SbgString *pString, const SbgString *pAssignString);

/*!
 * Assign a C null-terminated string to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringAssignCString(SbgString *pString, const char *pCString);

/*!
 * Assign formatted data from a variable argument list to a string.
 *
 * The string format, as well as the variable argument list, are the same as for vprintf.
 *
 * \param[in]   pString                     String.
 * \param[in]   pFormat                     Format.
 * \param[in]   args                        Variable argument list.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringAssignVF(SbgString *pString, const char *pFormat, va_list args) SBG_CHECK_FORMAT(printf, 2, 0);

/*!
 * Assign formatted data to a string.
 *
 * The string format, as well as the variable arguments, are the same as for printf.
 *
 * \param[in]   pString                     String.
 * \param[in]   pFormat                     Format.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringAssignF(SbgString *pString, const char *pFormat, ...) SBG_CHECK_FORMAT(printf, 2, 3);

/*!
 * Move content between two strings.
 *
 * On return, if successful, the source string is empty.
 *
 * \param[in]   pString                     String.
 * \param[in]   pSourceString               Source string.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringMove(SbgString *pString, SbgString *pSourceString);

/*!
 * Move a dynamically allocated C null-terminated string into a string.
 *
 * The C string must have been allocated using a standard C allocation function,
 * such as malloc() or strdup().
 *
 * On return, if successful, the C string buffer is owned by the string.
 *
 * \param[in]   pString                     String.
 * \param[in]   pCString                    C null-terminated string.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringMoveCString(SbgString *pString, char *pCString);

/*!
 * Extract the content of a string.
 *
 * If successful, the content returned is a C null-terminated string, becomes owned by
 * the caller, may be released with free(), and the given string becomes empty.
 *
 * \param[in]   pString                     String.
 * \return                                  C null-terminated string, NULL if an error occurred.
 */
SBG_COMMON_LIB_API char * sbgStringExtract(SbgString *pString);

/*!
 * Clear a string.
 *
 * After returning from this call the string is empty.
 *
 * \param[in]   pString                     String.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringClear(SbgString *pString);

/*!
 * Convert all the characters of a string to upper case.
 *
 * \param[in]   pString                     String.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToUpperCase(SbgString *pString);

/*!
 * Convert all the characters of a string to lower case.
 *
 * \param[in]   pString                     String.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToLowerCase(SbgString *pString);

/*!
 * Remove all spacing characters at the beginning of a string.
 *
 * \param[in]   pString                     String.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringTrimLeft(SbgString *pString);

/*!
 * Remove all spacing characters at the end of a string.
 *
 * \param[in]   pString                     String.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringTrimRight(SbgString *pString);

/*!
 * Remove all spacing characters at the beginning and the end of a string.
 *
 * \param[in]   pString                     String.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringTrim(SbgString *pString);

/*!
 * Truncate a string.
 *
 * If the given length is greater than or equal to the length of the string,
 * this function doesn't modify the string.
 *
 * \param[in]   pString                     String.
 * \param[in]   length                      Length, in bytes.
 */
SBG_COMMON_LIB_API void sbgStringTruncate(SbgString *pString, size_t length);

//----------------------------------------------------------------------//
//- Conversion methods                                                 -//
//----------------------------------------------------------------------//

/*!
 * Convert and assign a 8-bits signed integer value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromInt8(SbgString *pString, int8_t value);

/*!
 * Convert a string to a 8-bits signed integer value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToInt8(const SbgString *pString, int8_t *pValue);

/*!
 * Convert and assign a 8-bits unsigned integer value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromUint8(SbgString *pString, uint8_t value);

/*!
 * Convert a string to a 8-bits unsigned integer value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToUint8(const SbgString *pString, uint8_t *pValue);

/*!
 * Convert and assign a 16-bits signed integer value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromInt16(SbgString *pString, int16_t value);

/*!
 * Convert a string to a 16-bits signed integer value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToInt16(const SbgString *pString, int16_t *pValue);

/*!
 * Convert and assign a 16-bits unsigned integer value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromUint16(SbgString *pString, uint16_t value);

/*!
 * Convert a string to a 16-bits unsigned integer value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToUint16(const SbgString *pString, uint16_t *pValue);

/*!
 * Convert and assign a 32-bits signed integer value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromInt32(SbgString *pString, int32_t value);

/*!
 * Convert a string to a 32-bits signed integer value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToInt32(const SbgString *pString, int32_t *pValue);

/*!
 * Convert and assign a 32-bits unsigned integer value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromUint32(SbgString *pString, uint32_t value);

/*!
 * Convert a string to a 32-bits unsigned integer value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToUint32(const SbgString *pString, uint32_t *pValue);

/*!
 * Convert and assign a 64-bits signed integer value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromInt64(SbgString *pString, int64_t value);

/*!
 * Convert a string to a 64-bits signed integer value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToInt64(const SbgString *pString, int64_t *pValue);

/*!
 * Convert and assign a 64-bits unsigned integer value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromUint64(SbgString *pString, uint64_t value);

/*!
 * Convert a string to a 64-bits unsigned integer value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToUint64(const SbgString *pString, uint64_t *pValue);

/*!
 * Convert and assign a float value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromFloat(SbgString *pString, float value);

/*!
 * Convert a string to a float value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToFloat(const SbgString *pString, float *pValue);

/*!
 * Convert and assign a double value to a string.
 *
 * \param[in]   pString                     String.
 * \param[in]   value                       Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromDouble(SbgString *pString, double value);

/*!
 * Convert a string to a double value.
 *
 * \param[in]   pString                     String.
 * \param[out]  pValue                      Value.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToDouble(const SbgString *pString, double *pValue);

/*!
 * Read a string from a stream buffer.
 *
 * \param[in]   pString                     String.
 * \param[in]   pStream                     Stream buffer.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringFromStreamBuffer(SbgString *pString, SbgStreamBuffer *pStream);

/*!
 * Write a string to a stream buffer.
 *
 * \param[in]   pString                     String.
 * \param[out]  pStream                     Stream buffer.
 * \return                                  SBG_NO_ERROR if successful.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringToStreamBuffer(const SbgString *pString, SbgStreamBuffer *pStream);

//----------------------------------------------------------------------//
//- Iterator methods                                                   -//
//----------------------------------------------------------------------//

/*!
 * String iterator constructor.
 *
 * If the given string is modified while the iterator is used, the behavior is undefined.
 *
 * \param[in]   pIterator                   String iterator.
 * \param[in]   pString                     String.
 */
SBG_COMMON_LIB_API void sbgStringIteratorConstruct(SbgStringIterator *pIterator, const SbgString *pString);

/*!
 * Walk to the next token of a string.
 *
 * The token string must be constructed before calling this function. If an error occurs, the last error
 * of the token string is set by this function.
 *
 * \param[in]   pIterator                   String iterator.
 * \param[in]   pSeparators                 C null-terminated string containing separator characters.
 * \param[in]   skipEmptyTokens             If true, empty tokens are skipped.
 * \param[out]  pToken                      Token.
 * \return                                  SBG_NO_ERROR if successful,
 *                                          SBG_NOT_READY if there are no more tokens.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringIteratorWalk(SbgStringIterator *pIterator, const char *pSeparators, bool skipEmptyTokens, SbgString *pToken);

//----------------------------------------------------------------------//
//- Legacy functions                                                   -//
//----------------------------------------------------------------------//

/*!
 * Safely copy source string to destination according to destination provided buffer size.
 *
 * This method tends to replace strncpy where you would like to safely copy a string with buffer overflow checking.
 * If the provided output buffer is too small, the method will return a valid NULL terminated C string in destination.
 * This condition will be returned using SBG_BUFFER_OVERFLOW error code.
 *
 * \param[out]  pDestination                The destination string that should hold the copied input string.
 * \param[in]   pSource                     The source NULL terminated C string to copy to the destination.
 * \param[in]   destMaxSize                 The destination buffer size including the null terminating char.
 * \return                                  SBG_NO_ERROR if the source string has been copied to destination
 *                                          SBG_BUFFER_OVERFLOW if the destination is too small to hold the source string.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgStringCopy(char *pDestination, const char *pSource, size_t destMaxSize);

/*!
 * Skip all spacing characters from the beginning of a string.
 *
 * \param[in]   pInputStr                   Pointer on a read only string to trim.
 * \return                                  Pointer to the first useful char.
 */
SBG_COMMON_LIB_API const char *sbgStringFirstValidChar(const char *pInputStr);

//----------------------------------------------------------------------//
//- Footer (close extern C block)                                      -//
//----------------------------------------------------------------------//

#ifdef __cplusplus
}
#endif

#endif // SBG_STRING_H
