/*
 * attiny2313 encoder.c
 *
 * Created: 09.07.2022 13:43:21
 * Author : zartavr
 */ 

#include <avr/io.h>
#include "3461AS.h"

#define port_enc PORTD
#define port_enc_dir DDRD
#define port_enc_pin PIND
#define port_btn PORTA
#define port_btn_dir DDRA
#define port_btn_pin PINA
#define s1 1
#define s2 0
#define btn 1


int main(void)
{
    /* Replace with your application code */
	indicator_3461AS_init();
	DDRD |= (0<<s1)|(0<<s2);
	DDRA |= (0<<btn);
	int value = 0;
	int last_state = 0;
    while (1) 
    {
		int new_state = port_enc_pin & 0b00000011;
		switch(last_state)
		{
		case 2:
			{
			if(new_state == 3) value++;
			if(new_state == 0) value--; 
			break;
			}
 
		case 0:
			{
			if(new_state == 2) value++;
			if(new_state == 1) value--; 
			break;
			}
		case 1:
			{
			if(new_state == 0) value++;
			if(new_state == 3) value--; 
			break;
			}
		case 3:
			{
			if(new_state == 1) value++;
			if(new_state == 2) value--; 
			break;
			}
		}
		last_state = new_state;
		if(value<0){
			ind_print(-value);
		}
		else 
			ind_print(value);
		
    }
}

