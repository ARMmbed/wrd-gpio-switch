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

#include "wrd-gpio-switch/GPIOSwitch.h"
#include "wrd-gpio-expander/GPIOExpander.h"

#include "core-util/SharedPointer.h"
#include <queue>
#include <map>

using namespace mbed::util;

#if YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_PRESENT
namespace GPIOSwitch
{

#define LOCATION_SIZE YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_SIZE

#if (LOCATION_SIZE == 1)
static uint32_t locationValues[1] = { 0 };
static uint16_t locationAddress[1] = { YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_I2C_ADDRESS };

static GPIOExpander gpio[1] = { { YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_I2C_SDA,
                                  YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_I2C_SCL,
                                  YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_I2C_ADDRESS,
                                  YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_IRQ_PIN} };

#elif (LOCATION_SIZE == 2)
static uint32_t locationValues[2] = { 0, 0 };
static uint16_t locationAddress[2] = { YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_I2C_ADDRESS,
                                       YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO1_I2C_ADDRESS };

static GPIOExpander gpio[2] = { { YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_I2C_SDA,
                                  YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_I2C_SCL,
                                  YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_I2C_ADDRESS,
                                  YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO0_IRQ_PIN},
                                { YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO1_I2C_SDA,
                                  YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO1_I2C_SCL,
                                  YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO1_I2C_ADDRESS,
                                  YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_GPIO1_IRQ_PIN} };
#endif

    typedef enum {
        ACTION_READ,
        ACTION_WRITE,
        ACTION_DIRECTION,
        ACTION_INTERRUPT,
        ACTION_NONE
    } action_t;

    class Transaction
    {
    public:
        Transaction(uint8_t _pin,
                    uint8_t _index,
                    FunctionPointer1<void, int>& _callback)
            :   action(ACTION_READ),
                pin(_pin),
                index(_index),
                read(_callback)
        { }

        Transaction(uint8_t _pin,
                    uint8_t _index,
                    int8_t _parameter,
                    FunctionPointer0<void> _callback)
            :   action(ACTION_WRITE),
                pin(_pin),
                index(_index),
                parameter(_parameter),
                write(_callback)
        { }

        Transaction(action_t _action,
                    uint8_t _pin,
                    uint8_t _index,
                    int8_t _parameter)
            :   action(_action),
                pin(_pin),
                index(_index),
                parameter(_parameter)
        { }

        action_t action;
        uint8_t pin;
        uint8_t index;
        int8_t parameter;

        FunctionPointer1<void, int> read;
        FunctionPointer0<void> write;
    };

    static void enqueueTransaction(SharedPointer<Transaction>);
    static void processQueue(void);
    static void processQueueLander(void);
    static void processQueueDone(uint32_t);
    static std::queue<SharedPointer<Transaction > > sendQueue;
    static minar::callback_handle_t processQueueHandle = NULL;

    /*************************************************************************/

    typedef std::map<uint32_t, FunctionPointer1<void, int> > callbackMap_t;
    typedef callbackMap_t::iterator callbackIterator_t;

    static callbackMap_t interruptMap;

    /*************************************************************************/

    static void enqueueTransaction(SharedPointer<Transaction> transaction)
    {
        sendQueue.push(transaction);

        if (processQueueHandle == NULL)
        {
            processQueueHandle = minar::Scheduler::postCallback(processQueue)
                                    .tolerance(1)
                                    .getHandle();
        }
    }

    static void processQueueLander(void)
    {
        processQueueDone(0);
    }

    static void processQueueDone(uint32_t values)
    {
        SharedPointer<Transaction> transaction = sendQueue.front();
        sendQueue.pop();

        switch (transaction->action)
        {
            case ACTION_READ:
                {
                    if (transaction->read)
                    {
                        int value = (values >> transaction->pin) & 0x01;
                        minar::Scheduler::postCallback(transaction->read.bind(value))
                            .tolerance(1);
                    }
                }
                break;

            case ACTION_WRITE:
                {
                    if (transaction->write)
                    {
                        minar::Scheduler::postCallback(transaction->write)
                            .tolerance(1);
                    }
                }

            case ACTION_DIRECTION:
            case ACTION_INTERRUPT:
            default:
                break;
        }

        if (sendQueue.size() > 0)
        {
            processQueueHandle = minar::Scheduler::postCallback(processQueue)
                                    .tolerance(1)
                                    .getHandle();
        }
        else
        {
            /* clear task handle */
            processQueueHandle = NULL;
        }
    }

