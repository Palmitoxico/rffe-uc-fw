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

#include "chip.h"
#include "uart_17xx_40xx.h"
#include "gpio.h"

#include "GitSHA1.h"

#include <string.h>

TaskHandle_t vTaskBlinky_Handle;

void vTaskBlinky(void *pvParameters)
{
    const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;

    while(1)
    {
        gpio_pin_toggle(1, 23);
		Chip_UART_SendBlocking(LPC_UART0, g_GIT_SHA1, strlen(g_GIT_SHA1));
		Chip_UART_SendBlocking(LPC_UART0, "\r\n", sizeof("\r\n"));
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
    Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_UART0, SYSCTL_CLKDIV_2);

    Chip_UART_Init(LPC_UART0);
    Chip_UART_SetBaud(LPC_UART0, 19200);
    Chip_UART_ConfigData(LPC_UART0, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
    Chip_UART_TXEnable(LPC_UART0);

    xTaskCreate(vTaskBlinky, "Blinky", 120, NULL, tskIDLE_PRIORITY, &vTaskBlinky_Handle);
}
