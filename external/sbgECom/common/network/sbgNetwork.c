// Project headers
#include <sbgCommon.h>
#include <swap/sbgSwap.h>


// Local headers
#include "sbgNetwork.h"

//----------------------------------------------------------------------//
//- IP manipulation methods                                            -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API void sbgNetworkIpToString(sbgIpAddress ipAddr, char *pBuffer, size_t maxSize)
{
    assert(pBuffer);
    assert(maxSize >= 16);

    SBG_UNUSED_PARAMETER(maxSize);

    snprintf(pBuffer, maxSize, "%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8, sbgIpAddrGetA(ipAddr), sbgIpAddrGetB(ipAddr), sbgIpAddrGetC(ipAddr), sbgIpAddrGetD(ipAddr));
}

SBG_COMMON_LIB_API sbgIpAddress sbgNetworkIpFromString(const char *pBuffer)
{
    int                  ipAddrA;
    int                  ipAddrB;
    int                  ipAddrC;
    int                  ipAddrD;
    int                  numReadParams;
    sbgIpAddress         ip;
    sbgIpAddress         ret;
    char                 checkBuffer[SBG_NETWORK_IPV4_STRING_SIZE];

    assert(pBuffer);

    ret = SBG_IPV4_UNSPECIFIED_ADDR;

    numReadParams = sscanf(pBuffer, "%d.%d.%d.%d", &ipAddrA, &ipAddrB, &ipAddrC, &ipAddrD);

    if ((numReadParams == 4) && (ipAddrA >= 0) && (ipAddrA <= 255) && (ipAddrB >= 0) && (ipAddrB <= 255) && (ipAddrC >= 0) && (ipAddrC <= 255) && (ipAddrD >= 0) && (ipAddrD <= 255))
    {
        ip = sbgIpAddr((uint8_t)ipAddrA, (uint8_t)ipAddrB, (uint8_t)ipAddrC, (uint8_t)ipAddrD);

        sbgNetworkIpToString(ip, checkBuffer, SBG_ARRAY_SIZE(checkBuffer));

        if (strcmp(pBuffer, checkBuffer) == 0)
        {
            ret = ip;
        }
    }

    return ret;
}

//----------------------------------------------------------------------//
//- IP validation methods                                              -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API bool sbgIpNetMaskValid(sbgIpAddress netmask)
{
    uint32_t    y;
    uint32_t    z;

    //
    // First test that the netmask is not zero, if yes, it's valid
    //
    if (netmask != 0)
    {
        //
        // We are doing arithmetic so we have to make sure the netmask is using the platform endianness
        // The IP address is always stored in big endian so we have to swap it for little endian platforms
        //
#if SBG_CONFIG_BIG_ENDIAN == 0
        netmask = sbgSwap32(netmask);
#endif

        //
        // Compute the bitwise inverse
        //
        y = ~netmask;

        //
        // Add one to the inverse so if netmask was a proper one, there will be at most 1 bit set in this.
        //
        z = y + 1;

        //
        // Test that, simply and z with z - 1, which happens to be y. The result will be zero if all is OK, non zero otherwise.
        //
        if ((z&y) == 0)
        {
            //
            // We have a valid netmask
            //
            return true;
        }
        else
        {
            return false;
        }
    }

    return true;
}

