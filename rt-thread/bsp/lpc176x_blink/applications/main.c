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
#include <string.h>
#include "boot_iap.h"

uint32_t APP_ADDR[3] = {(uint32_t)0x00010000, (uint32_t)0x00040000};

uint8_t boot_flag;
typedef volatile uint32_t  vu32;
void (*jumpapp)();


void iap_load_app(uint32_t appxaddr)
{
    rt_thread_mdelay(1000);
    jumpapp=(void(*)())*(vu32*)(appxaddr+4);
    __set_MSP(*( vu32*) appxaddr);
    jumpapp();
}

void jump_to_app(int argc, char *argv[])
{
    uint8_t i = 0;
    rt_thread_delay(200);
    if(argv[1])
        i = strtol(argv[1], NULL, 10);
    rt_kprintf("i = %d\n", i);
    if (i < sizeof(APP_ADDR)/sizeof(*APP_ADDR)){
        rt_kprintf("Jump to app!\n");
        iap_load_app(APP_ADDR[i]);
    }
}
MSH_CMD_EXPORT(jump_to_app, jump app);


void led_on(void)
{
   LPC_GPIO2->FIODIR |= 1<<0;
   LPC_GPIO2->FIOCLR |= 1<<0;
}
MSH_CMD_EXPORT(led_on, take on led);

void led_off(void)
{
   LPC_GPIO2->FIODIR |= 1<<0;
   LPC_GPIO2->FIOSET |= 1<<0;
}
MSH_CMD_EXPORT(led_off, take off led);


void upgrade(void)
{
    int res;
    rt_device_t dev = rt_device_find("uart0");
    struct rym_ctx rctx;
    res = rym_recv_on_device(&rctx, dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                             erase_flash, write_flash, NULL, 1000);
    rt_kprintf("res = %d\n", res);
}
MSH_CMD_EXPORT(upgrade, upgrade version);

void boot_app(void)
{
    LPC_GPIO0->FIODIR &= ~(1<<3);
    LPC_GPIO2->FIOSET |= 1<<0;
    LPC_GPIO2->FIODIR |= 1<<0;    //LED
    rt_thread_delay(2000);
    if(!(LPC_GPIO0->FIOPIN & 1<<3)){
        rt_thread_mdelay(500);
        LPC_GPIO2->FIOCLR |= 1<<0;
        rt_thread_mdelay(1000);
        if(!(LPC_GPIO0->FIOPIN & 1<<3)){
            rt_kprintf("Jump app2\n");
            LPC_GPIO2->FIOSET |= 1<<0;
            iap_load_app(APP_ADDR[1]);
        }
        else
            rt_kprintf("Stop jump!\n");
        return;
    }
    rt_kprintf("Jump app1\n");
    iap_load_app(APP_ADDR[0]);
}

int main(void)
{
    rt_thread_mdelay(1000);
    rt_kprintf("Bootloader...\n");
    rt_kprintf("After 3 seconds, the system will automatically jump to app1\n");
    rt_kprintf("Press key for 1s to stop in BootLoader.\n");
    rt_kprintf("Press key for 2s to jump to app2.\n");
    boot_app();
}
