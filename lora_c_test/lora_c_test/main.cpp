/*
 * LoRa_test.cpp
 *
 * Created: 19-03-2018 17:55:42
 * Author : vikas
 */ 

#define F_CPU 16000000UL
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lib/spi.h"
#include "lib/sx1272TxRx.h"
#include "lib/usart.h"
#include "lib/error.h"

#define BAUD 9600
#define MYUBRR ((F_CPU/16/BAUD) - 1)



Packet ACK;
Packet receivedPacket;

int sendState;
int receiveState;

void rdtSend(SX1272RXTX *lora,Packet *pack) {
	lora->txInit(lora->LC, CH_17_868, 5, 950, 1023);
	lora->Send(pack);
	_NOP();
	lora->rxInit(lora->LC,950,1023);
	bool ackReceived = false;
	int count = 0;
	do {
		
		lora->receive(0x03FF);
		
		_delay_us(10);
		switch(lora->udtState){
			case UDT_RX_PACKET_CORRUPT:
				lora->Send(pack);
				
			break;
			case UDT_RX_TIMEOUT: {
				count++;
				if (count > 5){
					lora->Send(pack);
					count = 0;
				}
			}
			break;
			case UDT_RX_PACKET_RECEIVED:{
				if (lora->receivedPacket.ack == 0xA0){
					ackReceived = true;
				}
			}
		}
	}while(!ackReceived);
		
}

void rdtReceive(SX1272RXTX* lora) {
	
	bool packetReceived = false;
	lora->txInit(lora->LC, CH_17_868, -1, 950, 1023);
	_NOP();
	lora->rxInit(lora->LC,950,1023);
	do{
		lora->receive(1023);	
		switch(lora->udtState){
		case UDT_RX_PACKET_RECEIVED:{
			receivedPacket.copy(lora->receivedPacket);
			lora->Send(&ACK);
			packetReceived = true;
		}
		break;	
	}
	}while(!packetReceived);
}


int main(void)
{
	
	
	USART_Init(MYUBRR);
	SPI.begin(SPISettings(SPI_CLOCK_DIV4,MSBFIRST,SPI_MODE0));
	//receivedPacket.data = 0;
	
	printf("Press number to start Lora :\n");
 	uint8_t aa = 0x00;
 	scanf("%02x",&aa);
	
	uint8_t z = 0x00;
	do {
		z = SPI.readRegister(REG_VERSION); 
		printf("Lora's version is %02x\n",z);
		
		_delay_ms(1000);
	}while(z != 0x22);
	
	printf("Press a number to continue:\n ");
	aa = 0x00;
	scanf("%02x",&aa);
	//printf("You have entered aa as %02x\n",aa);
	
		
	Packet pack;
	char hello[] = "hello world";
	hello[11] = '\0';
	pack.length = 12;
	pack.data = (uint8_t*)hello;
	pack.ack = 0xA1;
	pack.dst = 0xFF;
	pack.seq = 0xFF;
	pack.type = 0x00;

	ACK.ack = 0xA0;
	ACK.type = 0x00;
	ACK.dst = 0xFF;
	ACK.seq = 0x00;
	ACK.length = 0;
	
	
	
	SX1272RXTX lora;
	lora.init();
	//lora.txInit(lora.LC, CH_17_868, -1, 950, 1023);
	lora.rxInit(lora.LC,950,1023);
	while (1) 
    {
	
	
	/*
	pack.seq++;
	pack.data[10] = pack.seq%26 + 65;
	rdtSend(&lora,&pack);
	printf("Packet Successfully Sent. count : %d\n",pack.seq);
	*/
	
	lora.receive(1023);
	if (lora.udtState == UDT_RX_PACKET_RECEIVED) {
	
	receivedPacket.copy(lora.receivedPacket);
	printf("Packet Type : %02x, Packet ACK : %02x,Packet length : %d, packet seq : %d\n",receivedPacket.type,receivedPacket.ack,receivedPacket.length, receivedPacket.seq);
	printf("Packet Data : ");
	for (int k = 0; k < receivedPacket.length; k++){
		printf("%c",receivedPacket.data[k]);
	}
	printf("\n");
	}
	
	}
	
}

