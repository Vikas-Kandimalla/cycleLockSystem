/*
 * LoRaRegisters.h
 *
 * Created: 12-04-2018 21:42:01
 *  Author: vikas
 */ 


#ifndef LORAREGISTERS_H_
#define LORAREGISTERS_H_


#define        REG_FIFO        					0x00
// FIFO Read write access
#define        REG_OP_MODE        				0x01 // Always select LoRa
// Operating mode selection.
// For Lora
// 0 -2 Mode SLEEP,STDBY,FSTX,TX,FSRX,RXCONT,RXSING,CAD
// 3-5 unused
// 6 Access Shared Register
// 7 Long Rage Mode 0 --> FSK/OOK 1 --> LoRa

#define        REG_FRF_MSB    					0x06
// LoRa frequency max byte
#define        REG_FRF_MID    					0x07
// LoRa frequency middle byte
#define        REG_FRF_LSB    					0x08
// LoRa frequency low byte
#define        REG_PA_CONFIG    				0x09 // LoRa Power options
#define        REG_PA_RAMP    					0x0A // Rise/Fall time of ramp up/down in FSK
#define        REG_OCP    						0x0B // Overload current protection settings
#define        REG_LNA    						0x0C // LNA gain settings
//***************************************************************************************
//****************** LORA PAGE REGISTERS	*********************************************
//***************************************************************************************
#define        REG_FIFO_ADDR_PTR  				0x0D // SPI interface address pinter in FIFO data buffer
#define        REG_FIFO_TX_BASE_ADDR 		    0x0E // write base address in FIFO data buffer for Tx modulator
#define        REG_FIFO_RX_BASE_ADDR   			0x0F // read base address in FIFO data buffer for Rx demodulator
#define        REG_FIFO_RX_CURRENT_ADDR   		0x10 // Start address (in data buffer) of last packet received
#define        REG_IRQ_FLAGS_MASK    			0x11 // MSB-LSB RxTimeoutMask,RxDoneMask,PayloadCRCErrorMask,ValidHeaderMask,TxDoneMask,CadDoneMask,FhssChangeChannelMask,CadDetectedMask
#define        REG_IRQ_FLAGS	    			0x12 // //////////////////////////
#define        REG_RX_NB_BYTES	    			0x13 // number of payload bytes of last packet received bytes
#define        REG_RX_HEADER_CNT_VALUE_MSB  	0x14 // No.of valid headers received since last transition into Rx mode.(Reset in sleep mode)
#define        REG_RX_HEADER_CNT_VALUE_LSB  	0x15 // //////////////////////////
#define        REG_RX_PACKET_CNT_VALUE_MSB  	0x16 // No.of valid packets received since last transition into Rx mode. ( Reset in  sleep mode).
#define        REG_RX_PACKET_CNT_VALUE_LSB  	0x17 // //////////////////////////
#define        REG_MODEM_STAT	  				0x18 // Modem status
#define        REG_PKT_SNR_VALUE	  			0x19 // packet SNR
#define        REG_PKT_RSSI_VALUE	  			0x1A // packet RSSI
#define        REG_RSSI_VALUE		  			0x1B // current RSSI
#define        REG_HOP_CHANNEL	  				0x1C // current value of frequency hopping channel in use
#define        REG_MODEM_CONFIG1	 		 	0x1D // modem configurations
#define        REG_MODEM_CONFIG2	  			0x1E // //////////////////////////
#define        REG_SYMB_TIMEOUT_LSB  			0x1F // Rx timeout LSB
#define        REG_PREAMBLE_MSB		  			0x20 // Preamble length MSB
#define        REG_PREAMBLE_LSB  				0x21 // preamble length LSB
#define        REG_PAYLOAD_LENGTH			 	0x22 // payload length in bytes. (Implicit mode)
#define        REG_MAX_PAYLOAD_LENGTH 			0x23 // Maximum payload length;
#define        REG_HOP_PERIOD	  				0x24 // Symbol periods b/w frequency hops
#define        REG_FIFO_RX_BYTE_ADDR 			0x25 // current value of Rx data buffer pointer
#define		   REG_FEI_MSB						0x28 // Estimated frequency error MSB
#define		   REG_FEI_MID						0x29 // /////////////////////////////
#define		   REG_FEI_LSB                      0x2A // /////////////////////////////
#define		   REG_RSSI_WIDEBAND                0x2C // Wideband RSSI measurement to generate a random number
#define        REG_DETECT_OPTIMIZE              0x31 // LoRa detection optimize
#define		   REG_LORA_INVERTQ					0x33 // invert the LoRa I and Q signals
#define        REG_DETECTION_THRESHOLD          0x37 // LoRa detection threshold
#define        REG_SYNC_WORD                    0x39 // LoRa sync word.
#define		   REG_DIO_MAPPING1                 0x40 // Mapping of pins DIO0 to DIO3
#define		   REG_DIO_MAPPING2                 0x41 // Mapping of pins DIO4, DIO5 and ClkOut frequency
#define		   REG_VERSION						0x42 // Semtech ID relating the silicon revision
#define        REG_AGC_REF	  					0x43 // Adjustment of AGC thresholds
#define        REG_AGC_THRESH1	  				0x44 // ////////////////////////////
#define        REG_AGC_THRESH2	  				0x45 // ////////////////////////////
#define        REG_AGC_THRESH3	  				0x46 // ////////////////////////////
#define        REG_PLL_HOP	  					0x4B // Control the fast frequency hopping mode
#define        REG_TCXO		  					0x58 // TCXO or XTAL input setting
#define        REG_PA_DAC		  				0x5A // Higher power settings of PA
#define        REG_PLL		  					0x5C // Control the PLL bandwidth
#define        REG_PLL_LOW_PN	  				0x5E // Control the low phase noise PLL bandwidth
#define        REG_FORMER_TEMP	  				0x6C // Stored temperature during former IQ calibration
#define        REG_BIT_RATE_FRAC	  			0x70 // Fractional part in bit rate division ratio




