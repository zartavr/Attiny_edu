/*
 * attiny2313_USART.c
 *
 * Created: 10.07.2022 15:08:25
 * Author : zartavr
 */ 
#define F_CPU 1000000
#define XTAL 1000000
#include <avr/io.h>
#include <util/delay.h>
#include "3461AS.h"

void uart_init(void);
void uart_send(unsigned char data );
unsigned char uart_receive(void);

int main(void)
{
    uart_init();
    while (1) 
    {
		char asdd = uart_receive();
		uart_send(asdd);
	}
}

void uart_init(void){
	/* Set baud rate */
	UBRRL = 12; //9600
	UCSRA |= (1<<U2X);
	/* Enable receiver and transmitter */
	UCSRB |= (1<<RXEN)|(1<<TXEN);
	UCSRC |= (1<<USBS)|(3<<UCSZ0)|(1<<UPM1); //8-bit, 2 stop, even parity
}

void uart_send(unsigned char data )
{
	/* Wait for empty transmit buffer */
	while (!( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
}

unsigned char uart_receive(void)
{
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) );
	/* Get and return received data from buffer */
	return UDR;
}

