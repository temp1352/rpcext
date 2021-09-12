#pragma once

#include "LogBase.h"
#include "Cleaning.h"

#ifndef __ATLSTR_H__
#include <atlstr.h>
#endif

enum LOG_FILE_FLAG
{
	LOG_STD_OUT = 0x10,
	LOG_AUTO_CLEAN = 0x0100,
	LOG_TERM = 0x1000,
};

typedef DWORD LOG_FILE_FLAGS;

class LOG_BASE_API LogFile
{
public:
	LogFile(LPCWSTR baseName = nullptr, HMODULE hModule = nullptr, LOG_FILE_FLAGS flags = LOG_AUTO_CLEAN, LONGLONG expireSpan = 0, DWORD outCategories = 0, int outLevel = 0);
	~LogFile();

	void Init();
	void Term();

	void LoadConfig(LPCWSTR name);

	void Write(LogCategory::Type category, LogLevel::Type level, LogOptions options, HRESULT hr);
	void Write(LogCategory::Type category, LogLevel::Type level, LogOptions options, LPCSTR text, int len = -1);
	void Write(LogCategory::Type category, LogLevel::Type level, LogOptions options, LPCWSTR text, int len = -1);

	void __cdecl WriteV(LogCategory::Type category, LogLevel::Type level, LogOptions options, LPCSTR format, ...);
	void __cdecl WriteV(LogCategory::Type category, LogLevel::Type level, LogOptions options, LPCWSTR format, ...);

	void __cdecl WriteV(LogCategory::Type category, LogLevel::Type level, LogOptions options, HRESULT hr, LPCSTR format, ...);
	void __cdecl WriteV(LogCategory::Type category, LogLevel::Type level, LogOptions options, HRESULT hr, LPCWSTR format, ...);

	void __cdecl WriteV(LogCategory::Type category, LogLevel::Type level, LogOptions options, UINT formatId, ...);

	void WriteBinary(LogCategory::Type category, LogLevel::Type level, LogOptions options, const void* pData, int size);
	void WriteBinaryV(LogCategory::Type category, LogLevel::Type level, LogOptions options, const void* pData, int size, LPCSTR format, ...);

	void Write(LogLevel::Type level, LPCWSTR text, int len, LogOptions options);
	void Write(LogLevel::Type level, LPCSTR text, int len, LogOptions options);
	void _Write(const void* pv, int len);

	HANDLE GetFile(LPSYSTEMTIME lpst);
	HANDLE CreateFile(LPSYSTEMTIME lpst);
	virtual HRESULT GetFileName(LPSYSTEMTIME lpst, WCHAR path[MAX_PATH]);

	DWORD WriteFile(HANDLE hFile, LPCVOID pv, int len);

	static HRESULT GetErrorDescription(HRESULT hRes, DWORD langId, BOOL clearErrorInfo, CStringA& errDesc) throw(...);
	static HRESULT GetErrorDescription(HRESULT hRes, DWORD langId, BOOL clearErrorInfo, CStringW& errDesc) throw(...);
	static HRESULT GetErrorDescription(HRESULT hRes, DWORD langId, BOOL clearErrorInfo, BSTR* pError);

public:
	int m_outLevel;
	DWORD m_outCategories = 0;

	LOG_FILE_FLAGS m_flags;

	LONGLONG m_expireSpan;

	WCHAR m_location[MAX_PATH];
	WCHAR m_baseName[MAX_PATH];

protected:
	HANDLE m_hFile;

	int m_day;
	ULONG m_size;
	ULONG m_maxSize;

	CComAutoCriticalSection m_cs;

public:

	static LogCleaning _logCleaning;
};

#ifndef LOG_BASE_EXPORTS

#pragma comment(lib, "LogBase.lib")

#endif