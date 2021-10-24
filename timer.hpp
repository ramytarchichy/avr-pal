#pragma once

#include <avr/interrupt.h>
#include "bitwise.hpp"

namespace
{
	const uint8_t A = 0;
	const uint8_t B = 1;

	enum class clock_select
	{
		NO_CLOCK=0,
		PRESCALING_NONE=1,
		PRESCALING_8=2,
		PRESCALING_64=3,
		PRESCALING_256=4,
		PRESCALING_1024=5,
		EXT_CLOCK_FALLING=6,
		EXT_CLOCK_RISING=7
	};

	template<class T, int N=2>
	struct timer
	{
		volatile T* counter;
		volatile uint8_t* timsk;
		volatile uint8_t* control[N];
		volatile T* compare[N];

		void set_clock(clock_select cs) const
		{
			*control[N-1] = static_cast<uint8_t>(cs);
		}

		void set_counter(T value) const
		{
			*counter = value;
		}

		T get_counter() const
		{
			return *counter;
		}

		void set_compare(uint8_t i, T value) const
		{
			*compare[i] = value;
		}

		T get_compare(uint8_t i) const
		{
			return *compare[i];
		}

		void set_interrupt(uint8_t interrupt, bool value) const
		{
			bitwise::write(*timsk, _BV(interrupt), value);
		}

		void set_compare_interrupt(uint8_t i, bool value) const
		{
			const uint8_t a[N] = {OCIE0A, OCIE0B};
			set_interrupt(a[i], value);
		}

		void set_overflow_interrupt(bool value) const
		{
			set_interrupt(TOIE0, value);
		}
	};

	const timer<uint8_t>  timer0 = {&TCNT0, &TIMSK0, {&TCCR0A, &TCCR0B}, {&OCR0A, &OCR0B}};
	const timer<uint16_t> timer1 = {&TCNT1, &TIMSK1, {&TCCR1A, &TCCR1B}, {&OCR1A, &OCR1B}};
	const timer<uint8_t>  timer2 = {&TCNT2, &TIMSK2, {&TCCR2A, &TCCR2B}, {&OCR2A, &OCR2B}};
}
