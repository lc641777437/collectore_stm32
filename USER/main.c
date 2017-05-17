/*
 * main.c
 *
 *  Created on: 2017/05/17
 *      Author: lc
 */
#include <stdio.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stmflash.h"

#include "delay.h"
#include "sys.h"
#include "spi.h"
#include "ads1258.h"
#include "usart.h"
#include "sdio_sdcard.h"
#include "malloc.h"
#include "exfuns.h"
#include "timer.h"
//#include "lwip_comm.h" //remove eth func
#include "exti.h"
#include "rtc.h"
#include "gpio.h"
#include "initstate.h"
#include "lcd12864.h"
#include "usbh_usr.h"
#include "fatfs_api.h"

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

static void SystemConfiguration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    delay_init();

    GPIO_init();
    UART_Init();
    TIM_Init();
    My_Flash_Read();
    mymem_init(SRAMIN);
    //mymem_init(SRAMCCM);
    exfuns_init();//Ϊfatfs��ر��������ڴ�
    SD_Card_Init();
    /*//remove eth func
    while(ETH_Mem_Malloc());		//�����ڴ�
	while(lwip_comm_mem_malloc());	//�����ڴ�
    LAN8720_Init();
    delay_ms(500);
    LWIP_Init();
    */
    ads1258_Init();
    My_RTC_Init();		 		//��ʼ��RTC
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,9);
    RTC_Set_AlarmA(1,0,0,0);
    lcd12864_GPIO_Init();
    LCD_Init();
    USB_Init();
}

int main(void)
{
	SystemConfiguration();

	while(1)
	{
        USBH_Process(&USB_OTG_Core, &USB_Host);
        if(get_InitState(USBSTATE) == USB_OK)
        {
            if(get_Save_Flag() == 1)
            {
                set_Save_Flag(0);
                Save_AD_RawData_USB();
            }
        }
        else if(get_InitState(SDSTATE) == FATFS_OK)
        {
            if(get_Save_Flag() == 1)
            {
                set_Save_Flag(0);
                Save_AD_RawData_SD();
            }
        }
        /*//remove eth func
        if(get_InitState(ETHSTATE)==TCP_OK)
        {
            lwip_periodic_handle();
        }
        */
	}
}

