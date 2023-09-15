/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-05     yangjie      First edition
 */

#include <rtthread.h>
#include <stdint.h>
#include <LPC17xx.h>
int main(void)
{
    LPC_GPIO0->FIODIR &= ~(1<<3);
    while(0){
        rt_kprintf("11111\n");
        if(LPC_GPIO0->FIOPIN&1<<3){
            rt_kprintf("off key1");
            LPC_GPIO2->FIODIR |= 1<<0;
            LPC_GPIO2->FIOSET |= 1<<0;
        }
        else{
            rt_kprintf("on key1");
            LPC_GPIO2->FIODIR |= 1<<0;
            LPC_GPIO2->FIOSET = 0;
        }
        rt_thread_mdelay(3000);
    }
}
