/*
 * millis.h
 *
 * Created: 13-04-2018 02:55:35
 *  Author: vikas
 */ 


#ifndef MILLIS_H_
#define MILLIS_H_


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/atomic.h>



volatile unsigned long timer0_millis;
volatile unsigned long timer0_minutes;

unsigned long temp = 0;


unsigned long millis(void);

ISR (TIMER0_COMPA_vect)
{
	timer0_millis++;
	if ((millis() - temp) > 60000){
		temp = millis();
		timer0_minutes++;
	}
}

unsigned long minutes() {
	
	unsigned long minutes;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		minutes = timer0_minutes;
	}
	
	return minutes;
}


unsigned long millis ()
{
	unsigned long millis_return;

	// Ensure this cannot be disrupted
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		millis_return = timer0_millis;
	}
	
	return millis_return;
}


void millis_init() {


	power_timer0_enable();
	cli();
	
	TCCR0A  = 0x00;
	TCCR0B  = 0x03;
	// Clock of 1/64
	TCNT0 = 0;
	// Enable the compare match interrupt
	TIMSK0 = 0x02;
	
	OCR0A = 250;
	
	TIFR0 = 0x02;
	
	temp = 0;
	// Now enable global interrupts
	sei();
	
}

#endif /* MILLIS_H_ */