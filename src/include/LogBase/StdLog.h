#pragma once

#include "LogFile.h"

class LOG_BASE_API StdLog
{
public:
	StdLog(LPCWSTR baseName = nullptr, HMODULE hModule = nullptr, LOG_FILE_FLAGS flags = LOG_AUTO_CLEAN, LONGLONG expireSpan = 0, DWORD outCategories = 0, int outLevel = 0);
	~StdLog();

	void Write(LogCategory::Type category, LogLevel::Type level, LogOptions options, LPCWSTR source, LPCWSTR text = nullptr);

	void WriteH(LogCategory::Type category, LogLevel::Type level, LogOptions options, HRESULT hr, LPCWSTR source);

	void __cdecl WriteV(LogCategory::Type category, LogLevel::Type level, LogOptions options, LPCWSTR source, LPCWSTR format, ...);

	void __cdecl WriteHV(LogCategory::Type category, LogLevel::Type level, LogOptions options, HRESULT hr, LPCWSTR source, LPCWSTR format, ...);

	void __cdecl WriteV(LogCategory::Type category, LogLevel::Type level, LogOptions options, LPCWSTR source, UINT formatId, ...);

	void __cdecl WriteHV(LogCategory::Type category, LogLevel::Type level, LogOptions options, HRESULT hr, LPCWSTR source, UINT formatId, ...);

	void __cdecl WriteV(LogCategory::Type category, LogLevel::Type level, LogOptions options, LPCWSTR source, HINSTANCE hInst, UINT formatId, ...);

	void __cdecl WriteHV(LogCategory::Type category, LogLevel::Type level, LogOptions options, HRESULT hr, LPCWSTR source, HINSTANCE hInst, UINT formatId, ...);

	void _Write(LogLevel::Type level, LPCWSTR source, LPCWSTR text, LogOptions options);

public:
	LogFile m_logFile;
};
