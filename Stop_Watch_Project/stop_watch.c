/*
 * Mini_Project_2.c
 *
 *  Created on: May 3, 2022
 *      Author: MohamedRagab
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// global variables to control values for seconds,minutes and hours
int sec = 0;
int min = 0;
int hour = 0;
// TIMER1 Enabling
void Timer1_CTC_Init(void){
	// NON-PWN mode (CTC mode)
	TCCR1A |= (1<<FOC1A) |(1<<FOC1B);
	//ENABLE compare mode 4
	TCCR1B |= (1<<WGM12) ;
	// set prescaler 64
	TCCR1B |= (1<<CS10) | (1<<CS11);
	// begin count from 0
	TCNT1 = 0;
	// SET compare match value
	OCR1A = 15624;
	// Enable timer_1 interrupt
	TIMSK |= (1<<OCIE1A);
	// enable global interrupt
	SREG |= (1<<7);
}
// interrupt service routine for timer 1
ISR(TIMER1_COMPA_vect)
{
	sec++;
	if (sec == 60){
		sec = 0;
		min++;
	}
	if (min == 60){
		min = 0;
		hour++;
	}
	if (hour == 24){
		sec  = 0;
		min = 0;
		hour = 0;
	}
}
void INT0_Init_Reset_Watch(void){
	// pin INT0/PD2 as input and enable pull-up resistor
	DDRD &= ~(1<<PD2);
	PORTD |=(1<<PD2);
	//INT0 with falling edge
	MCUCR |= (1<<ISC01);
	// enable INT0
	GICR |= (1<<INT0);
	// enable global interrupt
	SREG |= (1<<7);
}
// ISR OF INT0
ISR(INT0_vect){
	// reset Stop watch by making the global variables = 0 as well as TCNT1 so that the timer begins counting from 0 again
	sec = 0;
	min = 0;
	hour= 0;
	TCNT1 = 0;
}
void INT1_Init_Pause_Watch(void){
	// pin INT1/PD3 as input
	DDRD &= ~(1<<PD3);
	//INT1 with raising edge
	MCUCR |= (1<<ISC10)|(1<<ISC11);
	// enable INT1
	GICR |= (1<<INT1);
	// enable global interrupt
	SREG |= (1<<7);
}
// ISR OF INT1
ISR(INT1_vect){
	// stopping timer_1 clock
	TCCR1B &= ~(1<<CS10) & ~(1<<CS11) & ~(1<<CS12);
}
void INT2_Init_Resume_Watch(void){
	// pin INT2/PB2 as input and enable pull-up resistor
	DDRB &= ~(1<<PB2);
	PORTB |=(1<<PB2);
	//INT2 with falling edge
	MCUCR &= ~(1<<ISC2);
	// enable INT2
	GICR |= (1<<INT2);
	// enable global interrupt
	SREG |= (1<<7);
}
// ISR OF INT0
ISR(INT2_vect){
	// enabling timer_1 clock
	TCCR1B |= (1<<CS10) | (1<<CS11);
}
int main (void){
	// decoder pins as output
	DDRC |= 0x0F;
	// Enable/Disable pins as output
	DDRA |= 0x3F;
	// initial values
	PORTC &= ~(0x0F);
	PORTA &= ~(1);
	Timer1_CTC_Init();
	INT0_Init_Reset_Watch();
	INT1_Init_Pause_Watch();
	INT2_Init_Resume_Watch();
	while(1){
		PORTA = 1;
		PORTC = (sec%10);
		_delay_ms(2);
		PORTA = (1<<1);
		PORTC = (sec/10);
		_delay_ms(2);
		PORTA = (1<<2);
		PORTC = (min%10);
		_delay_ms(2);
		PORTA = (1<<3);
		PORTC = (min/10);
		_delay_ms(2);
		PORTA = (1<<4);
		PORTC = (hour%10);
		_delay_ms(2);
		PORTA = (1<<5);
		PORTC = (hour/10);
		_delay_ms(2);
	}
}