    static void processQueue(void)
    {
        // only process if queue is not empty
        if (sendQueue.size() > 0)
        {
            bool result = false;

            /* get next transaction */
            SharedPointer<Transaction> transaction = sendQueue.front();

            switch(transaction->action)
            {
                case ACTION_READ:
                    {
                        result = gpio[transaction->index].bulkRead(processQueueDone);
                    }
                    break;

                case ACTION_WRITE:
                    {
                        result = gpio[transaction->index]
                                    .bulkWrite(((uint32_t)1 << transaction->pin),
                                               ((uint32_t)transaction->parameter << transaction->pin),
                                               processQueueLander);
                    }
                    break;

                case ACTION_DIRECTION:
                    {
                        result = gpio[transaction->index]
                                    .bulkSetDirection(((uint32_t)1 << transaction->pin),
                                                      ((uint32_t)transaction->parameter << transaction->pin),
                                                      processQueueLander);
                    }
                    break;

                case ACTION_INTERRUPT:
                    {
                        result = gpio[transaction->index]
                                    .bulkSetInterrupt(((uint32_t)1 << transaction->pin),
                                                      ((uint32_t)transaction->parameter << transaction->pin),
                                                      processQueueLander);
                    }
                    break;

                default:
                    break;
            }

            /* I/O expander command failed, because the device is busy.
               Repost task into minar to try again later.
            */
            if (result == false)
            {
                processQueueHandle = minar::Scheduler::postCallback(processQueue)
                                        .getHandle();
            }
        }
    }

    static void interruptHandler(uint16_t location, uint32_t pins, uint32_t values)
    {
        for (uint8_t position = 0; position < 32; position++)
        {
            uint8_t pin = (pins >> position) & 0x01;

            if (pin)
            {
                uint32_t index = ((uint32_t)location << 16) | pin;

                callbackIterator_t result = interruptMap.find(index);

                if (result != interruptMap.end())
                {
                    result->second.call((values >> position) & 0x01);
                }
            }
        }
    }

}
#endif // end YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_PRESENT




/*****************************************************************************/
/* Public GPIOSwitch API                                                     */
/*****************************************************************************/

bool GPIOSwitch::readInput(uint8_t pin, uint16_t location, FunctionPointer1<void, int> callback)
{
    bool result = false;

#if YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_PRESENT
    for (uint8_t index = 0; index < LOCATION_SIZE; index++)
    {
        if (location == locationAddress[index])
        {
            SharedPointer<Transaction> transaction(new Transaction(pin, index, callback));

            FunctionPointer1<void, SharedPointer<Transaction> > fp(enqueueTransaction);
            minar::Scheduler::postCallback(fp.bind(transaction))
                .tolerance(1);

            result = true;
            break;
        }
    }
#else
    (void) pin;
    (void) location;
    (void) callback;
#endif

    return result;
}

int GPIOSwitch::readOutput(uint8_t pin, uint16_t location)
{
    int result = -1;

#if YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_PRESENT
    for (uint8_t index = 0; index < LOCATION_SIZE; index++)
    {
        if (location == locationAddress[index])
        {
            result = (locationValues[index] >> pin) & 0x01;
            break;
        }
    }
#else
    (void) pin;
    (void) location;
#endif

    return result;
}

void GPIOSwitch::writeOutputIrq(uint8_t pin, uint16_t location, int8_t value)
{
    FunctionPointer3<void, uint8_t, uint16_t, int8_t> fp(GPIOSwitch::writeOutputTask);
    minar::Scheduler::postCallback(fp.bind(pin, location, value))
        .tolerance(1);
}

