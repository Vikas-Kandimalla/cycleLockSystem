/*
 * sx1272fifo.h
 *
 * Created: 10-04-2018 00:04:59
 *  Author: vikas
 */ 


#ifndef SX1272FIFO_H_
#define SX1272FIFO_H_

#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "spi.h"
#include "usart.h"
#include "error.h"
#include "sx1272LoRa.h"



static	uint8_t addrPtr;
static	uint8_t txBasePtr;
static	uint8_t rxBasePtr;
static	uint8_t rxCurrentAddr;
static	uint8_t rxBytesNb;
static	uint8_t rxByteAddrPtr;
static	uint16_t validHeaderCnt;
static	uint16_t validPacketCnt;
static	uint16_t rxSymbTimeout;
static	uint16_t txSymbTimeout;
static	uint16_t preambleLength;
static	uint8_t payloadLength;
static	uint8_t payloadMaxlength;
static	uint8_t irqFlagMask;
static	uint8_t irqFlags;


	
uint8_t	getMask(uint8_t maskNum){

	return ((irqFlagMask >> maskNum) & 0x01);

}

void setMask(uint8_t maskNum,uint8_t mask){
	
	irqFlagMask = readRegister(REG_IRQ_FLAGS_MASK);	
		
	if (mask)
		irqFlagMask |= (1 << maskNum);
	else
		irqFlagMask &= ~(1 << maskNum);
	
	setValidState(50);		
	writeRegister(REG_IRQ_FLAGS_MASK,irqFlagMask);
	_delay_us(100);
	if (irqFlagMask != readRegister(REG_IRQ_FLAGS_MASK)){
		// set Error in error class;
		ErrorSetMsg(REGISTER_WRITE_MISMATCH_ERROR,"Flags Masks are not equal");
		return;
	}
	else{
		ErrorSet(NO_ERROR);
		return;
	}
}

void unMaskAll() {

	irqFlagMask = 0x00;
	writeRegister(REG_IRQ_FLAGS_MASK,irqFlagMask);
	_delay_us(100);
	if (irqFlagMask != readRegister(REG_IRQ_FLAGS_MASK)){
		// set Error in error class;
		ErrorSetMsg(REGISTER_WRITE_MISMATCH_ERROR,"Cannot clear all flag masks");
		return;
	}
	else{
		ErrorSet(NO_ERROR);
		return;
	}
}

void maskAll() {
	
	irqFlagMask = 0xFF;
	writeRegister(REG_IRQ_FLAGS_MASK,irqFlagMask);
	_delay_us(100);
	if (irqFlagMask != readRegister(REG_IRQ_FLAGS_MASK)){
		// set Error in error class;
		ErrorSetMsg(REGISTER_WRITE_MISMATCH_ERROR,"Cannot set all flags");
		return;
	}
	else{
		ErrorSet(NO_ERROR);
		return;
	}
}


	
uint8_t getFlag(uint8_t flagNum){
			
	irqFlags = readRegister(REG_IRQ_FLAGS);
	return ((irqFlags >> flagNum) & 0x01);
}

void clearFlag(uint8_t flagNum) {
			
	//flags = readRegister(REG_IRQ_FLAGS);
	writeRegister(REG_IRQ_FLAGS,  (1 << flagNum));
	_delay_us(10);
	irqFlags = readRegister(REG_IRQ_FLAGS);
	if (  ((irqFlags >> flagNum) & 0x01)) {
		ErrorSet(30 + flagNum);
		return;
	}
			
	ErrorSet(NO_ERROR);
	return;
}
		
void clearAll() {

	writeRegister(REG_IRQ_FLAGS,0xFF);
	irqFlags = readRegister(REG_IRQ_FLAGS);
	_delay_us(100);
	if (  irqFlags != 0x00) {
		ErrorSet(REGISTER_WRITE_MISMATCH_ERROR);
		return;
	}
			
	ErrorSet(NO_ERROR);
	return;
}

