/*
 *   I2C abstraction layer for lpc17xx
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
 * @file i2c_port.h
 * @brief I2C abstraction layer for lpc17xx
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#include "i2c_port.h"

enum i2c_port_transfer_status
{
    I2C_SENDING_START,
    I2C_SENDING_SLAVE_ADDR_W,
    I2C_WRITING_TX_DATA,
    I2C_SENDING_RESTART,
    I2C_SENDING_SLAVE_ADDR_R,
    I2C_READING_RX_DATA,
    I2C_SENDING_STOP,
    I2C_IDLE,
};

typedef struct
{
    uint8_t slave_addr;
    uint8_t* tx_buf;
    uint32_t tx_size;
    uint8_t* rx_buf;
    uint32_t rx_size;
    enum i2c_port_transfer_status status;

} i2c_port_transfer;

static i2c_port_transfer i2c_trans_buf[3];

static LPC_I2C_T* i2c_peripherals[] =
{
    LPC_I2C0,
    LPC_I2C1,
    LPC_I2C2,
};

static void (*i2c0_irq_callback)(enum i2c_port_trans_result) = NULL;
static void (*i2c1_irq_callback)(enum i2c_port_trans_result) = NULL;
static void (*i2c2_irq_callback)(enum i2c_port_trans_result) = NULL;

void port_i2c_init(int i2c_id)
{
    i2c_port_transfer* trans = &i2c_trans_buf[i2c_id];

    trans->status = I2C_IDLE;

    if (i2c_id == 0)
    {
        Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);
    }

    Chip_I2C_Init(i2c_id);
}

void port_i2c_deinit(int i2c_id)
{
    i2c_port_transfer* trans = &i2c_trans_buf[i2c_id];

    trans->status = I2C_IDLE;

    port_i2c_disable_irq(i2c_id);
    Chip_I2C_DeInit(i2c_id);
}

void port_i2c_enable_irq(int i2c_id, void (*irq_callback)(enum i2c_port_trans_result))
{
    LPC175X_6X_IRQn_Type i2c_irqn;
    switch(i2c_id)
    {
    case 0:
        i2c_irqn = I2C0_IRQn;
        i2c0_irq_callback = irq_callback;
        break;
    case 1:
        i2c_irqn = I2C1_IRQn;
        i2c1_irq_callback = irq_callback;
        break;
    case 2:
        i2c_irqn = I2C2_IRQn;
        i2c2_irq_callback = irq_callback;
        break;
    default:
        return;
    }

    NVIC_SetPriority(i2c_irqn, 5 << 3);
    NVIC_EnableIRQ(i2c_irqn);
}

void port_i2c_disable_irq(int i2c_id)
{
    LPC175X_6X_IRQn_Type i2c_irqn;
    switch(i2c_id)
    {
    case 0:
        i2c_irqn = I2C0_IRQn;
        i2c0_irq_callback = NULL;
        break;
    case 1:
        i2c_irqn = I2C1_IRQn;
        i2c1_irq_callback = NULL;
        break;
    case 2:
        i2c_irqn = I2C2_IRQn;
        i2c2_irq_callback = NULL;
        break;
    default:
        return;
    }

    NVIC_DisableIRQ(i2c_irqn);
}

int port_i2c_is_idle(int i2c_id)
{
    return (i2c_trans_buf[i2c_id].status == I2C_IDLE);
}

void port_i2c_start_transfer(int i2c_id, uint8_t slave_addr,
                            uint8_t* tx_buf, uint32_t tx_size,
                            uint8_t* rx_buf, uint32_t rx_size)
{
    LPC_I2C_T* i2cp = i2c_peripherals[i2c_id];
    i2c_port_transfer* trans = &i2c_trans_buf[i2c_id];

    trans->slave_addr = slave_addr;
    trans->tx_buf = tx_buf;
    trans->tx_size = tx_size;
    trans->rx_buf = rx_buf;
    trans->rx_size = rx_size;
    trans->status = I2C_SENDING_START;

    /*
     * Start the transfer
     */
    i2cp->CONCLR = I2C_CON_STO;
    i2cp->CONSET = I2C_CON_I2EN | I2C_CON_STA;

}

/*
 * I2C Interrupt handler state machine
 */
