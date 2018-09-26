/*
 * spi.h
 *
 * Created: 22-03-2018 12:00:52
 *  Author: vikas
 */ 




#ifndef SPI_H_
#define SPI_H_


#include <avr/io.h>
#include <avr/power.h>
#include <avr/common.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <stdlib.h>
#include "usart.h"
#include <stdio.h>

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#define DDR_SPI DDRB
#define SPI_PORT PORTB
#define SS	2
#define MOSI 3
#define MISO 4
#define SCK  5



#define SPI_CLOCK_DIV4 0x00			// clock Div = 0;
#define SPI_CLOCK_DIV16 0x01		// clock Div = 1;
#define SPI_CLOCK_DIV64 0x02		// clock Div = 2;
#define SPI_CLOCK_DIV128 0x03		// clock Div = 3;
#define SPI_CLOCK_DIV2 0x04			// clock Div = 4;
#define SPI_CLOCK_DIV8 0x05			// clock Div = 5;
#define SPI_CLOCK_DIV32 0x06		// clock Div = 7;

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C		// Used for LoRa

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR


#define bitRead(value, bitno) (((value) >> (bitno)) & 0x01)  // read a bit
#define bitSet(value, bitno) ((value) |= (1UL << (bitno)))    // set bit to '1'
#define bitClear(value, bitno) ((value) &= ~(1UL << (bitno))) // set bit to '0'


void SPIbegin(uint8_t clockDiv, uint8_t bitOrder, uint8_t dataMode) {
	
power_spi_enable();							// Enables the SPI module

							// Disable the interrupts for the following operations
//DDRB |= ((1 << SS) | (1 << SCK) | (1 << MOSI));

uint8_t spcr,spsr;
spcr = _BV(SPE) | _BV(MSTR) | ((bitOrder == LSBFIRST) ? _BV(DORD) : 0) |
(dataMode & SPI_MODE_MASK) | ((clockDiv) & SPI_CLOCK_MASK);
spsr = (clockDiv >> 2) & SPI_2XCLOCK_MASK;

bitSet(DDR_SPI,SS);
bitSet(DDR_SPI,SCK);
bitSet(DDR_SPI,MOSI);
bitClear(DDR_SPI,MISO);

bitSet(SPI_PORT,SS);						// Make SS pin high so that no data transfer takes place
SPSR = spsr;
SPCR = spcr;

printf("Settings of SPSR : %02x\n",SPSR);
printf("Settings of SPCR : %02x\n",SPCR);

}

inline uint8_t transfer(uint8_t data){
	
	bitClear(SPI_PORT,SS);				// start the data transfer
	_NOP();         
	SPDR = data;
	_NOP();
	while (!(SPSR & _BV(SPIF))) ;	// wait until SPIF becomes 1
	bitSet(SPI_PORT,SS);
	_NOP();
	return SPDR;
	
}

inline uint8_t writeRegister(uint8_t address,uint8_t data){
	
	cli();
	bitSet(address,7); // Set the WNR bit to 1;
	bitClear(SPI_PORT,SS);
	SPDR = address;
	_NOP();
	while(!(SPSR & _BV(SPIF)));
	SPDR = data;
	_NOP();
	while(!(SPSR & _BV(SPIF)));
	bitSet(SPI_PORT,SS);
	sei();
	return SPDR;
	
}

inline uint8_t readRegister(uint8_t address){
	
	cli();
	bitClear(address,7);
	bitClear(SPI_PORT,SS);
	SPDR = address;
	_NOP();
	while(!(SPSR & _BV(SPIF)));
	SPDR = 0xDD;
	_NOP();
	while(!(SPSR & _BV(SPIF)));
	bitSet(SPI_PORT,SS);
	
	sei();
	
	return SPDR;
}

inline uint8_t* readFiFo(uint8_t len){
	
	uint8_t* buf = (uint8_t*) malloc(sizeof(uint8_t) * len);
	cli();
	SPDR = 0x00;
	bitClear(SPI_PORT,SS);
	 _NOP();
	while(!(SPSR & _BV(SPIF)));
	for (uint8_t i = 0; i < len; i++){
	 SPDR = 0x00;
	 while(!(SPSR & _BV(SPIF)));
	 buf[i] = SPDR;
	}
	sei();
	bitSet(SPI_PORT,SS);
	return buf;
}

inline void writeFiFo(uint8_t* buf, uint8_t len){
	
	uint8_t address = 0x80;
	cli();
	SPDR = address;
	bitClear(SPI_PORT,SS);
	while(!(SPSR & _BV(SPIF)));
	for (uint8_t i = 0; i < len; i++){
		SPDR = buf[i];
		while(!(SPSR & _BV(SPIF)));
	}
	sei();
	bitSet(SPI_PORT,SS);
	return;
}

void SPIend(){
	
		SPCR &= ~_BV(SPE);
		power_spi_disable();	
}

#endif /* SPI_H_ */