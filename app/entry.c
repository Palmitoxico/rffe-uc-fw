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
#include "feram.h"
#include "cdce906.h"

#include "pin_mapping.h"
#include "GitSHA1.h"

#include <stdio.h>

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
        gpio_pin_toggle(led4_io.port, led4_io.pin);
        vTaskDelay(xDelay);
    }
}

void vTaskI2C(void *pvParameters)
{
    const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
    char str[260];
    float att;

    uint8_t mac[6], ip[4];
    feram_get_mac_addr(mac);
    hex_to_str(mac, str, 6);
    snprintf(str, 128, "Mac address: %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    uart_print(0, str, portMAX_DELAY);

    feram_get_ipv4_addr(ip);
    snprintf(str, 128, "IP address (static): %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
    uart_print(0, str, portMAX_DELAY);

    feram_get_gateway_addr(ip);
    snprintf(str, 128, "Gateway address: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
    uart_print(0, str, portMAX_DELAY);

    feram_get_mask_addr(ip);
    snprintf(str, 128, "Mask address: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
    uart_print(0, str, portMAX_DELAY);

    feram_get_attenuation(&att);
    snprintf(str, 128, "RF Attenuation: %f\r\n", att);
    uart_print(0, str, portMAX_DELAY);

    eth_addr_mode_t addr_mode;
    feram_get_eth_addressing(&addr_mode);
    switch (addr_mode)
    {
    case ETH_ADDR_MODE_STATIC:
        uart_print(0, "Network ip config: static\r\n", portMAX_DELAY);
        break;
    case ETH_ADDR_MODE_DHCP:
        uart_print(0, "Network ip config: dhcp\r\n", portMAX_DELAY);
        break;
    default:
        uart_print(0, "Network ip config: not configured\r\n", portMAX_DELAY);
        break;
    }

    if (cdce906_pll_init() == 0)
    {
        uart_print(0, "Ethernet PLL configured\r\n", portMAX_DELAY);
    }
    else
    {
        uart_print(0, "Fail to configure the ethernet PLL\r\n", portMAX_DELAY);
    }

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
     * Initialize UART0, I2C0 and I2C1
     */
    uart_init(0, 19200);
    i2c_init(0, 50000);
    i2c_init(1, 50000);

    /*
     * Put the FeRAM in read-only mode
     */
    gpio_set_pin_high(feram_wp_io.port, feram_wp_io.pin);

    /*
     * Create tasks
     */
    xTaskCreate(vTaskBlinky, "Blinky", 120, NULL, tskIDLE_PRIORITY, &vTaskBlinky_Handle);
    xTaskCreate(vTaskI2C, "I2C", 512, NULL, tskIDLE_PRIORITY, &vTaskI2C_Handle);
}
