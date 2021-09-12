#pragma once

#include <atlbase.h>
#include <atlstr.h>
#include <varenum.h>
#include <xguid.h>

namespace XTL
{
	class RegKey : public CRegKey
	{
	public:
		RegKey(HKEY hKey = nullptr)
			: CRegKey(hKey)
		{
		}

		LSTATUS __cdecl CreateF(HKEY hKeyParent, LPCTSTR format, ...)
		{
			va_list argList;
			va_start(argList, format);

			CString keyName;
			keyName.FormatV(format, argList);

			va_end(argList);

			return __super::Create(hKeyParent, keyName);
		}

		LSTATUS __cdecl OpenF(HKEY hKeyParent, LPCTSTR format, ...)
		{
			va_list argList;
			va_start(argList, format);

			CString keyName;
			keyName.FormatV(format, argList);

			va_end(argList);

			return __super::Open(hKeyParent, keyName);
		}

		LONG __cdecl CreateV(HKEY hKeyParent, int n, ...) throw()
		{
			LONG ret = ERROR_SUCCESS;

			va_list argList;
			va_start(argList, n);

			for (int i = 0; i < n; ++i)
			{
				LPCWSTR keyName = va_arg(argList, LPCWSTR);

				if (keyName && *keyName)
				{
					ret = Create(hKeyParent, keyName);
					if (ret != ERROR_SUCCESS)
						break;

					hKeyParent = m_hKey;
				}
			}

			va_end(argList);

			return ret;
		}

		LONG __cdecl OpenV(HKEY hKeyParent, REGSAM samDesired, int n, ...) throw()
		{
			LONG ret = ERROR_SUCCESS;

			va_list argList;
			va_start(argList, n);

			for (int i = 0; i < n; ++i)
			{
				LPCWSTR keyName = va_arg(argList, LPCWSTR);

				if (keyName && *keyName)
				{
					ret = Open(hKeyParent, keyName, samDesired);
					if (ret != ERROR_SUCCESS)
						break;

					hKeyParent = m_hKey;
				}
			}

			va_end(argList);

			return ret;
		}

		DWORD QueryDWORD(LPCTSTR valueName, DWORD defVal) throw()
		{
			DWORD result = 0;

			if (CRegKey::QueryDWORDValue(valueName, result) != ERROR_SUCCESS)
			{
				result = defVal;
			}

			return result;
		}

		LONG QueryStringValue(LPCTSTR valueName, LPWSTR value, ULONG chars, ULONG* pChars = nullptr)
		{
			LONG ret = CRegKey::QueryStringValue(valueName, value, &chars);
			if (ret == ERROR_SUCCESS)
			{
				if (pChars)
				{
					*pChars = chars;
				}
			}
			else if (value)
			{
				*value = '\0';
			}

			return ret;
		}

		LONG QueryString(LPCWSTR valueName, CString& value, LPCWSTR defVal = nullptr) throw()
		{
			DWORD size = 0;
			LONG ret = CRegKey::QueryStringValue(valueName, nullptr, &size);
			if (ret == ERROR_SUCCESS)
			{
				LPTSTR buff = value.GetBufferSetLength(size + 1);
				ret = CRegKey::QueryStringValue(valueName, buff, &size);
				value.ReleaseBuffer();
			}
			else if (defVal)
			{
				value = defVal;
			}

			return ret;
		}

		LONG QueryString(LPCWSTR valueName, CStringA& value, LPCSTR defVal = nullptr) throw()
		{
			DWORD size = 0;
			LONG ret = CRegKey::QueryStringValue(valueName, nullptr, &size);
			if (ret == ERROR_SUCCESS)
			{
				CTempBuffer<WCHAR> spBuff;
				spBuff.Allocate(size + 1);
				ret = CRegKey::QueryStringValue(valueName, spBuff, &size);

				value = spBuff;
			}
			else if (defVal)
			{
				value = defVal;
			}

			return ret;
		}

		LONG QueryStringValue(LPCWSTR valueName, BSTR* pValue, LPCWSTR defaultValue = nullptr)
		{
			if (*pValue)
			{
				SysFreeString(*pValue);
				*pValue = nullptr;
			}

			DWORD size = 0;
			LONG ret = CRegKey::QueryStringValue(valueName, nullptr, &size);
			if (ret != ERROR_SUCCESS)
			{
				if (defaultValue)
				{
					*pValue = SysAllocString(defaultValue);
				}

				return ret;
			}

			*pValue = SysAllocStringLen(nullptr, size);
			if (*pValue == nullptr)
				return ERROR_OUTOFMEMORY;

			ret = CRegKey::QueryStringValue(valueName, *pValue, &size);
			if (ret != ERROR_SUCCESS)
			{
				SysFreeString(*pValue);
				*pValue = nullptr;
			}

			return ret;
		}

