#include "stdafx.h"
#include "ThreadPool2.h"

#include "RpcExt.h"

RpcThreadPool2::RpcThreadPool2()
{
	m_init = false;

	m_hStopEvent = nullptr;
	m_hCompletion = nullptr;
}

RpcThreadPool2::~RpcThreadPool2()
{
	Uninitialize();
}

HRESULT RpcThreadPool2::Initialize()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool2::Initialize");

	HRESULT hr = m_cs.Init();
	if (FAILED(hr))
		return hr;

	m_hStopEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (!m_hStopEvent)
	{
		hr = AtlHresultFromLastError();
		goto onfail;
	}

	m_hCompletion = CreateIoCompletionPort(nullptr, nullptr, 0, 0);
	if (!m_hStopEvent)
	{
		hr = AtlHresultFromLastError();
		goto onfail;
	}

	m_init = true;

	return S_OK;

onfail:

	if (m_hStopEvent)
	{
		CloseHandle(m_hStopEvent);
		m_hStopEvent = nullptr;
	}

	return hr;
}

void RpcThreadPool2::Uninitialize()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::Uninitialize");

	if (!m_init)
		return;

	m_init = false;

	if (m_hStopEvent)
	{
		SetEvent(m_hStopEvent);
	}

	m_cs.Lock();

	if (m_hCompletion)
	{
		CloseHandle(m_hCompletion);
		m_hCompletion = nullptr;
	}

	if (m_hStopEvent)
	{
		CloseHandle(m_hStopEvent);
		m_hStopEvent = nullptr;
	}

	m_cs.Unlock();

	m_cs.Term();
}

HRESULT RpcThreadPool2::InitThreads()
{
	int nNumThreads = 0;

	if (nNumThreads == 0)
		nNumThreads = -2;

	if (nNumThreads < 0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);

		nNumThreads = (int)(-nNumThreads) * si.dwNumberOfProcessors;
	}

	return 0;
}

void RpcThreadPool2::TermThreads()
{
}

void RpcThreadPool2::WorkProc()
{
	HANDLE h = GetCurrentThread();

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	_rpcExt.m_threadStg.InitThreadInfo(APTTYPE_MTA);

	HANDLE hEvents[] = { 0, m_hStopEvent };

	for (;;)
	{
		DWORD dwWait = WaitForMultipleObjects(2, hEvents, FALSE, 0);
		if (dwWait == WAIT_OBJECT_0)
		{
		}
		else if (dwWait == WAIT_OBJECT_0 + 1)
		{
			break;
		}
		else if (dwWait == WAIT_TIMEOUT)
		{
			break;
		}
		else
		{
			_logFile.WriteV(LogCategory::System, LogLevel::Error, LogOption::Std, "RpcThreadPool::WorkProc, dwWait : %d", dwWait);
		}
	}

	_rpcExt.m_threadStg.UninitThreadInfo();

	CoUninitialize();
}

DWORD WINAPI RpcThreadPool2::_WorkProc(LPVOID pv)
{
	try
	{
		auto pThis = (RpcThreadPool*)pv;

		pThis->WorkProc();
	}
	catch (CXtlException e)
	{
		_logFile.WriteV(LogCategory::Test, LogLevel::Error, LogOption::Std, e.m_hr, L"RpcThreadPool2::_WorkProc - catch(CXtlException), source: %ws, message : %ws", e.m_source, e.m_message);
	}
	catch (CAtlException e)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, e.m_hr, L"RpcThreadPool2::_WorkProc, catch(CAtlException)");
	}
	catch (...)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, GetLastError(), L"RpcThreadPool2::_WorkProc, catch(...)");
	}

	return 0;
}
