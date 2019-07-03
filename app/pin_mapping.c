/*
 *   Application pin mapping
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

#include "pin_mapping.h"

struct pin_cfg_t pin_init_vec[] =
{
    led4_io,
    feram_wp_io,
    uart0_rx_io,
    uart0_tx_io,
    i2c0_sda_io,
    i2c0_scl_io,
    i2c1_sda_io,
    i2c1_scl_io,
};

const int pin_init_vec_len = sizeof(pin_init_vec) / sizeof(struct pin_cfg_t);
