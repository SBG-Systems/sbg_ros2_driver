// Project headers
#include <sbgCommon.h>


// Local headers
#include "sbgVersion.h"

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

/*!
 * Convert a qualifier from a string to the corresponding enum value.
 * 
 * \param[in]   pString                 The string to convert.
 * \param[out]  pValue                  Converted value.
 * \return                              SBG_NO_ERROR if converted successfully.
 */
static SbgErrorCode sbgVersionQualifierFromString(const char *pString, SbgVersionQualifier *pValue)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;

    assert(pString);
    assert(pValue);

    if (!strcmp(pString, "dev"))
    {
        *pValue = SBG_VERSION_QUALIFIER_DEV;
    }
    else if (!strcmp(pString, "alpha"))
    {
        *pValue = SBG_VERSION_QUALIFIER_ALPHA;
    }
    else if (!strcmp(pString, "beta"))
    {
        *pValue = SBG_VERSION_QUALIFIER_BETA;
    }
    else if (!strcmp(pString, "rc"))
    {
        *pValue = SBG_VERSION_QUALIFIER_RC;
    }
    else if (!strcmp(pString, "stable"))
    {
        *pValue = SBG_VERSION_QUALIFIER_STABLE;
    }
    else if (!strcmp(pString, "hotfix"))
    {
        *pValue = SBG_VERSION_QUALIFIER_HOT_FIX;
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Constructor                                                        -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API void sbgVersionCreateBasic(SbgVersion *pVersion, uint8_t major, uint8_t minor, uint8_t revision, uint8_t build)
{
    assert(pVersion);
    assert(major <= 127);

    pVersion->softwareScheme    = false;

    pVersion->major             = major;
    pVersion->minor             = minor;
    pVersion->rev               = revision;
    pVersion->build             = build;

    pVersion->qualifier         = SBG_VERSION_QUALIFIER_DEV;
}

SBG_COMMON_LIB_API void sbgVersionCreateSoftware(SbgVersion *pVersion, uint8_t major, uint8_t minor, uint16_t build, SbgVersionQualifier qualifier)
{
    assert(pVersion);
    assert(major <= 63);
    assert(minor <= 63);

    pVersion->softwareScheme    = true;

    pVersion->major             = major;
    pVersion->minor             = minor;
    pVersion->build             = build;
    pVersion->qualifier         = qualifier;

    pVersion->rev               = 0;
}

SBG_COMMON_LIB_API void sbgVersionConstructCopy(SbgVersion *pVersion, const SbgVersion *pOtherVersion)
{
    assert(pVersion);
    assert(pOtherVersion);

    //
    // Copy each member individually to avoid warnings about reading uninitialized bytes.
    //
    pVersion->softwareScheme    = pOtherVersion->softwareScheme;
    pVersion->qualifier         = pOtherVersion->qualifier;
    pVersion->major             = pOtherVersion->major;
    pVersion->minor             = pOtherVersion->minor;
    pVersion->rev               = pOtherVersion->rev;
    pVersion->build             = pOtherVersion->build;
}

//----------------------------------------------------------------------//
//- Version encoding / decoding methods                                -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API void sbgVersionDecode(uint32_t encodedVersion, SbgVersion *pVersionInfo)
{
    assert(pVersionInfo);

    //
    // Test if we have a software version scheme
    //
    if (encodedVersion&SBG_VERSION_SOFT_SCHEME)
    {
        //
        // We have a software scheme, decode it
        //
        pVersionInfo->softwareScheme = true;

        //
        // Decode the software scheme fields
        //
        pVersionInfo->qualifier = (SbgVersionQualifier)((encodedVersion >> SBG_VERSION_SOFT_SCHEME_QUALIFIER_SHIFT) & SBG_VERSION_SOFT_SCHEME_QUALIFIER_MASK);
        pVersionInfo->major     = (encodedVersion >> SBG_VERSION_SOFT_SCHEME_MAJOR_SHIFT) & SBG_VERSION_SOFT_SCHEME_MAJOR_MASK;
        pVersionInfo->minor     = (encodedVersion >> SBG_VERSION_SOFT_SCHEME_MINOR_SHIFT) & SBG_VERSION_SOFT_SCHEME_MINOR_MASK;
        pVersionInfo->build     = (encodedVersion >> SBG_VERSION_SOFT_SCHEME_BUILD_SHIFT) & SBG_VERSION_SOFT_SCHEME_BUILD_MASK;

        //
        // Set the revision to zero as it's not used
        //
        pVersionInfo->rev       = 0;
    }
    else
    {
        //
        // We have a basic scheme, decode it
        //
        pVersionInfo->softwareScheme = false;

        //
        // Decode the software scheme fields
        //
        pVersionInfo->major     = (encodedVersion >> 24) & 0xFF;
        pVersionInfo->minor     = (encodedVersion >> 16) & 0xFF;
        pVersionInfo->rev       = (encodedVersion >>  8) & 0xFF;
        pVersionInfo->build     = (encodedVersion >>  0) & 0xFF;

        //
        // Set the qualifier to zero
        //
        pVersionInfo->qualifier = SBG_VERSION_QUALIFIER_DEV;
    }
}

SBG_COMMON_LIB_API uint32_t sbgVersionEncode(const SbgVersion *pVersionInfo)
{
    uint32_t    encodedVersion = 0;

    assert(pVersionInfo);

    //
    // Test if we have a software scheme or a basic one
    //
    if (pVersionInfo->softwareScheme)
    {
        //
        // We have a software, scheme, so test that the version is valid
        //
        assert(pVersionInfo->major <= 63);
        assert(pVersionInfo->minor <= 63);
        assert(pVersionInfo->rev == 0);

        //
        // Indicate that we have a software version scheme
        //
        encodedVersion = SBG_VERSION_SOFTWARE(pVersionInfo->major, pVersionInfo->minor, pVersionInfo->build, pVersionInfo->qualifier);
    }
    else
    {
        //
        // We have a basic version scheme so check parameter validity
        //
        assert(pVersionInfo->major <= 127);
        assert(pVersionInfo->build <= UINT8_MAX);

        //
        // Encode the basic version information
        //
        encodedVersion = SBG_VERSION_BASIC(pVersionInfo->major, pVersionInfo->minor, pVersionInfo->rev, pVersionInfo->build);
    }

    return encodedVersion;
}

//----------------------------------------------------------------------//
//- Version comparaison methods                                        -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API int32_t sbgVersionCompare(const SbgVersion *pVersionA, const SbgVersion *pVersionB, SbgVersionCmpThreshold threshold)
{
    int32_t         result;

    assert(pVersionA);
    assert(pVersionB);

    //
    // Check that the two versions are using the same scheme
    //
    assert(pVersionA->softwareScheme == pVersionB->softwareScheme);

    //
    // Do the comparaison according to the selected threshold
    // Start by compairing the major field
    //
    result = pVersionA->major - pVersionB->major;

    //
    // Test if we have to also compare the minor field
    //
    if ( (result == 0) &&  ((threshold == SBG_VERSION_CMP_THRESHOLD_MINOR) || (threshold == SBG_VERSION_CMP_THRESHOLD_REVISION) || (threshold == SBG_VERSION_CMP_THRESHOLD_BUILD) || (threshold == SBG_VERSION_CMP_THRESHOLD_QUALIFIER)) )
    {
        //
        // Update the result using the minor indication
        //
        result = pVersionA->minor - pVersionB->minor;

        //
        // Test if we have to also compare the revision field (for basic version only)
        //
        if ( (result == 0) &&  ((threshold == SBG_VERSION_CMP_THRESHOLD_REVISION) || (threshold == SBG_VERSION_CMP_THRESHOLD_BUILD) || (threshold == SBG_VERSION_CMP_THRESHOLD_QUALIFIER)) )
        {
            //
            // Test if we have a software scheme or a basic scheme version
            //
            if (pVersionA->softwareScheme)
            {
                //
                // We have a software scheme so set the result to 0
                //
                result = 0;
            }
            else
            {
                //
                // We have a basic scheme so we can compare the revision field
                //
                result = pVersionA->rev - pVersionB->rev;
            }

            //
            // Test if we have to also compare the build field
            //
            if ( (result == 0) &&  ((threshold == SBG_VERSION_CMP_THRESHOLD_BUILD) || (threshold == SBG_VERSION_CMP_THRESHOLD_QUALIFIER)) )
            {
                //
                // Compare the build field
                //
                result = pVersionA->build - pVersionB->build;

                //
                // Test if we have to also compare the qualifier field
                //
                if ( (result == 0) && (threshold == SBG_VERSION_CMP_THRESHOLD_QUALIFIER) )
                {
                    //
                    // Test if we have a software scheme
                    //
                    if (pVersionA->softwareScheme)
                    {
                        //
                        // We have a software scheme so set the result to 0
                        //
                        result = pVersionA->qualifier - pVersionB->qualifier;
                    }
                    else
                    {
                        //
                        // We have a basic scheme so set the result to 0
                        //
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

SBG_COMMON_LIB_API int32_t sbgVersionCompareEncoded(uint32_t versionA, uint32_t versionB, SbgVersionCmpThreshold threshold)
{
    SbgVersion  versionAInfo;
    SbgVersion  versionBInfo;

    sbgVersionDecode(versionA, &versionAInfo);
    sbgVersionDecode(versionB, &versionBInfo);

    return sbgVersionCompare(&versionAInfo, &versionBInfo, threshold);
}

SBG_COMMON_LIB_API int32_t sbgVersionIsWithinRange(const SbgVersion *pLowerVersion, const SbgVersion *pHigherVersion, const SbgVersion *pVersion)
{
    assert(pLowerVersion);
    assert(pHigherVersion);
    assert(pVersion);

    //
    // Use the encoded version to speed up the comparaison
    //
    return sbgVersionIsWithinRangeEncoded(sbgVersionEncode(pLowerVersion), sbgVersionEncode(pHigherVersion), sbgVersionEncode(pVersion));
}

SBG_COMMON_LIB_API int32_t sbgVersionIsWithinRangeEncoded(uint32_t lowerVersion, uint32_t higherVersion, uint32_t version)
{
    //
    // Make sure that all versions are using the same scheme
    //
    assert(lowerVersion <= higherVersion);
    assert((    (sbgVersionIsUsingSoftwareScheme(lowerVersion) == sbgVersionIsUsingSoftwareScheme(higherVersion)) &&
                (sbgVersionIsUsingSoftwareScheme(lowerVersion) == sbgVersionIsUsingSoftwareScheme(version))) ||
            (   (sbgVersionIsUsingSoftwareScheme(lowerVersion) != sbgVersionIsUsingSoftwareScheme(higherVersion)) &&
                (sbgVersionIsUsingSoftwareScheme(lowerVersion) != sbgVersionIsUsingSoftwareScheme(version))));

    //
    // We can compare safely the encoded version directly
    //
    if (version < lowerVersion)
    {
        return -1;
    }
    else if (version > higherVersion)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//----------------------------------------------------------------------//
//- Version to string methods                                          -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API SbgErrorCode sbgVersionToString(const SbgVersion *pVersionInfo, char *pBuffer, size_t sizeOfBuffer)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;

    assert(pVersionInfo);
    assert(pBuffer);
    assert(sizeOfBuffer > 0);

    //
    // Test if we have a basic or software version scheme
    //
    if (pVersionInfo->softwareScheme)
    {
        //
        // We have a software version scheme
        // Test that the buffer is big enough to store the generated string (31.31.65535-hotfix)
        //
        if (sizeOfBuffer >= 19)
        {
            //
            // Generate the string version
            //
            sprintf(pBuffer, "%u.%u.%u-", pVersionInfo->major, pVersionInfo->minor, pVersionInfo->build);

            //
            // Append the qualifier
            //
            switch (pVersionInfo->qualifier)
            {
            case SBG_VERSION_QUALIFIER_DEV:
                strcat(pBuffer, "dev");
                break;
            case SBG_VERSION_QUALIFIER_ALPHA:
                strcat(pBuffer, "alpha");
                break;
            case SBG_VERSION_QUALIFIER_BETA:
                strcat(pBuffer, "beta");
                break;
            case SBG_VERSION_QUALIFIER_RC:
                strcat(pBuffer, "rc");
                break;
            case SBG_VERSION_QUALIFIER_STABLE:
                strcat(pBuffer, "stable");
                break;
            case SBG_VERSION_QUALIFIER_HOT_FIX:
                strcat(pBuffer, "hotfix");
                break;
            default:
                break;
            }
        }
        else
        {
            errorCode = SBG_BUFFER_OVERFLOW;
        }

    }
    else
    {
        //
        // We have a basic version scheme, generate the string
        // Test that the buffer is big enough to store the generated string
        //
        if (sizeOfBuffer >= 16)
        {
            sprintf(pBuffer, "%u.%u.%u.%u", pVersionInfo->major, pVersionInfo->minor, pVersionInfo->rev, pVersionInfo->build);
        }
        else
        {
            errorCode = SBG_BUFFER_OVERFLOW;
        }
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgVersionToStringEncoded(uint32_t version, char *pBuffer, size_t sizeOfBuffer)
{
    SbgVersion  versionInfo;

    sbgVersionDecode(version, &versionInfo);

    return sbgVersionToString(&versionInfo, pBuffer, sizeOfBuffer);
}

//----------------------------------------------------------------------//
//- String to version methods                                          -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API SbgErrorCode sbgVersionFromString(const char *pVersionStr, SbgVersion *pVersionInfo)
{
    SbgErrorCode    errorCode       = SBG_INVALID_PARAMETER;
    uint32_t        dotCount        = 0;
    uint32_t        dashCount       = 0;

    assert(pVersionStr);
    assert(pVersionInfo);

    memset(pVersionInfo, 0x00, sizeof(SbgVersion));

    //
    // Pre-parse the input string to count the number of '.' and '-' chars
    // It is used to quickly evaluate the expected version format
    //
    for (size_t i = 0; pVersionStr[i] != '\0'; i++)
    {
        if (pVersionStr[i] == '.')
        {
            dotCount++;
        }
        else if (pVersionStr[i] == '-')
        {
            dashCount++;
        }
    }

    //
    // Try to parse each version format
    //
    if (dashCount == 0)
    {
        if (dotCount == 3)
        {
            unsigned int        major;
            unsigned int        minor;
            unsigned int        rev;
            unsigned int        build;

            if (sscanf(pVersionStr, "%u.%u.%u.%u", &major, &minor, &rev, &build) == 4)
            {
                if ( (major <= 127) && (minor <= UINT8_MAX) && (rev <= UINT8_MAX)  && (build <= UINT8_MAX) )
                {
                    pVersionInfo->softwareScheme    = false;
                    pVersionInfo->major             = (uint8_t)major;
                    pVersionInfo->minor             = (uint8_t)minor;
                    pVersionInfo->rev               = (uint8_t)rev;
                    pVersionInfo->build             = (uint16_t)build;
                    
                    errorCode                       = SBG_NO_ERROR;
                }
            }
        }
        else if (dotCount == 2)
        {
            unsigned int        major;
            unsigned int        minor;
            unsigned int        rev;
            
            if (sscanf(pVersionStr, "%u.%u.%u", &major, &minor, &rev) == 3)
            {
                if ( (major <= 127) && (minor <= UINT8_MAX) && (rev <= UINT8_MAX) )
                {
                    pVersionInfo->softwareScheme    = false;
                    pVersionInfo->major             = (uint8_t)major;
                    pVersionInfo->minor             = (uint8_t)minor;
                    pVersionInfo->rev               = (uint8_t)rev;
                    
                    errorCode                       = SBG_NO_ERROR;
                }
            }
        }
        else if (dotCount == 1)
        {
            unsigned int        major;
            unsigned int        minor;
            
            if (sscanf(pVersionStr, "%u.%u", &major, &minor) == 2)
            {
                if ( (major <= 127) && (minor <= UINT8_MAX) )
                {
                    pVersionInfo->softwareScheme    = false;
                    pVersionInfo->major             = (uint8_t)major;
                    pVersionInfo->minor             = (uint8_t)minor;
                    
                    errorCode                       = SBG_NO_ERROR;
                }
            }
        }
        else if (dotCount == 0)
        {
            unsigned int        major;
            
            if (sscanf(pVersionStr, "%u", &major) == 1)
            {
                if (major <= 127)
                {
                    pVersionInfo->softwareScheme    = false;
                    pVersionInfo->major             = (uint8_t)major;
                    
                    errorCode                       = SBG_NO_ERROR;
                }
            }
        }
    }
    else if ( (dashCount == 1) && (dotCount == 2) )
    {
        char                qualifierStr[32];
        unsigned int        major;
        unsigned int        minor;
        unsigned int        build;
        
        if (sscanf(pVersionStr, "%u.%u.%u-%s", &major, &minor, &build, qualifierStr) == 4)
        {
            if ( (major <= 63) && (minor <= 63) && (build <= UINT16_MAX) )
            {
                pVersionInfo->softwareScheme    = true;
                pVersionInfo->major             = (uint8_t)major;
                pVersionInfo->minor             = (uint8_t)minor;
                pVersionInfo->build             = (uint16_t)build;

                errorCode = sbgVersionQualifierFromString(qualifierStr, &pVersionInfo->qualifier);
            }
        }
    }

    return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgVersionFromStringEncoded(const char *pVersionStr, uint32_t *pVersion)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;
    SbgVersion      versionInfo;

    assert(pVersionStr);
    assert(pVersion);

    //
    // Parse the version from a string
    //
    errorCode = sbgVersionFromString(pVersionStr, &versionInfo);

    if (errorCode == SBG_NO_ERROR)
    {
        *pVersion = sbgVersionEncode(&versionInfo);
    }
    else
    {
        *pVersion = 0;
    }

    return errorCode;
}

