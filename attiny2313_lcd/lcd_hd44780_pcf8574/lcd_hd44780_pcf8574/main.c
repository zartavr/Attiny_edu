/*
 * attiny2313_I2C.c
 *
 * Created: 12.07.2022 10:36:38
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

#define PORT_ADDR  0x68 // адрес slave

void uart_init(void);
void uart_send(unsigned char data );
unsigned char uart_receive(void);

void i2c_init(void);
void i2c_start(void);
void i2c_send(char data_byte);
char i2c_get_ack(void);
void i2c_stop(void);

void lcd_init(void);
void lcd_write(char data);
void lcd_print(char symbol);



int main(void)
{	
	// Ќастройка таймера
	TCCR0A = 0;
	TCCR0B = (1<<CS02)|(0<<CS01)|(1<<CS00); //предделитель clk/1024, normal mode
	TIMSK |= (1 << TOIE0); // –азрешение прерывани€ по совпадению
	// heart led как выход
	heart_led_dir |= (1<<heart_led);
	
	uart_init();
		
	_delay_ms(1000);
	i2c_init();
	
	i2c_start();
	
	i2c_send(0b01001110);
	uart_send(i2c_get_ack());
	i2c_send(0xFF);
	uart_send(i2c_get_ack());
	uart_send('s');
	lcd_init();
	
	
	lcd_print('H');
	lcd_print('e');
	lcd_print('l');
	lcd_print('l');
	lcd_print('o');
	
	lcd_print(' ');
	lcd_print('w');
	lcd_print('o');
	lcd_print('r');
	lcd_print('l');
	lcd_print('d');
	lcd_print('!');


	i2c_stop();
	sei(); //глобальное разрешение прерываний
    while (1) 
    {
		unsigned char mes = uart_receive();
		uart_send(mes);
	}
}

ISR(TIMER0_OVF_vect){
	heart_led_port ^= (1<<heart_led);
}

void uart_init(void){
	/* Set baud rate */
	//9600, 8-bit, 2-stop bit, even parity
	UBRRL = 12;
	UCSRA |= (1<<U2X);
	/* Enable receiver and transmitter */
	UCSRB |= (1<<RXEN)|(1<<TXEN);
	UCSRC |= (1<<USBS)|(3<<UCSZ0)|(1<<UPM1);
}

void uart_send(unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
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

void i2c_init(void){
	USI_DDR |= (1<<SDA)|(1<<SCL);
	USI_PORT |= (1<<SCL);
	USI_PORT |= (1<<SDA);
	USIDR = 0xFF;
//	USICR = (1<<USIWM1)|(1<<USICS1)|(1<<USICLK);
	USICR = (0 << USISIE) | (0 << USIOIE) | (1 << USIWM1) | (0 << USIWM0) | (1 << USICS1) | (0 << USICS0) | (1 << USICLK) | (0 << USITC);
	USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF)  | (1 << USIDC)  | (0x00 << USICNT0);
//	uart_send(USI_PIN);

//debug
//	uart_send(USI_DDR);
//	uart_send(USI_PIN);
//	uart_send(USICR);
// 	uart_send(USISR);
}

void i2c_start(void){
	USI_PORT |= (1<<SCL);
	while(!(USI_PIN&(1<<SCL)));
	USI_PORT &= ~(1<<SDA);
	_delay_ms(I2C_T);
	USI_PORT &= ~(1<<SCL);
	_delay_ms(I2C_T);
	USI_PORT |= (1<<SDA);
//	uart_send(USI_PIN);
//	uart_send(USISR);
}

void i2c_send(char data_byte){
	USISR |= 0b11110000; // чистим флаги, чтобы отпустить линию SCL и начать передачу и устанавливаем счетчик на передачу 8 бит
	USIDR = data_byte; // помещаем передаваемый байт в регистр передачи
	uart_send(USIDR);
	do 
	{
		_delay_ms(I2C_T);
		USICR |= (1<<USITC); // тик
		while(!(USI_PIN&(1<<SCL))); // ждем высокого сигнала на SCL
		_delay_ms(I2C_T);
		USICR |= (1<<USITC); // так;
	} while (!(USISR&(1<<USIOIF)));
	_delay_ms(I2C_T);
//	uart_send(USISR);
}

char i2c_get_ack(void){
	USISR |= 0b11111110;
	USIDR = 0xFF;
	USI_DDR &= ~(1<<SDA); // лини€ данных настроена на вход
	do
	{
		_delay_ms(I2C_T);
		USICR |= (1<<USITC); // тик
		while(!(USI_PIN&(1<<SCL))); // ждем высокого сигнала на SCL
		_delay_ms(I2C_T);
		USICR |= (1<<USITC); // так;
	} while (!(USISR&(1<<USIOIF)));
	USI_DDR |= (1<<SDA); // лини€ данных настроена на выход
	char res = USIDR;
	uart_send(res);
	return (res);
}

//dont work from example
void i2c_stop(void){
	USISR = 0b11110000;
	USIDR = 0x00;
	USI_PORT &= ~(1<<SDA);
//  uart_send(USI_PIN);
	_delay_ms(I2C_T);
	USIDR = 0xFF;
	USI_PORT |= (1<<SCL);
//	uart_send(USI_PIN);
	while(!(USI_PIN&(1<<SCL)));
	_delay_ms(I2C_T);
	USI_PORT |= (1<<SDA);
//	uart_send(USI_PIN);
	_delay_ms(I2C_T);
//	uart_send(USISR);
}

void lcd_init(void){
	i2c_send(0b00000000);
	uart_send(i2c_get_ack());
	_delay_ms(20);
	
	lcd_write(0b00110000);
	_delay_ms(5);
	lcd_write(0b00110000);
	_delay_ms(5);
	lcd_write(0b00110000);
	_delay_ms(5);
	
	lcd_write(0b00100000);
	
	lcd_write(0b00100000);
	lcd_write(0b10000000);
	
	lcd_write(0b00000000);
	lcd_write(0b11110000);
	
	lcd_write(0b00000000);
	lcd_write(0b00010000);
	
	lcd_write(0b00000000);
	lcd_write(0b01101000);	
}

void lcd_write(char data){
	char data_e1 = data|(1<<2);
	char data_e0 = data&(~(1<<2));
//	uart_send(data_e1);
	i2c_send(data_e1);
	i2c_get_ack();
	_delay_ms(5);
//	uart_send(data_e0);
	i2c_send(data_e0);
	i2c_get_ack();	
}

void lcd_print(char symbol){
	char Hbyte = symbol & 0xF0;
	char Lbyte = symbol << 4;

	i2c_send(Hbyte + 0x0D); // LED = 1 E = 1 RW = 0 RS = 1 
	i2c_get_ack();
	i2c_send(Hbyte + 0x09); // LED = 1 E = 0 RW = 0 RS = 1 
	i2c_get_ack();

	i2c_send(Lbyte + 0x0D); // LED = 1 E = 0 RW = 0 RS = 1 
	i2c_get_ack();
	i2c_send(Lbyte + 0x09); // LED = 1 E = 0 RW = 0 RS = 1 
	i2c_get_ack();
}

