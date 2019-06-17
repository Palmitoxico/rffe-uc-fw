/*
 *   Uart abstraction layer for lpc17xx (headers)
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
 * @file uart_port.h
 * @brief Uart abstraction layer for lpc17xx
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#ifndef UART_PORT_H_
#define UART_PORT_H_

#include <stdint.h>
#include <stddef.h>

extern void* port_uart_table[];
extern const unsigned int port_uart_table_len;

/**
 * @brief Get the UART
 * @param uart_id : UART peripheral id number
 * @return A pointer to the UART peripheral if sucessful or NULL if the id is out of range.
 */
static inline void* port_uart_get_addr(unsigned int uart_id)
{
    if(uart_id < port_uart_table_len)
        return port_uart_table[uart_id];
    else
        return NULL;
}

/**
 * @brief Initialize the UART peripheral
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return Nothing
 *
 * @note Enables the UART peripheral and configure its clocks
 * signals. Note: this function doesn't configure the pin multiplexer,
 * it should be done by the application when defining the pin_init_vec
 * array.
 */
void port_uart_init(void* uart_addr);

/**
 * @brief De-initialize the UART peripheral
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return Nothing
 *
 * @note Turn off all interrupts and disable the UART peripheral
 */
void port_uart_deinit(void* uart_addr);

/**
 * @brief Set the baudrate for the UART peripheral
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return Nothing
 *
 * @note The accuracy of the real baudrate will depend on the baudrate
 * selected and the UART base clock.
 */
void port_uart_set_baud(void* uart_addr, uint32_t baud);

int port_uart_tx_buf_full(void* uart_addr);

/**
 * @brief Write a byte to the UART TX buffer
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return 0 if sucessful, -1 if the buffer is full
 */
int port_uart_write_byte(void* uart_addr, uint8_t cbuf);

/**
 * @brief Read a byte from the UART RX buffer
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return 0 if sucessful, -1 if the buffer is empty
 */
int port_uart_read_byte(void* uart_addr, uint8_t* cbuf);

/**
 * @brief Disables the UART interrupts
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return Nothing
 *
 * @note This function disables all UART interrupts temporarily, and
 * these can be restored through @ref port_uart_enable_irqs.
 */
void port_uart_disable_irqs(void* uart_addr);

/**
 * @brief Enables the UART interrupts
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return Nothing
 */
void port_uart_enable_irqs(void* uart_addr);

/**
 * @brief Disables the UART TX interrupt
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return Nothing
 */
void port_uart_disable_tx_irq(void* uart_addr);

/**
 * @brief Enables the UART TX interrupt
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return Nothing
 */
void port_uart_enable_tx_irq(void* uart_addr);

/**
 * @brief Disables the UART RX interrupt
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return Nothing
 */
void port_uart_disable_rx_irq(void* uart_addr);

/**
 * @brief Enables the UART RX interrupt
 * @param uart_addr : UART peripheral pointer (obtained through @ref port_uart_get_addr)
 * @return Nothing
 */
void port_uart_enable_rx_irq(void* uart_addr);

/*
 * Interrupt call-backs
 */
int uart0_tx_irq_handle(void* uart_addr);
void uart0_rx_irq_handle(void* uart_addr);
int uart1_tx_irq_handle(void* uart_addr);
void uart1_rx_irq_handle(void* uart_addr);
int uart2_tx_irq_handle(void* uart_addr);
void uart2_rx_irq_handle(void* uart_addr);
int uart3_tx_irq_handle(void* uart_addr);
void uart3_rx_irq_handle(void* uart_addr);

#endif
