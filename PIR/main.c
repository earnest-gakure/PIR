

#include "TM4C123.h"                    // Device header

int state = 0;
void pir_init(void);
void delay(int time);

void GPIOA_Handler(void)
{
    state = 1;
    GPIOA->ICR = 0x04; // Clear interrupt flag
}

int main()
{
   

    __disable_irq(); // disable interrupts
	 pir_init();
    while (1)
    {
        if (state == 1)
        {
            GPIOA->DATA |= 0x08; // Use |= instead of = for setting the LED
            delay(3000);
            state = 0;
        }
        else
        {
            GPIOA->DATA &= ~(0x08);
        }
    }
}

void pir_init(void)
{

    SYSCTL->RCGCGPIO |= 0x01; // Enable clock for GPIOA

    GPIOA->DIR &= ~(0x04); // PIR data input
    GPIOA->DIR |= 0x08;    // LED
    GPIOA->DEN |= 0x0C;

    // PIR interrupt configuration
    GPIOA->IS &= ~(0x04); // Level sensitive
    GPIOA->IBE &= ~(0x04); // Single edge interrupt
    GPIOA->IEV |= 0x04;    // Rising edge interrupt select
    GPIOA->IM |= 0x04;     // Unmasking interrupt, i.e., it is sent to controller
    GPIOA->ICR = 0x04;     // Clearing interrupts

    NVIC->ISER[0] = (1 << 0); // Enable GPIOA interrupt in NVIC
    __enable_irq(); // Enable interrupts
}

void delay(int time)
{
    int i;
    SYSCTL->RCGCTIMER |= 0x01; // Enable clock for Timer 0
    TIMER0->CTL &= ~(0x01);
    TIMER0->CFG = 0x04;
    TIMER0->TAMR = 0x02;
    TIMER0->TAILR = 16000 - 1; // Set load. Delay = load * (1/16M) = 16000 * (1/16M) = 0.001 sec
    TIMER0->ICR = 0X01;
    TIMER0->CTL |= 0x01;
    for (i = 0; i < time; i++)
    {
        while ((TIMER0->RIS & 0x01) == 0)
            ;
        TIMER0->ICR = 0x01; /*clear interrupt */
    }
}