void sync() {
		
	setValidFIFO();
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return;
	}

	addrPtr = readRegister(REG_FIFO_ADDR_PTR);
	txBasePtr = readRegister(REG_FIFO_TX_BASE_ADDR);
	rxBasePtr = readRegister(REG_FIFO_TX_BASE_ADDR);
	rxCurrentAddr = readRegister(REG_FIFO_RX_CURRENT_ADDR);
	rxBytesNb = readRegister(REG_RX_NB_BYTES);
	rxByteAddrPtr = readRegister(REG_FIFO_RX_BYTE_ADDR);
	validHeaderCnt = 0x0000;
	validHeaderCnt |= readRegister(REG_RX_HEADER_CNT_VALUE_LSB);
	validHeaderCnt |= (readRegister(REG_RX_HEADER_CNT_VALUE_MSB)  << 8);
	validPacketCnt = 0;
	validPacketCnt |= readRegister(REG_RX_PACKET_CNT_VALUE_LSB);
	validPacketCnt |= (readRegister(REG_RX_PACKET_CNT_VALUE_MSB)  << 8);
	rxSymbTimeout = 0x0000;
	rxSymbTimeout |= ((readRegister(REG_MODEM_CONFIG2) << 8) & 0x03);
	rxSymbTimeout |= readRegister(REG_SYMB_TIMEOUT_LSB);
	txSymbTimeout = 0xFFFF;
	payloadLength = readRegister(REG_PAYLOAD_LENGTH);
	payloadMaxlength = readRegister(REG_MAX_PAYLOAD_LENGTH);
		
	return;
}

uint8_t getFifoAddrPtr() {

	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return 0;
	}
	uint8_t temp = readRegister(REG_FIFO_ADDR_PTR);
	ErrorSet(NO_ERROR);
	return temp;
}

void setFifoAddrPtr(uint8_t value){
		
	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return;
	}
	addrPtr = value;
	writeRegister(REG_FIFO_ADDR_PTR,addrPtr);
		
	if ( addrPtr != readRegister(REG_FIFO_ADDR_PTR)){
		ErrorSet(REGISTER_WRITE_MISMATCH_ERROR);
		return;
	}
		
	ErrorSet(NO_ERROR);
	return;
		
}

uint8_t getFifoTxBaseAddr() {
	return readRegister(REG_FIFO_TX_BASE_ADDR);
}

void setFifoTxBaseAddr(uint8_t value){
		
	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return ;
	}
	txBasePtr = value;
	writeRegister(REG_FIFO_TX_BASE_ADDR,txBasePtr);
	
	if ( txBasePtr != readRegister(REG_FIFO_TX_BASE_ADDR)){
		ErrorSet(REGISTER_WRITE_MISMATCH_ERROR);
		return;
	}
		
	ErrorSet(NO_ERROR);
	return;
		
}

uint8_t getFifoRxBaseAddr() {
	return readRegister(REG_FIFO_RX_BASE_ADDR);
}

void setFifoRxBaseAddr(uint8_t value){
	
	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return ;
	}
	rxBasePtr = value;
	writeRegister(REG_FIFO_RX_BASE_ADDR,rxBasePtr);
		
	if ( rxBasePtr != readRegister(REG_FIFO_RX_BASE_ADDR)){
		ErrorSet(REGISTER_WRITE_MISMATCH_ERROR);
		return;
	}
		
	ErrorSet(NO_ERROR);
	return;
		
}

uint8_t getFifoRxCurrentAddr() {
	
	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return 0;
	}
	uint8_t temp = readRegister(REG_FIFO_RX_CURRENT_ADDR);
	ErrorSet(NO_ERROR);
	return temp;
}

uint8_t getRxNbBytes() {

	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return 0;
	}
	ErrorSet(NO_ERROR);
	return readRegister(REG_RX_NB_BYTES);
	}

