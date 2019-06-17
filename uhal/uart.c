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
 * @file uart.c
 * @brief UART driver
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#include "uart_port.h"
#include "uart.h"

/**
 * @brief Uart queue handles
 *
 * @note The uhal uart driver supports up to 4 different uart
 * peripherals
 */
static QueueHandle_t uart0_rx_queue, uart0_tx_queue,
    uart1_rx_queue, uart1_tx_queue,
    uart2_rx_queue, uart2_tx_queue,
    uart3_rx_queue, uart3_tx_queue;

int uart_init(int uart_id, uint32_t baud)
{
    void* uart_addr = port_uart_get_addr(uart_id);
    if (uart_addr == NULL) return -1;

    port_uart_init(uart_addr);
    port_uart_set_baud(uart_addr, baud);

    switch (uart_id)
    {
    case 0:
        uart0_rx_queue = xQueueCreate(128, 1);
        uart0_tx_queue = xQueueCreate(128, 1);
        break;
    case 1:
        uart1_rx_queue = xQueueCreate(128, 1);
        uart1_tx_queue = xQueueCreate(128, 1);
        break;
    case 2:
        uart2_rx_queue = xQueueCreate(128, 1);
        uart2_tx_queue = xQueueCreate(128, 1);
        break;
    case 3:
        uart3_rx_queue = xQueueCreate(128, 1);
        uart3_tx_queue = xQueueCreate(128, 1);
        break;
    default:
        return -1;
        break;
    }

    port_uart_enable_rx_irq(uart_addr);
    port_uart_enable_irqs(uart_addr);
    return 0;
}

int uart_deinit(int uart_id)
{
    void* uart_addr = port_uart_get_addr(uart_id);
    if (uart_addr == NULL) return -1;

    port_uart_deinit(uart_addr);

    switch (uart_id)
    {
    case 0:
        vQueueDelete(uart0_rx_queue);
        vQueueDelete(uart0_tx_queue);
        break;
    case 1:
        vQueueDelete(uart1_rx_queue);
        vQueueDelete(uart1_tx_queue);
        break;
    case 2:
        vQueueDelete(uart2_rx_queue);
        vQueueDelete(uart2_tx_queue);
        break;
    case 3:
        vQueueDelete(uart3_rx_queue);
        vQueueDelete(uart3_tx_queue);
        break;
    default:
        return -1;
        break;
    }

    return 0;
}

int32_t uart_read(int uart_id, uint8_t* buf, int32_t size, TickType_t timeout)
{
    QueueHandle_t uart_rx_queue;
    switch (uart_id)
    {
    case 0:
        uart_rx_queue = uart0_rx_queue;
        break;
    case 1:
        uart_rx_queue = uart1_rx_queue;
        break;
    case 2:
        uart_rx_queue = uart2_rx_queue;
        break;
    case 3:
        uart_rx_queue = uart3_rx_queue;
        break;
    default:
        return -1;
        break;
    }
    int i;
    for (i = 0; i < size; i++)
    {
        if(xQueueReceive(uart_rx_queue, buf++, timeout) == pdFALSE)
        {
            break;
        }
    }
    return i;
}

int32_t uart_read_until(int uart_id, char* str, char separator, int32_t size, TickType_t timeout)
{
    QueueHandle_t uart_rx_queue;
    switch (uart_id)
    {
    case 0:
        uart_rx_queue = uart0_rx_queue;
        break;
    case 1:
        uart_rx_queue = uart1_rx_queue;
        break;
    case 2:
        uart_rx_queue = uart2_rx_queue;
        break;
    case 3:
        uart_rx_queue = uart3_rx_queue;
        break;
    default:
        return -1;
        break;
    }

    /*
     * Read characters until it reaches the separator character,
     * maximum string size or if the timeout is reached
     */
    int i;
    for (i = 0; i < (size - 1); i++)
    {
        if(xQueueReceive(uart_rx_queue, &str[i], timeout) == pdFALSE)
        {
            break;
        }
        else if(str[i] == separator)
        {
            break;
        }
    }

    /*
     * Null terminates the string
     */
    str[i + 1] = 0;

    return i;
}

