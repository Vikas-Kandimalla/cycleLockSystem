/*
 * sx1272.h
 *
 * Created: 12-04-2018 21:41:15
 *  Author: vikas
 */ 


#ifndef SX1272_H_
#define SX1272_H_

#include "LoRaRegisters.h"
#include "spi.h"
#include "typeDefs.h"
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

// GLOBAL CONSTANTS


static uint8_t modem = 0;
static uint8_t operationMode = 0x01;
static LoRaModemConfig modemConfig;
static Frequency frequency;
static PAConfig paConfig;
static PARamp paRamp;
static OCP ocp;
static LNA lna;
static uint8_t detectionThreshold = 0x0A; 
static uint8_t detectionOptimize = 0x03;


void setModemConfig(LoRaModemConfig);

uint8_t getOpMode() {
	operationMode = readRegister(REG_OP_MODE);
	modem = (operationMode >> 7);
	return operationMode;	
}

bool setLora() {
	if (modem == FSK){
		do{
			_delay_ms(100);
			writeRegister(REG_OP_MODE,FSK_SLEEP_MODE);
			_delay_ms(50);
			writeRegister(REG_OP_MODE, LORA_SLEEP_MODE);
			_delay_ms(50);
			writeRegister(REG_OP_MODE, LORA_STANDBY_MODE);
			_delay_ms(50);
			getOpMode();
		
		}while(operationMode != LORA_STANDBY_MODE);
		
	}
	
	// Recheck
	
	if (getOpMode() == LORA_STANDBY_MODE){
		ErrorSet(NO_ERROR);
		return true;
	}
	else{
		ErrorSetMsg(LORA_MODE_SET_ERROR, "Cannot set LoRa mode from FSK");
		return false;
	}
	
}

bool setValidFIFO() {
	
	getOpMode();
	if (modem == FSK) {
		return setLora();
	}
	
	if((operationMode==LORA_STANDBY_MODE))
	{
		ErrorSet(NO_ERROR);
		return true;
	}
	else if (operationMode==LORA_SLEEP_MODE)
	{
		writeRegister(REG_OP_MODE,LORA_STANDBY_MODE);
		if (getOpMode() != LORA_STANDBY_MODE){
			ErrorSetMsg(LORA_MODE_SET_ERROR,"Cannot set valid FIFO");
			return false;
		}
		
		ErrorSet(NO_ERROR);
		return true;
		
	}
	
	else
	{
		while ( getOpMode() != LORA_STANDBY_MODE )				// Wait for 100us to complete the ongoing transmission or reception
			_delay_us(10);
		
		return true;											// should add timeout if this is not working. But timeout is added in transmission or reception. PROBLEM solved.
	}
	
	
	
	ErrorSet(UNKNOWN_STATE_ERROR);					// This state wont reach
	return false;
}

bool checkValidState() {

	getOpMode();
	if((operationMode==LORA_STANDBY_MODE) || (operationMode==LORA_SLEEP_MODE))
	{
		ErrorSet(NO_ERROR);
		return true;
	}
	else
	{
		return false;
	}
	
}

void setValidState(uint8_t count){
	
		while(!checkValidState()){
			_delay_us(100);
				
			if (!count){
				ErrorSetMsg(SET_STATE_TIMEOUT,"set valid state timed out");
				break;
			}
			count--;
		}
	
}

void setOpMode(uint8_t value){
	setValidState(50);
	writeRegister(REG_OP_MODE,value);
}

void setFrequency(uint32_t f)
{
	setValidState(50);                     // Wait for some time. It time out then set the frequency forcefully
	frequency.MSB = (uint8_t) (f >> 16);
	frequency.MID = (uint8_t) (f >> 8);
	frequency.LSB = (uint8_t) (f);
	
	writeRegister(REG_FRF_MSB,frequency.MSB);
	writeRegister(REG_FRF_MID,frequency.MID);
	writeRegister(REG_FRF_LSB,frequency.LSB);
	
	return;
}

