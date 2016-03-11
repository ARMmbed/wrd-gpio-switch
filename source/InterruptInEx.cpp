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

#include "wrd-gpio-switch/InterruptInEx.h"
#include "wrd-gpio-switch/GPIOSwitch.h"

InterruptInEx::InterruptInEx(uint32_t _pin, uint32_t _location)
    :   pin(_pin),
        location(_location),
        value(0)
{
    if ((location == 0) && ((PinName) pin != NC))
    {
        local = SharedPointer<InterruptIn>(new InterruptIn((PinName) pin));
    }
}

int InterruptInEx::read()
{
    if (location)
    {
        return value;
    }
    else
    {
        return local->read();
    }
}

void InterruptInEx::internalRise(FunctionPointer0<void> callback)
{
    // location always non-zero

    // enable interrupts if not previously enabled
    if ((riseCallback == false) && (fallCallback == false))
    {
        FunctionPointer1<void, int> fp(this, &InterruptInEx::interruptHandler);
        GPIOSwitch::enableInterrupt(pin, location, fp);
    }

    // store callback
    riseCallback = callback;

    // disable interrupts if no longer enabled
    if ((riseCallback == false) && (fallCallback == false))
    {
        GPIOSwitch::disableInterrupt(pin, location);
    }
}

void InterruptInEx::internalFall(FunctionPointer0<void> callback)
{
    // location always non-zero

    // enable interrupts if not previously enabled
    if ((riseCallback == false) && (fallCallback == false))
    {
        FunctionPointer1<void, int> fp(this, &InterruptInEx::interruptHandler);
        GPIOSwitch::enableInterrupt(pin, location, fp);
    }

    // store callback
    fallCallback = callback;

    // disable interrupts if no longer enabled
    if ((riseCallback == false) && (fallCallback == false))
    {
        GPIOSwitch::disableInterrupt(pin, location);
    }
}

void InterruptInEx::mode(PinMode pull)
{
    if (location)
    {
        // not supported
    }
    else
    {
        local->mode(pull);
    }
}

void InterruptInEx::enable_irq()
{
    if (location)
    {
        FunctionPointer1<void, int> fp(this, &InterruptInEx::interruptHandler);
        GPIOSwitch::enableInterrupt(pin, location, fp);
    }
    else
    {
        local->enable_irq();
    }
}

void InterruptInEx::disable_irq()
{
    if (location)
    {
        GPIOSwitch::disableInterrupt(pin, location);
    }
    else
    {
        local->disable_irq();
    }
}

void InterruptInEx::interruptHandler(int _value)
{
    value = _value;

    if (value == 0)
    {
        if (fallCallback)
        {
            fallCallback.call();
        }
    }
    else
    {
        if (riseCallback)
        {
            riseCallback.call();
        }
    }
}
