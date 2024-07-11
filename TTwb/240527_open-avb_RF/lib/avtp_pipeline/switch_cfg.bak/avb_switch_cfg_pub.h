#if 1 // jay.choi.switch.conf

#else
/*************************************************************************************************************
Copyright (c) 2012-2015, Symphony Teleca Corporation, a Harman International Industries, Incorporated company
All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS LISTED "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS LISTED BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
Attributions: The inih library portion of the source code is licensed from 
Brush Technology and Ben Hoyt - Copyright (c) 2009, Brush Technology and Copyright (c) 2009, Ben Hoyt. 
Complete license and copyright information can be found at 
https://github.com/benhoyt/inih/commit/74d2ca064fb293bc60a77b0bd068075b293cf175.
*************************************************************************************************************/
#endif

/*
* HEADER SUMMARY : NULL mapping module public interface
*/

#ifndef AVB_SWITCH_CFG_PUB_H
#define AVB_SWITCH_CFG_PUB_H 1

#if 1 // jay.choi.switch.conf
#include "openavb_types_pub.h"

#define AVB_SWITCH_CFG

#if 1 // jay.choi.switch.conf
typedef void (*avb_switch_cfg_cfg_cb_t)(void);
typedef void (*avb_switch_cfg_gen_init_cb_t)(void);  
#else
/** Configuration callback into the interface module.
 *
 * This callback function is called during the reading of the configuration file
 * by the talker and listener for any named configuration item starting with
 * "intf_nv".
 * This is a convenient way to store new configuration name/value pairs that are
 * needed in an interface module.
 *
 * \param pMediaQ A pointer to the media queue for this stream
 * \param name The item name from the configuration file
 * \param value The item value from the configuration file
 */
typedef void (*openavb_intf_cfg_cb_t)(media_q_t *pMediaQ, const char *name, const char *value);

/** General initialize callback regardless if a talker or listener.
 *
 * This callback function is called when the openavbTLOpen() function is called for
 * the EAVB SDK API.
 *
 * \param pMediaQ A pointer to the media queue for this stream
 */
typedef void (*openavb_intf_gen_init_cb_t)(media_q_t *pMediaQ); 
#endif


/** Configuration callbacks structure.
 */
typedef struct {
	/// Configuration callback.
	avb_switch_cfg_cfg_cb_t switch_cfg_cfg_cb;
	/// General initialize callback.
	avb_switch_cfg_gen_init_cb_t switch_cfg_gen_init_cb;
#if 0 // jay.choi.switch.conf
	/// AVDECC initialize callback.
	openavb_intf_avdecc_init_cb_t	intf_avdecc_init_cb;
	/// Initialize transmit callback.
	openavb_intf_tx_init_cb_t		intf_tx_init_cb;
	/// Transmit callback.
	openavb_intf_tx_cb_t			intf_tx_cb;
	/// Initialize receive callback.
	openavb_intf_rx_init_cb_t		intf_rx_init_cb;
	// openavb_intf_rx_translate_cb_t	intf_rx_translate_cb;	// Hidden since it is for direct mapping -> interface CBs
	/// Receive callback.
	openavb_intf_rx_cb_t			intf_rx_cb;
	/// Stream end callback.
	openavb_intf_end_cb_t			intf_end_cb;
	/// General shutdown callback.
	openavb_intf_gen_end_cb_t		intf_gen_end_cb;
	/// Pointer to interface specific callbacks that a hosting application may call.
	/// It is pointer to openavb_intf_host_cb_list_t structure.
	void *						intf_host_cb_list;
	/// Source bit rate callback.
	openavb_intf_get_src_bitrate_t		intf_get_src_bitrate_cb;
	/// TX blocking in interface callback.
	openavb_intf_tx_blocking_in_intf_t	intf_tx_blocking_in_intf_cb;
#endif
} avb_switch_cfg_cb_t;

#else
#include "openavb_types_pub.h"

// NOTE: A define is used for the MediaQDataFormat identifier because it is needed in separate execution units (static / dynamic libraries)
// that is why a single static (static/extern pattern) definition can not be used.
#define MapPipeMediaQDataFormat "Pipe"
#endif

#endif  // OPENAVB_SWITCH_CFG_PUB_H
