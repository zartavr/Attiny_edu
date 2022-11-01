/*
 * Blink.c
 *
 * Created: 16.07.2022 13:02:27
 * Author : zartavr
 */ 

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>

#define LED_PORT PORTB
#define LED_DDR DDRB
#define led_pin 0 // 12 нога если если считать как принято


int main(void)
{
    /* Replace with your application code */
	LED_DDR &= ~(1<<led_pin);
    while (1) 
    {
		LED_PORT |= (1<<led_pin);
		_delay_ms(1000);
		LED_PORT &= ~(1<<led_pin);
		_delay_ms(1000);
    }
}