static inline int i2c_irq_generic(LPC_I2C_T* i2cp, i2c_port_transfer* trans, enum i2c_port_trans_result* res)
{
    volatile uint8_t i2c_status = i2cp->STAT & I2C_STAT_CODE_BITMASK;

    if (i2c_status == 0x00)
    {
        *res = I2C_TRANS_ERROR;
        i2cp->CONSET = I2C_CON_STO;
        trans->status = I2C_SENDING_STOP;
        return 0;
    }

    i2cp->CONCLR = I2C_CON_SI;

    switch (trans->status)
    {
    case I2C_SENDING_START:
        *res = I2C_TRANS_SUCCESS;
        i2cp->CONCLR = I2C_CON_STA;
        if (trans->tx_size > 0)
        {
            i2cp->DAT = (trans->slave_addr << 1);
            trans->status = I2C_SENDING_SLAVE_ADDR_W;
        }
        else if (trans->rx_size > 0)
        {
            i2cp->DAT = (trans->slave_addr << 1) | 1;
            trans->status = I2C_SENDING_SLAVE_ADDR_R;
        }
        else
        {
            i2cp->CONSET = I2C_CON_STO;
            trans->status = I2C_SENDING_STOP;
        }
        break;

    case I2C_SENDING_SLAVE_ADDR_W:

        /*
         * SLA+W sent NAK received, abort
         */
        if (i2c_status == 0x20)
        {
            i2cp->CONSET = I2C_CON_STO;
            trans->status = I2C_SENDING_STOP;
            *res = I2C_TRANS_NAK;
        }
        else
        {
            i2cp->DAT = *(trans->tx_buf++);
            trans->tx_size--;
            trans->status = I2C_WRITING_TX_DATA;
        }
        break;

    case I2C_WRITING_TX_DATA:
        /*
         * Data sent NAK received, abort
         */
        if (i2c_status == 0x30)
        {
            i2cp->CONSET = I2C_CON_STO;
            trans->status = I2C_IDLE;
            *res = I2C_TRANS_NAK;
            return 0;
        }

        /*
         * ACK received, proceed in writing data
         */
        else
        {
            if (trans->tx_size > 0)
            {
                i2cp->DAT = *(trans->tx_buf++);
                trans->tx_size--;
            }
            else if (trans->rx_size > 0)
            {
                i2cp->CONSET = I2C_CON_STA;
                trans->status = I2C_SENDING_RESTART;
            }
            else
            {
                i2cp->CONSET = I2C_CON_STO;
                trans->status = I2C_IDLE;
                return 0;
            }
        }
        break;

    case I2C_SENDING_RESTART:
        i2cp->CONCLR = I2C_CON_STA;
        i2cp->DAT = (trans->slave_addr << 1) | 1;
        trans->status = I2C_SENDING_SLAVE_ADDR_R;
        break;

    case I2C_SENDING_SLAVE_ADDR_R:

        /*
         * SLA+R sent NAK received, abort
         */
        if (i2c_status == 0x48)
        {
            i2cp->CONSET = I2C_CON_STO;
            trans->status = I2C_IDLE;
            *res = I2C_TRANS_NAK;
            return 0;
        }

        /*
         * ACK received, proceed in reading data
         */
        else
        {
            if (trans->rx_size > 1)
                i2cp->CONSET = I2C_CON_AA;
            trans->status = I2C_READING_RX_DATA;
        }
        break;

    case I2C_READING_RX_DATA:
        (*trans->rx_buf++) = i2cp->DAT;
        trans->rx_size--;
        if (trans->rx_size > 1)
        {
            i2cp->CONSET = I2C_CON_AA;
        }
        else if (trans->rx_size == 1)
        {
            i2cp->CONCLR = I2C_CON_AA;
        }
        else if (trans->rx_size == 0)
        {
            i2cp->CONSET = I2C_CON_STO;
            trans->status = I2C_IDLE;
            return 0;
        }
        break;

    default:
        return 0;
        break;
    }

    return 1;
}

void I2C0_IRQHandler()
{
    LPC_I2C_T* i2cp = LPC_I2C0;
    i2c_port_transfer* trans = &i2c_trans_buf[0];
    static enum i2c_port_trans_result res = I2C_TRANS_SUCCESS;

    if (i2c_irq_generic(i2cp, trans, &res) == 0)
    {
        if(i2c0_irq_callback != NULL)
            i2c0_irq_callback(res);
    }
}

void I2C1_IRQHandler()
{
    LPC_I2C_T* i2cp = LPC_I2C1;
    i2c_port_transfer* trans = &i2c_trans_buf[1];
    static enum i2c_port_trans_result res = I2C_TRANS_SUCCESS;

    if (i2c_irq_generic(i2cp, trans, &res) == 0)
    {
        if(i2c1_irq_callback != NULL)
            i2c1_irq_callback(res);
    }
}

void I2C2_IRQHandler()
{
    LPC_I2C_T* i2cp = LPC_I2C2;
    i2c_port_transfer* trans = &i2c_trans_buf[2];
    static enum i2c_port_trans_result res = I2C_TRANS_SUCCESS;

    if (i2c_irq_generic(i2cp, trans, &res) == 0)
    {
        if(i2c2_irq_callback != NULL)
            i2c2_irq_callback(res);
    }
}
