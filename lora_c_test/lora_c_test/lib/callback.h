/*
 * callback.h
 *
 * Created: 26-03-2018 20:38:10
 *  Author: vikas
 */ 


#ifndef CALLBACK_H_
#define CALLBACK_H_


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "sx1272TxRx.h"

// this ISR is fired whenever a match occurs
// hence, toggle led here itself..
static void callback_init(uint16_t);
void callback(void (*)() , uint16_t);



static void callback_init(uint16_t count)
{
	
		power_timer1_enable();
		// set up timer with prescaler = 64 and CTC mode
		
		TCCR1B |= (1 << WGM12)|(1 << CS12) | (1 << CS10);
		
		// initialize counter
		TCNT1 = 0;
		
		// initialize compare value
		OCR1A = count;
		
		// enable compare interrupt
		TIMSK1 |= (1 << OCIE1A);
		
		// enable global interrupts
		sei();
		
}


void callback( void (*fun)() , uint16_t wait){
	
	
	if (wait > 4095)
		wait = 0xFFFF;
	else
		wait *= 16;
	
	callback_init(wait);
	
}

#endif /* CALLBACK_H_ */