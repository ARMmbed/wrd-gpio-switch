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

#include "wrd-gpio-switch/DigitalOutEx.h"
#include "wrd-gpio-switch/GPIOSwitch.h"

DigitalOutEx::DigitalOutEx(uint32_t _pin, uint32_t _location)
    :   pin(_pin),
        location(_location)
{
    if (location)
    {
        GPIOSwitch::writeOutputTask(pin, location, 0);
        GPIOSwitch::setDirection(pin, location, 1);
    }
    else
    {
        local = SharedPointer<DigitalOut>(new DigitalOut((PinName) pin));
    }
}

DigitalOutEx::DigitalOutEx(uint32_t _pin, int value, uint32_t _location)
    :   pin(_pin),
        location(_location)
{
    if (location)
    {
        GPIOSwitch::writeOutputTask(pin, location, value);
        GPIOSwitch::setDirection(pin, location, 1);
    }
    else
    {
        local = SharedPointer<DigitalOut>(new DigitalOut((PinName) pin, value));
    }
}

void DigitalOutEx::write(int value)
{
    if (location)
    {
        GPIOSwitch::writeOutputIrq(pin, location, value);
    }
    else
    {
        local->write(value);
    }
}

void DigitalOutEx::write(int value, FunctionPointer0<void> callback)
{
    if (location)
    {
        GPIOSwitch::writeOutput(pin, location, value, callback);
    }
    else
    {
        local->write(value);
        minar::Scheduler::postCallback(callback)
            .tolerance(1);
    }
}

int DigitalOutEx::read()
{
    if (location)
    {
        return GPIOSwitch::readOutput(pin, location);
    }
    else
    {
        return local->read();
    }
}