void setPower(int p){
	
	if ( p > 20){
		paConfig.power = 20;
		paConfig.outputPower = 0x0F;
		paConfig.paSelect = 0x01;
		paConfig.paDec = 0x87;
	}
	else if (p > 17 && p < 20){
		paConfig.power = 17;
		paConfig.paDec = 0x84;
		paConfig.power = 0x0F;
		paConfig.paSelect = 1;
	}
	else if (p > 14 && p <= 17){
		paConfig.paDec = 0x84;
		paConfig.power = p;
		paConfig.paSelect = 1;
		paConfig.outputPower = p - 2;
	}
	else if (p < 14 && p >= -1){
		paConfig.paDec = 0x84;
		paConfig.power = p;
		paConfig.paSelect = 0;
		paConfig.outputPower = p + 1;
	}
	else {
		paConfig.power = 0;
		paConfig.paDec = 0x84;
		paConfig.paSelect = 0;
		paConfig.outputPower = 0x00;
	}
	
	uint8_t temp = (paConfig.paSelect << 7) | (paConfig.outputPower & 0x0F);
	
	writeRegister(REG_PA_CONFIG,temp);
	writeRegister(REG_PA_DAC,paConfig.paDec);
	
	
}

void setPARamp(uint8_t pll, uint8_t pr){
	
	paRamp.lowPnTxPllOff = pll;
	paRamp.paRampVal = pr;
	uint8_t temp = (pll << 4) | pr ;
	setValidState(50);
	writeRegister(REG_PA_RAMP,temp);
}

void setLnaGain(uint8_t boost, uint8_t gain){
	lna.lnaBoost = boost;
	lna.lnaGain = gain;
	uint8_t temp = (gain << 5) | boost;
	setValidState(50);
	writeRegister(REG_LNA,temp);
	return;
}

void setModemConfig(LoRaModemConfig config){
	uint8_t temp = 0x00;
	int flag = 0;
	temp = (config.codingRate<<3)|(config.bandwidth<<6)|(config.headerMode<<2)|(config.rxPayloadCrc<<1)|(config.lowDataRateOptimize); 
	setValidState(50);
	writeRegister(REG_MODEM_CONFIG1,temp);
	_delay_us(10);
	if (readRegister(REG_MODEM_CONFIG1) != temp)
		flag++;
	
	temp = (config.spreadingFactor<<4)|(config.txContinuousMode<<3)|(config.agcAutoOn<<2);
	temp &= 0xF6;
	uint8_t t = readRegister(REG_MODEM_CONFIG2);
	t &= 0x03;
	temp |= t;
	writeRegister(REG_MODEM_CONFIG2,temp);
	if (readRegister(REG_MODEM_CONFIG2) != temp)
		flag++;
		
	if(flag)
		ErrorSetMsg(MODEM_CONFIG_ERROR,"modem values not mathcing");
	else
		ErrorSet(NO_ERROR);
		
	return;	
}

void setDetectionThreshold(uint8_t value){
	setValidState(50);
	detectionThreshold = value;
	writeRegister(REG_DETECTION_THRESHOLD,value);
}

void setDetectionOptimize(uint8_t value){
	setValidState(50);
	detectionOptimize = value;
	writeRegister(REG_DETECT_OPTIMIZE,value);
} 

void setOcp(uint8_t ocpvalue,uint8_t trimvalue) {                   // sets OCP registers
	
	ocp.ocpOn = ocpvalue;
	ocp.ocpTrim = trimvalue;
	
	uint8_t temp = (ocpvalue << 5) | trimvalue;
	 
	setValidState(50);
	writeRegister(REG_OCP,temp);
	
	return;
}

void sx1272init() {
	
	printf("LoRa Modem initialization started.\n");
	
	setLora();
	modemConfig.bandwidth = SX1272_BW_125;
	modemConfig.codingRate = CR_5;
	modemConfig.headerMode = HEADER_ON;
	modemConfig.rxPayloadCrc = CRC_ON;
	modemConfig.spreadingFactor = SF_7;
	modemConfig.txContinuousMode = 0;
	modemConfig.agcAutoOn = AGC_AUTO_ON;
	
	setModemConfig(modemConfig);
	
	setLnaGain(LNA_DEFAULT,LNA_LOW_GAIN);
	setFrequency(CH_17_868);
	setDetectionThreshold(DT_SF7);
	setDetectionOptimize(DO_SF7);
	setPARamp(0x01,MS_40);
	setPower(-1);
	setOcp(OCP_ENABLE,0x0B);
	
	
	
		printf("Modem Settings : \n");
		printf("BandWidth : 125KHz\n");
		printf("Coding rate : 4/5\n");
		printf("Header mode : explicit\n");
		printf("Payload CRC : ON\n");
		printf("spreading factor : 7\n");
		printf("TX continuous mode : OFF\n");
		printf("AGC auto : ON\n");
		printf("LNA Gain Boost : OFF\n");
		printf("Carrier Frequency : 868MHz\n");
		printf("Power : -1dB\n");
		printf("LoRa initialization finished\n");
		
	
}






#endif /* SX1272_H_ */