/*
 * 3461AS 4x_indicator.c
 *
 * Created: 09.07.2022 11:09:13
 * Author : zartavr
 */ 
#include <avr/io.h>
#include <util/delay.h>

//Катоды подключены к соответствующим пинам порта D
#define port_cathode PORTD
#define port_cathode_dir DDRD
#define seg1 5
#define seg2 4
#define seg3 3
#define seg4 2
//Аноды подключены к соответствующим пинам порта B
#define port_anode PORTB
#define port_anode_dir DDRB
#define led_a 0
#define led_b 1
#define led_c 2
#define led_d 3
#define led_e 4
#define led_f 5
#define led_g 6
#define led_dp 7

void indicator_3461AS_init();
void ind_print(int number);
char ind_dict(char figure);
void ind_dot_on(void);
void ind_dot_off(void);

void indicator_3461AS_init()
{
	port_cathode_dir |= (1<<seg1)|(1<<seg2)|(1<<seg3)|(1<<seg4); //Настраиваем пины катодов на выход
	port_anode_dir |= (1<<led_a)|(1<<led_b)|(1<<led_c)|(1<<led_d)|(1<<led_e)|(1<<led_f)|(1<<led_g)|(1<<led_dp); //Настраиваем пины анодов на выход
	port_anode &= 0b00000000;
	port_cathode &= 0b00000000;
}

void ind_print(int number)
{
	int pause_us = 500;
	int fig1 = number / 1000;
	int fig2 = number / 100 % 10;
	int fig3 = number / 10 % 10;
	int fig4 = number % 10;
	
	port_anode |= ind_dict(fig1);
	port_cathode |= (0<<seg1)|(1<<seg2)|(1<<seg3)|(1<<seg4);
	_delay_us(pause_us);
	port_anode &= 0b00000000;
	port_cathode &= 0b00000000;
	port_anode |= ind_dict(fig2);
	port_cathode |= (1<<seg1)|(0<<seg2)|(1<<seg3)|(1<<seg4);
	_delay_us(pause_us);
	port_anode &= 0b00000000;
	port_cathode &= 0b00000000;
	port_anode |= ind_dict(fig3);
	port_cathode |= (1<<seg1)|(1<<seg2)|(0<<seg3)|(1<<seg4);
	_delay_us(pause_us);
	port_anode &= 0b00000000;
	port_cathode &= 0b00000000;
	port_anode |= ind_dict(fig4);
	port_cathode |= (1<<seg1)|(1<<seg2)|(1<<seg3)|(0<<seg4);
	_delay_us(pause_us);
	port_anode &= 0b00000000;
	port_cathode &= 0b00000000;
}

char ind_dict(char figure)
{
	switch (figure){
		case 0: return (1<<led_a)|(1<<led_b)|(1<<led_c)|(1<<led_d)|(1<<led_e)|(1<<led_f);
		case 1: return (1<<led_b)|(1<<led_c);
		case 2: return (1<<led_a)|(1<<led_b)|(1<<led_d)|(1<<led_e)|(1<<led_g);
		case 3: return (1<<led_a)|(1<<led_b)|(1<<led_c)|(1<<led_d)|(1<<led_g);
		case 4: return (1<<led_b)|(1<<led_c)|(1<<led_f)|(1<<led_g);
		case 5: return (1<<led_a)|(1<<led_c)|(1<<led_d)|(1<<led_f)|(1<<led_g);
		case 6: return (1<<led_a)|(1<<led_c)|(1<<led_d)|(1<<led_e)|(1<<led_f)|(1<<led_g);
		case 7: return (1<<led_a)|(1<<led_b)|(1<<led_c);
		case 8: return (1<<led_a)|(1<<led_b)|(1<<led_c)|(1<<led_d)|(1<<led_e)|(1<<led_f)|(1<<led_g);
		case 9: return (1<<led_a)|(1<<led_b)|(1<<led_c)|(1<<led_d)|(1<<led_f)|(1<<led_g);
		default: return 0;
	}
}

void ind_dot_on(void){
	port_anode |= (1<<led_dp);
}

void ind_dot_off(void){
	port_anode |= (0<<led_dp);
}
