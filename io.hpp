#pragma once

#include <avr/io.h>
#include "bitwise.hpp"

namespace
{
	namespace io
	{
		typedef volatile uint8_t* reg;

		enum class pin_mode
		{
			INPUT=0,
			OUTPUT=1
		};

		//Helper function to find mask index from pin number
		uint8_t _find_mask_index(uint8_t pin)
		{
		#ifdef RESET_DISABLED
			if (pin == 22) return 6;
		#endif
			if (pin >= 14) return pin-14;
			if (pin >= 8)  return pin-8;
			return pin;
		}

		//Helper functions to find register from pin number
		#ifdef INTERNAL_OSCILLATOR
			#define _XTAL_PINS || pin == 20 || pin == 21
		#else
			#define _XTAL_PINS
		#endif
		#define _declare_find(prefix)                               \
			reg _find_register_##prefix (uint8_t pin)               \
			{                                                       \
				if (pin <= 7) return &prefix##D;                    \
				if (pin <= 13 _XTAL_PINS) return &prefix##B;        \
				return &prefix##C;                                  \
			}
		_declare_find(DDR)
		_declare_find(PORT)
		_declare_find(PIN)

		void set_pin_mode(uint8_t pin, pin_mode mode)
		{
			bitwise::write(*_find_register_DDR(pin), _BV(_find_mask_index(pin)), static_cast<bool>(mode));
		}

		void write(uint8_t pin, bool state)
		{
			bitwise::write(*_find_register_PORT(pin), _BV(_find_mask_index(pin)), state);
		}

		bool read(uint8_t pin)
		{
			return *_find_register_PIN(pin) & _find_mask_index(pin);
		}

		void toggle(uint8_t pin)
		{
			bitwise::flip(*_find_register_PORT(pin), _BV(_find_mask_index(pin)));
		}
	}
}
