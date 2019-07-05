/*
 *   FeRAM utility functions
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

#include "feram.h"
#include "pin_mapping.h"
#include "gpio.h"

#include "FreeRTOS.h"

#include <string.h>

const int feram_i2c_id = 1;
const uint8_t feram_i2c_addr = 0x50;

const int feram_mac_addr_offset = 0;
const int feram_ip_addr_offset = 0x10;
const int feram_mask_addr_offset = 0x20;
const int feram_gateway_addr_offset = 0x30;
const int feram_attenuation_offset = 0x40;
const int feram_eth_addr_offset = 0x50;

int feram_read(uint16_t addr, uint8_t* buffer, uint16_t size)
{
    i2c_transfer_t i2c_trans;
    uint8_t tx_buf;

    tx_buf = (addr & 0xFF);
    i2c_trans.slave_addr = feram_i2c_addr | (0x07 & (addr >> 8));
    i2c_trans.tx_buf = &tx_buf;
    i2c_trans.tx_size = 1;
    i2c_trans.rx_buf = buffer;
    i2c_trans.rx_size = size;
    return i2c_master_transfer(1, &i2c_trans);
}

int feram_write(uint16_t addr, const uint8_t* buffer, uint16_t size)
{
    i2c_transfer_t i2c_trans;
    uint8_t* tx_buf = (uint8_t*) pvPortMalloc(size + 1);

    tx_buf[0] = (addr & 0xFF);
    memcpy(&tx_buf[1], buffer, size);

    i2c_trans.slave_addr = feram_i2c_addr | (0x07 & (addr >> 8));
    i2c_trans.tx_buf = tx_buf;
    i2c_trans.tx_size = size + 1;
    i2c_trans.rx_size = 0;

    gpio_set_pin_low(feram_wp_io.port, feram_wp_io.pin);
    int res = i2c_master_transfer(1, &i2c_trans);
    gpio_set_pin_high(feram_wp_io.port, feram_wp_io.pin);

    vPortFree(tx_buf);
    return res;
}

int feram_get_eth_addressing(eth_addr_mode_t* addr_mode)
{
    int err;
    uint8_t buf;

    err = feram_read(feram_eth_addr_offset, &buf, sizeof(uint8_t));

    switch (buf)
    {
    case 0:
        *addr_mode = ETH_ADDR_MODE_STATIC;
        break;
    case 1:
        *addr_mode = ETH_ADDR_MODE_DHCP;
        break;
    default:
        *addr_mode = ETH_ADDR_MODE_NONE;
        break;
    }

    return err;
}

int feram_set_eth_addressing(eth_addr_mode_t addr_mode)
{
    uint8_t buf;

    switch (addr_mode)
    {
    case ETH_ADDR_MODE_STATIC:
        buf = 0;
        break;
    case ETH_ADDR_MODE_DHCP:
        buf = 1;
        break;
    default:
        buf = 0xFF;
        break;
    }

    return feram_write(feram_eth_addr_offset, &buf, sizeof(uint8_t));
}

int feram_get_mac_addr(uint8_t mac[6])
{
    return feram_read(feram_mac_addr_offset, mac, 6);
}

int feram_set_mac_addr(const uint8_t mac[6])
{
    return feram_write(feram_mac_addr_offset, mac, 6);
}

int feram_get_ipv4_addr(uint8_t ip[4])
{
    return feram_read(feram_ip_addr_offset, ip, 4);
}

int feram_set_ipv4_addr(const uint8_t ip[4])
{
    return feram_write(feram_ip_addr_offset, ip, 4);
}

int feram_get_mask_addr(uint8_t mask[4])
{
    return feram_read(feram_mask_addr_offset, mask, 4);
}

int feram_set_mask_addr(const uint8_t mask[4])
{
    return feram_write(feram_mask_addr_offset, mask, 4);
}

int feram_get_gateway_addr(uint8_t gateway[4])
{
    return feram_read(feram_gateway_addr_offset, gateway, 4);
}

int feram_set_gateway_addr(const uint8_t gateway[4])
{
    return feram_write(feram_gateway_addr_offset, gateway, 4);
}

int feram_get_attenuation(float *att)
{
    int err;
    uint8_t att_buf[4];
    int32_t att_int;

    err = feram_read(feram_attenuation_offset, att_buf, sizeof(att_buf));

    att_int = (att_buf[0] << 24) | (att_buf[1] << 16) | (att_buf[2] << 8) | att_buf[3];

    if (att)
    {
        *att = (float) (att_int/2.0);
    }
    else
    {
        err = -1;
    }

    return err;
}

int feram_set_attenuation(float att)
{
    int err;
    uint8_t att_buf[4];
    int32_t att_int = (int) (att*2);

    att_buf[0] = (att_int >> 24) & 0xFF;
    att_buf[1] = (att_int >> 16) & 0xFF;
    att_buf[2] = (att_int >> 8) & 0xFF;
    att_buf[3] = att_int & 0xFF;

    err = feram_write(feram_attenuation_offset, att_buf, sizeof(att_buf));

    return err;
}
