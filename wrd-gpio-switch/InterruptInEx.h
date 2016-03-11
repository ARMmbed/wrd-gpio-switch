/*
 * Copyright (c) 2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WRD_GPIO_INTERRUPT_IN_EX_H__
#define __WRD_GPIO_INTERRUPT_IN_EX_H__

#include "mbed-drivers/mbed.h"
#include "core-util/SharedPointer.h"

using namespace mbed::util;

class InterruptInEx
{
public:
    InterruptInEx(uint32_t pin, uint32_t location = 0);

    /**
     * @brief Read pin value.
     * @return Pin value, high 1, low 0
     */
    int read();
#ifdef MBED_OPERATORS
    operator int()
    {
        return read();
    }
#endif

    /**
     * @brief Attach a function to call when a rising edge occurs on the input
     *
     * @param callback A pointer to a void function, or 0 to set as none
     */
    void rise(void (*callback)(void))
    {
        if (location)
        {
            FunctionPointer0<void> fp(callback);
            internalRise(callback);
        }
        else
        {
            local->rise(callback);
        }
    }

    /**
     * @brief Attach a member function to call when a rising edge occurs on the input
     *
     * @param object pointer to the object to call the member function on
     * @param member pointer to the member function to be called
     */
    template<typename T>
    void rise(T* object, void (T::*member)(void))
    {
        if (location)
        {
            FunctionPointer0<void> fp(object, member);
            internalRise(fp);
        }
        else
        {
            local->rise(object, member);
        }
    }

    /**
     * @brief Attach a function to call when a falling edge occurs on the input
     *
     * @param callback A pointer to a void function, or 0 to set as none
     */
    void fall(void (*callback)(void))
    {
        if (location)
        {
            FunctionPointer0<void> fp(callback);
            internalFall(fp);
        }
        else
        {
            local->fall(callback);
        }
    }

    /**
     * @brief Attach a member function to call when a falling edge occurs on the input
     *
     * @param object pointer to the object to call the member function on
     * @param member pointer to the member function to be called
     */
    template<typename T>
    void fall(T* object, void (T::*member)(void))
    {
        if (location)
        {
            FunctionPointer0<void> fp(object, member);
            internalFall(fp);
        }
        else
        {
            local->fall(object, member);
        }
    }

    /**
     * @brief Enable pullup or pulldown.
     *
     * @param pull mode PullUp, PullDown, PullNone
     */
    void mode(PinMode pull);

    /**
     * @brief Enable IRQ.
     * @details This method depends on hw implementation, might enable one
     *          port interrupts.
     */
    void enable_irq();

    /**
     * @brief Disable IRQ.
     * @details This method depends on hw implementation, might disable one
     *          port interrupts.
     */
    void disable_irq();


private:
    void internalRise(FunctionPointer0<void>);
    void internalFall(FunctionPointer0<void>);

    void interruptHandler(int);

    uint32_t pin;
    uint32_t location;
    SharedPointer<InterruptIn> local;
    int value;
    FunctionPointer0<void> riseCallback;
    FunctionPointer0<void> fallCallback;
};

#endif // __WRD_GPIO_INTERRUPT_IN_EX_H__
