/*
	The blinky demo using an os timer
	TODO : work out why this resets after a while
*/

#include <ets_sys.h>
#include "user_interface.h"
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>

#include "driver/sigma_delta.h"

// see eagle_soc.h for these definitions
#define LED_GPIO 4
#define LED_GPIO_MUX PERIPHS_IO_MUX_GPIO4_U
#define LED_GPIO_FUNC FUNC_GPIO4

#define DELAY 5 /* milliseconds */

LOCAL os_timer_t blink_timer;
LOCAL uint8 cont=0;
LOCAL bool flag=0;

LOCAL void ICACHE_FLASH_ATTR blink_cb(void *arg)
{
	set_sigma_duty_156KHz(cont);
	if(flag==0){
		if(cont==255){
			flag=1;
			cont=254;
		}
		cont++;
	}
	else{
		if(cont==0){
			flag=0;
			cont=1;
		}
		cont--;
	}


}

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 8;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR user_rf_pre_init(void)
{
}

void ICACHE_FLASH_ATTR user_init(void)
{
	//PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);
	sigma_delta_setup(LED_GPIO_MUX,LED_GPIO,LED_GPIO_FUNC);


	os_timer_disarm(&blink_timer);
    os_timer_setfn(&blink_timer, (os_timer_func_t *)blink_cb, (void *)0);
	os_timer_arm(&blink_timer, DELAY, 1);

}
