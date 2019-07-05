/*
 *   FeRAM utility functions (header)
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

#ifndef FERAM_H_
#define FERAM_H_

#include <stdint.h>

#include "i2c.h"

typedef enum
{
    ETH_ADDR_MODE_DHCP,
    ETH_ADDR_MODE_STATIC,
    ETH_ADDR_MODE_NONE,
} eth_addr_mode_t;

/**
 * @brief Read arbitrary data from the FeRAM
 * @param addr : Starting address
 * @param buffer : A pointer to store the data read
 * @param size : Number of bytes to read
 * @return 0 if success, a negative number otherwise
 */
int feram_read(uint16_t addr, uint8_t* buffer, uint16_t size);

/**
 * @brief Write arbitrary data from the FeRAM
 * @param addr : Starting address
 * @param buffer : A pointer to data to be written
 * @param size : Number of bytes to write
 * @return 0 if success, a negative number otherwise
 */
int feram_write(uint16_t addr, const uint8_t* buffer, uint16_t size);

/**
 * @brief Read the address mode from the FeRAM (static / dhcp)
 * @param addr_mode : A pointer to store the address mode read
 * @return 0 if success, a negative number otherwise
 */
int feram_get_eth_addressing(eth_addr_mode_t* addr_mode);

/**
 * @brief Write the address mode to the FeRAM (static / dhcp)
 * @param addr_mode : The address mode
 * @return 0 if success, a negative number otherwise
 */
int feram_set_eth_addressing(eth_addr_mode_t addr_mode);

/**
 * @brief Read the mac address from the FeRAM
 * @param mac : A pointer to store the mac address read
 * @return 0 if success, a negative number otherwise
 */
int feram_get_mac_addr(uint8_t mac[6]);

/**
 * @brief Write the mac address to the FeRAM
 * @param mac : A pointer to the the mac address buffer
 * @return 0 if success, a negative number otherwise
 */
int feram_set_mac_addr(const uint8_t mac[6]);

/**
 * @brief Read the ip address from the FeRAM
 * @param ip : A pointer to store the ip address read
 * @return 0 if success, a negative number otherwise
 */
int feram_get_ipv4_addr(uint8_t ip[4]);

/**
 * @brief Write the ip address to the FeRAM
 * @param ip : A pointer to the the ip address buffer
 * @return 0 if success, a negative number otherwise
 */
int feram_set_ipv4_addr(const uint8_t ip[4]);

/**
 * @brief Read the network mask from the FeRAM
 * @param mask : A pointer to store the network mask read
 * @return 0 if success, a negative number otherwise
 */
int feram_get_mask_addr(uint8_t mask[4]);

/**
 * @brief Write the network mask to the FeRAM
 * @param mask : A pointer to the the network mask buffer
 * @return 0 if success, a negative number otherwise
 */
int feram_set_mask_addr(const uint8_t mask[4]);

/**
 * @brief Read the gateway address from the FeRAM
 * @param gateway : A pointer to store the gateway address read
 * @return 0 if success, a negative number otherwise
 */
int feram_get_gateway_addr(uint8_t gateway[4]);

/**
 * @brief Write the gateway address to the FeRAM
 * @param gateway : A pointer to the the gateway address buffer
 * @return 0 if success, a negative number otherwise
 */
int feram_set_gateway_addr(const uint8_t gateway[4]);

/**
 * @brief Read the attenuation value from the FeRAM
 * @param att : A pointer to store attenuation value read
 * @return 0 if success, a negative number otherwise
 */
int feram_get_attenuation(float *att);

/**
 * @brief Write the address value to the FeRAM
 * @param att : The attenuation value
 * @return 0 if success, a negative number otherwise
 */
int feram_set_attenuation(float att);

#endif
