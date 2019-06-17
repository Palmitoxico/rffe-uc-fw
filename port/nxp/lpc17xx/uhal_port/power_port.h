/*
 *   Power management abstraction layer for lpc17xx
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
 * @file power_port.h
 * @brief Power management abstraction layer for lpc17xx
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#ifndef POWER_PORT_H_
#define POWER_PORT_H_

#include "chip.h"

/**
 * @brief Enter MCU soft-sleep mode
 * @return None
 *
 * @note The soft-sleep mode stops the CPU clock, but keeps all
 * peripherals running and interrupts enabled. The SRAM contents are
 * preserved.
 */
static inline void power_soft_sleep()
{
    Chip_PMU_Sleep(LPC_PMU, PMU_MCU_SLEEP);
}

/**
 * @brief Enter MCU deep-sleep mode
 * @return None
 *
 * @note The deep-sleep mode stops the CPU and peripherals clock, only
 * some selected interrupts are enabled (WWDT, BOD, WKT). The SRAM
 * contents are preserved.
 */
static inline void power_deep_sleep()
{
    Chip_PMU_DeepSleepState(LPC_PMU);
}

/**
 * @brief Shutdown the MCU
 * @return None
 *
 * @note This cuts almost all MCU power, an external reset is required
 * to wakeup the system. The SRAM contents not are preserved.
 */
static inline void power_shutdown_sleep()
{
    Chip_PMU_PowerDownState(LPC_PMU);
}

#endif
