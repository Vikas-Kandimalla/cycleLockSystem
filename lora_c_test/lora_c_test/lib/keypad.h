
/*
 * keypad.h
 *
 * Created: 15-04-2018 17:27:13
 *  Author: vikas
 */ 


#include <avr/interrupt.h>
#include <avr/io.h>


#define bitRead(value, bitno) (((value) >> (bitno)) & 0x01)  // read a bit
#define bitSet(value, bitno) ((value) |= (1UL << (bitno)))    // set bit to '1'
#define bitClear(value, bitno) ((value) &= ~(1UL << (bitno))) // set bit to '0'


const uint8_t KEY_MAP[4][3] = {	{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'R','0','E'}  };


void keypad_init() {

	bitSet(DDRD,7);
	bitSet(DDRD,6);
	bitSet(DDRD,5);
	bitSet(DDRD,4);

	bitClear(DDRC,0);
	bitClear(DDRC,1);
	bitClear(DDRC,2);


	PORTD &= ~(0xF0);
	
	PORTC |= (1 << PORTC2) | (1 << PORTC1) | (1 << PORTC0);
	
	
	PCICR = 0x00;
	PCMSK1 = 0x07;
	
	
	
}

char getChar() {
	
	int flag = 0;
	int error = 0;
	int row = 0;
	int col = 0;
	
	PORTD |= 0xF0;
	_NOP();
	
	for (int i = 4; i < 8; i++){
		
		bitClear(PORTD,i);
		_NOP();
		
		for (int j = 0; j < 3; j++){
			
			if (bitRead(PINC,j) == 0){
				
				_delay_ms(20);
				
				if ((bitRead(PINC,j) == 0) && flag == 0) {
					row = i - 4;
					col = j;
					flag = 1;
				}
				else
				error = 1;
				
			}
		}
		
		bitSet(PORTD,i);
		_NOP();
	}
	
		
		PORTD &= ~(0xF0);
		_NOP();
	
	if (flag & !error){
		return KEY_MAP[row][col];
	}
	else
		return 0;
	
}

char getReliableChar() {
	char temp;
	
	do{
		temp = getChar();
	}while(temp == 0);
	
	while (getChar() == temp);
	
	bitSet(PORTD,3);
	_delay_ms(75);
	bitClear(PORTD,3);
	return temp;
}

