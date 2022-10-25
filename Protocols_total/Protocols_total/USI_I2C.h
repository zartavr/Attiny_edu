/* 
 * 25.10.2022 
 * Первая попытка оформить библиотеку периферии в ООП
 * Разрабатывалась под Attiny2313A с другими МК врядли будет работать
 */

// TODO: Нужны ли дефайны
/*
 * #define __AVR_AT90Tiny2313__
 * #define F_CPU 1000000UL	      // Sets up the default speed for delay.h
 * #include <util/delay.h>
 */
#include <avr/io.h>

//---------------------- USI I2C----------------------------//
// Регистры состояния линии данных SDA и тактирования SCL
#define USI_DDR DDRB
#define USI_PORT PORTB
#define USI_PIN PINB
#define I2C_T	5		// Задержка при переключении состояния линий

//---------------------- Распиновка-------------------------//
#define SDA 5			// Линия данных
#define SCL 7			// Линия тактирования


class I2C {
public:
	void init (void);
	void send (int8_t addr, int8_t &data, int8_t data_size);
	// addr = 0..127;  data - tx buffer, data_size - в байтах	
		
private:
	void start(void);
	void transmit(char data_byte);
	char get_ack(void);
	void stop(void);
};