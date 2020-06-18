/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include "fpioa.h"
#include "gpiohs.h"
#include "sysctl.h"

int irq_flag = 1;

#define PIN_LED 12
#define PIN_KEY 16

#define GPIO_LED 3
#define GPIO_KEY 2

void irq_gpiohs2(void *gp)
{
    irq_flag = gpiohs_get_pin(GPIO_KEY);

    printf("IRQ The PIN is %d\n", irq_flag);

    if (irq_flag)
        gpiohs_set_pin(GPIO_LED, GPIO_PV_LOW);
    else
        gpiohs_set_pin(GPIO_LED, GPIO_PV_HIGH);
}

int main(void)
{
    plic_init();
    sysctl_enable_irq();

    fpioa_set_function(PIN_LED, FUNC_GPIOHS3);
    gpiohs_set_drive_mode(GPIO_LED, GPIO_DM_OUTPUT);
    gpio_pin_value_t value = GPIO_PV_HIGH;
    gpiohs_set_pin(GPIO_LED, value);

    fpioa_set_function(PIN_KEY, FUNC_GPIOHS2);
    gpiohs_set_drive_mode(GPIO_KEY, GPIO_DM_INPUT);
    gpiohs_set_pin_edge(GPIO_KEY, GPIO_PE_BOTH);
    gpiohs_set_irq(GPIO_KEY, 1, irq_gpiohs2);

    while (1)
    {
        sleep(1);
        if (irq_flag)
            gpiohs_set_pin(GPIO_LED, value = !value);
        int val = gpiohs_get_pin(GPIO_KEY);
        printf("The PIN is %d\n", val);
    }
    return 0;
}
