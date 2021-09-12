#pragma once

#include "LogBase.h"
#include "FileGroups.h"

class LOG_BASE_API LogCleaning
{
public:
	LogCleaning();
	~LogCleaning();

	HRESULT Submit(LPCWSTR location, LPCWSTR filterString, LONGLONG expireSpan);

	HRESULT Start();
	HRESULT Stop();

protected:
	void ThreadProc();

	static DWORD WINAPI _ThreadProc(LPVOID pv);

	void OnTimer();

	HRESULT Clean(LPCWSTR location, LPCWSTR filterString, LONGLONG expireSpan);
	HRESULT RemoveFile(LPCWSTR fileName, WIN32_FIND_DATA& fd, LONGLONG expireSpan);
	BOOL Filter(LPCWSTR fileName, LPCWSTR filterString);

protected:
	HANDLE m_hStopEvent;
	HANDLE m_hThread;

	FileGroups m_fileGroups;

	CComAutoCriticalSection m_cs;
};