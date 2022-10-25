/*
 * USI_I2C.c
 *
 * Created: 25.10.2022 23:13:06
 *  Author: zartavr
 */ 

#include "USI_I2C.h"
/*-------------I2C public methods--------------*/
void I2C::init(void){
	USI_DDR |= (1<<SDA)|(1<<SCL);						// ����������� ����� SDA � SCL �� �����
	USI_PORT |= (1<<SCL)|(1<<SDA);						// ����������� � ������� ��������
	USIDR = 0xFF;										// ��������� ����� ������-��������
	
	// ���������� ���������, I2C ��� ��������� SCL
	USICR = (0 << USISIE) | (0 << USIOIE) |
	(1 << USIWM1) | (0 << USIWM0) |
	(1 << USICS1) | (0 << USICS0) |(1 << USICLK) | (0 << USITC); 	// ��������� ������������ USI
	
	// ����� ������ ���������, ��������� �������� � 0
	USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF)  | (1 << USIDC)  | (0x00 << USICNT0);
}

void I2C::send(int8_t addr, int8_t &data, int8_t data_size){
	// addr = 0..127;  data - tx buffer, data_size - � ������
	start();
	int8_t addr_byte = (addr<<1) + 1; // +1 - Read/Write bit = write
	transmit(*addr_byte);
	get_ack();
	while (data_size > 0)
	{
		transmit(data);
		data++;
		get_ack();
		data_size--;
	}
	stop();
}

/*-----------I2C private methods--------------*/
void I2C::start(void){
	// ��� SCL = 1, SDA = 1 -> 0
	USI_PORT |= (1<<SCL);
	while(!(USI_PIN&(1<<SCL)));	// ���� �������� ������� �� SCL
	USI_PORT &= ~(1<<SDA);
	_delay_ms(I2C_T);
	USI_PORT &= ~(1<<SCL);
	_delay_ms(I2C_T);
	USI_PORT |= (1<<SDA);
}

void I2C::stop(void){
	// ��� SCL = 1, SDA = 0 -> 1
	// !!! ��� �� ����� ������� �������� ��������� ��������� SDA, ��� ������� � USIDR
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

void I2C::transmit(char data_byte){
	USISR = 0b11110000; // ������ �����, ����� ��������� ����� SCL � ������ �������� � ������������� ������� �� �������� 8 ���
	USIDR = data_byte;	// �������� ������������ ���� � ������� ��������

	do
	{
		_delay_ms(I2C_T);
		USICR |= (1<<USITC);		// ���, SCL 0 -> 1
		while(!(USI_PIN&(1<<SCL)));	// ���� �������� ������� �� SCL
		_delay_ms(I2C_T);
		USICR |= (1<<USITC);		// ���, SCL 1 -> 0, ������������� �������� ��������� SDA � ���� � USIDR
	} while (!(USISR&(1<<USIOIF))); // �� ������������ ��������
	_delay_ms(I2C_T);
}

char I2C::get_ack(void){
	USISR = 0b11111110;		// ������ �����, ����� ��������� ����� SCL � ������ �������� � ������������� ������� �� �������� 1 ����
	USIDR = 0xFF;			// ��������� ����� ������, �.�. ACK = 0
	USI_DDR &= ~(1<<SDA);	// ����� ������ ��������� �� ����
	do
	{
		_delay_ms(I2C_T);
		USICR |= (1<<USITC);			// ���, SCL 0 -> 1
		while(!(USI_PIN&(1<<SCL)));		// ���� �������� ������� �� SCL
		_delay_ms(I2C_T);
		USICR |= (1<<USITC);			// ���, SCL 1 -> 0, ������������� �������� ��������� USIDR � ���� � SDA
	} while (!(USISR&(1<<USIOIF)));
	USI_DDR |= (1<<SDA);	// ����� ������ ��������� �� �����
	char res = USIDR;		// ACK = 0xFE; NACK = 0xFF
	// !!! ����� �������� ������� ������� �������� ACK/NACK + USISR
	return (res);			// ����������� ���������� ��������
}

