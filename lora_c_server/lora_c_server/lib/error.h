/*
 * error.h
 *
 * Created: 27-03-2018 18:39:18
 *  Author: vikas
 */ 


#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>


#define NO_ERROR						0x00
#define REGISTER_READ_ERROR				0x01
#define REGISTER_WRITE_WRROR			0x02
#define REGISTER_WRITE_MISMATCH_ERROR	0x03
#define MALLOC_ZERO_LENGTH				0x04
#define LORA_MODE_SET_ERROR				0x05
#define INVALID_PACKET_SIZE				0x06
#define SLEEP_MODE_ERROR				0x07
#define LORA_MODE_SET_TIMEOUT			0x08
#define UNKNOWN_STATE_ERROR				0x09
#define LORA_MODE_ERROR					0x0A
#define FREQUENCY_SET_ERROR				0x0B
#define MODEM_CONFIG_ERROR				0x0C
#define LORA_MODEM_BUSY					0x0D
#define PREAMBLE_SET_ERROR				0x0E
#define TX_INIT_ERROR					0x0F
#define SET_STATE_TIMEOUT				0x10


static uint8_t er;
	

void ErrorSet(uint8_t k){
		er = k;
		if (er)
			printf("ERROR: Error code : %d",k);
		return;
}

void ErrorSetMsg(uint8_t k, char* msg){
	er = k;
	if (er)
	printf("ERROR: Error code : %02x, Error message : %s\n",k,msg);
	return;
}


uint8_t ErrorGet() {
		
		uint8_t temp = er;
		er = 0;
		return temp;
}



#endif /* ERROR_H_ */