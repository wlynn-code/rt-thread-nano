#ifndef __BOOT_IAP_H
#define __BOOT_IAP_H
#include <rtdef.h>
#include <ymodem.h>

#define APPLICATION_ADDRESS    0x00010000
#define START_SECTOR           16

/* End of the Flash address */
#define USER_FLASH_END_ADDRESS        0x0007ffff
/* Define the user application size */
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1)

enum rym_code erase_flash(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len);
enum rym_code write_flash(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len);

#endif
