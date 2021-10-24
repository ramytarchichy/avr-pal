#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

namespace
{
	template<uint8_t X, uint8_t Y>
	class uint6_array2
	{
	public:
		uint8_t get(uint8_t x, uint8_t y) const
		{
			return get(get_1d_addr(x, y));
		}

		void set(uint8_t x, uint8_t y, uint8_t value)
		{
			set(get_1d_addr(x, y), value);
		}

	private:
		uint8_t data[static_cast<size_t>(ceil(X*Y*6/8.0f))];

		static uint16_t get_1d_addr(uint8_t x, uint8_t y)
		{
			return static_cast<uint16_t>(y)*X+x;
		}

		uint8_t get(uint16_t i) const
		{
			const uint8_t* const ptr = data + i*6/8;

			switch (i%4)
			{
			case 0:
				return *ptr >> 2;
			
			case 1:
				return ((*ptr & 0x03) << 4) | ((*(ptr+1) & 0xF0) >> 4);

			case 2:
				return ((*ptr & 0x0F) << 2) | ((*(ptr+1) & 0xC0) >> 6);

			case 3:
			default:
				return *ptr & 0x3F;
			}
		}

		// Warning: This WILL mess up the data if value is greater than 63.
		void set(uint16_t i, uint8_t value)
		{
			uint8_t* const ptr = data + i*6/8;

			switch (i%4)
			{
			case 0:
				*ptr = (*ptr & 0x03) | value << 2;
				break;
			
			case 1:
				*ptr = (*ptr & 0xFC) | value >> 4;
				*(ptr+1) = (*(ptr+1) & 0x0F) | ((value & 0x0F) << 4);
				break;

			case 2:
				*ptr = (*ptr & 0xF0) | value >> 2;
				*(ptr+1) = (*(ptr+1) & 0x3F) | ((value & 0x03) << 6);
				break;

			case 3:
				*ptr = (*ptr & 0xC0) | value;
				break;
			}
		}
	};
}
