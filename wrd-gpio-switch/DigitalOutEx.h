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
#include "core-util/SharedPointer.h"

using namespace mbed::util;

class DigitalOutEx
{
public:

    /**
     * @brief Create a DigitalOutEx connected to the pin and location.
     * @details The optional location specifies which IC the I/O pin is on.
     *          The main MCU is location 0. For I2C I/O expanders, the location
     *          is the I2C address.
     *
     * @param pin digital pin to connect to.
     * @param location digital pin location.
     */
    DigitalOutEx(uint32_t pin, uint32_t location = 0);

    /**
     * @brief Create a DigitalOutEx connected to the pin and location
     *        with a default value.
     * @details The optional location specifies which IC the I/O pin is on.
     *          The main MCU is location 0. For I2C I/O expanders, the location
     *          is the I2C address.
     *
     * @param pin digital pin to connect to.
     * @param value pin out, 1 high, 0 low.
     * @param location digital pin location.
     */
    DigitalOutEx(uint32_t pin, int value, uint32_t location);

    /**
     * @brief Return the output setting, represented as 0 or 1 (int).
     * @return An integer representing the output setting of the pin,
     *         0 for logical 0, 1 for logical 1
     */
    int read();

    /**
     * @brief Set the output, specified as 0 or 1 (int).
     * @details Commands to external I/O expanders are queued up and processed
     *          sequentially. Use asynchronous API for better timing control.
     *
     * @param value An integer specifying the pin output value, 0 for logical 0,
     *              1 (or any other non-zero value) for logical 1.
     */
    void write(int value);

    /**
     * @brief Set the output, specified as 0 or 1 (int).
     * @details The callback can be used to chain operations.
     *
     * @param value An integer specifying the pin output value, 0 for logical 0,
     *              1 (or any other non-zero value) for logical 1.
     * @param callback The function to call once the new value has been applied.
     */
    void write(int value, FunctionPointer0<void> callback);

    /**
     * @brief Set the output, specified as 0 or 1 (int).
     * @details The callback can be used to chain operations.
     *
     * @param value An integer specifying the pin output value, 0 for logical 0,
     *              1 (or any other non-zero value) for logical 1.
     * @param object Pointer to object to perform callback on.
     * @param member Pointer to member function in object.
     */
    template <typename T>
    void write(int value, T* object, void (T::*member)(void))
    {
        FunctionPointer0<void> fp(object, member);
        write(value, fp);
    }

#ifdef MBED_OPERATORS
    /**
     * @brief A shorthand for read().
     *
     * @return An integer representing the output setting of the pin,
     *         0 for logical 0, 1 for logical 1
     */
    operator int()
    {
        return read();
    }

    /**
     * @brief A shorthand for write().
     */
    DigitalOutEx& operator=(int value)
    {
        write(value);
        return *this;
    }

    /**
     * @brief Assignment operator.
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
