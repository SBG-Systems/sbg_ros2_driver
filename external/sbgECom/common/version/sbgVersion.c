// Project headers
#include <sbgCommon.h>


// Local headers
#include "sbgVersion.h"

//----------------------------------------------------------------------//
//- Constructor                                                        -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API void sbgVersionCreateBasic(SbgVersion *pVersion, uint8_t major, uint8_t minor, uint8_t revision, uint8_t build)
{
	assert(pVersion);
	assert(major < 128);

	pVersion->softwareScheme	= false;

	pVersion->major				= major;
	pVersion->minor				= minor;
	pVersion->rev				= revision;
	pVersion->build				= build;

	pVersion->qualifier			= SBG_VERSION_QUALIFIER_DEV;
}

SBG_COMMON_LIB_API void sbgVersionCreateSoftware(SbgVersion *pVersion, uint8_t major, uint8_t minor, uint16_t build, SbgVersionQualifier qualifier)
{
	assert(pVersion);
	assert(major < 64);
	assert(minor < 64);

	pVersion->softwareScheme	= true;

	pVersion->major				= major;
	pVersion->minor				= minor;
	pVersion->build				= build;
	pVersion->qualifier			= qualifier;

	pVersion->rev				= 0;
}

SBG_COMMON_LIB_API void sbgVersionConstructCopy(SbgVersion *pVersion, const SbgVersion *pOtherVersion)
{
	assert(pVersion);
	assert(pOtherVersion);

	//
	// Copy each member individually to avoid warnings about reading uninitialized bytes.
	//
	pVersion->softwareScheme	= pOtherVersion->softwareScheme;
	pVersion->qualifier			= pOtherVersion->qualifier;
	pVersion->major				= pOtherVersion->major;
	pVersion->minor				= pOtherVersion->minor;
	pVersion->rev				= pOtherVersion->rev;
	pVersion->build				= pOtherVersion->build;
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
		pVersionInfo->qualifier	= (SbgVersionQualifier)((encodedVersion >> SBG_VERSION_SOFT_SCHEME_QUALIFIER_SHIFT) & SBG_VERSION_SOFT_SCHEME_QUALIFIER_MASK);
		pVersionInfo->major		= (encodedVersion >> SBG_VERSION_SOFT_SCHEME_MAJOR_SHIFT) & SBG_VERSION_SOFT_SCHEME_MAJOR_MASK;
		pVersionInfo->minor		= (encodedVersion >> SBG_VERSION_SOFT_SCHEME_MINOR_SHIFT) & SBG_VERSION_SOFT_SCHEME_MINOR_MASK;
		pVersionInfo->build		= (encodedVersion >> SBG_VERSION_SOFT_SCHEME_BUILD_SHIFT) & SBG_VERSION_SOFT_SCHEME_BUILD_MASK;

		//
		// Set the revision to zero as it's not used
		//
		pVersionInfo->rev		= 0;
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
		pVersionInfo->major		= (encodedVersion >> 24) & 0xFF;
		pVersionInfo->minor		= (encodedVersion >> 16) & 0xFF;
		pVersionInfo->rev		= (encodedVersion >>  8) & 0xFF;
		pVersionInfo->build		= (encodedVersion >>  0) & 0xFF;

		//
		// Set the qualifier to zero
		//
		pVersionInfo->qualifier	= SBG_VERSION_QUALIFIER_DEV;
	}
}