void GPIOSwitch::writeOutputTask(uint8_t pin, uint16_t location, int8_t value)
{
#if YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_PRESENT
    for (uint8_t index = 0; index < LOCATION_SIZE; index++)
    {
        if (location == locationAddress[index])
        {
            // construct transaction
            SharedPointer<Transaction> transaction(new Transaction(pin, index, value, (void (*)(void)) NULL));

            FunctionPointer1<void, SharedPointer<Transaction> > fp(enqueueTransaction);
            minar::Scheduler::postCallback(fp.bind(transaction))
                .tolerance(1);

            // update cache
            if (value)
            {
                locationValues[index] |= (uint32_t)value << pin;
            }
            else
            {
                locationValues[index] &= ~((uint32_t)1 << pin);
            }
            break;
        }
    }
#else
    (void) pin;
    (void) location;
    (void) value;
#endif
}

bool GPIOSwitch::writeOutput(uint8_t pin, uint16_t location, int8_t value, FunctionPointer0<void> callback)
{
    bool result = false;

#if YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_PRESENT
    for (uint8_t index = 0; index < LOCATION_SIZE; index++)
    {
        if (location == locationAddress[index])
        {
            // construct transaction
            SharedPointer<Transaction> transaction(new Transaction(pin, index, value, callback));

            FunctionPointer1<void, SharedPointer<Transaction> > fp(enqueueTransaction);
            minar::Scheduler::postCallback(fp.bind(transaction))
                .tolerance(1);

            // update cache
            if (value)
            {
                locationValues[index] |= (uint32_t)value << pin;
            }
            else
            {
                locationValues[index] &= ~((uint32_t)1 << pin);
            }

            result = true;
            break;
        }
    }
#else
    (void) pin;
    (void) location;
    (void) value;
    (void) callback;
#endif

    return result;
}

bool GPIOSwitch::setDirection(uint8_t pin, uint16_t location, int8_t direction)
{
    bool result = false;

#if YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_PRESENT
    for (uint8_t index = 0; index < LOCATION_SIZE; index++)
    {
        if (location == locationAddress[index])
        {
            SharedPointer<Transaction> transaction(new Transaction(ACTION_DIRECTION, pin, index, direction));

            FunctionPointer1<void, SharedPointer<Transaction> > fp(enqueueTransaction);
            minar::Scheduler::postCallback(fp.bind(transaction))
                .tolerance(1);

            result = true;
            break;
        }
    }
#else
    (void) pin;
    (void) location;
    (void) direction;
#endif

    return result;
}



/*****************************************************************************/
/* Public GPIOSwitch API                                                     */
/*****************************************************************************/

bool GPIOSwitch::enableInterrupt(uint8_t pin, uint16_t location, FunctionPointer1<void, int> callback)
{
    bool result = false;

#if YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_PRESENT
    for (uint8_t index = 0; index < LOCATION_SIZE; index++)
    {
        if (location == locationAddress[index])
        {
            // schedule transaction for enabling interrupt
            SharedPointer<Transaction> transaction(new Transaction(ACTION_INTERRUPT, pin, index, 1));

            FunctionPointer1<void, SharedPointer<Transaction> > fp(enqueueTransaction);
            minar::Scheduler::postCallback(fp.bind(transaction))
                .tolerance(1);

            // store external callback function
            uint32_t interruptMapIndex = ((uint32_t)location << 16) | ((uint32_t)0x01 << pin);
            interruptMap[interruptMapIndex] = callback;

            // register local callback function
            gpio[index].setInterruptHandler(GPIOSwitch::interruptHandler);

            result = true;
            break;
        }
    }
#else
    (void) pin;
    (void) location;
    (void) callback;
#endif

    return result;

}

bool GPIOSwitch::disableInterrupt(uint8_t pin, uint16_t location)
{
    bool result = false;

#if YOTTA_CFG_HARDWARE_WRD_GPIO_EXPANDER_PRESENT
    for (uint8_t index = 0; index < LOCATION_SIZE; index++)
    {
        if (location == locationAddress[index])
        {
            SharedPointer<Transaction> transaction(new Transaction(ACTION_INTERRUPT, pin, index, 0));

            FunctionPointer1<void, SharedPointer<Transaction> > fp(enqueueTransaction);
            minar::Scheduler::postCallback(fp.bind(transaction))
                .tolerance(1);

            // remove external callback function
            uint32_t index = ((uint32_t)location << 16) | ((uint32_t)0x01 << pin);
            interruptMap.erase(index);

            result = true;
            break;
        }
    }
#else
    (void) pin;
    (void) location;
#endif

    return result;

}
