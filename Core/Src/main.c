#include "main.h"
#include "clock.h"
#include "gpio.h"
#include "timer.h"
#include "IC_74AC165D.h"
#include "IC_74HC594.h" 
#include "LCD_Lib.h"
#include "uart.h"
#include <stdio.h>


//timer
uint32_t time20ms = 0;
uint32_t count=0;

//button, 7seg
int num=0;
int state1;
int state2;
int lock;
uint8_t start_count=0;
//LCD
CLCD_Name LCD1;
char lcd_send[16];
//uart
uart1 Uart1;
char  buff[2];
char char_count[7];
char ms[]='ms';

//timer interrupt
void TIM1_UP_IRQHandler(void)
{

    if (++ time20ms > 20)
    {
        time20ms = 0;
        
    }
    else
    {
        if(start_count==1)
        {
          count++;
        }
    }
    TIM1->SR &= ~(TIM_SR_UIF);
}
//uart interrupt
void USART1_IRQHandler(void) 
{
    if (USART1->SR & USART_SR_RXNE) 
    {
        Uart1.data = USART1->DR;
        if (Uart1.data == 0x0A) //enter
        {    
            Uart1.index = 0;
            
        } 
        else 
        {
            Uart1.buffer[Uart1.index++] = Uart1.data;
            if (Uart1.index >= 5) 
            {  
                Uart1.index = 0;
            }
        }
        int temp0 = Uart1.buffer[0]-'0';
        int temp1 = Uart1.buffer[1]-'0';
        num=temp0*10+temp1;
    } 
}
int main(void)
{
 
  //system config:

    //HAL
    HAL_Init();
    //system clock
    SystemClock_Config();
    //system peripheral
    system_clock_peripheral_init();
    //timer init
    timer1_Init();
    timer2_Init();
    //uart1 init
    uart1_init();
    
    
  //user config:
  
    //ic I/O
    ic_74AC165D_init();
    //led 7 
    ic_74HC594_init();
    //I/O init:
    
    //lcd setup
    gpio_init(GPIOD,0,0);
    gpio_init(GPIOD,1,0);
    gpio_init(GPIOD,2,0);
    gpio_init(GPIOD,3,0);
    gpio_init(GPIOD,4,0);
    gpio_init(GPIOD,5,0);
    gpio_init(GPIOD,6,0);
    gpio_init(GPIOD,7,0);
    
    gpio_write(GPIOD,4,1);
    
    CLCD_4BIT_Init(&LCD1, 16, 2,
    GPIOD, GPIO_PIN_7, GPIOD, GPIO_PIN_5,
    GPIOD, GPIO_PIN_0, GPIOD, GPIO_PIN_1,
    GPIOD, GPIO_PIN_2, GPIOD, GPIO_PIN_3);
    
while (1)
{
        shiftIn();
        if((data&(1<<0))== 0 && lock == 0)
        {     
            start_count=1;
            if(state1==1)
            {
              count=0;
            }
            else
            {
              lock=1;
            }
            
        }
        if((data&(1<<5))== 0 && state2 ==0)
        {
            start_count=0;
            state1=1;
            state2=1;
            sprintf(char_count, "%dms\n", count,ms);
            uart_sendstring(char_count);
        }
        if((data&(1<<5))== (1<<5))
        {
            
            state2=0;
           
        }
        if((data&(1<<0))== (1<<0))
        {
            
            lock=0;
           
        }
        quetled(count);

}
  
}





