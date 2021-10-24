#include "tv.hpp"

#include <stdio.h>

const uint8_t PIN_LED     = 0;
const uint8_t PIN_SYNC    = 9;

tv<24, 24> video (timer1, PIN_SYNC);

int main()
{
	//LED
	io::set_pin_mode(PIN_LED, io::pin_mode::OUTPUT);

	//Enable interrupts
	sei();

	video.clear();

	video.print_P(0, 0, PSTR("*** ATMEGA328P ***"));
	video.print_P(0, 1, PSTR("2048 BYTES RAM"));

	uint16_t f = 0;
	for(uint16_t i = 0; i <= 2048; ++i)
	{
		void* v = malloc(i);
		if (v == NULL)
			f = i;
		free(v);
		if(f) break;
	}

	char b[16];
	sprintf_P(b, PSTR("%u BYTES FREE"), f);
	video.print(0, 2, b);
	sprintf_P(b, PSTR("%u BYTES USED"), 2048-f);
	video.print(0, 3, b);

	video.print_P(0, 5, PSTR("READY."));

	while(true);
}

ISR(TIMER1_OVF_vect)   { video.tickOverflow(); }
ISR(TIMER1_COMPB_vect) { video.tickCompareB(); }
