#pragma once

#include <xconvert.h>

namespace XTL
{
	enum class GuidFormat
	{
		Default = 0,
		NoBrace = 0x01,
		Lower = 0x02,
		Upper = 0x04,
	};

	inline GuidFormat operator|(GuidFormat a, GuidFormat b)
	{
		return static_cast<GuidFormat>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline bool operator&(GuidFormat a, GuidFormat b)
	{
		return static_cast<int>(a) & static_cast<int>(b);
	}

	class GuidUtil
	{
	public:
		static ULONG Hash(REFGUID guid)
		{
			const DWORD* pdwData = reinterpret_cast<const DWORD*>(&guid);

			return pdwData[0] ^ pdwData[1] ^ pdwData[2] ^ pdwData[3];
		}

		template <typename Char = char>
		static bool GetNextField(const Char*& str, int n, VARTYPE vt, void* pv)
		{
			unsigned long v = 0;

			for (; *str && isspace(*str); ++str)
				;

			if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
			{
				str += 2;
			}

			for (; *str && --n >= 0; ++str)
			{
				short x = Convert::hexValue(*str);
				if (x == -1)
					break;

				v = v * 16 + x;
			}

			switch (vt)
			{
			case VT_UI4:
				*(unsigned long*)pv = v;
				break;

			case VT_UI2:
				*(unsigned short*)pv = (unsigned short)v;
				break;

			case VT_UI1:
				*(unsigned char*)pv = (unsigned char)v;
				break;

			default:
				ATLASSERT(false);
			}

			return true;
		}

		template <typename Char = char>
		static bool MatchChar(const Char*& str, char ch)
		{
			for (; *str && isspace(*str); ++str)
				;

			if (*str == ch)
			{
				for (++str; *str && isspace(*str); ++str)
					;

				return true;
			}

			return false;
		}

		template <typename Char = char>
		static HRESULT FromString(const Char* str, GUID* pGuid)
		{
			if (str == nullptr)
				return E_POINTER;

			bool brace = false;
			if (MatchChar(str, '{'))
			{
				brace = true;
			}

			if (!GetNextField(str, 8, VT_UI4, &pGuid->Data1))
				return S_FALSE;

			if (!MatchChar(str, L'-'))
				return S_FALSE;

			if (!GetNextField(str, 4, VT_UI2, &pGuid->Data2))
				return S_FALSE;

			if (!MatchChar(str, L'-'))
				return S_FALSE;

			if (!GetNextField(str, 4, VT_UI2, &pGuid->Data3))
				return S_FALSE;

			if (!MatchChar(str, L'-'))
				return S_FALSE;

			for (int i = 0; i < 8; ++i)
			{
				if (!GetNextField(str, 2, VT_UI1, &pGuid->Data4[i]))
					return S_FALSE;

				if (i == 1 && !MatchChar(str, L'-'))
					return S_FALSE;
			}

			if (brace && !MatchChar(str, L'}'))
				return S_FALSE;

			return S_OK;
		}

		template<typename Char>
		static void Format(Char*& pbuf, LPBYTE pData, int size, bool reverse)
		{
			static const char _hexChars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

			if (reverse)
			{
				pData += size - 1;
			}

			for (int i = 0; i < size; ++i)
			{
				*pbuf++ = _hexChars[(*pData >> 4) & 0x0F];
				*pbuf++ = _hexChars[*pData & 0x0F];

				if (reverse)
				{
					--pData;
				}
				else
				{
					++pData;
				}
			}
		}

		static HRESULT ToString(REFGUID guid, GuidFormat format, BSTR* pbstr)
		{
			int len = ToString(guid, format, (WCHAR*)nullptr, 0);

			*pbstr = SysAllocStringLen(nullptr, len);
			if (*pbstr == nullptr)
				return E_OUTOFMEMORY;

			ToString(guid, format, *pbstr, len + 1);

			return S_OK;
		}

		template <typename Char = wchar_t, size_t size>
		static int ToString(REFGUID guid, GuidFormat format, Char (&buf)[size])
		{
			return ToString(guid, format, buf, size);
		}

		template<typename Char = wchar_t>
		static int ToString(REFGUID guid, GuidFormat format, Char* pbuf, DWORD size)
		{
			int len = (format & GuidFormat::NoBrace) ? 36 : 38;
			if (!pbuf)
			{
				return len;
			}

			if (size <= len)
				return -1;

			auto p = pbuf;

			if (!(format & GuidFormat::NoBrace))
			{
				*p++ = '{';
			}

			Format(p, (LPBYTE)&guid.Data1, 4, true);
			*p++ = '-';
			Format(p, (LPBYTE)&guid.Data2, 2, true);
			*p++ = '-';
			Format(p, (LPBYTE)&guid.Data3, 2, true);
			*p++ = '-';
			Format(p, (LPBYTE)guid.Data4, 2, false);
			*p++ = '-';
			Format(p, (LPBYTE)guid.Data4 + 2, 6, false);

			if (!(format & GuidFormat::NoBrace))
			{
				*p++ = '}';
			}

			*p = '\0';

			if (format & GuidFormat::Lower)
			{
				for (int i = 0; i < len; ++i)
				{
					pbuf[i] = tolower(pbuf[i]);
				}
			}

			return len;
		}
	};

	const int _GUID_STRING_LEN = 39;

	class GuidString
	{
	public:
		GuidString(REFGUID guid, GuidFormat format = GuidFormat::Default)
		{
			GuidUtil::ToString(guid, format, m_str);
		}

		~GuidString()
		{
		}

		operator LPCWSTR() const throw()
		{
			return m_str;
		}

		WCHAR m_str[_GUID_STRING_LEN + 1];
	};

	class GuidStringA
	{
	public:
		GuidStringA(REFGUID guid, GuidFormat format = GuidFormat::Default)
		{
			GuidUtil::ToString(guid, format, m_str);
		}

		~GuidStringA()
		{
		}

		operator LPCSTR() const throw()
		{
			return m_str;
		}

		char m_str[_GUID_STRING_LEN + 1];
	};
};