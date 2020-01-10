#include "config.h"
#include <MDR32Fx.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_adc.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_uart.h>
#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_ssp.h"
#include "MDR32F9Qx_dma.h"
#include "MDR32F9Qx_it.h"
#include "MDR32F9Qx_timer.h"
#include "leds.h"
#include "fp.h"
#include "Serial.h"
#include "i2c.h"
#include "modbus.h"
#include "ferret_proto.h"
#include "spi_ferret_proto.h"

#define BufferSize         Led_group
#define ALL_PORTS_CLK   (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                         RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                         RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

/*
static struct cfg_params default_params = {
		0, // crc

		"0000000000000000", // serial

		1, // is_leds_on
		0, // user_led_type
		10, // leds_timeout
		0, // rtc_alarm
		0, // is_rtc_alarm_periodic

		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    }, // port_state
		{ 0, 0, 0, 0 }, // port_mode
		//{ 0, 0, 0, 0 }, // alarm_out_state
		//{ 0, 0, 0, 0 } // ps_out_state
};*/

unsigned char SSP_Target=0;
#pragma data_alignment = 2
cfg_board_settings board_set;

DMA_ChannelInitTypeDef  DMA_InitSSP1_RX,  DMA_InitSSP2_RX,  DMA_InitSSP1_TX,  DMA_InitSSP2_TX,DMA_InitADC1,   DMA_InitUART;
DMA_CtrlDataInitTypeDef DMA_PriSSP1_RX,   DMA_PriSSP2_RX,   DMA_PriSSP1_TX,   DMA_PriSSP2_TX, DMA_PriADC1,    DMA_PriUART;
DMA_CtrlDataInitTypeDef DMA_AltADC1;

SSP_InitTypeDef sSSP;
PORT_InitTypeDef PortInitStructure;
static UART_InitTypeDef UART_InitStructure;

TIMER_CntInitTypeDef sTIM_CntInit;
TIMER_ChnInitTypeDef sTIM_ChnInit;
TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;



uint16_t SrcBuf1[30];
uint8_t SrcBuf2[BufferSize];
uint16_t DstBuf1[100];
uint8_t DstBuf2[BufferSize];
unsigned char ssp2_cnt=0,ssp1_cnt=0;

ADC_InitTypeDef sADC;
ADCx_InitTypeDef sADCx;
uint16_t ADCConvertedValue[10];


void ClockConfigure(void)
{

  /*RST_CLK_HSEconfig(RST_CLK_HSE_ON);
  RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
  RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1,RST_CLK_CPU_PLLmul10);
  */
  RST_CLK_DeInit();
  RST_CLK_HSEconfig(RST_CLK_HSE_ON);
  while(RST_CLK_HSEstatus() != SUCCESS);
  if (RST_CLK_HSEstatus() == SUCCESS)                     /* Good HSE clock */
  {
      /* Select HSE clock as CPU_PLL input clock source */
      /* Set PLL multiplier to 7                        */
      RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul10);   // PLLCPUo = PLLCPUi x (PLLCPUMUL+1)
      /* Enable CPU_PLL */
      RST_CLK_CPU_PLLcmd(ENABLE);
      if (RST_CLK_HSEstatus() == SUCCESS)                     /* Good CPU PLL */
      {
        /* Set CPU_C3_prescaler to 2 */
        RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);  // was RST_CLK_CPUclkDIV1 with 4 MHz
        /* Set CPU_C2_SEL to CPU_PLL output instead of CPU_C1 clock */
        RST_CLK_CPU_PLLuse(ENABLE);
        /* Select CPU_C3 clock on the CPU clock MUX */
        RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
        /* LED1 blinking with 7*HSE/2 clock as input clock source */
      }
  }


  /* Configure CPU_PLL clock */
  //RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv1,0);

  /* Enables the RTCHSE clock on all ports */
  RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);

  RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_SSP2 |  RST_CLK_PCLK_EEPROM),ENABLE);
  RST_CLK_PCLKcmd(( RST_CLK_PCLK_UART2 | RST_CLK_PCLK_TIMER1),ENABLE);
}

