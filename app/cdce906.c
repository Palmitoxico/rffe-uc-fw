/*
 *   CDCE906 PLL Driver
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

#include "i2c.h"

uint8_t cdce906_config[] =
{
    0, /* Starting address */
    26, /* Length */
    0x1,
    9,   /* PLL 1 M */
    25,   /* PLL 1 N */
    0x0,
    9,   /* PLL 2 M */
    25, /* PLL 2 N */
    0xE0,
    9,   /* PLL 3 M */
    25,   /* PLL 3 N */
    (3<<5),
    0x0,
    0x0,
    0x0,
    0x2, /* P0 div */
    0x1,  /* P1 div */
    0x1, /* P2 div */
    0x1, /* P3 div */
    0x1, /* P4 div */
    0x1, /* P5 div */
    0x38, /* Y0 */
    0,    /* Y1 */
    0,    /* Y2 */
    0,    /* Y3 */
    0,    /* Y4 */
    0,    /* Y5 */
    (3<<4)|0xB,
};

int cdce906_pll_init()
{
    i2c_transfer_t i2c_trans;
    int err;

    i2c_trans.slave_addr = 0b1101001;
    i2c_trans.tx_buf = cdce906_config;
    i2c_trans.tx_size = sizeof(cdce906_config);
    i2c_trans.rx_size = 0;

    err = i2c_master_transfer(0, &i2c_trans);
    if (i2c_trans.status != I2C_TRANSFER_COMPLETE)
    {
        return -1;
    }
    else if (err < 0)
    {
        return -2;
    }
    return 0;
}
