#pragma once

#include "timer.hpp"
#include "font.hpp"
#include "io.hpp"
#include "uint6_array2.hpp"

namespace
{
	template<uint8_t ROWS, uint8_t COLUMNS>
	class tv
	{
	public:
		tv(const timer<uint16_t>& timer, uint8_t pin_sync) : _timer(timer)
		{
			UCSR0C = _BV (UMSEL00) | _BV (UMSEL01); // Master SPI mode

			// inverted fast pwm mode on timer 1:
			TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM11); 
			TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);

			_timer.set_compare_interrupt(B, true);
			_timer.set_compare(B, F_CPU*11.4f/1000000);
			ICR1 = F_CPU*64/1000000;

			io::set_pin_mode(pin_sync, io::pin_mode::OUTPUT);
		}

		void tickOverflow()
		{
			++line;

			if (line == 6)
			{
				_timer.set_compare(A, F_CPU*27.3f/1000000);
			}
			else if (line == 11)
			{
				_timer.set_compare(A, F_CPU*2.35f/1000000);
			}
			else if (line == 16)
			{
				_timer.set_compare_interrupt(B, true);
				_timer.set_overflow_interrupt(false);
				//_timer.set_compare(A, F_CPU*4.7f/1000000);
				ICR1 = F_CPU*64/1000000;
				line = 0;
			}
		}

		void tickCompareB()
		{
			static uint8_t next_line[COLUMNS];
			static uint8_t b = 0;

			//Draw Line
			if (line >= OFFSET_TOP && line%2 == (b&_BV(0)))
			{
				for(uint8_t i = 0; i < COLUMNS; ++i)
				{
					while (!(UCSR0A & _BV(UDRE0)));
					UDR0 = next_line[i];
					UCSR0B = _BV(TXEN0);
				}
				UCSR0B = 0;
			}
			
			++line;

			//Vertical Blanking
			if (line == 260)
			{
				_timer.set_compare_interrupt(B, false);
				_timer.set_overflow_interrupt(true);
				_timer.set_counter(0);
				//_timer.set_compare(A, F_CPU*2.35f/1000000);
				ICR1 = F_CPU*32/1000000;
				line = 0;
				bitwise::flip(b, _BV(0));

				return;
			}

			//Render next line
			if (line >= OFFSET_TOP && line%2!=(b&_BV(0)))
			{
				const uint8_t l = line - OFFSET_TOP;
				const uint8_t m = l % FONT_HEIGHT;

				if ((b&_BV(1)) || (m&&(b&_BV(0))) || ((l-1)%FONT_HEIGHT && !(b&_BV(0))))
				{
					for(uint8_t i = 0; i < COLUMNS; ++i)
						next_line[i] = pgm_read_byte(&font[frame_buffer.get(i, l/FONT_HEIGHT)][m]);
					
					bitwise::clear(b, _BV(1));
				}
				else
				{
					for(uint8_t i = 0; i < COLUMNS; ++i)
						next_line[i] = 0;

					bitwise::set(b, _BV(1));
					line -= 1;
				}
			}
		}

		void set(uint8_t x, uint8_t y, char c)
		{
			frame_buffer.set(x, y, c-' ');
		}

		void area(uint8_t x, uint8_t y, uint8_t x2, uint8_t y2, char c)
		{
			c-=' ';
			for(uint8_t j = y; j <= y2; ++j)
				for(uint8_t i = x; i <= x2; ++i)
					frame_buffer.set(i, j, c);
		}

		void fill(char c)
		{
			area(0, 0, COLUMNS-1, ROWS-1, c);
		}

		void clear()
		{
			fill(' ');
		}

		void print(uint8_t x, uint8_t y, const char* str)
		{
			for(uint8_t i = 0; i+x<COLUMNS && str[i]; ++i)
				set(i+x, y, str[i]);
		}

		void print_P(uint8_t x, uint8_t y, const char* str)
		{
			for(uint8_t i = 0; i+x<COLUMNS && pgm_read_byte(&str[i]); ++i)
				set(i+x, y, pgm_read_byte(&str[i]));
		}

		void scroll_down(uint8_t rows=1)
		{
			for(uint8_t j = 0; j < ROWS-rows; ++j)
				for(uint8_t i = 0; i < COLUMNS; ++i)
					frame_buffer.set(i, j, frame_buffer.get(i, j+rows));
		}

	private:
		const static uint8_t OFFSET_TOP = 28;
		const timer<uint16_t>& _timer;

		uint16_t line = 0;

		uint6_array2<COLUMNS, ROWS> frame_buffer;
	};
}
