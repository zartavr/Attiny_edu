/*
 * attiny_i2c_soft.c
 *
 * Created: 27.08.2022 23:35:43
 * Author : zartavr
 */ 

#include <avr/interrupt.h>
#define __AVR_AT90Tiny2313__
#define F_CPU 1000000UL	      // Sets up the default speed for delay.h
#include <util/delay.h>
#include <avr/io.h>

#define heart_led 0
#define heart_led_dir DDRB
#define heart_led_port PORTB

#define SDA 5
#define SCL 7
#define USI_DDR DDRB
#define USI_PORT PORTB
#define USI_PIN PINB
#define I2C_T	5


int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

