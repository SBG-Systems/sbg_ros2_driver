/*!
 * \file            sbgInterfaceUdp.h
 * \ingroup         common
 * \author          SBG Systems
 * \date            05 February 2013
 *
 * \brief           This file implements an UDP interface.
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
#ifndef SBG_INTERFACE_UDP_H
#define SBG_INTERFACE_UDP_H

#ifdef __cplusplus
extern "C" {
#endif

// sbgCommonLib headers
#include <sbgCommon.h>
#include <interfaces/sbgInterface.h>

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

/*!
 * Initialize an unconnected UDP interface for read and write operations.
 *
 * Connected/Unconnected modes:
 * 
 * The UDP socket is unconnected so any host sending data on localPort should be received.
 * However, only the host specified in remoteAddr should received sent data  on remotePort.
 * 
 * You can enable the connected mode with the `sbgInterfaceUdpSetConnectedMode` method.
 * If this case, only datagrams sent by the host with remoteAddr ip and remotePort
 * will be received and any other traffic should be discarded.
 * 
 * Broadcast:
 * 
 * You can specify a SBG_IPV4_BROADCAST_ADDR address to send an UDP datagram to all hosts
 * However, you have to allow broadcasted packets with the `sbgInterfaceUdpAllowBroadcast` method.
 *
 * \param[in]   pInterface                      Pointer on an allocated interface instance to initialize.
 * \param[in]   remoteAddr                      IP address to send data to.
 * \param[in]   remotePort                      Ethernet port to send data to.
 * \param[in]   localPort                       Ethernet port on which the interface is listening.
 * \return                                      SBG_NO_ERROR if the interface has been created.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgInterfaceUdpCreate(SbgInterface *pInterface, sbgIpAddress remoteAddr, uint32_t remotePort, uint32_t localPort);

/*!
 * Define if the UDP socket should use connected or unconnected mode.
 *
 * If connected mode is enabled, the interface only accepts datagrams from remortAddr & remotePort.
 * 
 * \param[in]   pInterface                      Pointer on a valid UDP interface created using sbgInterfaceUdpCreate.
 * \param[in]   useConnected                    Set to true to enable connected mode and false for unconnected.
 */
SBG_COMMON_LIB_API void sbgInterfaceUdpSetConnectedMode(SbgInterface *pInterface, bool useConnected);

/*!
 * Returns if the UDP socket is using or not connected mode.
 * 
 * \param[in]   pInterface                      Pointer on a valid UDP interface created using sbgInterfaceUdpCreate.
 * \return                                      true if connected mode is enabled or false otherwise.
 */
SBG_COMMON_LIB_API bool sbgInterfaceUdpGetConnectedMode(const SbgInterface *pInterface);

/*!
 * Define if a socket can send broadcasted packets.
 *
 * \param[in]   pInterface                      Pointer on a valid UDP interface created using sbgInterfaceUdpCreate.
 * \param[in]   allowBroadcast                  Set to true to allow this socket to send broadcasted UDP packets.
 * \return                                      SBG_NO_ERROR if the allow broadcast status has been changed.
 */
SBG_COMMON_LIB_API SbgErrorCode sbgInterfaceUdpAllowBroadcast(SbgInterface *pInterface, bool allowBroadcast);

#ifdef __cplusplus
}
#endif

#endif // SBG_INTERFACE_UDP_H
