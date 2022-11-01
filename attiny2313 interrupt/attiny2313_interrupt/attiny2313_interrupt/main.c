/*
 * attiny2313_interrupt.c
 *
 * Created: 11.07.2022 8:36:46
 * Author : zartavr
 */ 

#define F_CPU 1000000UL

#include <util/delay.h>
#include <avr/io.h>
#include "3461AS.h"
#include <avr/interrupt.h>

void uart_init(void);
void uart_send(char* str);

volatile int val = 0;
char tx_buffer[] = "asdasdasd";

int main(void)
{
	indicator_3461AS_init();
	uart_init();
	sei();
    /* Replace with your application code */
    while (1) 
    {
		static int dot_flag = 0;
		if(val == 105){
			dot_flag = 1;
		}
		if (val == 106)
		{
			dot_flag = 0;
		}
		if (val == 44)
		{
			char text[1];
			text[0] = DDRB;
			uart_send(text);
			val = DDRB;
			dot_flag = 1;
		}
		ind_print(val);
		if(dot_flag){
			ind_dot_on();
		}
		else{
			ind_dot_off();
		}
    }
}

ISR(USART0_RX_vect){
	int buf = UDR;
	int fig = -1;
	static int flag = 0;
	if(flag == 1){
		val = 0;
		flag = 0;
	}
	if ((buf > 47)&&(buf<58)){
		switch(buf)
		{
			case 48:
				fig = 0;
				break;
			case 49:
				fig = 1;
				break;
			case 50:
				fig = 2;
				break;
			case 51:
				fig = 3;
				break;
			case 52:
				fig = 4;
				break;
			case 53:
				fig = 5;
				break;
			case 54:
				fig = 6;
				break;
			case 55:
				fig = 7;
				break;
			case 56:
				fig = 8;
				break;
			case 57:
				fig = 9;
				break;
		}
	}
	if (fig != -1){
		val = val*10 + fig;
	}
	if(buf == 10){
		flag = 1;
	}
}

void uart_init(void){
	/* Set baud rate */
	UBRRL = 12;
	UCSRA |= (1<<U2X);
	/* Enable receiver and transmitter */
	UCSRB |= (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	UCSRC |= (1<<USBS);
}

void uart_send(char* str)
{
	int i = 0;
	while(str[i]){
		/* Wait for empty transmit buffer */
		while ( !( UCSRA & (1<<UDRE)) );
		/* Put data into buffer, sends the data */
		UDR = str[i];
		i++;
	}
	
}

