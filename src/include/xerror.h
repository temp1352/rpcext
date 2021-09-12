
#pragma once

#include "xdef.h"

#include <atlbase.h>
#include <atltime.h>
#include <atlstr.h>

namespace XTL
{
	inline void _String_FormatV(CString& str, UINT formatId, va_list args)
	{
		CString format;
		if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
		{
			str.FormatV(format, args);
		}
	}

	inline void _String_FormatV(CStringA& str, UINT formatId, va_list args)
	{
		CStringA format;
		if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
		{
			str.FormatV(format, args);
		}
	}

	inline void _String_AppendFormatV(CString& str, UINT formatId, va_list args)
	{
		CString format;
		if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
		{
			str.FormatV(format, args);
		}
	}

	inline void _String_AppendFormatV(CStringA& str, UINT formatId, va_list args)
	{
		CStringA format;
		if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
		{
			str.FormatV(format, args);
		}
	}

	enum
	{
		XTL_RESERVE_ERRORINFO = 0x010000,
	};

	enum XTL_REPORT_ERROR_FLAG
	{
		XTL_REF_APPEND_FORMAT_MESSAGE = 0x01,
	};

	class Error
	{
	public:
		static HRESULT __cdecl GetErrorDescription(HRESULT hRes, DWORD langId, BSTR* pErrDesc, LPCWSTR format, ...)
		{
			CComBSTR error;
			HRESULT hr = GetErrorDescription(hRes, langId, &error);
			if (FAILED(hr))
			{
				return hr;
			}

			if (*pErrDesc)
			{
				SysFreeString(*pErrDesc);
				*pErrDesc = nullptr;
			}

			va_list argList;
			va_start(argList, format);

			int len1 = _vscwprintf(format, argList);
			int len2 = error.Length();
			int len = len1 + len2 + 1;
			*pErrDesc = SysAllocStringLen(nullptr, len);
			if (*pErrDesc != nullptr)
			{
				vswprintf_s(*pErrDesc, len1 + 1, format, argList);

				(*pErrDesc)[len1] = ',';
				wcsncpy_s(*pErrDesc + len1 + 1, len2 + 1, error, len2);
				(*pErrDesc)[len] = '\0';
			}

			va_end(argList);

			return *pErrDesc ? S_OK : E_OUTOFMEMORY;
		}

		static HRESULT GetErrorDescription(HRESULT hRes, DWORD langId, BSTR* pErrDesc)
		{
			if (!pErrDesc)
				return E_POINTER;

			if (*pErrDesc)
			{
				SysFreeString(*pErrDesc);
				*pErrDesc = nullptr;
			}

			CComPtr<IErrorInfo> spInfo;
			HRESULT hr = GetErrorInfo(0, &spInfo);
			if (SUCCEEDED(hr) && spInfo)
			{
				hr = spInfo->GetDescription(pErrDesc);

				if (langId & XTL_RESERVE_ERRORINFO)
				{
					SetErrorInfo(0, spInfo);
				}

				if (SUCCEEDED(hr) && *pErrDesc)
				{
					return hr;
				}
			}

			LPWSTR message = nullptr;
			if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					nullptr, hRes, langId & 0x0000ffff, (LPWSTR)&message, 0, nullptr) == 0)
			{
				if (hRes != S_OK)
				{
					WCHAR buffer[64];

					swprintf_s(buffer, L"HRESULT 0x%8.8x", hRes);

					*pErrDesc = SysAllocString(buffer);

					return *pErrDesc ? S_OK : E_OUTOFMEMORY;
				}
			}
			else
			{
				int len = wcslen(message);
				while (len > 0 && (message[len - 1] == '\r' || message[len - 1] == '\n'))
				{
					len--;
				}

				*pErrDesc = SysAllocStringLen(message, len);

				LocalFree(message);

				return *pErrDesc ? S_OK : E_OUTOFMEMORY;
			}

