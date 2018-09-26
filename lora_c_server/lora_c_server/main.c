/*
 * lora_c_test.c
 *
 * Created: 12-04-2018 21:31:05
 * Author : vikas
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
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


#define BAUD 9600
#define MYUBRR ((F_CPU/16/BAUD) - 1)

Packet ACK;

char key[] = {'0', '1' , '2', '3'};
uint8_t len;
uint8_t sequence = 50;

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





int main(void)
{
	
	USART_Init(MYUBRR);
	SPIbegin(SPI_CLOCK_DIV4,MSBFIRST,SPI_MODE0);
	millis_init();
	receivedPacket.data = 0;
	
	char a = '0';
	printf("Press Enter to continue.\n");
	scanf("%c",&a);
	uint8_t version = 0x00;
	do {
		version = readRegister(REG_VERSION);
		printf("Lora's version is %02x\n",version);
		_delay_ms(100);
	}while(version != 0x22);
	
	printf("Press Enter to continue.\n");
	scanf("%c",&a);
	
	sx1272init();
	
	printf("Press Enter to continue.\n");
	scanf("%c",&a);
	
	sync();
	
	printf("FIFO registers sync complete.\nPress Enter to continue.\n");
	scanf("%c",&a);
	
	txInit(modemConfig,CH_17_868,5,400,500);
	rxInit(modemConfig,400,500);
	setLnaGain(LNA_BOOST,LNA_MAX_GAIN);
	printf("LNA BOOST : ON, LNA GAIN : MAX\n");
	setPower(13);
	printf("Power : 13dB\n");
    /* Replace with your application code */
    
	
	
	/*
	while(1){
		rdtReceive();
		if (udtState == UDT_RX_PACKET_RECEIVED){
			printf("Packet src : %02x, packet type : %02x, packet seq : %02x\n",receivedPacket.src,receivedPacket.type,receivedPacket.seq);
		}
	}
	*/
	
	
	
	bool keySent = false;
	
	while(1) {
		
		if (keySent == false) {
			
			printf("KEYREQUEST:: Enter a key.\n");
			for (int i = 0; i < 4; i++){
				scanf("%c", key+i);
				printf("%c",key[i]);
			}
			
			printf("\n");
			keySent = true;
			len = 4;
		}
			
			receive(1000);
			switch(udtState){
				
				case UDT_RX_PACKET_CORRUPT:
				
				
				break;
				
				case UDT_RX_PACKET_RECEIVED:{
					
					if(receivedPacket.ack != PACKET_ACK) {
						
						if (receivedPacket.type == PACKET_KEY_REQUEST){
							send(SEND_KEY_PACKET(receivedPacket.src,receivedPacket.seq,(uint8_t*) key,len));
							printf("KEY packet Sent.\n");
							keySent = true;
						}
						
						if (receivedPacket.type == PACKET_KEY_ACCEPT){
							
							send(ACK_PACKET(receivedPacket.src,PACKET_ACK,receivedPacket.seq));
							printf("key update ack send.\n");
							printf("KEYUPDATE:: key=");
							for (int i = 0; i < 4; i++)
							printf("%c",receivedPacket.data[i]);
							printf(" ,device=%02x\n",receivedPacket.src);
							keySent = false;
						}
												
					
					}
					
					
				}
				
			}
		
		
		
		
		
	}
	
	
	SPIend();
}

