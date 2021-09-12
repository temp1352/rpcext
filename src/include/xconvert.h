#pragma once

namespace XTL
{
	class Convert
	{
	public:
		static short hexValue(char chIn) throw()
		{
			unsigned char ch = (unsigned char)chIn;
			if (ch >= '0' && ch <= '9')
				return (short)(ch - '0');
			if (ch >= 'A' && ch <= 'F')
				return (short)(ch - 'A' + 10);
			if (ch >= 'a' && ch <= 'f')
				return (short)(ch - 'a' + 10);

			return -1;
		}

		static bool hexToInt(const char* str, int& n)
		{
			n = 0;
			for (; *str; ++str)
			{
				short x = hexValue(*str);
				if (x == -1)
					return false;

				n = n * 16 + x;
			}

			return true;
		}

		static bool decToInt(const char* str, int& n)
		{
			n = 0;
			for (; *str; ++str)
			{
				if (*str < '0' || *str > '9')
					return false;

				n = n * 10 + (*str - '0');
			}

			return true;
		}

		static bool strToInt(const char* str, int& n)
		{
			if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
			{
				return hexToInt(str + 2, n);
			}

			bool negative = false;
			if (*str == '-')
				negative = true;

			if (decToInt(str, n))
			{
				if (negative)
					n = -n;

				return true;
			}

			return false;
		}
	};
}