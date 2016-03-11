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

#ifndef __WRD_GPIO_DIGITAL_IN_EX_H__
#define __WRD_GPIO_DIGITAL_IN_EX_H__

#include "mbed-drivers/mbed.h"
#include "core-util/SharedPointer.h"

using namespace mbed::util;

class DigitalInEx
{
public:
    /**
     * @brief Create a DigitalInEx connected to the specified pin.
     * @details The optional location specifies which IC the I/O pin is on.
     *          The main MCU is location 0. For I2C I/O expanders, the location
     *          is the I2C address.
     *
     * @param pin digital pin to connect to
     * @param location digital pin location
     * @param mode the initial mode of the pin
     */
    DigitalInEx(uint32_t pin, uint32_t location = 0);

    /**
     * @brief Create a DigitalInEx connected to the specified pin.
     * @details The optional location specifies which IC the I/O pin is on.
     *          The main MCU is location 0. For I2C I/O expanders, the location
     *          is the I2C address.
     *
     * @param pin digital pin to connect to
     * @param location digital pin location
     * @param mode the initial mode of the pin
     */
    DigitalInEx(uint32_t pin, uint32_t location, PinMode mode);

    /**
     * @brief Read the input, represented as 0 or 1 (int)
     * @details For inputs on external I/O devices, a cached value is returned
     *          immediately and a new value is fetched into the cache.
     *          When possible, the asynchronous API should be used.
     *
     * @return An integer representing the state of the input pin,
     *         0 for logical 0, 1 for logical 1
     */
    int read(void);

    /**
     * @brief Read the input, represented as 0 or 1 (int)
     * @details The value is returned in the callback function.
     *          This function call reads the newest value from external I/O
     *          devices.
     *
     * @param callback [description]
     */
    void read(FunctionPointer1<void, int> callback);

    /**
     * @brief Set the input pin mode.
     * @param pull PullUp, PullDown, PullNone, OpenDrain
     */
    void mode(PinMode pull);

#ifdef MBED_OPERATORS
    /**
     * @brief And operator shorthand for read()
     * @return An integer representing the state of the input pin,
     *         0 for logical 0, 1 for logical 1
     */
    operator int()
    {
        return read();
    }
#endif

private:
    void updateCache(int);

    uint32_t pin;
    uint32_t location;
    int cache;

    SharedPointer<DigitalIn> local;
};

#endif // __WRD_GPIO_DIGITAL_IN_EX_H__
