/*
 * sx1272TxRx.h
 *
 * Created: 27-03-2018 15:14:02
 *  Author: vikas
 */ 


#ifndef SX1272TXRX_H_
#define SX1272TXRX_H_

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/cpufunc.h>

#include "spi.h"
#include "sx1272LoRa.h"
#include "sx1272fifo.h"
#include "error.h"
#include "millis.h"


#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80
#define RFLR_IRQFLAGS_RXDONE                        0x40
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10
#define RFLR_IRQFLAGS_TXDONE                        0x08
#define RFLR_IRQFLAGS_CADDONE                       0x04
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02
#define RFLR_IRQFLAGS_CADDETECTED                   0x01

#define RFLR_DIOMAPPING1_DIO0_MASK                  0x3F
#define RFLR_DIOMAPPING1_DIO0_00                    0x00
#define RFLR_DIOMAPPING1_DIO0_01                    0x40

#define	RX_TIMEOUT									7
#define RX_DONE										6
#define PAYLOAD_CRC_ERROR							5
#define VALID_HEADER								4
#define TX_DONE										3	
#define CADDONE										2
#define FHSS_CHANGE_CHANNEL							1
#define CAD_DETECTED								0

#define SOURCE_ADDR									0x45


#define DETECTION_OPTIMIZE_MASK                 0xF8
#define DETECTION_OPTIMIZE_SF7_TO_SF12          0x03 // Default
#define DETECTION_OPTIMIZE_SF6                  0x05



#define DETECTION_THRESH_SF7_TO_SF12            0x0A // Default
#define DETECTION_THRESH_SF6                    0x0C



///////////////////////////////////////////////////


#define  UDT_RX_PACKET_RECEIVED		0
#define UDT_RX_PACKET_CORRUPT		1
#define UDT_RX_TIMEOUT				2
#define UDT_TX_DONE					3
#define UDT_TX_RUNNING				4
#define UDT_RX_RUNNING				5 
#define UDT_IDLE					6


const unsigned int headerSize = 6;
const uint8_t SRC_ADDRESS = 0x97;
unsigned long min;
static Packet receivedPacket;
static int udtState = UDT_IDLE;
static bool modemBusy = false;



void Tx(uint32_t);


uint8_t getPacketSize(Packet *p){
	return p->length + headerSize;
}

void txInit(LoRaModemConfig modemConfig,uint32_t frequency,uint8_t power,uint16_t preambleLength, uint16_t timeout) {
	
	setValidState(50);
	 if( modemConfig.spreadingFactor > 12 )
	 {
		 modemConfig.spreadingFactor = 12;
	 }
	 else if( modemConfig.spreadingFactor < 6 )
	 {
		 modemConfig.spreadingFactor = 6;
	 }
	 
	 if( ( ( modemConfig.bandwidth == 0 ) && ( ( modemConfig.spreadingFactor == 11 ) || ( modemConfig.spreadingFactor == 12 ) ) ) ||
	 ( ( modemConfig.bandwidth == 1 ) && ( modemConfig.spreadingFactor == 12 ) ) )
	 {
		 modemConfig.lowDataRateOptimize = 0x01;
	 }
	 else
	 {
		modemConfig.lowDataRateOptimize = 0x00;
	 }
	 
	 setModemConfig(modemConfig);
	 
	 if (ErrorGet() != NO_ERROR){
		 ErrorSet(TX_INIT_ERROR);
		 return;
	 }
	 
	 setFrequency(frequency);
	 setPower(power);
	 setPreambleLength(preambleLength);
	 setTxTimeout(timeout);
	 
	 if (modemConfig.spreadingFactor == SF_6){
		 setDetectionOptimize(DETECTION_OPTIMIZE_SF6 & DETECTION_OPTIMIZE_MASK);
		 setDetectionThreshold(DETECTION_THRESH_SF6);
	 }
	 else{
		 setDetectionOptimize(DETECTION_OPTIMIZE_SF7_TO_SF12 & DETECTION_OPTIMIZE_MASK);
		 setDetectionThreshold(DETECTION_THRESH_SF7_TO_SF12);
	 }
	 
	 ErrorSet(NO_ERROR);
	 
	 return;
	
}

void rxInit(LoRaModemConfig modemConfig,uint16_t preambleLength,uint16_t symbTimeout){
	
	if( modemConfig.spreadingFactor > 12 )
	{
		modemConfig.spreadingFactor = 12;
	}
	else if( modemConfig.spreadingFactor < 6 )
	{
		modemConfig.spreadingFactor = 6;
	}
	
	if( ( ( modemConfig.bandwidth == 0 ) && ( ( modemConfig.spreadingFactor == 11 ) || ( modemConfig.spreadingFactor == 12 ) ) ) ||
	( ( modemConfig.bandwidth == 1 ) && ( modemConfig.spreadingFactor == 12 ) ) )
	{
		modemConfig.lowDataRateOptimize = 0x01;
	}
	else
	{
		modemConfig.lowDataRateOptimize = 0x00;
	}
	
	setModemConfig(modemConfig);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(TX_INIT_ERROR);
		return;
	}
	
	setPreambleLength(preambleLength);
	setSymbTimeout(symbTimeout);
	
	ErrorSet(NO_ERROR);
	return;
}