SBG_COMMON_LIB_API uint32_t sbgVersionEncode(const SbgVersion *pVersionInfo)
{
	uint32_t	encodedVersion = 0;

	assert(pVersionInfo);

	//
	// Test if we have a software scheme or a basic one
	//
	if (pVersionInfo->softwareScheme)
	{
		//
		// We have a software, scheme, so test that the version is valid
		//
		assert((pVersionInfo->major <= 63) && (pVersionInfo->minor <= 63) && (pVersionInfo->rev == 0));

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
	int32_t	result;

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
	SbgVersion	versionAInfo;
	SbgVersion	versionBInfo;

	//
	// Decode the versions
	//
	sbgVersionDecode(versionA, &versionAInfo);
	sbgVersionDecode(versionB, &versionBInfo);

	//
	// Do the comparaison
	//
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
	assert((	(sbgVersionIsUsingSoftwareScheme(lowerVersion) == sbgVersionIsUsingSoftwareScheme(higherVersion)) &&
				(sbgVersionIsUsingSoftwareScheme(lowerVersion) == sbgVersionIsUsingSoftwareScheme(version))) ||
			(	(sbgVersionIsUsingSoftwareScheme(lowerVersion) != sbgVersionIsUsingSoftwareScheme(higherVersion)) &&
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
	SbgErrorCode	errorCode = SBG_NO_ERROR;

	assert(pVersionInfo);
	assert(pBuffer);
	assert(sizeOfBuffer>0);

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
			//
			// Output buffer is to small
			//
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
			//
			// Generate the string version
			//
			sprintf(pBuffer, "%u.%u.%u.%u", pVersionInfo->major, pVersionInfo->minor, pVersionInfo->rev, pVersionInfo->build);
		}
		else
		{
			//
			// Output buffer is to small
			//
			errorCode = SBG_BUFFER_OVERFLOW;
		}
	}

	//
	// Return status of operation
	//
	return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgVersionToStringEncoded(uint32_t version, char *pBuffer, size_t sizeOfBuffer)
{
	SbgVersion	versionInfo;

	//
	// Decode the versions
	//
	sbgVersionDecode(version, &versionInfo);

	//
	// Return the version as a string
	//
	return sbgVersionToString(&versionInfo, pBuffer, sizeOfBuffer);
}

//----------------------------------------------------------------------//
//- String to version methods                                          -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API SbgErrorCode sbgVersionFromString(const char *pVersionStr, SbgVersion *pVersionInfo)
{
	SbgErrorCode	errorCode = SBG_NO_ERROR;
	char			qualifierStr[32];
	unsigned int	major;
	unsigned int	minor;
	unsigned int	rev;
	unsigned int	build;

	assert(pVersionStr);
	assert(pVersionInfo);

	//
	// Zero init the returned version struct
	//
	memset(pVersionInfo, 0x00, sizeof(SbgVersion));

	//
	// Try to parse a basic version
	//
	if (sscanf(pVersionStr, "%u.%u.%u.%u", &major, &minor, &rev, &build) == 4)
	{
		//
		// We have read successfully a basic version
		//
		pVersionInfo->softwareScheme = false;

		//
		// Fill the version numbers
		//
		pVersionInfo->major	= (uint8_t)major;
		pVersionInfo->minor	= (uint8_t)minor;
		pVersionInfo->rev	= (uint8_t)rev;
		pVersionInfo->build	= (uint8_t)build;
	}
	else if (sscanf(pVersionStr, "%u.%u.%u-%s", &major, &minor, &build, qualifierStr) == 4)
	{
		//
		// We have read successfully a software scheme version
		//
		pVersionInfo->softwareScheme = true;

		//
		// Fill the version numbers
		//
		pVersionInfo->major	= (uint8_t)major;
		pVersionInfo->minor	= (uint8_t)minor;
		pVersionInfo->build	= (uint16_t)build;

		//
		// Also convert the qualifier
		//
		if (!strcmp(qualifierStr, "dev"))
		{
			//
			// Dev qualifier
			//
			pVersionInfo->qualifier = SBG_VERSION_QUALIFIER_DEV;
		}
		else if (!strcmp(qualifierStr, "alpha"))
		{
			//
			// Alpha qualifier
			//
			pVersionInfo->qualifier = SBG_VERSION_QUALIFIER_ALPHA;
		}
		else if (!strcmp(qualifierStr, "beta"))
		{
			//
			// Beta qualifier
			//
			pVersionInfo->qualifier = SBG_VERSION_QUALIFIER_BETA;
		}
		else if (!strcmp(qualifierStr, "rc"))
		{
			//
			// Release Candidate qualifier
			//
			pVersionInfo->qualifier = SBG_VERSION_QUALIFIER_RC;
		}
		else if (!strcmp(qualifierStr, "stable"))
		{
			//
			// Stable qualifier
			//
			pVersionInfo->qualifier = SBG_VERSION_QUALIFIER_STABLE;
		}
		else if (!strcmp(qualifierStr, "hotfix"))
		{
			//
			// Hot Fix qualifier
			//
			pVersionInfo->qualifier = SBG_VERSION_QUALIFIER_HOT_FIX;
		}
		else
		{
			//
			// Unknown qualifier
			//
			errorCode = SBG_INVALID_PARAMETER;
		}
	}
	else
	{
		//
		// Invalid format
		//
		errorCode = SBG_INVALID_PARAMETER;
	}

	return errorCode;
}

SBG_COMMON_LIB_API SbgErrorCode sbgVersionFromStringEncoded(const char *pVersionStr, uint32_t *pVersion)
{
	SbgErrorCode	errorCode = SBG_NO_ERROR;
	SbgVersion		versionInfo;

	assert(pVersionStr);
	assert(pVersion);

	//
	// Parse the version from a string
	//
	errorCode = sbgVersionFromString(pVersionStr, &versionInfo);

	//
	// Test that no error has occurred
	//
	if (errorCode == SBG_NO_ERROR)
	{
		//
		// Encode the version and return it
		//
		*pVersion = sbgVersionEncode(&versionInfo);
	}
	else
	{
		//
		// An error has occurred so return a zero version
		//
		*pVersion = 0;
	}

	//
	// Return error
	//
	return errorCode;
}