			return S_FALSE;
		}

		static void AppendCode(CString& str, HRESULT hRes)
		{
			WCHAR code[13] = L"";
			int codeLen = swprintf_s(code, L"(0x%8.8x)", hRes);

			int len = str.GetLength();
			if (len < codeLen || wcscmp((LPCWSTR)str + (len - codeLen), code) != 0)
			{
				str += code;
			}
		}

		static void AppendCode(CStringA& str, HRESULT hRes)
		{
			CHAR code[13] = "";
			int codeLen = sprintf_s(code, "(0x%8.8x)", hRes);

			int len = str.GetLength();
			if (len < codeLen || strcmp((LPCSTR)str + (len - codeLen), code) != 0)
			{
				str += code;
			}
		}

		static void __cdecl GetErrorDescription(CString& str, __in HRESULT hRes, __in DWORD langId = 0, UINT formatId = 0, ...) throw(...)
		{
			CComPtr<IErrorInfo> spInfo;
			HRESULT hr = GetErrorInfo(0, &spInfo);
			if (SUCCEEDED(hr) && spInfo)
			{
				CComBSTR bstr;
				hr = spInfo->GetDescription(&bstr);

				if (langId & XTL_RESERVE_ERRORINFO)
				{
					SetErrorInfo(0, spInfo);
				}

				if (SUCCEEDED(hr) && bstr)
				{
					str = CW2T(bstr);
					AppendCode(str, hRes);

					return;
				}
			}

			LPWSTR message = nullptr;
			if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					nullptr, hRes, langId & 0x0000ffff, (LPWSTR)&message, 0, nullptr) == 0)
			{
				if (formatId != 0)
				{
					va_list argList;
					va_start(argList, formatId);
					_String_FormatV(str, formatId, argList);
					va_end(argList);
				}
				else
				{
					str.Empty();
				}
			}
			else
			{
				str = CString(message);
				LocalFree(message);

				int len = str.GetLength();
				while (len > 0 && (str[len - 1] == '\r' || str[len - 1] == '\n'))
				{
					len--;
				}

				str.Truncate(len);
			}

			AppendCode(str, hRes);
		}

		static void __cdecl GetErrorDescription(CStringA& str, __in HRESULT hRes, __in DWORD langId = 0, UINT formatId = 0, ...) throw(...)
		{
			CComPtr<IErrorInfo> spInfo;
			HRESULT hr = GetErrorInfo(0, &spInfo);
			if (SUCCEEDED(hr) && spInfo)
			{
				CComBSTR bstr;
				hr = spInfo->GetDescription(&bstr);

				if (SUCCEEDED(hr) && bstr)
				{
					str = CW2A(bstr);
					AppendCode(str, hRes);

					return;
				}
			}

			LPTSTR message = nullptr;
			if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					nullptr, hRes, langId, (LPTSTR)&message, 0, nullptr) == 0)
			{
				if (formatId)
				{
					va_list argList;
					va_start(argList, formatId);
					_String_FormatV(str, formatId, argList);
					va_end(argList);
				}
			}
			else
			{
				str = CStringA(message);
				LocalFree(message);

				int len = str.GetLength();
				while (len > 0 && (str[len - 1] == '\r' || str[len - 1] == '\n'))
				{
					len--;
				}
				str.Truncate(len);
			}

			AppendCode(str, hRes);
		}

		static HRESULT ReportError(HRESULT hRes, UINT textId)
		{
			CString text;
			if (text.LoadString(_AtlBaseModule.GetResourceInstance(), textId))
			{
				return _ReportError(CLSID_NULL, text, IID_NULL, hRes);
			}

			return E_FAIL;
		}

		static HRESULT ReportError(HRESULT hRes, LPCWSTR text)
		{
			return _ReportError(CLSID_NULL, text, IID_NULL, hRes);
		}

		static HRESULT ReportError(HRESULT hr, int codePage, LPCSTR error)
		{
			if (!error)
				return hr;

			int len = strlen(error);
			int lenW = MultiByteToWideChar(codePage, 0, error, len, nullptr, 0);

			CTempBuffer<WCHAR, 1024> spBuf;
			if (!spBuf.Allocate(lenW + 1))
				return hr;

			MultiByteToWideChar(codePage, 0, error, len, spBuf, lenW);
			spBuf[lenW] = '\0';

			Error::ReportError(hr, spBuf);

			return hr;
		}

		static HRESULT ReportError(REFCLSID clsid, HRESULT hRes, UINT textId)
		{
			CString text;
			if (text.LoadString(_AtlBaseModule.GetResourceInstance(), textId))
			{
				return _ReportError(clsid, text, IID_NULL, hRes);
			}

			return E_FAIL;
		}

		static HRESULT ReportError(REFCLSID clsid, HRESULT hRes, LPCWSTR text)
		{
			return _ReportError(clsid, text, IID_NULL, hRes);
		}

		static HRESULT ReportError(REFCLSID clsid, REFIID riid, HRESULT hRes, UINT textId)
		{
			CString text;
			if (text.LoadString(_AtlBaseModule.GetResourceInstance(), textId))
			{
				return _ReportError(clsid, text, riid, hRes);
			}

			return E_FAIL;
		}

		static HRESULT ReportError(REFCLSID clsid, REFIID riid, HRESULT hRes, LPCWSTR text)
		{
			return _ReportError(clsid, text, riid, hRes);
		}

		static HRESULT __cdecl ReportErrorF(HRESULT hRes, UINT formatId, ...)
		{
			CString text, format;
			if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatV(format, argList);
				va_end(argList);

				return _ReportError(CLSID_NULL, text, IID_NULL, hRes);
			}

			return E_FAIL;
		}

		static HRESULT __cdecl ReportErrorF(HRESULT hRes, LPCWSTR format, ...)
		{
			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatV(format, argList);
			va_end(argList);

			return _ReportError(CLSID_NULL, text, IID_NULL, hRes);
		}

		static HRESULT __cdecl ReportErrorF(REFCLSID clsid, HRESULT hRes, UINT formatId, ...)
		{
			CString text, format;
			if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatV(format, argList);
				va_end(argList);

				return _ReportError(clsid, text, IID_NULL, hRes);
			}

			return E_FAIL;
		}

		static HRESULT __cdecl ReportErrorF(REFCLSID clsid, HRESULT hRes, LPCWSTR format, ...)
		{
			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatV(format, argList);
			va_end(argList);

			return _ReportError(clsid, text, IID_NULL, hRes);
		}

		static HRESULT __cdecl ReportErrorF(REFCLSID clsid, REFIID riid, HRESULT hRes, UINT formatId, ...)
		{
			CString text, format;

			if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatV(format, argList);
				va_end(argList);

				return _ReportError(clsid, text, riid, hRes);
			}

			return E_FAIL;
		}

		static HRESULT __cdecl ReportErrorF(REFCLSID clsid, REFIID riid, HRESULT hRes, LPCWSTR format, ...)
		{
			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatV(format, argList);
			va_end(argList);

			return _ReportError(clsid, text, riid, hRes);
		}

		static HRESULT __cdecl ReportMessageF(HRESULT hRes, UINT formatId, ...)
		{
			CString text, format;
			if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatMessageV(format, &argList);
				va_end(argList);

				return _ReportError(CLSID_NULL, text, IID_NULL, hRes);
			}

			return E_FAIL;
		}

		static HRESULT __cdecl ReportMessageF(HRESULT hRes, LPCWSTR format, ...)
		{
			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatMessageV(format, &argList);
			va_end(argList);

			return _ReportError(CLSID_NULL, text, IID_NULL, hRes);
		}

		static HRESULT __cdecl ReportMessageF(REFCLSID clsid, HRESULT hRes, UINT formatId, ...)
		{
			CString text, format;
			if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatMessageV(format, &argList);
				va_end(argList);

				return _ReportError(clsid, text, IID_NULL, hRes);
			}

			return E_FAIL;
		}

		static HRESULT __cdecl ReportMessageF(REFCLSID clsid, HRESULT hRes, LPCWSTR format, ...)
		{
			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatMessageV(format, &argList);
			va_end(argList);

			return _ReportError(clsid, text, IID_NULL, hRes);
		}

		static HRESULT __cdecl ReportMessageF(REFCLSID clsid, REFIID riid, HRESULT hRes, UINT formatId, ...)
		{
			CString text, format;
			if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatMessageV(format, &argList);
				va_end(argList);

				return _ReportError(clsid, text, riid, hRes);
			}

			return E_FAIL;
		}

		static HRESULT __cdecl ReportMessageF(REFCLSID clsid, REFIID riid, HRESULT hRes, LPCWSTR format, ...)
		{
			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatMessageV(format, &argList);
			va_end(argList);

			return _ReportError(clsid, text, riid, hRes);
		}

		static HRESULT __cdecl ReportErrorEx(REFCLSID clsid, REFIID riid, HRESULT hRes, DWORD flags, UINT formatId, ...)
		{
			CString text, format;
			if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatV(format, argList);
				va_end(argList);

				if (flags & XTL_REF_APPEND_FORMAT_MESSAGE)
				{
					CComBSTR str;
					CComPtr<IErrorInfo> spInfo;
					if (GetErrorInfo(0, &spInfo) == S_OK && spInfo && spInfo->GetDescription(&str) == S_OK)
					{
						text += L"\r\n-->";
						text += str;
					}
					else
					{
						LPWSTR message = nullptr;
						if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								nullptr, hRes, 0, (LPWSTR)&message, 0, nullptr))
						{
							text += L"\r\n-->";

							int i = wcslen(message) - 1;
							while (i >= 0 && (message[i] == '\r' || message[i] == '\n'))
							{
								message[i--] = '\0';
							}

							text += message;

							LocalFree(message);
						}
					}
				}

				return _ReportError(clsid, text, riid, hRes);
			}

			return E_FAIL;
		}

		static HRESULT __cdecl ReportErrorEx(REFCLSID clsid, REFIID riid, HRESULT hRes, DWORD flags, LPCWSTR format, ...)
		{
			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatV(format, argList);
			va_end(argList);

			if (flags & XTL_REF_APPEND_FORMAT_MESSAGE)
			{
				CComBSTR str;
				CComPtr<IErrorInfo> spInfo;
				if (GetErrorInfo(0, &spInfo) == S_OK && spInfo && spInfo->GetDescription(&str) == S_OK)
				{
					text += L"\r\n-->";
					text += str;
				}
				else
				{
					LPWSTR message = nullptr;
					if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							nullptr, hRes, 0, (LPWSTR)&message, 0, nullptr))
					{
						text += L"\r\n-->";
						text += message;

						LocalFree(message);
					}
				}
			}

			return _ReportError(clsid, text, riid, hRes);
		}

		static HRESULT _ReportError(const CLSID& clsid, UINT nID, const IID& iid = GUID_NULL,
			HRESULT hRes = 0, HINSTANCE hInst = _AtlBaseModule.GetResourceInstance())
		{
			return _SetErrorInfo(clsid, (LPCOLESTR)MAKEINTRESOURCE(nID), 0, nullptr, iid, hRes, hInst);
		}

		static HRESULT _ReportError(const CLSID& clsid, UINT nID, DWORD dwHelpID,
			LPCOLESTR lpszHelpFile, const IID& iid = GUID_NULL, HRESULT hRes = 0,
			HINSTANCE hInst = _AtlBaseModule.GetResourceInstance())
		{
			return _SetErrorInfo(clsid, (LPCOLESTR)MAKEINTRESOURCE(nID), dwHelpID,
				lpszHelpFile, iid, hRes, hInst);
		}

		static HRESULT _ReportError(const CLSID& clsid, LPCSTR lpszDesc,
			DWORD dwHelpID, LPCSTR lpszHelpFile, const IID& iid = GUID_NULL, HRESULT hRes = 0)
		{
			ATLASSERT(lpszDesc != nullptr);
			if (lpszDesc == nullptr)
				return E_POINTER;

			USES_CONVERSION_EX;
			LPCOLESTR pwszDesc = A2COLE_EX(lpszDesc, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
			if (pwszDesc == nullptr)
				return E_OUTOFMEMORY;

			LPCWSTR pwzHelpFile = nullptr;
			if (lpszHelpFile != nullptr)
			{
				pwzHelpFile = A2CW_EX(lpszHelpFile, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
				if (pwzHelpFile == nullptr)
					return E_OUTOFMEMORY;
			}

			return _SetErrorInfo(clsid, pwszDesc, dwHelpID, pwzHelpFile, iid, hRes, nullptr);
		}

		static HRESULT _ReportError(const CLSID& clsid, LPCSTR lpszDesc,
			const IID& iid = GUID_NULL, HRESULT hRes = 0)
		{
			return _ReportError(clsid, lpszDesc, 0, nullptr, iid, hRes);
		}

		static HRESULT _ReportError(const CLSID& clsid, LPCOLESTR lpszDesc,
			const IID& iid = GUID_NULL, HRESULT hRes = 0)
		{
			return _SetErrorInfo(clsid, lpszDesc, 0, nullptr, iid, hRes, nullptr);
		}

		static HRESULT _ReportError(const CLSID& clsid, LPCOLESTR lpszDesc, DWORD dwHelpID,
			LPCOLESTR lpszHelpFile, const IID& iid = GUID_NULL, HRESULT hRes = 0)
		{
			return _SetErrorInfo(clsid, lpszDesc, dwHelpID, lpszHelpFile, iid, hRes, nullptr);
		}

		static HRESULT _SetErrorInfo(const CLSID& clsid, LPCOLESTR lpszDesc, DWORD dwHelpID,
			LPCOLESTR lpszHelpFile, const IID& iid, HRESULT hRes, HINSTANCE hInst)
		{
			USES_CONVERSION_EX;

			WCHAR szDesc[1024] = { 0 };

			// For a valid HRESULT the id should be in the range [0x0200, 0xffff]
			if (IS_INTRESOURCE(lpszDesc)) //id
			{
				UINT nID = LOWORD((DWORD_PTR)lpszDesc);
				ATLASSERT((nID >= 0x0200 && nID <= 0xffff) || hRes != 0);

				if (LoadString(hInst, nID, szDesc, 1024) == 0)
				{
					ATLASSERT(FALSE);
					Checked::tcscpy_s(szDesc, _countof(szDesc), _T("Unknown Error"));
				}

				lpszDesc = T2OLE_EX(szDesc, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
#ifndef _UNICODE

				if (lpszDesc == nullptr)
					return E_OUTOFMEMORY;

#endif
				if (hRes == 0)
					hRes = MAKE_HRESULT(3, FACILITY_ITF, nID);
			}

			CComPtr<ICreateErrorInfo> pICEI;
			if (SUCCEEDED(CreateErrorInfo(&pICEI)))
			{
				CComPtr<IErrorInfo> pErrorInfo;
				pICEI->SetGUID(iid);

				LPOLESTR lpsz = nullptr;
				ProgIDFromCLSID(clsid, &lpsz);
				if (lpsz != nullptr)
				{
					pICEI->SetSource(lpsz);
					CoTaskMemFree(lpsz);
				}

				if (dwHelpID != 0 && lpszHelpFile != nullptr)
				{
					pICEI->SetHelpContext(dwHelpID);
					pICEI->SetHelpFile(const_cast<LPOLESTR>(lpszHelpFile));
				}

				pICEI->SetDescription((LPOLESTR)lpszDesc);
				if (SUCCEEDED(pICEI->QueryInterface(__uuidof(IErrorInfo), (void**)&pErrorInfo)))
				{
					SetErrorInfo(0, pErrorInfo);
				}
			}

			return (hRes == 0) ? DISP_E_EXCEPTION : hRes;
		}
	};

	class Message
	{
	public:
		static int _MessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
		{
			WCHAR path[MAX_PATH];
			if (!lpCaption || *lpCaption == '\0')
			{
				GetModuleFileName(nullptr, path, MAX_PATH);
				::PathRemoveExtension(path);

				lpCaption = ::PathFindFileName(path);
			}

			return ::MessageBox(hWnd, lpText, lpCaption, uType);
		}

		static int __cdecl MessageBoxV(HWND hWnd, LPCWSTR format, ...)
		{
			ATLASSERT(format);

			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatV(format, argList);
			va_end(argList);

			return _MessageBox(hWnd, text, nullptr, MB_OK | MB_ICONERROR);
		}

		static int __cdecl MessageBoxV(HWND hWnd, UINT formatId, ...)
		{
			CString text, format;
			if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatV(format, argList);
				va_end(argList);

				return _MessageBox(hWnd, text, nullptr, MB_OK | MB_ICONERROR);
			}

			return 0;
		}

		static int __cdecl MessageBoxV(HWND hWnd, LPCTSTR caption, UINT type, LPCWSTR format, ...)
		{
			ATLASSERT(format);

			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatV(format, argList);
			va_end(argList);

			return _MessageBox(hWnd, text, caption, type);
		}

		static int __cdecl MessageBoxV(HWND hWnd, LPCTSTR caption, UINT type, UINT formatId, ...)
		{
			CString text, format;
			if (format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatV(format, argList);
				va_end(argList);

				return _MessageBox(hWnd, text, caption, type);
			}

			return 0;
		}

		static int __cdecl MessageBoxV(HWND hWnd, UINT captionId, UINT type, LPCWSTR format, ...)
		{
			ATLASSERT(format);

			CString caption, text;
			caption.LoadString(captionId);

			va_list argList;
			va_start(argList, format);
			text.FormatV(format, argList);
			va_end(argList);

			return _MessageBox(hWnd, text, caption, type);
		}

		static int __cdecl MessageBoxV(HWND hWnd, UINT captionId, UINT type, UINT formatId, ...)
		{
			CString caption, text, format;
			if (caption.LoadString(captionId) && format.LoadString(_AtlBaseModule.GetResourceInstance(), formatId))
			{
				va_list argList;
				va_start(argList, formatId);
				text.FormatV(format, argList);
				va_end(argList);

				return _MessageBox(hWnd, text, caption, type);
			}

			return 0;
		}

		static int MessageBoxH(HWND hWnd, HRESULT hr = E_FAIL, DWORD langId = 0)
		{
			CString error;
			Error::GetErrorDescription(error, hr, langId, 0);

			if (!error.IsEmpty())
			{
				return _MessageBox(hWnd, error, nullptr, MB_OK | MB_ICONERROR);
			}

			return 0;
		}

		static int _cdecl MessageBoxHV(HWND hWnd, HRESULT hr, LPCTSTR format, ...)
		{
			CString text;

			va_list argList;
			va_start(argList, format);
			text.FormatV(format, argList);
			va_end(argList);

			CString error;
			Error::GetErrorDescription(error, hr, 0, 0);

			text += _T("\n") + error;

			if (!error.IsEmpty())
			{
				return _MessageBox(hWnd, text, nullptr, MB_OK | MB_ICONERROR);
			}

			return 0;
		}

		static int _cdecl MessageBoxHV(HWND hWnd, HRESULT hr, UINT formatId, ...)
		{
			CString text;
			CString format;
			format.LoadString(formatId);

			va_list argList;
			va_start(argList, formatId);
			text.FormatV(format, argList);
			va_end(argList);

			CString error;
			Error::GetErrorDescription(error, hr, 0, 0);

			text += _T("\n") + error;

			if (!error.IsEmpty())
			{
				return _MessageBox(hWnd, text, nullptr, MB_OK | MB_ICONERROR);
			}

			return 0;
		}
	};

	class CXtlException : public CAtlException
	{
	public:
		CXtlException(HRESULT hr, LPCWSTR source)
		{
			m_hr = hr;
			m_source = source;
		}

		CXtlException(HRESULT hr, LPCWSTR source, const CString& message)
		{
			m_hr = hr;
			m_source = source;
			m_message = message;
		}

		LPCTSTR Message()
		{
			if (m_message.IsEmpty())
			{
				Error::GetErrorDescription(m_message, m_hr);
			}

			return m_message;
		}

		void ReportError(REFCLSID clsid = CLSID_NULL)
		{
			Error::_ReportError(clsid, Message());
		}

		operator HRESULT() const throw()
		{
			return (m_hr);
		}

		CString m_source;
		CString m_message;
	};

	ATL_NOINLINE __declspec(noreturn) inline void XtlThrow(HRESULT hr, LPCWSTR source)
	{
		throw CXtlException(hr, source);
	}

	ATL_NOINLINE __declspec(noreturn) inline void __cdecl XtlThrow(HRESULT hr, LPCWSTR source, const CString& message)
	{
		throw CXtlException(hr, source, message);
	}

	ATL_NOINLINE __declspec(noreturn) inline void __cdecl XtlThrow(HRESULT hr, LPCWSTR source, LPCSTR format, ...)
	{
		va_list argList;
		va_start(argList, format);

		CStringA str1;
		str1.FormatV(format, argList);
		va_end(argList);

		CString str(str1);

		if (hr != E_FAIL)
		{
			CString str2;
			Error::GetErrorDescription(str2, hr);
			str += L"\r\n" + str2;
		}

		throw CXtlException(hr, source, str);
	}

	ATL_NOINLINE __declspec(noreturn) inline void __cdecl XtlThrow(HRESULT hr, LPCWSTR source, LPCWSTR format, ...)
	{
		va_list argList;
		va_start(argList, format);

		CString str;
		str.FormatV(format, argList);
		va_end(argList);

		if (hr != E_FAIL)
		{
			CString str2;
			Error::GetErrorDescription(str2, hr);
			str += L"\r\n" + str2;
		}

		throw CXtlException(hr, source, str);
	}

	ATL_NOINLINE __declspec(noreturn) inline void __cdecl XtlThrow(HRESULT hr, LPCWSTR source, UINT resId, ...)
	{
		va_list argList;
		va_start(argList, resId);

		CString format;
		format.LoadString(resId);

		CString text;
		text.FormatV(format, argList);
		va_end(argList);

		if (hr != E_FAIL)
		{
			CString str2;
			Error::GetErrorDescription(str2, hr);
			text += L"\r\n" + str2;
		}

		throw CXtlException(hr, source, text);
	}

	inline CString XtlSource(LPCSTR file, int line)
	{
		USES_CONVERSION;

		CString source;
		source.Format(L"%ws(%d)", A2W(file), line);

		return source;
	}

	ATL_NOINLINE __declspec(noreturn) inline void XtlThrowFileLine(HRESULT hr, LPCSTR file, int line)
	{
		USES_CONVERSION;

		CStringA source;
		source.Format("%s(%d)", file, line);

		throw CXtlException(hr, A2W(source));
	}

	ATL_NOINLINE __declspec(noreturn) inline void XtlThrowWin32(DWORD dwError, LPCWSTR source = nullptr)
	{
		HRESULT error = HRESULT_FROM_WIN32(dwError);
		source ? XtlThrow(error, source) : XtlThrowFileLine(error, __FILE__, __LINE__);
	}

	ATL_NOINLINE __declspec(noreturn) inline void XtlThrowLastError(LPCWSTR source = nullptr)
	{
		HRESULT error = HRESULT_FROM_WIN32(::GetLastError());
		source ? XtlThrow(error, source) : XtlThrowFileLine(error, __FILE__, __LINE__);
	}

#define XTL_THROW(hr) XtlThrowFileLine(hr, __FILE__, __LINE__)
};