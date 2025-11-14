/*!
 * \file            sbgEComCmdEthernet.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            14 November 2016
 *
 * \brief           Ethernet configuration related commands.
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

#ifndef SBG_ECOM_CMD_ETHERNET_H
#define SBG_ECOM_CMD_ETHERNET_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Ethernet interface configuration                                   -//
//----------------------------------------------------------------------//

/*!
 * Enum that defines the different type of IP acquisition method.
 */
typedef enum _SbgEComEthernetMode
{
    SBG_ECOM_ETHERNET_DHCP          = 0,            /*!< The TCP/IP configuration should be acquired from a DHCP server. */
    SBG_ECOM_ETHERNET_STATIC        = 1             /*!< The TCP/IP configuration is manually defined. */
} SbgEComEthernetMode;

/*!
 * Structure that contains all Ethernet configuration or settings.
 */
typedef struct _SbgEComEthernetConf
{
    SbgEComEthernetMode mode;                       /*!< Define how the device will acquire its IP address, either DHCP or Static. */
    sbgIpAddress        ipAddress;                  /*!< For static mode, defines the device IP address. */
    sbgIpAddress        netmask;                    /*!< For static mode, defines the device net mask. */
    sbgIpAddress        gateway;                    /*!< For static mode, defines the gateway to use. */
    sbgIpAddress        dns1;                       /*!< For static mode, defines the primary DNS to use. */
    sbgIpAddress        dns2;                       /*!< For static mode, defines the secondary DNS to use. */
} SbgEComEthernetConf;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Get the configuration for the Ethernet interface.
 * 
 * \note This method only returns the Ethernet configuration and NOT the IP address currently used by the device.  
 * You should rather use sbgEComEthernetInfo to retrieve the current assigned IP.
 * 
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pEthernetConf               SbgEComEthernetConf struct to hold the read configuration from the device.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComEthernetGetConf(SbgEComHandle *pHandle, SbgEComEthernetConf *pEthernetConf);

/*!
 * Set the configuration for the Ethernet interface.
 * 
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pEthernetConf               SbgEComEthernetConf struct to hold the new configuration to apply.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComEthernetSetConf(SbgEComHandle *pHandle, const SbgEComEthernetConf *pEthernetConf);

/*!
 * Get the current assigned and used IP address as well as network information.
 * 
 * In opposition to sbgEComEthernetGetConf, this method will not return the Ethernet configuration.
 * It will rather return the IP address currently used by the device.
 * 
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pEthernetConf               SbgEComEthernetConf struct to hold the read IP settings from the device.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComEthernetInfo(SbgEComHandle *pHandle, SbgEComEthernetConf *pEthernetConf);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_ETHERNET_H