static inline int uart_tx_handle_generic(void* uart_addr, QueueHandle_t tx_queue)
{
    uint8_t cbuf;

    /*
     * Transfer the maximum number of bytes available to the UART
     * hardware buffer before overflowing it
     */
    while (port_uart_tx_buf_full(uart_addr) == 0)
    {
        if (xQueueReceiveFromISR(tx_queue, &cbuf, NULL) != pdTRUE)
        {
            break;
        }
        port_uart_write_byte(uart_addr, cbuf);
    }

    /*
     * Disable UART TX interrupts if the tx queue is empty
     */
    if (xQueueIsQueueEmptyFromISR(tx_queue) != pdFALSE)
    {
        port_uart_disable_tx_irq(uart_addr);
        return -1;
    }
    return 0;
}

int32_t uart_write(int uart_id, const uint8_t* buf, int32_t size, TickType_t timeout)
{
    void* uart_addr = port_uart_get_addr(uart_id);
    QueueHandle_t uart_tx_queue;
    switch (uart_id)
    {
    case 0:
        uart_tx_queue = uart0_tx_queue;
        break;
    case 1:
        uart_tx_queue = uart1_tx_queue;
        break;
    case 2:
        uart_tx_queue = uart2_tx_queue;
        break;
    case 3:
        uart_tx_queue = uart3_tx_queue;
        break;
    default:
        return -1;
        break;
    }

    int i;

    for (i = 0; i < size; i++)
    {
        if(xQueueSend(uart_tx_queue, buf++, timeout) != pdTRUE)
        {
            break;
        }
    }

    /*
     * TODO: It may be a good idea to put this under a critical
     * section
     */
    port_uart_disable_tx_irq(uart_addr);
    uart_tx_handle_generic(uart_addr, uart_tx_queue);
    port_uart_enable_tx_irq(uart_addr);

    return i;
}

int32_t uart_print(int uart_id, const char* str, TickType_t timeout)
{
    void* uart_addr = port_uart_get_addr(uart_id);
    QueueHandle_t uart_tx_queue;
    switch (uart_id)
    {
    case 0:
        uart_tx_queue = uart0_tx_queue;
        break;
    case 1:
        uart_tx_queue = uart1_tx_queue;
        break;
    case 2:
        uart_tx_queue = uart2_tx_queue;
        break;
    case 3:
        uart_tx_queue = uart3_tx_queue;
        break;
    default:
        return -1;
        break;
    }

    int i;

    for (i = 0; (*str); i++)
    {
        if(xQueueSend(uart_tx_queue, str++, timeout) != pdTRUE)
        {
            break;
        }
    }

    /*
     * TODO: It may be a good idea to put this under a critical
     * section
     */
    port_uart_disable_tx_irq(uart_addr);
    uart_tx_handle_generic(uart_addr, uart_tx_queue);
    port_uart_enable_tx_irq(uart_addr);

    return i;
}


static inline void uart_rx_handle_generic(void* uart_addr, QueueHandle_t rx_queue)
{
    uint8_t cbuf;

    /*
     * Note: if the uart_rx_queue is full, the received characters
     * will be discarded
     */
    while (port_uart_read_byte(uart_addr, &cbuf) == 0)
    {
        xQueueSendFromISR(rx_queue, &cbuf, NULL);
    }
}

int uart0_tx_irq_handle(void* uart_addr)
{
    return uart_tx_handle_generic(uart_addr, uart0_tx_queue);
}

void uart0_rx_irq_handle(void* uart_addr)
{
    uart_rx_handle_generic(uart_addr, uart0_rx_queue);
}

int uart1_tx_irq_handle(void* uart_addr)
{
    return uart_tx_handle_generic(uart_addr, uart1_tx_queue);
}

void uart1_rx_irq_handle(void* uart_addr)
{
    uart_rx_handle_generic(uart_addr, uart1_rx_queue);
}

int uart2_tx_irq_handle(void* uart_addr)
{
    return uart_tx_handle_generic(uart_addr, uart2_tx_queue);
}

void uart2_rx_irq_handle(void* uart_addr)
{
    uart_rx_handle_generic(uart_addr, uart2_rx_queue);
}

int uart3_tx_irq_handle(void* uart_addr)
{
    return uart_tx_handle_generic(uart_addr, uart3_tx_queue);
}

void uart3_rx_irq_handle(void* uart_addr)
{
    uart_rx_handle_generic(uart_addr, uart3_rx_queue);
}
