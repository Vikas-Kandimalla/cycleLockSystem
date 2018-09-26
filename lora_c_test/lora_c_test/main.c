/*
 * lora_c_test.c
 *
 * Created: 12-04-2018 21:31:05
 * Author : vikas
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "lib/error.h"
#include "lib/spi.h"
#include "lib/LoRaRegisters.h"
#include "lib/typeDefs.h"
#include "lib/sx1272LoRa.h"
#include "lib/sx1272fifo.h"
#include "lib/sx1272TxRx.h"
#include "lib/millis.h"
#include "lib/packetDefs.h"
#include "lib/keypad.h"


#define BAUD 9600
#define MYUBRR ((F_CPU/16/BAUD) - 1)


void authenticate();
bool keyTimedout = true;
bool authenticated = false;
bool firstKeyUpdate = false;


Packet ACK;
uint8_t key[4] = {'0','0','0','0'};
uint8_t len;
uint8_t sequence = 0x00;
uint8_t destination = 0x97;


unsigned long keyTimeout = 10;


ISR(PCINT1_vect){
	uint8_t sreg = SREG;
	authenticate();
	SREG = sreg;
	PCIFR = 0x02;
	return;
}


void rdtSend(Packet *pack) {
	
	send(pack);
	_NOP();
	
	bool ackReceived = false;
	do {
		
		receive(1023);
		
		switch(udtState){
			case UDT_RX_PACKET_CORRUPT:
			send(pack);
			
			break;
			case UDT_RX_TIMEOUT: {
				send(pack);
			}
			break;
			case UDT_RX_PACKET_RECEIVED:{
				if (receivedPacket.type == PACKET_ACK && receivedPacket.seq == pack->seq){
					ackReceived = true;
				}
			}
		}
	}while(!ackReceived);
	
}

void rdtReceive() {
	
	bool packetReceived = false;
	
	do{
		receive(500);
		switch(udtState){
			case UDT_RX_PACKET_RECEIVED:{
			
				if(receivedPacket.type != PACKET_ACK) {
					send(ACK_PACKET(destination,PACKET_ACK,sequence));
					packetReceived = true;
				}
			}
			break;
		}
	}while(!packetReceived);
}

void rdtKeyReceiveUntil(uint8_t con) {
	
	bool packetReceived = false;
	
	do{
		receive(500);
		switch(udtState){
			case UDT_RX_PACKET_RECEIVED:{
				if(receivedPacket.type != PACKET_ACK && receivedPacket.type == con) {
					send(ACK_PACKET(receivedPacket.src,PACKET_ACK,receivedPacket.seq));
					printf("ACK sent.\n");
					packetReceived = true;
				}
				
				packetReceived = true;
			}
			break;
		}
	}while(!packetReceived);
}

void updateKey() {
	
	
	Packet *pack = KEY_REQUEST_PACKET(destination,sequence);
	printf("Packet Key Request sent.\n");
	send(pack);
	_NOP();
	
	bool ackReceived = false;
	do {
		
		receive(1023);
		
		switch(udtState){
			case UDT_RX_PACKET_CORRUPT:
				printf("Packet Key Request sent.\n");
				send(pack);
			
			break;
			case UDT_RX_TIMEOUT: {
				printf("Packet Key Request sent.\n");
				send(pack);
			}
			break;
			case UDT_RX_PACKET_RECEIVED:{
				if (receivedPacket.ack == PACKET_DATA_ACK && receivedPacket.seq == pack->seq){
					
					if(receivedPacket.type == PACKET_KEY){
						printf("Key received : ");
						len = receivedPacket.length;
						for (int i = 0; i < receivedPacket.length; i++){
							key[i] = (char)receivedPacket.data[i];
							printf("%c",key[i]);
						}
						printf("\n");
						ackReceived = true;
					}
					
				}
			}
		}
	}while(!ackReceived);
	
	sequence++;
	
	pack = KEY_ACCEPT_PACKET(destination,sequence,(uint8_t*) key, len);
	
	printf("Key accept packet sent.\n");
	send(pack);
	_NOP();
	
	ackReceived = false;
	do {
		
		receive(1023);
		
		switch(udtState){
			case UDT_RX_PACKET_CORRUPT:
			printf("Key accept packet sent.\n");
			send(pack);
			
			break;
			case UDT_RX_TIMEOUT: {
				printf("Key accept packet sent.\n");
				send(pack);
			}
			break;
			case UDT_RX_PACKET_RECEIVED:{
				if (receivedPacket.type == PACKET_ACK && receivedPacket.seq == pack->seq){
					printf("Packet Accept ACK received.\n");
					ackReceived = true;
				}
			}
		}
	}while(!ackReceived);
	
}
void authenticate() {
	
	printf("\nEnter the pass code : ");
	int i = 0;
	char a[4]="0000";
	char temp;
	uint8_t flag = 1;
	for (i = 0; i>=0&&i<=4;)
	{
		
		temp=getReliableChar();
		if(temp!='R' && temp!='E')
		{
			printf("%c",temp);
			a[i]=temp;
			i++;
		}
		else if(temp=='R')
		
		{
			i--;
			printf("\b");
		}
		else if(temp=='E')
		{
			break;
		}
	}
	for(i=0;i<4;i++)
	{
		if(a[i]==key[i])
		{
			flag&=1;
		}
		else
		{
			flag&=0;
		}
	}
	
	if (flag){
		printf("\nkey is correct.\n");
		authenticated = true;
		bitSet(PORTC,4);
		
		
	}
	
	else {
		printf("\nIncorrect key.\n");
		authenticated = false;
		bitSet(PORTC,5);
	}
	
}

int main(void)
{
	
	
	USART_Init(MYUBRR);
	SPIbegin(SPI_CLOCK_DIV4,MSBFIRST,SPI_MODE0);
	millis_init();
	receivedPacket.data = 0;
	unsigned long min = minutes();
	keyTimedout = true;
	uint8_t version = 0x00;
	do {
		version = readRegister(REG_VERSION);
		printf("Lora's version is %02x\n",version);
		_delay_ms(100);
	}while(version != 0x22);
	sx1272init();
	sync();
	txInit(modemConfig,CH_17_868,5,400,500);
	rxInit(modemConfig,400,500);
	
	setLnaGain(LNA_BOOST,LNA_MAX_GAIN);
	printf("LNA BOOST : ON, LNA GAIN : MAX\n");
	setPower(13);
	printf("Power : 13dB\n");
	
	keypad_init();
	PCICR = 0x00;
	sei();
	
	bitSet(DDRC,3);
	bitSet(DDRC,4);
	bitSet(DDRC,5);
	bitSet(DDRD,2);
	bitSet(DDRD,3);
	
	/*
	while(1){
		temp = KEY_REQUEST_PACKET(destination,sequence++);
		rdtSend(temp);
		printf("Packet sent. seq : %d\n",temp->seq);
	}
	*/
	
	
	
	while (1) 
    {
		
		
		
		if (minutes() - min > keyTimeout){
			keyTimedout = true;
			min = minutes();
		}
		
		if (keyTimedout || authenticated){
			
			if (authenticated){
				bitSet(PORTD,2);
				_delay_ms(1000);
				bitClear(PORTD,2);
			}
			
			printf("Updating key\n");
			updateKey();
			printf("Ready to authenticate.\n");
			
			firstKeyUpdate = true;
			keyTimedout = false;
			authenticated = false;
			
		}
		
		bitClear(PORTC,4);
		bitClear(PORTC,5);
		bitSet(PORTC,3);	
		_delay_ms(20);
		PCICR = 0x02;
		
		_delay_ms(500);
		PCICR = 0x00;
		bitClear(PORTC,3);
			
	}
	
	SPIend();
}

