#pragma once

namespace
{
	namespace bitwise
	{
		template<class T>
		T mask(T i)
		{
			return 1 << i;
		}

		template<class T1, class T2>
		bool get(T1 target, T2 mask)
		{
			return target & mask;
		}

		template<class T1, class T2>
		void set(T1& target, T2 mask)
		{
			target |= mask;
		}

		template<class T1, class T2>
		void clear(T1& target, T2 mask)
		{
			target &= ~mask;
		}

		template<class T1, class T2>
		void flip(T1& target, T2 mask)
		{
			target ^= mask;
		}

		template<class T1, class T2>
		void write(T1& target, T2 mask, bool value)
		{
			if (value)
				set(target, mask);
			else
				clear(target, mask);
		}
	}
}