void PortConfigure(void)
{
  /************************ PortA *************************/


  PortInitStructure.PORT_Pin = EXT_BLOC_OUT | MHZ_5 | EXT_BLOC_OUT_OE_L | TP27;
  PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
  PortInitStructure.PORT_OE = PORT_OE_OUT;
  PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;
  PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;

  PORT_Init(MDR_PORTA, &PortInitStructure);
  PORT_ResetBits(MDR_PORTA, EXT_BLOC_OUT);



  PortInitStructure.PORT_Pin = EXT_BLOC_IN;
  PortInitStructure.PORT_OE = PORT_OE_IN;
  PORT_Init(MDR_PORTA, &PortInitStructure);


  /************************ PortA END *************************/


  /************************ PortB ************************/

  PortInitStructure.PORT_Pin = EXTSPI_INT | OFF_5V;
  PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
  PortInitStructure.PORT_OE = PORT_OE_OUT;
  PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;
  PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;

  PORT_Init(MDR_PORTB, &PortInitStructure);
  PORT_ResetBits(MDR_PORTB,OFF_5V);

   PortInitStructure.PORT_Pin = EXTSPI_GPIO | PWRGD | EXT_LOWPWR | EXT_LEDOFF;
   PortInitStructure.PORT_OE = PORT_OE_IN;
  PORT_Init(MDR_PORTB, &PortInitStructure);

 /************************ PortB END *************************/

   /************************ PortC ************************/

  PortInitStructure.PORT_Pin = TER_PRES ;
  PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
  PortInitStructure.PORT_OE = PORT_OE_IN;
  PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;
  PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;

  PORT_Init(MDR_PORTC, &PortInitStructure);

 /************************ PortC END *************************/


  /************************ PortD ************************/

  PortInitStructure.PORT_Pin = TP28 | LED_OFF_L  |  TP26 | LED_TEST ;
  PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
  PortInitStructure.PORT_OE = PORT_OE_OUT;
  PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;
  PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;

  PORT_Init(MDR_PORTD, &PortInitStructure);

  PORT_ResetBits(MDR_PORTD, LED_TEST);
  PORT_SetBits(MDR_PORTD, LED_OFF_L );  // ¬ключаем питание сдвиговых регистров

 /************************ PortD END *************************/

 /************************ PortE ************************/

#define TP31            PORT_Pin_0
#define TP32            PORT_Pin_1
#define RCK_PWDI_L      PORT_Pin_2
#define OE_PWDI_L       PORT_Pin_3

  PortInitStructure.PORT_Pin = TP31 | TP32  |  RCK_PWDI_L   | OE_PWDI_L ;
  PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
  PortInitStructure.PORT_OE = PORT_OE_OUT;
  PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;
  PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;

  PORT_Init(MDR_PORTE, &PortInitStructure);


  /************************ PortE END ************************/




  /************************ PortF ************************/

  PortInitStructure.PORT_Pin = LED_RSK_L | LED_OE_H | LED_SCLR_H ;
  PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
  PortInitStructure.PORT_OE = PORT_OE_OUT;
  PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;
  PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;

  PORT_Init(MDR_PORTF, &PortInitStructure);
  PORT_SetBits(MDR_PORTF, LED_OE_H);
  PORT_SetBits(MDR_PORTF, LED_RSK_L);
  PORT_ResetBits(MDR_PORTD, LED_SCLR_H);
  PORT_SetBits(MDR_PORTE, OE_PWDI_L);
  /************************ PortF END *************************/




 /*

  PORT_SetBits(MDR_PORTB, PORT_Pin_6+PORT_Pin_7+PORT_Pin_8+PORT_Pin_9+PORT_Pin_10);*/


 PORT_SetBits(MDR_PORTB, OFF_5V );
 PORT_SetBits(MDR_PORTD, LED_OFF_L);


}



