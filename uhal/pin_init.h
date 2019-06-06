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
 * @file pin_init.h
 * @brief Pin initiazation
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#ifndef PIN_INIT_H_
#define PIN_INIT_H_

#include "pin_cfg.h"

struct pin_cfg_t
{
    int port;
    int pin;
    enum pin_func_t func;
    enum pin_mode_t mode;
    enum pin_dir_t dir;
};

extern struct pin_cfg_t pin_init_vec[];
extern const int pin_init_vec_len;

void pin_init();

#endif
