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
     * @brief [brief description]
     * @details [long description]
     *
     * @param pin [description]
     * @param location [description]
     * @param callback [description]
     */
    bool readInput(uint8_t pin, uint16_t location, FunctionPointer1<void, int> callback);

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param pin [description]
     * @param location [description]
     */
    int readOutput(uint8_t pin, uint16_t location);

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param pin [description]
     * @param location [description]
     * @param value [description]
     */
    bool writeOutput(uint8_t pin, uint16_t location, int8_t value);

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param pin [description]
     * @param location [description]
     * @param value [description]
     * @param callback [description]
     */
    bool writeOutput(uint8_t pin, uint16_t location, int8_t value, FunctionPointer0<void> callback);

    /**
     * @brief [brief description]
     * @details [long description]
     *
     * @param pin [description]
     * @param location [description]
     *
     * @return [description]
     */
    bool setDirection(uint8_t pin, uint16_t location, int8_t direction);
}

#endif // __WRD_GPIO_SWITCH_H__
