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

#ifndef __WRD_GPIO_SWITCH_H__
#define __WRD_GPIO_SWITCH_H__

#include "mbed-drivers/mbed.h"

using namespace mbed::util;

namespace GPIOSwitch
{
    /**
     * @brief Read pin value from external I/O device.
     * @details All calls are processed through a FIFO queue.
     *
     * @param pin Zero indexed pin number.
     * @param location Address for external I/O device. For I2C devices,
     *                 the I2C address is the location.
     * @param callback Function to call with pin value as parameter.
     * @return bool true command accepted by location, false location not found.
     */
    bool readInput(uint8_t pin, uint16_t location, FunctionPointer1<void, int> callback);

    /**
     * @brief Read output value for a pin on an external I/O device.
     * @details A cached value is returned and not read from the device for optimization.
     *
     * @param pin Zero indexed pin number.
     * @param location Address for external I/O device. For I2C devices,
     *                 the I2C address is the location.
     * @return pin value. 0 is low, 1 is high.
     */
    int readOutput(uint8_t pin, uint16_t location);

    /**
     * @brief Set output pin value. This function is safe to call from interrupt context.
     * @details Calls writeOutputTask through MINAR to decouple from interrupt context.
     *
     * @param pin Zero indexed pin number.
     * @param location Address for external I/O device. For I2C devices,
     *                 the I2C address is the location.
     * @param value pin value. 0 is low, 1 is high.
     */
    void writeOutputIrq(uint8_t pin, uint16_t location, int8_t value);

    /**
     * @brief Set output pin value.
     * @details All calls are processed through a FIFO queue.
     *
     * @param pin Zero indexed pin number.
     * @param location Address for external I/O device. For I2C devices,
     *                 the I2C address is the location.
     * @param value pin value. 0 is low, 1 is high.
     */
    void writeOutputTask(uint8_t pin, uint16_t location, int8_t value);

    /**
     * @brief Set output pin value.
     * @details All calls are processed through a FIFO queue. A callback function
     *          is called when the command has been processed.
     *
     * @param pin Zero indexed pin number.
     * @param location Address for external I/O device. For I2C devices,
     *                 the I2C address is the location.
     * @param value pin value. 0 is low, 1 is high.
     * @param callback Function to be called when command has been processed.
     * @return bool true command accepted by location, false location not found.
     */
    bool writeOutput(uint8_t pin, uint16_t location, int8_t value, FunctionPointer0<void> callback);

    /**
     * @brief Set pin direction.
     * @details All calls are processed through a FIFO queue.
     *
     * @param pin Zero indexed pin number.
     * @param location Address for external I/O device. For I2C devices,
     *                 the I2C address is the location.
     * @param direction 0 is input, 1 is output.
     *
     * @return bool true command accepted by location, false location not found.
     */
    bool setDirection(uint8_t pin, uint16_t location, int8_t direction);

    /**
     * @brief Enable interrupts on pin at location.
     * @details This call sets the pin direction to input.
     *
     * @param pin Zero indexed pin number.
     * @param location Address for external I/O device. For I2C devices,
     *                 the I2C address is the location.
     * @param callback Function to be called with pin value upon change.
     * @return bool true command accepted by location, false location not found.
     */
    bool enableInterrupt(uint8_t pin, uint16_t location, FunctionPointer1<void, int> callback);

    /**
     * @brief Disable interrupts on pin at location.
     *
     * @param pin Zero indexed pin number.
     * @param location Address for external I/O device. For I2C devices,
     *                 the I2C address is the location.
     *
     * @return bool true command accepted by location, false location not found.
     */
    bool disableInterrupt(uint8_t pin, uint16_t location);
}

#endif // __WRD_GPIO_SWITCH_H__
