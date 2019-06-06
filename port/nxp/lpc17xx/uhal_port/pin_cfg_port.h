/*
 *   Pin configuration abstraction layer for lpc17xx
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
 * @file pin_cfg_port.h
 * @brief Pin configuration abstraction layer for lpc17xx
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#ifndef PIN_CFG_PORT_H_
#define PIN_CFG_PORT_H_

#include "chip_lpc175x_6x.h"
#include "iocon_17xx_40xx.h"

enum pin_dir_t
{
    PIN_DIR_INPUT = 0,
    PIN_DIR_OUTPUT = 1,
};

enum pin_func_t
{
    PIN_FUNC_GPIO = IOCON_FUNC0,
    PIN_FUNC_0 = IOCON_FUNC0,
    PIN_FUNC_1 = IOCON_FUNC1,
    PIN_FUNC_2 = IOCON_FUNC2,
    PIN_FUNC_3 = IOCON_FUNC3,
};

enum pin_mode_t
{
    /* Generic modes */
    PIN_MODE_PULLDOWN = IOCON_MODE_PULLDOWN,
    PIN_MODE_PULLUP = IOCON_MODE_PULLUP,
    PIN_MODE_OD,
    /* lpc17xx specific modes */
    PIN_MODE_INACT = IOCON_MODE_INACT,
    PIN_MODE_REPEATER = IOCON_MODE_REPEATER,
};

/**
 * @brief Configure the pin mux and mode
 * @param port   : GPIO port number
 * @param pin    : GPIO pin number
 * @param func   : GPIO function (gpio, alternative functions)
 * @param mode   : GPIO mode (pullup, pulldown, etc)
 * @return Nothing
 * @note Some modes are target specific.
 */
static inline void pin_config(int port, int pin, enum pin_func_t func, enum pin_mode_t mode)
{
    Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, func | mode);
}

#endif
