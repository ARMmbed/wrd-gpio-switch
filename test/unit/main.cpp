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
#include "wrd-gpio-switch/InterruptInEx.h"

#if YOTTA_CFG_HARDWARE_WRD_LED_PRESENT
#else
#error missing WRD LED configuration
#endif

#if (YOTTA_CFG_HARDWARE_WRD_BUTTON_SIZE > 0) && \
    (YOTTA_CFG_HARDWARE_WRD_LED_SIZE > 0)
#define BUTTON_0_ENABLED
InterruptInEx button0(YOTTA_CFG_HARDWARE_WRD_BUTTON_BUTTON0_PIN,
                      YOTTA_CFG_HARDWARE_WRD_BUTTON_BUTTON0_LOCATION);

DigitalOutEx led0(YOTTA_CFG_HARDWARE_WRD_LED_LED0_PIN,
                  YOTTA_CFG_HARDWARE_WRD_LED_LED0_LOCATION);

static void toggleLed0()
{
    led0 = !led0;
}
#endif

#if (YOTTA_CFG_HARDWARE_WRD_BUTTON_SIZE > 1) && \
    (YOTTA_CFG_HARDWARE_WRD_LED_SIZE > 1)
#define BUTTON_1_ENABLED
InterruptInEx button1(YOTTA_CFG_HARDWARE_WRD_BUTTON_BUTTON1_PIN,
                      YOTTA_CFG_HARDWARE_WRD_BUTTON_BUTTON1_LOCATION);

DigitalOutEx led1(YOTTA_CFG_HARDWARE_WRD_LED_LED1_PIN,
                  YOTTA_CFG_HARDWARE_WRD_LED_LED1_LOCATION);

static void toggleLed1()
{
    led1 = !led1;
}
#endif

/*****************************************************************************/
/* App start                                                                 */
/*****************************************************************************/

void app_start(int, char *[])
{
#ifdef BUTTON_0_ENABLED
    button0.fall(toggleLed0);
#endif

#ifdef BUTTON_1_ENABLED
    button1.fall(toggleLed1);
#endif
}
