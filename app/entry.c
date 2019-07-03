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
#include "i2c.h"

#include "GitSHA1.h"

#include <string.h>

TaskHandle_t vTaskBlinky_Handle, vTaskI2C_Handle;

void hex_to_str(const uint8_t* buf, char* str, int buf_size)
{
    const char* hex_table = "0123456789ABCDEF";
    int i;

    for(i = 0; i < buf_size; i++)
    {
        str[i*2] = hex_table[(buf[i] >> 4)];
        str[i*2 + 1] = hex_table[(buf[i] & 0x0F)];
    }
    str[i*2] = 0;
}

void vTaskBlinky(void *pvParameters)
{
    const TickType_t xDelay = 200 / portTICK_PERIOD_MS;

    while(1)
    {
        gpio_pin_toggle(1, 23);
        vTaskDelay(xDelay);
    }
}

void vTaskI2C(void *pvParameters)
{
    const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
    i2c_transfer_t i2c_trans;
    uint8_t tx_buf[8], rx_buf[128];
    char str[260];

    /*
     * FeRAM
     */
    tx_buf[0] = 0x01;
    i2c_trans.slave_addr = 0b1010000;
    i2c_trans.tx_buf = tx_buf;
    i2c_trans.tx_size = 1;
    i2c_trans.rx_buf = rx_buf;
    i2c_trans.rx_size = 128;
    i2c_master_transfer(1, &i2c_trans);

    hex_to_str(rx_buf, str, 128);
    uart_print(0, "FeRAM data (0x0000 - 0x007F): ", portMAX_DELAY);
    uart_print(0, str, portMAX_DELAY);
    uart_print(0, "\r\n", portMAX_DELAY);

    /*
     * PLL
     */
    tx_buf[0] = 0x00;
    i2c_trans.slave_addr = 0b1101001;
    i2c_trans.tx_buf = tx_buf;
    i2c_trans.tx_size = 1;
    i2c_trans.rx_buf = rx_buf;
    i2c_trans.rx_size = 1;
    i2c_master_transfer(0, &i2c_trans);

    hex_to_str(rx_buf, str, 1);
    uart_print(0, "PLL address 0x00 content: ", portMAX_DELAY);
    uart_print(0, str, portMAX_DELAY);
    uart_print(0, "\r\n", portMAX_DELAY);

    while(1)
    {
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
    i2c_init(0, 50000);
    i2c_init(1, 50000);
    xTaskCreate(vTaskBlinky, "Blinky", 120, NULL, tskIDLE_PRIORITY, &vTaskBlinky_Handle);
    xTaskCreate(vTaskI2C, "I2C", 512, NULL, tskIDLE_PRIORITY, &vTaskI2C_Handle);
}