//const uint8_t true  = 1;
//const uint8_t false	= 0;

// --------------------------------------------------------------------------
// ----------------------------- CONSTANTS FOR LORA MODE --------------------
// --------------------------------------------------------------------------
//Constants for frequency settings

const uint32_t CH_04_868 = 0xD7CCCC; // channel 04, central freq = 863.20MHz
const uint32_t CH_05_868 = 0xD7E000; // channel 05, central freq = 863.50MHz
const uint32_t CH_06_868 = 0xD7F333; // channel 06, central freq = 863.80MHz
const uint32_t CH_07_868 = 0xD80666; // channel 07, central freq = 864.10MHz
const uint32_t CH_08_868 = 0xD81999; // channel 08, central freq = 864.40MHz
const uint32_t CH_09_868 = 0xD82CCC; // channel 09, central freq = 864.70MHz
const uint32_t CH_10_868 = 0xD84CCC; // channel 10, central freq = 865.20MHz, = 865200000*RH_LORA_FCONVERT
const uint32_t CH_11_868 = 0xD86000; // channel 11, central freq = 865.50MHz
const uint32_t CH_12_868 = 0xD87333; // channel 12, central freq = 865.80MHz
const uint32_t CH_13_868 = 0xD88666; // channel 13, central freq = 866.10MHz
const uint32_t CH_14_868 = 0xD89999; // channel 14, central freq = 866.40MHz
const uint32_t CH_15_868 = 0xD8ACCC; // channel 15, central freq = 866.70MHz
const uint32_t CH_16_868 = 0xD8C000; // channel 16, central freq = 867.00MHz
const uint32_t CH_17_868 = 0xD90000; // channel 17, central freq = 868.00MHz
const uint32_t CH_18_868 = 0xD90666; // 868.1MHz for LoRaWAN test
const uint32_t CH_00_900 = 0xE1C51E; // channel 00, central freq = 903.08MHz
const uint32_t CH_01_900 = 0xE24F5C; // channel 01, central freq = 905.24MHz
const uint32_t CH_02_900 = 0xE2D999; // channel 02, central freq = 907.40MHz
const uint32_t CH_03_900 = 0xE363D7; // channel 03, central freq = 909.56MHz
const uint32_t CH_04_900 = 0xE3EE14; // channel 04, central freq = 911.72MHz
const uint32_t CH_05_900 = 0xE47851; // channel 05, central freq = 913.88MHz
const uint32_t CH_06_900 = 0xE5028F; // channel 06, central freq = 916.04MHz
const uint32_t CH_07_900 = 0xE58CCC; // channel 07, central freq = 918.20MHz
const uint32_t CH_08_900 = 0xE6170A; // channel 08, central freq = 920.36MHz
const uint32_t CH_09_900 = 0xE6A147; // channel 09, central freq = 922.52MHz
const uint32_t CH_10_900 = 0xE72B85; // channel 10, central freq = 924.68MHz
const uint32_t CH_11_900 = 0xE7B5C2; // channel 11, central freq = 926.84MHz
const uint32_t CH_12_900 = 0xE4C000; // default channel 915MHz, the module is configured with it

const uint32_t CH_00_433 = 0x6C5333; // 433.3MHz
const uint32_t CH_01_433 = 0x6C6666; // 433.6MHz
const uint32_t CH_02_433 = 0x6C7999; // 433.9MHz
const uint32_t CH_03_433 = 0x6C9333; // 434.3MHz
//Constants for bandwidth 
const uint8_t SX1272_BW_125 = 0x00;
const uint8_t SX1272_BW_250 = 0x01;
const uint8_t SX1272_BW_500 = 0x02;

