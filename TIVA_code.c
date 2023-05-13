/*
 * UART.c
 *
 *  Created on: 26-Mar-2023
 *      Author: analn
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include <time.h>
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
# define LOCK_F (*((volatile unsigned long *)0x40025520))
# define CR_F (*((volatile unsigned long *)0x40025524))

void delay(void);
int r;
int bin_size=256;
volatile uint32_t clock_value;
uint32_t ui32ADC0Value[1];
uint32_t ui32ADC1Value[1];
//uint32_t ui32ADC0Value[4];
//uint32_t ui32ADC1Value[4];
volatile uint32_t digital_output;
volatile unsigned char b1;
volatile unsigned char b0;

int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_3 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//    Initialising GPIOA
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2);
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);


//    Initialising UART
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_ODD));

//    Initialising ADC
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

//    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0);
//    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH1);
//    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH0);
//    ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH1);
//    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH0);
//    ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH1);
//    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
//    ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceEnable(ADC0_BASE, 1);
    ADCSequenceEnable(ADC1_BASE, 1);

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2);
//    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2);

//    Initialising PORTF
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    LOCK_F = 0x4C4F434B;
    CR_F = GPIO_PIN_0 | GPIO_PIN_4;

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
//

    clock_value = SysCtlClockGet();


    UARTCharGet(UART0_BASE);

    while (1)
    {
//         for (r=0;r<bin_size;r++)
//         {
           ADCIntClear(ADC0_BASE, 1);
           ADCIntClear(ADC1_BASE, 1);
           ADCProcessorTrigger(ADC0_BASE, 1);
           ADCProcessorTrigger(ADC1_BASE, 1);

           while(!ADCIntStatus(ADC0_BASE, 1, false))
           {
           }

           ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

//           digital_output = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3])/4;
           digital_output = ui32ADC0Value[0];
           b1 = (unsigned char)(digital_output>>6);
           b0 = (unsigned char)((digital_output>>0) & 0x003F);


           if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 1)
           {

               UARTCharPut(UART0_BASE, b0);
               UARTCharPut(UART0_BASE, b1);
           }
           else
           {

               UARTCharPut(UART0_BASE, (b0 | 0x80));
               UARTCharPut(UART0_BASE, (b1 | 0x80));
           }
//           UARTCharGet(UART0_BASE);
           delay();
           while(!ADCIntStatus(ADC1_BASE, 1, false))
             {
             }

           ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1Value);

//           digital_output = (ui32ADC1Value[0] + ui32ADC1Value[1] + ui32ADC1Value[2] + ui32ADC1Value[3])/4;
           digital_output = ui32ADC1Value[0];
           b1 = (unsigned char)(digital_output>>6);
           b0 = (unsigned char)((digital_output>>0) & 0x003F);


           if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 1)
           {

               UARTCharPut(UART0_BASE, b0 | 0x40);
               UARTCharPut(UART0_BASE, b1 | 0x40);
           }
           else
           {

               UARTCharPut(UART0_BASE, (b0 | 0xC0));
               UARTCharPut(UART0_BASE, (b1 | 0xC0));
           }

    //       if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
//         }
    }
}


void delay()
{
     SysCtlDelay(6000);
}
