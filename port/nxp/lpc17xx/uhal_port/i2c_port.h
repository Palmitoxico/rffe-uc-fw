/*
 *   I2C abstraction layer for lpc17xx (headers)
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
 * @file i2c_port.h
 * @brief I2C abstraction layer for lpc17xx (headers)
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#ifndef I2C_PORT_H_
#define I2C_PORT_H_

#include <stdint.h>

#include "chip.h"

enum i2c_port_trans_result
{
    I2C_TRANS_SUCCESS,
    I2C_TRANS_NAK,
    I2C_TRANS_ERROR,
};

/**
 * @brief Initialize the UART peripheral
 * @param i2c_id : I2C peripheral number
 * @return Nothing
 *
 * @note Enables the I2C peripheral. Note: this function doesn't
 * configure the pin multiplexer, it should be done by the application
 * when defining the pin_init_vec array.
 */
void port_i2c_init(int i2c_id);

/**
 * @brief De-initialize the I2C peripheral
 * @param i2c_id : I2C peripheral number
 * @return Nothing
 *
 * @note Turn off all interrupts and disable the I2C peripheral.
 */
void port_i2c_deinit(int i2c_id);

/**
 * @brief Configure the I2C clock rate
 * @param i2c_id : I2C peripheral number
 * @return Nothing
 *
 * @note The maximum allowed clock for I2C1 and I2C2 is 400kHz.
 */
static inline void port_i2c_set_clock(int i2c_id, uint32_t clock)
{
    /*
     * TODO: Investigate why clock rates above 50kHz makes the I2C
     * periperhal misbehave. It may have something to do with the PCLK
     * not been high enough.
     */
    Chip_I2C_SetClockRate(i2c_id, clock);
}

/**
 * @brief Enables the I2C interrupt
 * @param i2c_id : I2C peripheral number
 * @param irq_callback : Callback function pointer
 * @return Nothing
 *
 * @note The callback function will be called when the transmission is
 * finished and the I2C peripheral is idle.
 */
void port_i2c_enable_irq(int i2c_id, void (*irq_callback)(enum i2c_port_trans_result));

/**
 * @brief Disables the I2C interrupts
 * @param i2c_id : I2C peripheral number
 * @return Nothing
 *
 * @note This function disables the I2C interrupt temporarily, and
 * these can be restored through @ref port_i2c_enable_irq.
 */
void port_i2c_disable_irq(int i2c_id);

/**
 * @brief Checks if the I2C peripheral is idle
 * @param i2c_id : I2C peripheral number
 * @return 1 if the I2C peripheral is idle, 0 otherwise.
 */
int port_i2c_is_idle(int i2c_id);

/**
 * @brief Start a I2C transfer
 * @param i2c_id : I2C peripheral number
 * @param slave_addr : Slave address (bits 0 to 6)
 * @param tx_buf : Pointer to the transmit buffer
 * @param tx_size : Number of bytes to transmit
 * @param rx_buf : Pointer to the receive buffer
 * @param rx_size : Number of bytes to receive
 * @return Nothing
 *
 * @note This function should only be called when the I2C peripheral
 * is idle (check @ref port_i2c_is_idle) and is not thread safe. This
 * function is non-blocking and will just reset the I2C state machine
 * and start a new transaction. Before calling this function, you
 * should enable the I2C interrupt (@ref port_i2c_enable_irq).
 */
void port_i2c_start_transfer(int i2c_id, uint8_t slave_addr,
                            uint8_t* tx_buf, uint32_t tx_size,
                            uint8_t* rx_buf, uint32_t rx_size);

#endif
