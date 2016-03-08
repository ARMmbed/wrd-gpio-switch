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

#ifndef __WRD_GPIO_DIGITAL_OUT_EX_H__
#define __WRD_GPIO_DIGITAL_OUT_EX_H__

#include "mbed-drivers/mbed.h"
#include "wrd-gpio-switch/GPIOSwitch.h"

#include "core-util/SharedPointer.h"

using namespace mbed::util;

class DigitalOutEx
{
public:

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param pin [description]
     * @param location [description]
     */
    DigitalOutEx(uint32_t pin, uint32_t location = 0);

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param pin [description]
     * @param value [description]
     * @param location [description]
     */
    DigitalOutEx(uint32_t pin, int value, uint32_t location);

    /**
     * @brief Return the output setting, represented as 0 or 1 (int).
     * @details [long description]
     * @return An integer representing the output setting of the pin,
     *         0 for logical 0, 1 for logical 1
     */
    int read();

    /**
     * @brief Set the output, specified as 0 or 1 (int)
     * @details [long description]
     *
     * @param value An integer specifying the pin output value, 0 for logical 0,
     *              1 (or any other non-zero value) for logical 1
     */
    void write(int value);

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param value [description]
     * @param callback [description]
     */
    void write(int value, FunctionPointer0<void> callback);

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param value [description]
     * @param object [description]
     * @param member [description]
     */
    template <typename T>
    void write(int value, T* object, void (T::*member)(void))
    {
        FunctionPointer0<void> fp(object, member);
        write(value, fp);
    }

#ifdef MBED_OPERATORS

    /**
     * @brief A shorthand for read()
     * @details [long description]
     *
     * @param read [description]
     * @return [description]
     */
    operator int()
    {
        return read();
    }

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param value [description]
     */
    DigitalOutEx& operator=(int value)
    {
        write(value);
        return *this;
    }

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param rhs [description]
     */
    DigitalOutEx& operator=(DigitalOutEx& rhs)
    {
        write(rhs.read());
        return *this;
    }

#endif

private:
    uint32_t pin;
    uint32_t location;
    SharedPointer<DigitalOut> local;
};

#endif // __WRD_GPIO_DIGITAL_OUT_EX_H__
