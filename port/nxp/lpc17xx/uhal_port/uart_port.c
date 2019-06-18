/*
 *   Uart abstraction layer for lpc17xx
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
 * @file uart_port.c
 * @brief Uart abstraction layer for lpc17xx
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#include "chip.h"

#include "uart_port.h"

typedef struct lpc_uart_cfg {
    LPC_USART_T * ptr;
    IRQn_Type irq;
    CHIP_SYSCTL_PCLK_T sysclk;
} lpc_uart_cfg_t;

static lpc_uart_cfg_t port_usart_cfg[4] = {
    {LPC_UART0, UART0_IRQn, SYSCTL_PCLK_UART0},
    {LPC_UART1, UART1_IRQn, SYSCTL_PCLK_UART1},
    {LPC_UART2, UART2_IRQn, SYSCTL_PCLK_UART2},
    {LPC_UART3, UART3_IRQn, SYSCTL_PCLK_UART3}
};

void* port_uart_table[] =
{
    &port_usart_cfg[0],
    &port_usart_cfg[1],
    &port_usart_cfg[2],
    &port_usart_cfg[3],
};

const unsigned int port_uart_table_len = sizeof(port_uart_table) / sizeof(void*);

void port_uart_init(void* uart_addr)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    Chip_Clock_SetPCLKDiv(uart_cfg->sysclk, SYSCTL_CLKDIV_2);
    Chip_UART_Init(uart_cfg->ptr);
    Chip_UART_ConfigData(uart_cfg->ptr, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
    Chip_UART_TXEnable(uart_cfg->ptr);
}

void port_uart_deinit(void* uart_addr)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    Chip_UART_IntDisable(uart_cfg->ptr, UART_IER_THREINT);
    Chip_UART_IntDisable(uart_cfg->ptr, UART_IER_RBRINT);
    NVIC_DisableIRQ(uart_cfg->irq);
    Chip_UART_TXDisable(uart_cfg->ptr);
    Chip_UART_DeInit(uart_cfg->ptr);
}

void port_uart_set_baud(void* uart_addr, uint32_t baud)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    Chip_UART_SetBaud(uart_cfg->ptr, baud);
}

int port_uart_tx_buf_full(void* uart_addr)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    return ((Chip_UART_ReadLineStatus(uart_cfg->ptr) & UART_LSR_THRE) == 0);
}

int port_uart_write_byte(void* uart_addr, uint8_t cbuf)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    if ((Chip_UART_ReadLineStatus(uart_cfg->ptr) & UART_LSR_THRE) != 0)
    {
        Chip_UART_SendByte(uart_cfg->ptr, cbuf);
	}
    else
    {
        return -1;
    }
    return 0;
}

int port_uart_read_byte(void* uart_addr, uint8_t* cbuf)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    if (Chip_UART_ReadLineStatus(uart_cfg->ptr) & UART_LSR_RDR)
    {
		*cbuf = Chip_UART_ReadByte(uart_cfg->ptr);
	}
    else
    {
        return -1;
    }
    return 0;
}

void port_uart_enable_irqs(void* uart_addr)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    NVIC_SetPriority(uart_cfg->irq, 5 << 3);
    NVIC_EnableIRQ(uart_cfg->irq);
}

void port_uart_disable_irqs(void* uart_addr)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    NVIC_SetPriority(uart_cfg->irq, 5 << 3);
    NVIC_DisableIRQ(uart_cfg->irq);
}

void port_uart_disable_tx_irq(void* uart_addr)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    Chip_UART_IntDisable(uart_cfg->ptr, UART_IER_THREINT);
}

void port_uart_enable_tx_irq(void* uart_addr)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    Chip_UART_IntEnable(uart_cfg->ptr, UART_IER_THREINT);
}

void port_uart_disable_rx_irq(void* uart_addr)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    Chip_UART_IntDisable(uart_cfg->ptr, UART_IER_RBRINT);
}

void port_uart_enable_rx_irq(void* uart_addr)
{
    lpc_uart_cfg_t* uart_cfg = uart_addr;
    Chip_UART_IntEnable(uart_cfg->ptr, UART_IER_RBRINT);
}

void UART0_IRQHandler()
{
    /*
     * Check if the UART tx interrupt is enabled
     */
    if (LPC_UART0->IER & UART_IER_THREINT)
    {
        uart0_tx_irq_handle(&port_usart_cfg[0]);
    }

    uart0_rx_irq_handle(&port_usart_cfg[0]);
}

void UART1_IRQHandler()
{
    /*
     * Check if the UART tx interrupt is enabled
     */
    if (LPC_UART1->IER & UART_IER_THREINT)
    {
        uart1_tx_irq_handle(&port_usart_cfg[0]);
    }

    uart1_rx_irq_handle(&port_usart_cfg[0]);
}

void UART2_IRQHandler()
{
    /*
     * Check if the UART tx interrupt is enabled
     */
    if (LPC_UART2->IER & UART_IER_THREINT)
    {
        uart2_tx_irq_handle(&port_usart_cfg[0]);
    }

    uart2_rx_irq_handle(&port_usart_cfg[0]);
}

void UART3_IRQHandler()
{
    /*
     * Check if the UART tx interrupt is enabled
     */
    if (LPC_UART3->IER & UART_IER_THREINT)
    {
        uart3_tx_irq_handle(&port_usart_cfg[0]);
    }

    uart3_rx_irq_handle(&port_usart_cfg[0]);
}
