/*
 * packetDefs.h
 *
 * Created: 13-04-2018 18:30:09
 *  Author: vikas
 */ 


#ifndef PACKETDEFS_H_
#define PACKETDEFS_H_

#include <stdlib.h>

#define PACKET_DATA								0xA0
#define PACKET_ACK								0xB0
#define PACKET_DATA_ACK							0xAB

#define PACKET_KEY_REQUEST						0xD0
#define PACKET_KEY								0xD1
#define PACKET_KEY_ACCEPT						0xD2
#define PACKET_KEY_OVERIDE						0xD3
#define PACKET_FIN								0x70
#define PACKET_FINACK							0x71


Packet* ACK_PACKET(uint8_t dst,uint8_t ack,uint8_t seq) {
	
	Packet *temp = (Packet* ) malloc(sizeof(Packet));
	temp->type = PACKET_ACK;
	temp->src = SOURCE_ADDR;
	temp->dst = dst;
	temp->ack = ack;
	temp->seq = seq;
	temp->length = 0;
	temp->data = 0;
	
	return temp;
}

Packet* KEY_REQUEST_PACKET(uint8_t dst,uint8_t seq) {
	Packet *temp = (Packet* ) malloc(sizeof(Packet));
	temp->type = PACKET_KEY_REQUEST;
	temp->dst = dst;
	temp->ack = PACKET_DATA;
	temp->length = 0;
	temp->data = 0;
	temp->src = SOURCE_ADDR;
	temp->seq = seq;
	
	return temp;
}

Packet* SEND_KEY_PACKET(uint8_t dst, uint8_t seq, uint8_t *key,uint8_t len){
	
	Packet *temp = (Packet*) malloc(sizeof(Packet));
	temp->type = PACKET_KEY;
	temp->dst = dst;
	temp->src = SOURCE_ADDR;
	temp->seq = seq;
	temp->ack = PACKET_DATA_ACK;
	temp->length = len;
	temp->data = key;
	
	return temp;	
}

Packet* KEY_ACCEPT_PACKET(uint8_t dst, uint8_t seq, uint8_t *key,uint8_t len){
	
	Packet *temp = (Packet* ) malloc(sizeof(Packet));
	temp->type	=	PACKET_KEY_ACCEPT;
	temp->ack	=	PACKET_DATA;
	temp->data	=	key;
	temp->seq	= seq;
	temp->length = len;
	temp->dst = dst;
	temp->src = SOURCE_ADDR;
	
	return temp;
}

Packet* KEY_OVERIDE_PACKET(uint8_t dst,uint8_t seq, uint8_t *key,uint8_t len){
	
	Packet *temp = (Packet* ) malloc(sizeof(Packet));
	temp->type = PACKET_KEY_OVERIDE;
	temp->ack = PACKET_DATA;
	temp->dst = dst;
	
	temp->src = SOURCE_ADDR;
	temp->length = len;
	temp->data = key;
	temp->seq = seq;
	
	return temp;
	
}

Packet* FIN_PACKET(uint8_t dst,uint8_t seq){

	Packet *temp = (Packet* ) malloc(sizeof(Packet));
	temp->type = PACKET_FIN;
	temp->dst = dst;
	temp->ack = PACKET_DATA;
	temp->length = 0;
	temp->data = 0;
	temp->src = SOURCE_ADDR;
	temp->seq = seq;

	return temp;
}

Packet* FINACK_PACKET(uint8_t dst,uint8_t seq){

	Packet *temp = (Packet* ) malloc(sizeof(Packet));
	temp->type = PACKET_FINACK;
	temp->dst = dst;
	temp->ack = PACKET_DATA;
	temp->length = 0;
	temp->data = 0;
	temp->src = SOURCE_ADDR;
	temp->seq = seq;

	return temp;
}





#endif /* PACKETDEFS_H_ */