void SPIConfigure(void)
{
  DMA_StructInit(&DMA_InitSSP2_RX);
  DMA_StructInit(&DMA_InitSSP2_TX);

  PortInitStructure.PORT_Pin   = (PORT_Pin_6 | PORT_Pin_5);
  PortInitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
  PortInitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PortInitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
  PortInitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_Init(MDR_PORTD, &PortInitStructure);

  PortInitStructure.PORT_Pin   = (PORT_Pin_3| PORT_Pin_1 | PORT_Pin_2);
  PortInitStructure.PORT_OE    = PORT_OE_IN;
  PortInitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
  PortInitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PortInitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
  PORT_Init(MDR_PORTF, &PortInitStructure);
  PortInitStructure.PORT_Pin   = (PORT_Pin_0 );
  PortInitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_Init(MDR_PORTF, &PortInitStructure);

  /* Reset all SSP settings */
  SSP_DeInit(MDR_SSP1);
  SSP_DeInit(MDR_SSP2);

  SSP_BRGInit(MDR_SSP1,SSP_HCLKdiv2);
  SSP_BRGInit(MDR_SSP2,SSP_HCLKdiv2);

  /* SSP2 MASTER configuration ------------------------------------------------*/
  SSP_StructInit (&sSSP);

  sSSP.SSP_SCR  = 1;
  sSSP.SSP_CPSDVSR = 10;
  sSSP.SSP_Mode = SSP_ModeMaster;
  sSSP.SSP_WordLength = SSP_WordLength8b;
  sSSP.SSP_SPH = SSP_SPH_1Edge;
  sSSP.SSP_SPO = SSP_SPO_High;
  sSSP.SSP_FRF = SSP_FRF_SPI_Motorola;
  sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
  SSP_Init (MDR_SSP2,&sSSP);

  /* SSP1 SLAVE configuration ------------------------------------------------*/
  sSSP.SSP_SPH = SSP_SPH_2Edge;
  sSSP.SSP_SPO = SSP_SPO_High;
  sSSP.SSP_WordLength = SSP_WordLength16b;
  sSSP.SSP_SCR  = 0;
  sSSP.SSP_CPSDVSR = 12;
  sSSP.SSP_Mode = SSP_ModeSlave;
  SSP_Init (MDR_SSP1,&sSSP);

  SSP_ITConfig(MDR_SSP2,SSP_IT_RX,ENABLE);
  NVIC_ClearPendingIRQ(SSP2_IRQn);
  NVIC_EnableIRQ(SSP2_IRQn);

  SSP_ITConfig(MDR_SSP1,SSP_IT_RX,ENABLE);
  NVIC_ClearPendingIRQ(SSP1_IRQn);
  NVIC_EnableIRQ(SSP1_IRQn);


  /* Enable SSP1 */
  SSP_Cmd(MDR_SSP1, ENABLE);
  /* Enable SSP2 */
  SSP_Cmd(MDR_SSP2, ENABLE);
}

void init_SPI_Mode(unsigned short mode)
{
  SSP_DeInit(MDR_SSP1);

  SSP_BRGInit(MDR_SSP1,SSP_HCLKdiv2);

  SSP_StructInit (&sSSP);
  sSSP.SSP_SPH = mode;
  sSSP.SSP_SPO = SSP_SPO_Low;
  sSSP.SSP_WordLength = SSP_WordLength16b;
  sSSP.SSP_SCR  = 0;
  sSSP.SSP_CPSDVSR = 12;
  sSSP.SSP_Mode = SSP_ModeSlave;
  sSSP.SSP_FRF = SSP_FRF_SPI_Motorola;
  sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;

  SSP_Init (MDR_SSP1,&sSSP);
  SSP_ITConfig(MDR_SSP1,SSP_IT_RX,ENABLE);

  //NVIC_ClearPendingIRQ(SSP1_IRQn);
  NVIC_EnableIRQ(SSP1_IRQn);


  SSP_Cmd(MDR_SSP1, ENABLE);
}
void UARTConfigure(void)
{
  PortInitStructure.PORT_OE = PORT_OE_OUT;
  PortInitStructure.PORT_FUNC = PORT_FUNC_ALTER;
  PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;
  PortInitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
  PortInitStructure.PORT_Pin = PORT_Pin_1;
  PORT_Init(MDR_PORTD, &PortInitStructure);

  PortInitStructure.PORT_OE = PORT_OE_IN;
  PortInitStructure.PORT_Pin = PORT_Pin_0;
  PORT_Init(MDR_PORTD, &PortInitStructure);

  UART_DeInit(MDR_UART2);
  UART_BRGInit(MDR_UART2, UART_HCLKdiv1);
  NVIC_EnableIRQ(UART2_IRQn);

  UART_StructInit (&UART_InitStructure);

  UART_InitStructure.UART_BaudRate                = 115200;  //57600; //
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;
  UART_InitStructure.UART_StopBits                = UART_StopBits1;
  UART_InitStructure.UART_Parity                  = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
  /* Configure UART1 parameters */
  UART_Init (MDR_UART2,&UART_InitStructure);

  //UART_ITConfig (MDR_UART2, UART_IT_RX, ENABLE);

  UART_Cmd(MDR_UART2,ENABLE);
}

void SysTickStart(uint32_t ticks)
{
  SysTick->LOAD = 80000;//0x14000;
  SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk | \
                  SysTick_CTRL_CLKSOURCE_Msk;
}




void System_Init(void)
{
  ClockConfigure();
  PortConfigure();


  SysTickStart(0);
  SCB->AIRCR = 0x05FA0000 | ((uint32_t)0x500);
  SCB->VTOR = 0x08000000;
  /* Disable all interrupt */
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICER[0] = 0xFFFFFFFF;
  DMA_DeInit();


//  SPIConfigure();
//  init_SPI_Mode(SSP_SPH_2Edge);
  UARTConfigure();
  Init_UART2();

}
