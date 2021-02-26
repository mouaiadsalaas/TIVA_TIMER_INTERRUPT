
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/watchdog.h"
#include "driverlib/adc.h"
#include "driverlib/can.h"
#include "driverlib/eeprom.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/ssi.h"
#include "inc/hw_hibernate.h"
#include "inc/hw_gpio.h"
#include "driverlib/hibernate.h"
#include "inc/hw_uart.h"

volatile uint32_t millis = 0;
volatile uint32_t beginTime;
volatile uint32_t timeOut;

uint32_t step=0;
uint32_t functiontodo =0;
volatile int8_t subStep  = 0;
volatile int8_t failStep = 0;

volatile int8_t currentStep   = 0;
volatile int8_t successStep   = 0;


enum substeps{

    STEP_0 = 0,
    STEP_1,
    STEP_2,
    STEP_3,
    STEP_WAIT,
    STEP_GPRS_CONNECTED
};
void SysTickInt(void)
{
  uint32_t status=0;

  status = TimerIntStatus(TIMER5_BASE,true);
  TimerIntClear(TIMER5_BASE,status);
  millis++;
}

void setStep( int sub, int current, int success, uint32_t time, uint32_t begin ){
    subStep = sub;
    currentStep = current;
    successStep = success;
    timeOut = time;
    beginTime = begin;

    //function to do
}

 void Functions2Do(void){
     switch (functiontodo) {
        case 0:
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 , GPIO_PIN_0);
            break;

        case 1:
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1 , GPIO_PIN_1);
            break;

        case 2:
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 , GPIO_PIN_2);
            break;

        case 3:
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3 , GPIO_PIN_3);
            break;

        default:
            break;
    }

 }
 void Functions2shut(void){
     switch (functiontodo) {
        case 0:
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 , !GPIO_PIN_0);
            break;

        case 1:
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1 , !GPIO_PIN_1);
            break;

        case 2:
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 , !GPIO_PIN_2);
            break;

        case 3:
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3 , !GPIO_PIN_3);
            break;

        default:
            break;
    }

 }
void Duties2Do( void ){

    switch( subStep ){

        case STEP_0:

            setStep(STEP_WAIT, STEP_0, STEP_1, 2000, millis);
            functiontodo = 0;
            break;

        case STEP_1:
            setStep(STEP_WAIT, STEP_1, STEP_2, 1000, millis);
            functiontodo = 1;

            break;

        case STEP_2:
           setStep(STEP_WAIT, STEP_2, STEP_3,  500, millis);
           functiontodo = 2;

           break;

        case STEP_3:
            setStep(STEP_WAIT, STEP_3, STEP_GPRS_CONNECTED, 250, millis);
            functiontodo = 3;

            break;


        case STEP_GPRS_CONNECTED:

            subStep = STEP_0;
            break;


        case STEP_WAIT:
            if( millis - beginTime > timeOut ){
                    subStep = successStep;
                    Functions2shut();
                }else{
                    Functions2Do();
                }

    }

}

/*
  Timer setup
*/
void TimerBegin(){

  uint32_t Period;
  Period = 50000; //1ms

  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
  //SysCtlDelay(3);

  TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER5_BASE, TIMER_A, Period -1);

  TimerIntRegister(TIMER5_BASE, TIMER_A, SysTickInt);
  TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

  TimerEnable(TIMER5_BASE, TIMER_A);
}


void Wait (uint32_t tempo) {
  volatile uint32_t temp = millis;
  while ( (millis-temp) < tempo);

}

int main(){

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ| SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 , !GPIO_PIN_2);

    TimerBegin();

  while(1){

      Duties2Do();

  }
}
