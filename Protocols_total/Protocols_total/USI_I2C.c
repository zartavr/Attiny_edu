/*
 * USI_I2C.c
 *
 * Created: 25.10.2022 23:13:06
 *  Author: zartavr
 */ 

#include "USI_I2C.h"
/*-------------I2C public methods--------------*/
void I2C::init(void){
	USI_DDR |= (1<<SDA)|(1<<SCL);						// Настраиваем линии SDA и SCL на выход
	USI_PORT |= (1<<SCL)|(1<<SDA);						// Устанливаем в высокое состоние
	USIDR = 0xFF;										// Заполняем буфер приемо-передачи
	
	// Прерывания отключены, I2C без удержания SCL
	USICR = (0 << USISIE) | (0 << USIOIE) |
	(1 << USIWM1) | (0 << USIWM0) |
	(1 << USICS1) | (0 << USICS0) |(1 << USICLK) | (0 << USITC); 	// Настройка тактирования USI
	
	// Сброс флагов состояния, установка счетчика в 0
	USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF)  | (1 << USIDC)  | (0x00 << USICNT0);
}

void I2C::send(int8_t addr, int8_t &data, int8_t data_size){
	// addr = 0..127;  data - tx buffer, data_size - в байтах
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
	// При SCL = 1, SDA = 1 -> 0
	USI_PORT |= (1<<SCL);
	while(!(USI_PIN&(1<<SCL)));	// ждем высокого сигнала на SCL
	USI_PORT &= ~(1<<SDA);
	_delay_ms(I2C_T);
	USI_PORT &= ~(1<<SCL);
	_delay_ms(I2C_T);
	USI_PORT |= (1<<SDA);
}

void I2C::stop(void){
	// При SCL = 1, SDA = 0 -> 1
	// !!! Тут не очень понятно работает установка состояния SDA, оно связано с USIDR
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
	USISR = 0b11110000; // чистим флаги, чтобы отпустить линию SCL и начать передачу и устанавливаем счетчик на передачу 8 бит
	USIDR = data_byte;	// помещаем передаваемый байт в регистр передачи

	do
	{
		_delay_ms(I2C_T);
		USICR |= (1<<USITC);		// тик, SCL 0 -> 1
		while(!(USI_PIN&(1<<SCL)));	// ждем высокого сигнала на SCL
		_delay_ms(I2C_T);
		USICR |= (1<<USITC);		// так, SCL 1 -> 0, автоматически меняется состояние SDA в соот с USIDR
	} while (!(USISR&(1<<USIOIF))); // до переполнения счетчика
	_delay_ms(I2C_T);
}

char I2C::get_ack(void){
	USISR = 0b11111110;		// чистим флаги, чтобы отпустить линию SCL и начать передачу и устанавливаем счетчик на передачу 1 бита
	USIDR = 0xFF;			// Заполняем буфер данных, т.к. ACK = 0
	USI_DDR &= ~(1<<SDA);	// линия данных настроена на вход
	do
	{
		_delay_ms(I2C_T);
		USICR |= (1<<USITC);			// тик, SCL 0 -> 1
		while(!(USI_PIN&(1<<SCL)));		// ждем высокого сигнала на SCL
		_delay_ms(I2C_T);
		USICR |= (1<<USITC);			// так, SCL 1 -> 0, автоматически меняется состояние USIDR в соот с SDA
	} while (!(USISR&(1<<USIOIF)));
	USI_DDR |= (1<<SDA);	// линия данных настроена на выход
	char res = USIDR;		// ACK = 0xFE; NACK = 0xFF
	// !!! Можно добавить возврат статуса передачи ACK/NACK + USISR
	return (res);			// Возвращение результата передачи
}

