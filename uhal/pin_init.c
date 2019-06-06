/*
 *   Pin initiazation
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
 * @file pin_init.c
 * @brief Pin initiazation
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#include "pin_init.h"
#include "gpio.h"

void pin_init()
{
    int i;
    gpio_init();
    for (i = 0; i < pin_init_vec_len; ++i)
    {
        pin_config(pin_init_vec[i].port, pin_init_vec[i].pin, pin_init_vec[i].func, pin_init_vec[i].mode);
        if (pin_init_vec[i].func == PIN_FUNC_GPIO)
        {
            gpio_set_pin_dir(pin_init_vec[i].port, pin_init_vec[i].pin, pin_init_vec[i].dir);
        }
    }
}
