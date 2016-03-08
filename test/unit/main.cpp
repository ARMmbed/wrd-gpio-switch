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

#include "mbed-drivers/mbed.h"

#include "wrd-gpio-switch/DigitalOutEx.h"

#if YOTTA_CFG_HARDWARE_WRD_LED_PRESENT
#else
#error missing WRD LED configuration
#endif

DigitalOutEx led(YOTTA_CFG_HARDWARE_WRD_LED_LED0_PIN,
                 YOTTA_CFG_HARDWARE_WRD_LED_LED0_LOCATION);

void toggleLed()
{
    led = !led;
}

/*****************************************************************************/
/* App start                                                                 */
/*****************************************************************************/

void app_start(int, char *[])
{
    minar::Scheduler::postCallback(toggleLed)
        .period(minar::milliseconds(1000));
}
