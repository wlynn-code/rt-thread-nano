#include <stdint.h>
#include <rtthread.h>
#include "boot_iap.h"
#include "lpc_ipc.h"

uint8_t end_sector;
int write_len, write_flag;
uint32_t addr = APPLICATION_ADDRESS;
uint32_t write_buf[2048];
enum rym_code erase_flash(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    char *file_name, *file_size;
    file_name = (char *)&buf[0];
    file_size = (char *)&buf[rt_strlen(file_name) + 1];
    //rt_kprintf("File:%s\n", file_name);
    uint32_t size = atol(file_size);
    end_sector = size/0x8000 + START_SECTOR;
    uint8_t ret_code;
    ret_code = Chip_IAP_PreSectorForReadWrite(START_SECTOR, end_sector);

	/* Error checking */
	if (ret_code != IAP_CMD_SUCCESS) {
        return RYM_CODE_NAK;
	}

	/* Erase the last sector */
	ret_code = Chip_IAP_EraseSector(START_SECTOR, end_sector);

	/* Error checking */
	if (ret_code != IAP_CMD_SUCCESS) {
        return RYM_CODE_NAK;
	}
    ret_code = Chip_IAP_PreSectorForReadWrite(START_SECTOR, end_sector);

	/* Error checking */
	if (ret_code != IAP_CMD_SUCCESS) {
        return RYM_CODE_NAK;
	}
    return RYM_CODE_ACK;
}

enum rym_code write_flash(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    uint8_t ret_code;
    uint32_t data = (uint32_t)buf;
    if(write_flag==0 && len<256){
        write_flag = 1;
        memcpy(write_buf, buf, len);
        write_len = len;
        return RYM_CODE_ACK;
    }
    ret_code = Chip_IAP_PreSectorForReadWrite(START_SECTOR, end_sector);
    if(write_flag == 1){
        memcpy(&write_buf[write_len/4], buf, len);
        write_len += len;
        rt_thread_mdelay(1);
        ret_code = Chip_IAP_CopyRamToFlash(addr, write_buf, write_len);
        write_flag = 0;
        addr += write_len;
        if(ret_code == IAP_CMD_SUCCESS)
            return RYM_CODE_ACK;
    }
    rt_thread_mdelay(2);
    memcpy(write_buf, buf, len);
	ret_code = Chip_IAP_CopyRamToFlash(addr, write_buf, len);
    addr += len;
	/* Error checking */
	if (ret_code != IAP_CMD_SUCCESS) {
        return RYM_CODE_NAK;
	}
    return RYM_CODE_ACK;
}


int erase(void)
{
    //rt_kprintf("File:%s\n", file_name);
    int end_sector = 1 + START_SECTOR;
    uint8_t ret_code;
    ret_code = Chip_IAP_PreSectorForReadWrite(START_SECTOR, end_sector);

	/* Error checking */
	if (ret_code != IAP_CMD_SUCCESS) {
		rt_kprintf("Chip_IAP_PreSectorForReadWrite() failed to execute, return code is: %x\r\n", ret_code);
        return RYM_CODE_NAK;
	}

	/* Erase the last sector */
	ret_code = Chip_IAP_EraseSector(START_SECTOR, end_sector);

	/* Error checking */
	if (ret_code != IAP_CMD_SUCCESS) {
		rt_kprintf("Chip_IAP_EraseSector() failed to execute, return code is: %x\r\n", ret_code);
        return RYM_CODE_NAK;
	}
    ret_code = Chip_IAP_PreSectorForReadWrite(START_SECTOR, end_sector);

	/* Error checking */
	if (ret_code != IAP_CMD_SUCCESS) {
		rt_kprintf("Chip_IAP_PreSectorForReadWrite() failed to execute, return code is: %x\r\n", ret_code);
        return RYM_CODE_NAK;
	}
    return RYM_CODE_ACK;
}
MSH_CMD_EXPORT(erase, erase flash);

int write(void)
{
    uint8_t ret_code;
    uint32_t addr = APPLICATION_ADDRESS;
    uint8_t buf[1028] = {1,2,3,4,5,6,7,8};
    uint32_t data = (uint32_t)buf;
    rt_kprintf("data:%lx\n", data);
    int i = 0;
    for(i = 0; i < 1024; i++){
        buf[i] = i;
    }
    for(i = 0; i < 1; i++){
        rt_thread_mdelay(1);
        rt_kprintf("%d ", i);
        ret_code = Chip_IAP_PreSectorForReadWrite(START_SECTOR, START_SECTOR+2);

    	/* Error checking */
    	if (ret_code != IAP_CMD_SUCCESS) {
    		rt_kprintf("Chip_IAP_PreSectorForReadWrite() failed to execute, return code is: %x\r\n", ret_code);
            return RYM_CODE_NAK;
        }
    	ret_code = Chip_IAP_CopyRamToFlash(addr, (uint32_t*)data, 1024);
        addr+=1024;
    	/* Error checking */
    	if (ret_code != IAP_CMD_SUCCESS) {
    		rt_kprintf("Chip_IAP_CopyRamToFlash() failed to execute, return code is: %x\r\n", ret_code);
            return RYM_CODE_NAK;
    	}
    }
    return RYM_CODE_ACK;
}
MSH_CMD_EXPORT(write, erase flash);

int read(void)
{
    rt_kprintf("write_len = %d\n", write_len);
}
MSH_CMD_EXPORT(read, erase flash);

