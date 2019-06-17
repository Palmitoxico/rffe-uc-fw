/*
 *   Application entry point
 *
 *   Copyright (C) 2019 Augusto Fraga Giachero <afg@augustofg.net>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

#include "FreeRTOS.h"
#include "task.h"

#include "gpio.h"
#include "uart.h"

#include "GitSHA1.h"

#include <string.h>

TaskHandle_t vTaskBlinky_Handle;

void vTaskBlinky(void *pvParameters)
{
    const TickType_t xDelay = 200 / portTICK_PERIOD_MS;

    while(1)
    {
        gpio_pin_toggle(1, 23);
        uart_print(0, g_GIT_SHA1, portMAX_DELAY);
        uart_print(0, "\r\n", portMAX_DELAY);
        vTaskDelay(xDelay);
    }
}

/**
 * @brief Application entry function
 *
 * @note This function is called before giving control to FreeRTOS, it
 * should be used to create tasks, initialize data structures and
 * periphrerals. WARNNING: Don't call uhal blocking functions here, as
 * it uses FreeRTOS semaphores and queues and FreeRTOS isn't running
 * yet.
 */
void app_config()
{
    /*
     * Initialize UART0
     */
    uart_init(0, 19200);
    xTaskCreate(vTaskBlinky, "Blinky", 120, NULL, tskIDLE_PRIORITY, &vTaskBlinky_Handle);
}
