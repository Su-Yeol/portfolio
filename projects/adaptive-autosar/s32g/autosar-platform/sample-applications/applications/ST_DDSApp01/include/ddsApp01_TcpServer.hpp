// --------------------------------------------------------------------------
// |              _    _ _______     .----.      _____         _____        |
// |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
// |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
// |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
// |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
// |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
// |                              . _ _  .                                  |
// --------------------------------------------------------------------------
//
// All Rights Reserved.
// Any use of this source code is subject to a license agreement with the
// AUTOSAR development cooperation.
// More information is available at www.autosar.org.
//
// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/*!
 *  \file
 *  \brief DDS TCP Server.
 *
 *  \details Contains DDS TCP Server for socket communication.
 *   data recieving  (event sender/receiver, ...).
 */

#ifndef DDSAPP01_TCPSERVER_HPP_
#define DDSAPP01_TCPSERVER_HPP_

#include <stdint.h>
#include <pthread.h>
#include <vector>
#include "ara/log/logger.h"

using std::vector;

class ddsApp01TcpServer
{
    private:
	/// @brief Litening socket file descriptor
	std::int32_t sockfd;
	/// @brief Accepted socket file descriptor
	std::int32_t accepted;

    protected:

	/// @brief Read TCP stream until find new line.
	/// @param line (out) line that was read.
	/// @return The result of reading line.
	/// true: success, false: fail
	bool ReadLine(std::string& line);

	/// @brief Bind TCP server.
	/// @param port TCP port.
	/// @return The result of binding.
	/// true: success, false: fail
	bool Bind(const std::int16_t port);

	/// @brief Listen connecting TCP client.
	/// @return The result of lietenning.
	/// true: success, false: fail
	bool Listen(void);

	/// @brief Listen accepting TCP client.
	/// @return The result of accepting.
	/// true: success, false: fail
	bool Accept(void);

	/// @brief Disconnect connection to TCP client.
	void Disconnect(void); 


    public: 
        /** Constructor
         * 
         * \return		none
         */
	ddsApp01TcpServer(const std::int16_t port);
        
        /** Destructor
         * 
         * \return		none
         */
	~ddsApp01TcpServer(void);
       
        /** Running TCP Server
        *  
	    *  @param request request message  
        *  \return		boolean
	    *  \retval		TRUE:		successful
	    *  \retval		FALSE:		error occurred
        */
        bool Run(std::string& request);

};


#endif  // DDSAPP01_TCPSERVER_HPP_