uint16_t getRxHeaderCnt() {

	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return 0;
	}
	uint16_t temp = 0x0000;
	uint8_t value = readRegister(REG_RX_HEADER_CNT_VALUE_MSB);
	temp = (value << 8);
	value = readRegister(REG_RX_HEADER_CNT_VALUE_LSB);
	temp |= value;
	ErrorSet(NO_ERROR);
	return temp;
}

uint16_t getRxPacketCnt() {
		
	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return 0;
	}
	uint16_t temp = 0x0000;
	uint8_t value = readRegister(REG_RX_PACKET_CNT_VALUE_MSB);
	temp = (value << 8);
	value = readRegister(REG_RX_PACKET_CNT_VALUE_LSB);
	temp |= value;
	ErrorSet(NO_ERROR);
	return temp;
}

uint16_t getSymbTimeout() {
		
	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return 0;
	}
	uint16_t temp = 0x0000;
	uint8_t value;
	temp = (uint16_t) readRegister(REG_MODEM_CONFIG2);
	temp = (temp << 8) & 0x03FF;
	value = readRegister(REG_SYMB_TIMEOUT_LSB);
	temp |= (uint16_t)value;
	ErrorSet(NO_ERROR);
	return temp;
}

void setSymbTimeout(uint16_t value){
		
	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return;
	}
	uint8_t temp,i;
	temp = ((value >> 8) & 0x03);
	i = readRegister(REG_MODEM_CONFIG2);
	i &= 0B11111100;
	i |= temp;
		
	writeRegister(REG_MODEM_CONFIG2,i);
	temp = (uint8_t) value;
	writeRegister(REG_SYMB_TIMEOUT_LSB,temp);
	ErrorSet(NO_ERROR);
	return;
}
	
void setPreambleLength(uint16_t value) {
		
	setValidState(50);
	if (ErrorGet() != NO_ERROR){
		ErrorSet(PREAMBLE_SET_ERROR);
		return;
	}
	
	uint8_t temp = (uint8_t) value;
	writeRegister(REG_PREAMBLE_LSB,temp);
	temp = (uint8_t)(value >> 8);
	writeRegister(REG_PREAMBLE_MSB,temp);
	preambleLength = value;
	ErrorSet(NO_ERROR);
	return;
}

uint16_t getTxTimeout() {
	return txSymbTimeout;
}

void setTxTimeout(uint16_t wait){
	txSymbTimeout = wait;
	return;
}

uint8_t getPayloadLength() {
	setValidFIFO();
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return 0;
	}
	uint8_t temp = readRegister(REG_PAYLOAD_LENGTH);
	ErrorSet(NO_ERROR);
	return temp;
}

void setPayloadLength(uint8_t value){
	setValidFIFO();
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return ;
	}
	writeRegister(REG_PAYLOAD_LENGTH,value);
	ErrorSet(NO_ERROR);
	return;
		
}

uint8_t getPayloadMaxLength() {

	setValidFIFO();
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return 0;
	}
	uint8_t temp = readRegister(REG_MAX_PAYLOAD_LENGTH);
	ErrorSet(NO_ERROR);
	return temp;
}

void setPayloadMaxLength(uint8_t value){
	setValidFIFO();
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return ;
	}
	writeRegister(REG_MAX_PAYLOAD_LENGTH,value);
	ErrorSet(NO_ERROR);
	return;
		
}

uint8_t getfifoRxByteAddrPtr(){
	uint8_t st0 = readRegister(REG_OP_MODE);
	setValidFIFO();
	if (ErrorGet() != NO_ERROR){
		ErrorSet(LORA_MODE_SET_ERROR);
		return 0;
	}
		
	uint8_t value = readRegister(REG_FIFO_RX_BYTE_ADDR);
		
	writeRegister(REG_OP_MODE,st0);
	ErrorSet(NO_ERROR);
	return value;
}



#endif /* SX1272FIFO_H_ */