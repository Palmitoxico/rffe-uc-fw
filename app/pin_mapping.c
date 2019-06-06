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

#include "pin_init.h"

struct pin_cfg_t pin_init_vec[] =
{
	{0, 2, PIN_FUNC_1, PIN_MODE_INACT, PIN_DIR_OUTPUT}, /* UART0 RXD P0.2 */
	{0, 3, PIN_FUNC_1, PIN_MODE_INACT, PIN_DIR_INPUT}, /* UART0 TXD P0.3 */
	{1, 23, PIN_FUNC_GPIO, PIN_MODE_INACT, PIN_DIR_OUTPUT}, /* LED4 P1.23 */
};

const int pin_init_vec_len = sizeof(pin_init_vec) / sizeof(struct pin_cfg_t);
