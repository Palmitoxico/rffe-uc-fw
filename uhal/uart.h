/*
 *   UART driver
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

/**
 * @file uart.h
 * @brief UART driver
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */
 
#ifndef UART_H_
#define UART_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"

/**
 * @brief Uart initialization
 * @param uart_id : Uart number id
 * @param baud : Uart baudrate
 * @return 0 if sucessful, -1 otherwise
 *
 * @note This function configures the UART peripheral and queues to
 * enable concurrent access. Don't call it twice for the same uart_id,
 * otherwise it will leak memory, if you want to change the baud-rate
 * call uart_deinit first.
 */
int uart_init(int uart_id, uint32_t baud);

/**
 * @brief Uart de-initialization
 * @param uart_id : Uart number id
 * @return 0 if sucessful, -1 otherwise
 *
 * @note This function disables the UART and frees the corresponding
 * queue.
 */
int uart_deinit(int uart_id);

/**
 * @brief Write bytes to the UART
 * @param uart_id : Uart number id
 * @param buf : Data buffer
 * @param size : Size of data buffer in bytes
 * @param timeout : Timeout in FreeRTOS ticks
 * @return number of transmited bytes
 *
 * @note This function is not thread-safe.
 */
int32_t uart_write(int uart_id, const uint8_t* buf, int32_t size, TickType_t timeout);

/**
 * @brief Read bytes from the UART
 * @param uart_id : Uart number id
 * @param buf : Data buffer
 * @param size : Number of bytes to read
 * @param timeout : Timeout in FreeRTOS ticks
 * @return number of bytes read
 */
int32_t uart_read(int uart_id, uint8_t* buf, int32_t size, TickType_t timeout);

/**
 * @brief Read a string from the UART until it reaches the specified separator
 * @param uart_id : Uart number id
 * @param str : String pointer
 * @param size : Maximum size of str
 * @param separator : Separator character.
 * @param timeout : Timeout in FreeRTOS ticks
 * @return number of bytes read
 */
int32_t uart_read_until(int uart_id, char* str, char separator, int32_t size, TickType_t timeout);

/**
 * @brief Write a null terminated string to the UART
 * @param uart_id : Uart number id
 * @param str : String pointer
 * @param timeout : Timeout in FreeRTOS ticks
 * @return number of transmited bytes
 *
 * @note This function is not thread-safe.
 */
int32_t uart_print(int uart_id, const char* str, TickType_t timeout);

#endif
