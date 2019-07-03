/*
 *   I2C master driver
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
 * @file i2c.h
 * @brief I2C master driver
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

enum i2c_transfer_status
{
    I2C_TRANSFER_PENDING,
    I2C_TRANSFER_COMPLETE,
    I2C_TRANSFER_NAK_RECEIVED,
    I2C_TRANSFER_ERROR,
};

typedef struct
{
    uint8_t slave_addr;
    uint8_t* tx_buf;
    uint32_t tx_size;
    uint8_t* rx_buf;
    uint32_t rx_size;
    TaskHandle_t task;
    enum i2c_transfer_status status;
} i2c_transfer_t;

/**
 * @brief I2C initialization
 * @param i2c_id : I2C number id
 * @param clock : I2C clock
 * @return 0 if sucessful, -1 otherwise
 *
 * @note This function configures the I2C peripheral and queues to
 * enable concurrent access. Don't call it twice for the same i2c_id,
 * otherwise it will leak memory, if you want to change the clock
 * call i2c_deinit first.
 */
int i2c_init(int i2c_id, uint32_t clock);

/**
 * @brief I2C de-initialization
 * @param i2c_id : I2C number id
 * @return 0 if sucessful, -1 otherwise
 *
 * @note This function disables the I2C peripheral and frees the
 * corresponding queue. This function should only be called when the
 * I2C peripheral is idle.
 */
int i2c_deinit(int i2c_id);

/**
 * @brief I2C master transfer
 * @param i2c_id : I2C number id
 * @param trans : I2C transfer data structure
 * @return 0 if sucessful, -1 otherwise
 *
 * @note This function tries to execute an I2C transfer. It will block
 * until the transfer is executed or an error has occured.
 */
int i2c_master_transfer(int i2c_id, i2c_transfer_t* trans);

#endif
