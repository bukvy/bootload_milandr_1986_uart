#ifndef CONFIG_H_
#define CONFIG_H_

#include "MDR32F9Qx_it.h"

#define Led_group 18
#define PORTS_COUNT	48
#define GROUPS_COUNT	12
#define GROUP_SIZE      4


#define SERIAL_NUM_COUNT 16
 #define MAX_CFG_CHANNELS 48

#define LEDS_PERIOD_SEC 1

//------------------  PORTA---------------

#define EXT_BLOC_OUT            PORT_Pin_0
#define MHZ_5                   PORT_Pin_1
#define EXT_BLOC_OUT_OE_L       PORT_Pin_2
#define EXT_BLOC_IN             PORT_Pin_3
#define TP27                    PORT_Pin_5



//------------------  PORTB---------------

#define EXTSPI_INT            PORT_Pin_5
#define EXTSPI_GPIO           PORT_Pin_6
#define PWRGD                 PORT_Pin_7
#define OFF_5V                PORT_Pin_8
#define EXT_LOWPWR            PORT_Pin_9
#define EXT_LEDOFF            PORT_Pin_10

//------------------  PORTC---------------

#define TER_PRES            PORT_Pin_2

//------------------  PORTD---------------

#define TP28            PORT_Pin_2
#define LED_OFF_L       PORT_Pin_3
#define TP26            PORT_Pin_4
#define LED_TEST        PORT_Pin_7


//------------------  PORTE---------------

#define TP31            PORT_Pin_0
#define TP32            PORT_Pin_1
#define RCK_PWDI_L      PORT_Pin_2
#define OE_PWDI_L       PORT_Pin_3


//------------------  PORTF---------------



#define LED_RSK_L   PORT_Pin_4
#define LED_OE_H    PORT_Pin_5
#define LED_SCLR_H  PORT_Pin_6

#define LED_PORTF   (LED_RSK_L | LED_OE_H| LED_SCLR_H)


enum ch_state {
	CH_STATE_OFF,
	CH_STATE_ON
};

enum ssp_target {
	tNONE	= 0,	//0
	tDI 	= 1,	//1
	tDO		= 2,
	tLEDS 	  = 3
};

enum ssp_mode {
	tRECV	= 0,	//0
	tTRANS 	= 1,	//1
};

enum ch_mode {
	CH_MODE_DI_DC_2MA       = 0x00,   //+ цифровой вход типа "сухой контакт" с выдачей 2 мА
	CH_MODE_DI_DC_10MA      = 0x01,   //+ цифровой вход типа "сухой контакт" с выдачей 10 мА
	CH_MODE_DI_IEC 	        = 0x02,   //+ цифровой вход в режиме "потенциальный вход"

        CH_MODE_DI_IC_2MA       = 0x03,   //+ Цифровой вход типа «сухой контакт» с выдачей тока 2 мА в импульсном режиме
        CH_MODE_DI_IC_10MA      = 0x04,   //+ Цифровой вход типа «сухой контакт» с выдачей тока 10 мА в импульсном режиме

        CH_MODE_DI_DC_10MA_CC   = 0x05,   //+ Цифровой вход типа «сухой контакт» с выдачей тока 10 мА c контролем обрыва цепи
        CH_MODE_DI_IC_10MA_CC   = 0x06,   //+ Цифровой вход типа «сухой контакт» с выдачей тока 10 мА в импульсном режиме с контролем обрыва цепи
        CH_MODE_DI_IEC_CC       = 0x07,   //+ Цифровой вход в режиме «потенциальный вход» с контролем обрыва цепи

        CH_MODE_DI_IEC_CC_KZ    = 0x08,   //+ Цифровой вход в режиме «потенциальный вход» с контролем обрыва цепи и короткого замыкания

        CH_MODE_DI_DC_10MA_CC_KZ= 0x09,   //+ Цифровой вход типа «сухой контакт» с выдачей тока 10 мА c контролем обрыва цепи и короткого замыкания
        CH_MODE_DI_IC_10MA_CC_KZ= 0x0A,   //+ Цифровой вход типа «сухой контакт» с выдачей тока 10 мА в импульсном режиме с контролем обрыва цепи и короткого замыкания
};

#define NEED_ON_10MA(TYPE_ID, PORT_POWER)   \
         ((PORT_POWER) &&       \
          ((TYPE_ID == CH_MODE_DI_DC_10MA) ||           \
           (TYPE_ID == CH_MODE_DI_IC_10MA) ||           \
           (TYPE_ID == CH_MODE_DI_DC_10MA_CC) ||        \
           (TYPE_ID == CH_MODE_DI_IC_10MA_CC)  ||       \
           (TYPE_ID == CH_MODE_DI_DC_10MA_CC_KZ) ||     \
           (TYPE_ID == CH_MODE_DI_IC_10MA_CC_KZ)) \
            )


enum ch_status {
	CH_NORMAL	= 0,	//0
	CH_ALARM 	= 1,	//1
	CH_KZ		= 2,
	CH_OBRYV 	= 3,
        CH_FAIL 	= 5,

};



#define u8      unsigned char
#pragma pack(push, 1)

typedef struct {
    unsigned short flag_save;
    unsigned short flag_diag;
    unsigned short flag_error;
    unsigned short flag_clear_cntrs;
} cfg_flags;


typedef struct
{
    unsigned short  crc;
    unsigned char   serial[SERIAL_NUM_COUNT];
    unsigned char   is_leds_on;
    unsigned char   user_led_type;
    unsigned char   leds_timeout;
    unsigned char   is_rtc_alarm_periodic;
    unsigned long   rtc_alarm;
    unsigned char   port_state[MAX_CFG_CHANNELS];
    unsigned char   port_mode[MAX_CFG_CHANNELS];
    unsigned char   port_block_state[MAX_CFG_CHANNELS];
    unsigned char   port_block_mode[MAX_CFG_CHANNELS];
} cfg_params;


typedef struct {
    unsigned char   value[MAX_CFG_CHANNELS];
    short           temperature;
    short           eeprom_test_cnt;
    unsigned long   eeprom_result;
} cfg_values;


typedef struct {
  cfg_params    settings;
  cfg_values    values;
  cfg_flags     flags;
} cfg_board_settings;

#pragma pack(pop)


extern cfg_board_settings board_set;

extern uint8_t DstBuf2[Led_group];
extern uint16_t DstBuf1[100];
extern unsigned char ssp2_cnt,ssp1_cnt;

extern unsigned char SSP_Target;
extern DMA_ChannelInitTypeDef DMA_InitSSP1,DMA_InitSSP2,DMA_InitADC1;
extern DMA_CtrlDataInitTypeDef DMA_PriSSP1,DMA_PriSSP2,DMA_PriADC1;
extern DMA_CtrlDataInitTypeDef DMA_AltADC1;

void init_SPI_Mode(unsigned short mode);
void SendUart2_DMA(unsigned char *ptr,unsigned char cnt);
void InitDMA_SSP1(void);
void InitDMA_SSP1_tx(unsigned char *buf,unsigned char cnt);
void InitDMA_SSP2(unsigned char *buf,unsigned char cnt);
void System_Init(void);
void ReInitDMA(void);
#endif /* CONFIG_H_ */
