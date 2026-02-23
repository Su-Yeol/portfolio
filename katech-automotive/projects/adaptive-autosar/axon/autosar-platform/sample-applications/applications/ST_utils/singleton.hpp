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

/**
 * \brief   Template class "singleton"
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup Interface
 */

#ifndef INTERFACES_SINGLETON_H_
#define INTERFACES_SINGLETON_H_

namespace APD
{
namespace INTERFACES
{
/** Template class for singleton pattern */
template <typename C>
class Singleton
{
public:
    /** Getter of the singleton instance
     *
     * \return Instance of the class
     */
    static C* instance()
    {
        if (nullptr == _instance) {
            _instance = new C();
        }

        return _instance;
    }

    /** Destructor of class Singleton */
    virtual ~Singleton()
    {
        _instance = nullptr;
    }

    /** Deleted copy constructor */
    Singleton(Singleton const&) = delete;
    
    /** Deleted move constructor */
    Singleton(Singleton&&) = delete;
    
    /** Deleted copy assignment
     *
     * \return deleted
     */
    Singleton& operator=(Singleton const&) = delete;
    
    /** Deleted move assignment
     *
     * \return deleted
     */
    Singleton& operator=(Singleton&&) = delete;

private:
    /** singleton instance */
    static C* _instance;

protected:
    /** Constructor of class Singleton */
    Singleton()
    {}
};
/** singleton instance */
template <typename C>
C* Singleton<C>::_instance = nullptr;

}  // namespace INTERFACES
}  // namespace APD

#endif /* INTERFACES_SINGLETON_H_ */
