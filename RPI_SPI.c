/*
Name & Email: Nathaniel Fey, nfey001@ucr.edu
Partner & Email: NONE
Lab Section: 021
Assignment: Lab 3 SPI, part 1 follower

I acknowledge all content contained herein, excluding template or example code, is my own original work.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Timer.h"

unsigned char receiveData; // hold data received from master

enum follow_states {WAIT, READ} state;

void SPI_ServantInit()
{
	// Set DDRB to have MISO line as output and MOSI, SCK and SS as input
	DDRB = 0x40; PORTB = 0xBF;
	
	// Set SPCR register to enable SPI and enable SPI interrupt (pg. 168)
	SPCR |= (1<<SPE) | (1<<SPIE);
	
	// global interrupts
	sei();
}

ISR(SPI_STC_vect)
{
	// this is enabled in init with the SPCR register's "SPI Interrupt enable"
	// SPDR contains the received data
	receiveData = SPDR;
}

unsigned char SPI_Receive()
{
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}
void follow_tick()
{
	switch (state) // transitions
	{
		case WAIT:
		state = READ;
		break;
		case READ:
		state = WAIT;
		break;
		default:
		state = WAIT;
		break;
	}
	
	switch (state) // actions
	{
		case WAIT:
		receiveData = SPI_Receive();
		break;
		case READ:
		PORTA = receiveData;
		break;
	}
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	SPI_ServantInit();
	while (1)
	{
		//PORTA = 0x01;
		follow_tick();
		
	}
}

