/*
 *   Application pin mapping (header)
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
 * @file pin_mapping.h
 * @brief Application pin mapping
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#ifndef PIN_MAPPING_H_
#define PIN_MAPPING_H_

#include "pin_init.h"

/*
 * GPIOs
 */

/* Debug led 4 */
static const struct pin_cfg_t led4_io =
{
    .port = 1,
    .pin = 23,
    .func = PIN_FUNC_GPIO,
    .mode = PIN_MODE_INACT,
    .dir =  PIN_DIR_OUTPUT,
};

/* FeRAM Write Protect signal */
static const struct pin_cfg_t feram_wp_io =
{
    .port = 0,
    .pin = 21,
    .func = PIN_FUNC_GPIO,
    .mode = PIN_MODE_INACT,
    .dir =  PIN_DIR_OUTPUT,
};

/*
 * Special purpose
 */

/* UART0 */
static const struct pin_cfg_t uart0_rx_io =
{
    .port = 0,
    .pin = 2,
    .func = PIN_FUNC_1,
    .mode = PIN_MODE_INACT,
    .dir = PIN_DIR_OUTPUT,
};
static const struct pin_cfg_t uart0_tx_io =
{
    .port = 0,
    .pin = 3,
    .func = PIN_FUNC_1,
    .mode = PIN_MODE_INACT,
    .dir = PIN_DIR_OUTPUT,
};

/* I2C0 */
static const struct pin_cfg_t i2c0_sda_io =
{
    .port = 0,
    .pin = 27,
    .func = PIN_FUNC_1,
    .mode = PIN_MODE_INACT,
    .dir = PIN_DIR_OUTPUT,
};
static const struct pin_cfg_t i2c0_scl_io =
{
    .port = 0,
    .pin = 28,
    .func = PIN_FUNC_1,
    .mode = PIN_MODE_INACT,
    .dir = PIN_DIR_OUTPUT,
};

/* I2C1 */
static const struct pin_cfg_t i2c1_sda_io =
{
    .port = 0,
    .pin = 19,
    .func = PIN_FUNC_3,
    .mode = PIN_MODE_INACT,
    .dir = PIN_DIR_OUTPUT,
};
static const struct pin_cfg_t i2c1_scl_io =
{
    .port = 0,
    .pin = 20,
    .func = PIN_FUNC_3,
    .mode = PIN_MODE_INACT,
    .dir = PIN_DIR_OUTPUT,
};

#endif
