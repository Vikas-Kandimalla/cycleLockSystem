/*
 * typeDefs.h
 *
 * Created: 12-04-2018 21:49:35
 *  Author: vikas
 */ 


#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#define false	0
#define true	1

typedef uint8_t bool;


typedef struct temp001 {
	
	uint8_t bandwidth;
	uint8_t codingRate;
	uint8_t headerMode;
	uint8_t rxPayloadCrc;
	uint8_t lowDataRateOptimize;
	uint8_t spreadingFactor;
	uint8_t txContinuousMode;
	uint8_t agcAutoOn;

} LoRaModemConfig;

typedef struct temp002 {
	uint8_t lowPnTxPllOff;
	uint8_t paRampVal;
} PARamp;

typedef struct temp003 {
	
	uint8_t paSelect;
	uint8_t power;
	uint8_t outputPower;
	uint8_t paDec;

} PAConfig;

typedef struct temp004 {
	uint8_t ocpOn;
	uint8_t ocpTrim;
}OCP;

typedef struct temp005 {
	uint8_t lnaGain;
	uint8_t lnaBoost;
} LNA;

typedef struct temp006 {
	uint8_t MSB;
	uint8_t MID;
	uint8_t LSB;
} Frequency;

typedef struct temp007 {
	
	uint8_t dst; // destination address
	uint8_t ack; // packet type
	uint8_t type;
	uint8_t src;  // source address
	uint8_t seq; // packet number
	uint8_t length;  // packet length
	uint8_t* data;   // packet data	
} Packet;


#endif /* TYPEDEFS_H_ */