		inline HRESULT __cdecl SetStringValueV(LPCWSTR valueName, LPCWSTR format, ...)
		{
			ATLASSERT(format);

			va_list argList;
			va_start(argList, format);

			CString value;
			value.FormatV(format, argList);

			va_end(argList);

			return SetStringValue(valueName, value);
		}

		LONG QueryDoubleValue(LPCWSTR valueName, double& val)
		{
			ULONG bytes = sizeof(double);
			LONG ret = CRegKey::QueryBinaryValue(valueName, &val, &bytes);
			if (ret == ERROR_SUCCESS)
			{
				if (bytes != sizeof(double))
				{
					val = 0;
					return ERROR_INVALID_DATA;
				}
			}

			return ret;
		}

		LONG SetDoubleValue(LPCWSTR valueName, double val)
		{
			return CRegKey::SetBinaryValue(valueName, &val, sizeof(double));
		}

		int _GetNextField(LPCWSTR& value, LPWSTR result, DWORD cchText)
		{
			while (iswspace(*value))
			{
				++value;
			}

			ULONG n = 0;
			for (; *value; ++value)
			{
				if (n < cchText)
				{
					result[n++] = *value;
				}

				if (*value == ',')
				{
					++value;
					break;
				}
			}

			result[n] = '\0';

			return n;
		}

		int _ParseLongArray(LPCWSTR value, LONG* pVal, ULONG size)
		{
			WCHAR field[11];

			memset(pVal, 0, size * sizeof(LONG));

			ULONG i = 0;
			for (; i < size && *value; ++i)
			{
				if (_GetNextField(value, field, 10))
				{
					pVal[i] = _wtol(field);
				}
			}

			return i;
		}

		int _ParseShortArray(LPCWSTR value, SHORT* pVal, ULONG size)
		{
			WCHAR field[11];

			memset(pVal, 0, size * sizeof(SHORT));

			ULONG i = 0;
			for (; i < size && *value; ++i)
			{
				if (_GetNextField(value, field, 10))
				{
					pVal[i] = _wtoi(field);
				}
			}

			return i;
		}

		LONG QueryLongArray(LPCWSTR valueName, LONG* pVal, ULONG size)
		{
			CString value;
			LONG ret = QueryString(valueName, value);
			if (ret == ERROR_SUCCESS)
			{
				_ParseLongArray(value, pVal, size);
			}

			return ret;
		}

		LONG SetLongArray(LPCWSTR valueName, LONG* pVal, ULONG size)
		{
			CString value;

			for (ULONG i = 0; i < size; ++i)
			{
				if (value.GetLength())
				{
					value += L",";
				}

				value.AppendFormat(L"%d", pVal[i]);
			}

			return SetStringValue(valueName, value);
		}

		LONG QueryShortArray(LPCWSTR valueName, SHORT* pVal, ULONG size)
		{
			CString value;

			LONG ret = QueryString(valueName, value);
			if (ret == ERROR_SUCCESS)
			{
				_ParseShortArray(value, pVal, size);
			}

			return ret;
		}

		LONG SetShortArray(LPCWSTR valueName, SHORT* pVal, ULONG size)
		{
			CString value;

			for (ULONG i = 0; i < size; ++i)
			{
				if (value.GetLength())
				{
					value += L",";
				}

				value.AppendFormat(L"%d", pVal[i]);
			}

			return SetStringValue(valueName, value);
		}

		LONG QueryPoint(LPCWSTR valueName, POINT& point)
		{
			TCHAR value[65] = L"";
			LONG ret = QueryStringValue(valueName, value, 64);
			if (ret == ERROR_SUCCESS)
			{
				LONG val[2];
				_ParseLongArray(value, val, 2);

				point.x = val[0];
				point.y = val[1];
			}

			return ret;
		}

		LONG SetPoint(LPCWSTR valueName, POINT& point)
		{
			TCHAR value[65] = L"";
			swprintf_s(value, 64, L"%d,%d", point.x, point.y);

			return SetStringValue(valueName, value);
		}

		LONG QueryRect(LPCWSTR valueName, LPRECT lpRect)
		{
			TCHAR value[65] = L"";
			LONG ret = QueryStringValue(valueName, value, 64);
			if (ret == ERROR_SUCCESS)
			{
				LONG val[4];
				_ParseLongArray(value, val, 4);

				lpRect->left = val[0];
				lpRect->top = val[1];
				lpRect->right = val[2];
				lpRect->bottom = val[3];
			}

			return ret;
		}

