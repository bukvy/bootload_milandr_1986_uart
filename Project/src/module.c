#include "module.h"
#include "leds.h"
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_ssp.h>
#include "config.h"
#include "serial.h"
#include "modbus.h"
#include "string.h"
#include "ferret_proto.h"
#include "flash.h"
#include "spi_ferret_proto.h"

volatile uint8_t bus_show_state = 0;
uint8_t data_type = PHYSICAL_VALUE;
uint32_t g_debug_flags = 0x00;
uint32_t g_debug_channel = 0x00;


uint32_t g_mod_buf_raw[PORTS_COUNT];

float g_mod_buf_val_freq[PORTS_COUNT];

uint16_t g_mod_buf_val[PORTS_COUNT];

uint16_t g_mod_ch_period[PORTS_COUNT]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

mod_wanted_t mod_wanted = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
							  0, 0, 0, 0, 0 };



/* Che:
Формат масивов new_mode и last_mode следующий:
1. Начинается масив с конца, то есть с 18 элемента;
2. В масиве лежат структуры по 5 bit:
  2.1. 1 бит - питание группы из 4 входов; [1]
  2.2. 4 бита - источник 10 мА;            [2][3][4][5]
3. Структуры лежат так:
Номер байта             17                                  16
              [1][2][3][4][5] [1][2][3]         [4][5] [1][2][3][4][5][1]

*/
uint8_t mode_change=0;
unsigned char new_mode[18]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char last_mode[18]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char tDO_mode[18]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void process_tDO(void); // Function only for testing of out ports make toggle of out ports




uint32_t GetTickCount(void) {   return get_ticks();   }


tick setTimeout(u32 milliseconds)
{
    return milliseconds + GetTickCount();
}
int  isTimeout(tick timeout)
{
    tick t = GetTickCount();
    if (timeout < 0x80000000)
            return ((timeout <= t) && (t < timeout + 0x80000000));
    else    return ((timeout <= t) || (t < timeout - 0x80000000));
}




uint32_t get_ticks(void) {
    return TimerCounter;
}






void main_refresh_task(void) {
/*
	if (g_debug_flags & DBG_LOAD_THREADS)
		printf("main_refresh_task starting...\n");
*/


    for (;;) {
                druck_rck();
    }

}