void send(Packet *pack)
{
			
		//printf("Send() called.\n");
		
		
		uint32_t txTimeout = 0;
		checkValidState(50);
		
		if (ErrorGet() != NO_ERROR){
			ErrorSet(LORA_MODE_SET_ERROR);
			return;
		}		
		// Initializes the payload size
			
		uint8_t size = getPacketSize(pack);
		if ( size > getPayloadMaxLength()){
			ErrorSetMsg(INVALID_PACKET_SIZE,"Packet Size larger than max allowed length");
		}
		
		setPayloadLength(size);
		setFifoTxBaseAddr(0x00);
		setFifoAddrPtr(0x00);
	
		// FIFO operations can not take place in Sleep mode
		setValidFIFO();	
			
		writeRegister(REG_FIFO,pack->dst);
		writeRegister(REG_FIFO,pack->type);
		writeRegister(REG_FIFO,pack->ack);
		writeRegister(REG_FIFO,pack->src);
		writeRegister(REG_FIFO,pack->length);
		writeRegister(REG_FIFO,pack->seq);
		writeFiFo( (uint8_t*)pack->data,pack->length);
		
		txTimeout = getTxTimeout();
		Tx(txTimeout);
			
}

void Tx( uint32_t timeout )
{
			
	// Clear all flags	
	clearAll();
	// mask all flags
	maskAll();
	// unmask only TX_DONE
	setMask(TX_DONE,0);
	// DIO0=TxDone
	writeRegister( REG_DIO_MAPPING1, 0x40 );
	modemBusy = true;
	setOpMode(LORA_TX_MODE);
	
	while (! getFlag(TX_DONE)){
		_NOP();
	}
	clearFlag(TX_DONE);
	
	return;
}

void Rx(uint16_t timeout){
	
	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return;
	}
	
	clearAll();
	maskAll();
	
	setMask(RX_TIMEOUT,0);
	setMask(RX_DONE,0);
	setMask(PAYLOAD_CRC_ERROR,0);
	setMask(VALID_HEADER,0);
	
	// DIO0=RxDone
	writeRegister( REG_DIO_MAPPING1, 0x40);
	
	do {
	setFifoAddrPtr(0x00);
	}while(getFifoAddrPtr() != 0x00);
	
	do {
	setFifoRxBaseAddr(0x00);
	}while(getFifoAddrPtr() != 0x00);
	
	
	
	setSymbTimeout(timeout);
	udtState = UDT_RX_RUNNING;
	modemBusy = true;
	min = millis();
	
	setOpMode(LORA_RX_MODE);
	return;
}

bool receive(uint16_t timeout){
	
	
	Rx(timeout);
	
	unsigned long t = millis();
	while( (getFlag(RX_DONE) == 0) && ((millis() - t) < timeout) ){
		
	}  // wait while RX is DONE or RX is timed out; blocking function
		
		
	if (getFlag(RX_TIMEOUT)){
		// RX is timed out restart the RX phase 
		// set the error and exit
		// RETURN NULL
		clearFlag(RX_TIMEOUT);
		udtState = UDT_RX_TIMEOUT;
		return false;
	}
	else if (getFlag(RX_DONE)) {
		
		if (modemConfig.rxPayloadCrc){
				
			if (!getFlag(VALID_HEADER)){
					// NOT A VALID HEADER
					// SET THE ERROR FUNCTION 
					// RETURN NULL
					// 
					udtState = UDT_RX_PACKET_CORRUPT;
					printf("Error: Packet corrupt.\n");
					return false;
			}
			
			
			if (getFlag(PAYLOAD_CRC_ERROR)){
				// NOT A VALID PACKET
				// SET THE ERROR FUNCTION
				// RETURN NULL
				clearFlag(PAYLOAD_CRC_ERROR);
				udtState = UDT_RX_PACKET_CORRUPT;
				printf("Error: Packet corrupt.\n");
				return false;
			}
		}
			
			
		// NO ERRORS SO FAR
		
		if (receivedPacket.data != 0)
			free(receivedPacket.data);
		
		uint8_t size =			readRegister(REG_RX_NB_BYTES);
		receivedPacket.dst =	readRegister(REG_FIFO);
		receivedPacket.type =	readRegister(REG_FIFO);
		receivedPacket.ack =	readRegister(REG_FIFO);
		receivedPacket.src =	readRegister(REG_FIFO);
		receivedPacket.length = readRegister(REG_FIFO);
		receivedPacket.seq =	readRegister(REG_FIFO);
		
		if (size != receivedPacket.length + headerSize) {
			printf("Warning: Length Mismatch Warning.\n");
			clearFlag(RX_DONE);
			clearFlag(VALID_HEADER);
			udtState = UDT_RX_PACKET_CORRUPT;
			receivedPacket.data = 0;
			return false;
		}
		else {
		
		receivedPacket.data = readFiFo(receivedPacket.length);
		clearFlag(RX_DONE);
		clearFlag(VALID_HEADER);
		
		udtState = UDT_RX_PACKET_RECEIVED;
			
		return true;
		}
			
	}
		
	return false;
}







#endif /* SX1272TXRX_H_ */