		LONG SetRect(LPCWSTR valueName, LPCRECT lpRect)
		{
			TCHAR value[65] = L"";
			swprintf_s(value, 64, L"%d,%d,%d,%d", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);

			return SetStringValue(valueName, value);
		}

		LONG QueryIndexOfStringArray(LPCWSTR valueName, LPCWSTR* pValues, int size, int* pVal)
		{
			CString value;
			LONG ret = QueryString(valueName, value);
			if (ret == ERROR_SUCCESS)
			{
				for (int i = 0; i < size; ++i)
				{
					if (_wcsicmp(value, pValues[i]) == 0)
					{
						*pVal = i;
						break;
					}
				}
			}

			return ret;
		}

		LONG QueryGUID(LPCWSTR valueName, LPGUID lpGuid)
		{
			TCHAR value[40] = L"";
			LONG ret = QueryStringValue(valueName, value, 40);
			if (ret == ERROR_SUCCESS)
			{
				GuidUtil::FromString(value, lpGuid);
			}

			return ret;
		}

		LONG SetGUID(LPCWSTR valueName, LPGUID lpGuid)
		{
			WCHAR value[40];
			GuidUtil::ToString(*lpGuid, GuidFormat::Default, value);

			return SetStringValue(valueName, value);
		}

		LONG QueryValue(LPCWSTR valueName, VARTYPE vt, void* pv, DWORD size)
		{
			LONG ret = -1;

			switch (vt)
			{
			case VT_BSTR:
				ret = QueryStringValue(valueName, (BSTR*)pv);
				break;

			case VT_WSTR:
				ret = QueryStringValue(valueName, (LPWSTR)pv, size / sizeof(WCHAR));
				break;

			case VT_I4:
			case VT_UI4:
				ret = QueryDWORDValue(valueName, (DWORD&)*(LONG*)pv);
				break;

			case VT_I2:
			case VT_UI2:
				ret = QueryDWORDValue(valueName, (DWORD&)*(SHORT*)pv);
				break;

			case VT_INT:
			case VT_UINT:
				ret = QueryDWORDValue(valueName, (DWORD&)*(int*)pv);
				break;

			case VT_GUID:
				ret = QueryGUID(valueName, (LPGUID)pv);
				break;
			}

			return ret;
		}

		LONG SetValue(LPCWSTR valueName, VARTYPE vt, void* pv, DWORD size)
		{
			LONG ret = -1;

			switch (vt)
			{
			case VT_BSTR:
				ret = SetStringValue(valueName, *(BSTR*)pv);
				break;

			case VT_WSTR:
				ret = SetStringValue(valueName, (LPWSTR)pv);
				break;

			case VT_I4:
			case VT_UI4:
				ret = SetDWORDValue(valueName, *(LONG*)pv);
				break;

			case VT_I2:
			case VT_UI2:
				ret = SetDWORDValue(valueName, *(SHORT*)pv);
				break;

			case VT_INT:
			case VT_UINT:
				ret = SetDWORDValue(valueName, *(int*)pv);
				break;

			case VT_GUID:
				ret = SetGUID(valueName, (LPGUID)pv);
				break;
			}

			return ret;
		}
	};

	template <ULONG cchName = MAX_PATH>
	class RegKeyEnumerator
	{
	public:
		RegKeyEnumerator(HKEY hKey)
		{
			m_hKey = hKey;
			m_index = 0;
			memset(m_name, 0, sizeof(m_name));
		}

		LPCWSTR GetNextKey()
		{
			if (RegEnumKey(m_hKey, m_index++, m_name, cchName) == ERROR_SUCCESS)
			{
				return m_name;
			}

			return nullptr;
		}

		void Reset()
		{
			m_index = 0;
			memset(m_name, 0, sizeof(m_name));
		}

		HKEY m_hKey;
		DWORD m_index;
		TCHAR m_name[cchName];
	};

	template <ULONG cchValueName = 80>
	class RegValueEnumerator
	{
	public:
		RegValueEnumerator(HKEY hKey)
		{
			m_hKey = hKey;
			m_index = 0;
			memset(m_valueName, 0, sizeof(m_valueName));
		}

		LPCWSTR GetNextValue(LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
		{
			DWORD cValueName = cchValueName;
			if (RegEnumValue(m_hKey, m_index++, m_valueName, &cValueName, nullptr, lpType, lpData, lpcbData) == ERROR_SUCCESS)
			{
				return m_valueName;
			}

			return nullptr;
		}

		void Reset()
		{
			m_index = 0;
			memset(m_valueName, 0, sizeof(m_valueName));
		}

		HKEY m_hKey;
		DWORD m_index;
		TCHAR m_valueName[cchValueName];
	};

};