const uint8_t BW_7_8 = 0x00;
const uint8_t BW_10_4 = 0x01;
const uint8_t BW_15_6 = 0x02;
const uint8_t BW_20_8 = 0x03;
const uint8_t BW_31_25 = 0x04;
const uint8_t BW_41_7 = 0x05;
const uint8_t BW_62_5 = 0x06;
const uint8_t BW_125 = 0x07;
const uint8_t BW_250 = 0x08;
const uint8_t BW_500 = 0x09;


const double SignalBwLog[] =
{
	5.0969100130080564143587833158265,
	5.397940008672037609572522210551,
	5.6989700043360188047862611052755
};

//LORA CODING RATE:
const uint8_t CR_5 = 0x01;
const uint8_t CR_6 = 0x02;
const uint8_t CR_7 = 0x03;
const uint8_t CR_8 = 0x04;

//LORA SPREADING FACTOR:
const uint8_t SF_6 = 0x06;
const uint8_t SF_7 = 0x07;
const uint8_t SF_8 = 0x08;
const uint8_t SF_9 = 0x09;
const uint8_t SF_10 = 0x0A;
const uint8_t SF_11 = 0x0B;
const uint8_t SF_12 = 0x0C;

//LORA MODES:
const uint8_t FSK_SLEEP_MODE =		0x00;
const uint8_t FSK_STANDBY_MODE =	0x01;
const uint8_t LORA_SLEEP_MODE =		0x80;
const uint8_t LORA_STANDBY_MODE =	0x81;
const uint8_t LORA_FSTX_MODE =		0x82;
const uint8_t LORA_TX_MODE =		0x83;
const uint8_t LORA_FSRX_MODE =		0x84;
const uint8_t LORA_RX_CONT_MODE =   0x85;
const uint8_t LORA_RX_MODE =		0x86;
const uint8_t LORA_CAD_MODE =		0x87;

#define LNA_MAX_GAIN                0x23
#define LNA_OFF_GAIN                0x00
#define LNA_LOW_GAIN				0xC0
// end
const uint8_t LORA_STANDBY_FSK_REGS_MODE = 0xC1;


//DetectionOptimizeComstants
const uint8_t DO_SF7 =             0x03;
const uint8_t DO_SF6=              0x05;


//DetectionThreshold Constants
const uint8_t DT_SF7=              0x0A;
const uint8_t DT_SF6=              0x0C;


//constants for ModemCOnfig
const uint8_t LOW_DATARATE_OP_ON=  0x01;
const uint8_t LOW_DATARATE_OP_OFF= 0x00;
const uint8_t TX_CONTINUOUS_ON=    0x01;
const uint8_t TX_CONTINUOUS_OFF=   0x00;
const uint8_t HEADER_ON =             0;
const uint8_t HEADER_OFF =            1;
const uint8_t CRC_ON =                1;
const uint8_t CRC_OFF =               0;
const uint8_t AGC_AUTO_ON =           1;
const uint8_t AGC_AUTO_OFF=           0;

//other constants
const uint8_t LORA =                  1;
const uint8_t FSK =                   0;
const uint8_t BROADCAST_0 =        0x00;
const uint8_t MAX_LENGTH =          255;
const uint8_t MAX_PAYLOAD =         251;
const uint8_t MAX_LENGTH_FSK =       64;
const uint8_t MAX_PAYLOAD_FSK =      60;
const uint8_t ACK_LENGTH =            7;
const uint8_t OFFSET_RSSI = 139;
const uint8_t NOISE_FIGURE = 6.0;
const uint8_t NOISE_ABSOLUTE_ZERO = 174.0;
const uint16_t MAX_TIMEOUT = 10000;		//10000 msec = 10.0 sec
const uint16_t MAX_WAIT = 12000;		//12000 msec = 12.0 sec
const uint8_t MAX_RETRIES = 5;
const uint8_t CORRECT_PACKET = 0;
const uint8_t INCORRECT_PACKET = 1;
//constants for pa ramp

const uint8_t LOW_CONSUMPTION_PLL_TR= 0x01;
const uint8_t LOW_CONSUMPTION_PLL_R= 0x00;
const uint8_t MS_3_4=0x00;
const uint8_t MS_250=0x04;
const uint8_t MS_125=0x05;
const uint8_t MS_100=0x06;
const uint8_t MS_62=0x07;
const uint8_t MS_50=0x08;
const uint8_t MS_40=0x09;
const uint8_t MS_31=0x0A;
const uint8_t MS_25=0x0B;
const uint8_t MS_20=0x0C;
const uint8_t MS_15=0x0D;
const uint8_t MS_12=0x0E;
const uint8_t MS_10=0x0F;

//constants for PA 
const uint8_t RFIO=0x00;
const uint8_t PA_BOOST=0x01;
//constants for lna
const uint8_t LNA_DEFAULT=0x00;
const uint8_t LNA_BOOST=0x03;
//constants for OCP
const uint8_t OCP_ENABLE=0x01;
const uint8_t OCP_DISABLE=0x00;




#endif /* LORAREGISTERS_H_ */