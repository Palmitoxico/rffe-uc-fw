/*
 *   I2C master driver
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
 * @file i2c.c
 * @brief I2C master driver
 *
 * @author Augusto Fraga Giachero <afg@augustofg.net>, LNLS
 */

#include "i2c_port.h"
#include "i2c.h"

#include "semphr.h"

/**
 * @brief I2C queue handles
 *
 * @note The uhal I2C driver supports up to 4 different i2c
 * peripherals
 */
static QueueHandle_t i2c0_trans_queue, i2c1_trans_queue,
    i2c2_trans_queue, i2c3_trans_queue;

static SemaphoreHandle_t i2c0_mutex, i2c1_mutex, i2c2_mutex, i2c3_mutex;

static i2c_transfer_t* i2c0_current_trans = NULL;
static i2c_transfer_t* i2c1_current_trans = NULL;
static i2c_transfer_t* i2c2_current_trans = NULL;
static i2c_transfer_t* i2c3_current_trans = NULL;

/*
 * This function is called when a I2C transaction has finished, it
 * will start a new transaction if the transaction queue is not empty.
 */
static inline int i2c_master_irq_handle_generic(int i2c_id, QueueHandle_t i2c_queue, i2c_transfer_t* trans, enum i2c_port_trans_result res)
{
    if (trans != NULL)
    {
        switch(res)
        {
        case I2C_TRANS_SUCCESS:
            trans->status = I2C_TRANSFER_COMPLETE;
            break;
        case I2C_TRANS_NAK:
            trans->status = I2C_TRANSFER_NAK_RECEIVED;
            break;
        case I2C_TRANS_ERROR:
            trans->status = I2C_TRANSFER_ERROR;
            break;
        }
        xTaskNotifyFromISR(trans->task, 0, eIncrement, NULL);
    }

    if (xQueueReceiveFromISR(i2c_queue, &trans, NULL) == pdTRUE)
    {
        port_i2c_start_transfer(i2c_id, trans->slave_addr,
                            trans->tx_buf, trans->tx_size,
                            trans->rx_buf, trans->rx_size);
    }
    else
    {
        return -1;
    }

    return 0;
}

void i2c0_master_irq_handle(enum i2c_port_trans_result res)
{
    i2c_master_irq_handle_generic(0, i2c0_trans_queue, i2c0_current_trans, res);
}

void i2c1_master_irq_handle(enum i2c_port_trans_result res)
{
    i2c_master_irq_handle_generic(1, i2c1_trans_queue, i2c1_current_trans, res);
}

void i2c2_master_irq_handle(enum i2c_port_trans_result res)
{
    i2c_master_irq_handle_generic(2, i2c2_trans_queue, i2c2_current_trans, res);
}

void i2c3_master_irq_handle(enum i2c_port_trans_result res)
{
    i2c_master_irq_handle_generic(3, i2c3_trans_queue, i2c3_current_trans, res);
}

int i2c_init(int i2c_id, uint32_t clock)
{
    port_i2c_init(i2c_id);
    port_i2c_set_clock(i2c_id, clock);
    switch (i2c_id)
    {
    case 0:
        i2c0_trans_queue = xQueueCreate(32, sizeof(i2c_transfer_t*));
        i2c0_mutex = xSemaphoreCreateMutex();
        port_i2c_enable_irq(0, i2c0_master_irq_handle);
        break;
    case 1:
        i2c1_trans_queue = xQueueCreate(32, sizeof(i2c_transfer_t*));
        i2c1_mutex = xSemaphoreCreateMutex();
        port_i2c_enable_irq(1, i2c1_master_irq_handle);
        break;
    case 2:
        i2c2_trans_queue = xQueueCreate(32, sizeof(i2c_transfer_t*));
        i2c2_mutex = xSemaphoreCreateMutex();
        port_i2c_enable_irq(2, i2c2_master_irq_handle);
        break;
    case 3:
        i2c3_trans_queue = xQueueCreate(32, sizeof(i2c_transfer_t*));
        i2c3_mutex = xSemaphoreCreateMutex();
        port_i2c_enable_irq(3, i2c3_master_irq_handle);
        break;
    default:
        return -1;
        break;
    }
    return 0;
}

int i2c_deinit(int i2c_id)
{
    port_i2c_disable_irq(0);
    switch (i2c_id)
    {
    case 0:
        vQueueDelete(i2c0_trans_queue);
        vSemaphoreDelete(i2c0_mutex);
        break;
    case 1:
        vQueueDelete(i2c1_trans_queue);
        vSemaphoreDelete(i2c1_mutex);
        break;
    case 2:
        vQueueDelete(i2c2_trans_queue);
        vSemaphoreDelete(i2c2_mutex);
        break;
    case 3:
        vQueueDelete(i2c3_trans_queue);
        vSemaphoreDelete(i2c3_mutex);
        break;
    default:
        return -1;
        break;
    }
    return 0;
}

/*
 * TODO: Implement a timeout to the i2c_master_transfer function. This
 * is a little tricky if the transmission is ongoing.
 */
int i2c_master_transfer(int i2c_id, i2c_transfer_t* trans)
{
    QueueHandle_t i2c_queue;
    SemaphoreHandle_t i2c_mutex;
    i2c_transfer_t** i2c_current_trans;

    switch (i2c_id)
    {
    case 0:
        i2c_queue = i2c0_trans_queue;
        i2c_mutex = i2c0_mutex;
        i2c_current_trans = &i2c0_current_trans;
        break;
    case 1:
        i2c_queue = i2c1_trans_queue;
        i2c_mutex = i2c1_mutex;
        i2c_current_trans = &i2c1_current_trans;
        break;
    case 2:
        i2c_queue = i2c2_trans_queue;
        i2c_mutex = i2c2_mutex;
        i2c_current_trans = &i2c2_current_trans;
        break;
    case 3:
        i2c_queue = i2c3_trans_queue;
        i2c_mutex = i2c3_mutex;
        i2c_current_trans = &i2c3_current_trans;
        break;
    default:
        return -1;
        break;
    }

    trans->status = I2C_TRANSFER_PENDING;
    trans->task = xTaskGetCurrentTaskHandle();

    /*
     * Critical region
     */
    xSemaphoreTake(i2c_mutex, portMAX_DELAY);
    if(port_i2c_is_idle(i2c_id))
    {
        *i2c_current_trans = trans;
        port_i2c_start_transfer(i2c_id, trans->slave_addr,
                            trans->tx_buf, trans->tx_size,
                            trans->rx_buf, trans->rx_size);
    }
    else
    {
        if(xQueueSend(i2c_queue, &trans, portMAX_DELAY) != pdTRUE)
        {
            xSemaphoreGive(i2c_mutex);
            return -2;
        }
    }
    xSemaphoreGive(i2c_mutex);

    while(trans->status == I2C_TRANSFER_PENDING)
    {
        ulTaskNotifyTake(0, portMAX_DELAY);
    }

    return 0;
}
