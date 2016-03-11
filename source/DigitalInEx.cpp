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

#include "wrd-gpio-switch/DigitalInEx.h"
#include "wrd-gpio-switch/GPIOSwitch.h"

DigitalInEx::DigitalInEx(uint32_t _pin, uint32_t _location)
    :   pin(_pin),
        location(_location),
        cache(0),
        local(NULL)
{
    if (location)
    {
        GPIOSwitch::setDirection(pin, location, 0);
    }
    else
    {
        local = SharedPointer<DigitalIn>(new DigitalIn((PinName) pin));
    }
}

DigitalInEx::DigitalInEx(uint32_t _pin, uint32_t _location, PinMode mode)
    :   pin(_pin),
        location(_location),
        cache(0),
        local(NULL)
{
    if (location)
    {
        GPIOSwitch::setDirection(pin, location, 0);
    }
    else
    {
        local = SharedPointer<DigitalIn>(new DigitalIn((PinName) pin, mode));
    }
}

int DigitalInEx::read(void)
{
    if (location)
    {
        // schedule cache to be updated
        FunctionPointer1<void, int> fp(this, &DigitalInEx::updateCache);
        GPIOSwitch::readInput(pin, location, fp);

        return cache;
    }
    else
    {
        return local->read();
    }
}

void DigitalInEx::read(FunctionPointer1<void, int> callback)
{
    if (location)
    {
        GPIOSwitch::readInput(pin, location, callback);
    }
    else
    {
        minar::Scheduler::postCallback(callback.bind(local->read()))
            .tolerance(1);
    }
}

void DigitalInEx::mode(PinMode pull)
{
    if (location)
    {
        // not implemented
    }
    else
    {
        local->mode(pull);
    }
}

void DigitalInEx::updateCache(int value)
{
    cache = value;
}
