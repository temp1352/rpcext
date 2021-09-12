#include "StdAfx.h"
#include "ThreadPool.h"

#include "RpcExt.h"

RpcTaskList::RpcTaskList()
{
	m_pHead = nullptr;
	m_pTail = nullptr;

	m_count = 0;
}

RpcTaskList::~RpcTaskList()
{
	RemoveAll();
}

HRESULT RpcTaskList::AddTail(IRpcHandler* pHandler)
{
	ATLASSERT(pHandler);

	HRESULT hr = S_OK;

	_Node* pNode = new _Node;
	if (!pNode)
		return E_OUTOFMEMORY;

	pHandler->AddRef();
	pNode->pHandler = pHandler;
	pNode->pNext = nullptr;

	m_cs.Lock();

	if (m_pTail)
		m_pTail->pNext = pNode;
	else
		m_pHead = pNode;

	m_pTail = pNode;

	++m_count;

	m_cs.Unlock();

	return hr;
}

HRESULT RpcTaskList::RemoveHead(IRpcHandler** ppHandler)
{
	if (!ppHandler)
		return E_POINTER;

	HRESULT hr = S_FALSE;

	m_cs.Lock();

	_Node* pNode = m_pHead;
	if (pNode)
	{
		*ppHandler = pNode->pHandler;

		m_pHead = pNode->pNext;
		if (m_pTail == pNode)
		{
			m_pTail = nullptr;
		}

		delete pNode;

		--m_count;

		hr = S_OK;
	}

	m_cs.Unlock();

	return hr;
}

void RpcTaskList::RemoveAll()
{
	m_cs.Lock();

	_Node* pNode = m_pHead;
	while (pNode)
	{
		_Node* pTemp = pNode;
		pNode = pNode->pNext;

		pTemp->pHandler->Release();
		delete pTemp;
	}

	m_pHead = nullptr;
	m_pTail = nullptr;

	m_count = 0;

	m_cs.Unlock();
}

ULONG RpcTaskList::GetCount()
{
	m_cs.Lock();

	ULONG count = m_count;

	m_cs.Unlock();

	return count;
}

// RpcThreadPool

RpcThreadPool::RpcThreadPool()
{
	m_init = false;

	m_timeout = _rpcExt.m_settings.threadPool.threadTimeout;
	m_maxThreadCount = _rpcExt.m_settings.threadPool.maxThreadCount;
	m_maxTaskCount = _rpcExt.m_settings.threadPool.maxThreadCount;

	m_threadCount = 0;
	m_activeThreadCount = 0;

	m_hStopEvent = nullptr;
	m_hExeEvent = nullptr;
}

RpcThreadPool::~RpcThreadPool()
{
	Uninitialize();
}

HRESULT RpcThreadPool::Initialize()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::Initialize");

	HRESULT hr = m_cs.Init();
	if (FAILED(hr))
		return hr;

	m_hStopEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (!m_hStopEvent)
	{
		hr = AtlHresultFromLastError();
		goto onfail;
	}

	m_hExeEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_hExeEvent)
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

	if (m_hExeEvent)
	{
		CloseHandle(m_hExeEvent);
		m_hExeEvent = nullptr;
	}

	return hr;
}

void RpcThreadPool::Uninitialize()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::Uninitialize");

	if (!m_init)
		return;

	m_init = false;

	if (m_hStopEvent)
	{
		SetEvent(m_hStopEvent);
	}

	m_threads.WaitAll(60000);
	m_threads.Close();
	m_threads.RemoveAll();

	m_cs.Lock();

	if (m_hExeEvent)
	{
		CloseHandle(m_hExeEvent);
		m_hExeEvent = nullptr;
	}

	if (m_hStopEvent)
	{
		CloseHandle(m_hStopEvent);
		m_hStopEvent = nullptr;
	}

	m_cs.Unlock();

	m_cs.Term();
}

void RpcThreadPool::WorkProc()
{
	HANDLE h = GetCurrentThread();

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	_rpcExt.m_threadStg.InitThreadInfo(APTTYPE_MTA);

	InterlockedIncrement(&m_threadCount);

	HANDLE hEvents[] = { m_hExeEvent, m_hStopEvent };

	for (;;)
	{
		DWORD wait = WaitForMultipleObjects(2, hEvents, FALSE, m_timeout);
		if (wait == WAIT_OBJECT_0)
		{
			ExecuteProc();
		}
		else if (wait == WAIT_OBJECT_0 + 1)
		{
			break;
		}
		else if (wait == WAIT_TIMEOUT)
		{
			break;
		}
		else
		{
			_logFile.WriteV(LogCategory::System, LogLevel::Error, LogOption::Std, "RpcThreadPool::WorkProc, wait : %d", wait);
		}
	}

	InterlockedDecrement(&m_threadCount);

	_rpcExt.m_threadStg.UninitThreadInfo();

	CoUninitialize();

	_ExitThread();
}

DWORD WINAPI RpcThreadPool::_WorkProc(LPVOID pv)
{
	try
	{
		auto pThis = (RpcThreadPool*)pv;

		pThis->WorkProc();
	}
	catch (CXtlException e)
	{
		_logFile.WriteV(LogCategory::Test, LogLevel::Error, LogOption::Std, e.m_hr, L"RpcThreadPool::_WorkProc - catch(CXtlException), source: %ws, message : %ws", e.m_source, e.m_message);
	}
	catch (CAtlException e)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, e.m_hr, L"RpcThreadPool::_WorkProc, catch(CAtlException)");
	}
	catch (...)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, GetLastError(), L"RpcThreadPool::_WorkProc, catch(...)");
	}

	return 0;
}

void RpcThreadPool::ExecuteProc()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::ExecuteProc, thread_id : %d", GetCurrentThreadId());

	InterlockedIncrement(&m_activeThreadCount);

	_ExecuteContext context;
	context.hStopeEvent = m_hStopEvent;

	static LONG _count = 0;

	for (;;)
	{
		CComPtr<IRpcHandler> spHandler;
		HRESULT hr = m_taskList.RemoveHead(&spHandler);
		if (hr != S_OK)
			break;

		LONG n = InterlockedIncrement(&_count);
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::ExecuteProc(%d), Execute, thread_id : %d", n, GetCurrentThreadId());

		hr = spHandler->Execute(&context);

		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::ExecuteProc(%d) end", n);
	}

	InterlockedDecrement(&m_activeThreadCount);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::ExecuteProc end, thread_id : %d", GetCurrentThreadId());
}

HRESULT RpcThreadPool::Execute(IRpcHandler* pHandler)
{
	ATLASSERT(pHandler);

	if (!m_init)
		return E_UNEXPECTED;

	static LONG _count = 0;

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::Execute(%d), task_count : %d, thread_count : %d, active_thread_count : %d",
		InterlockedIncrement(&_count), m_taskList.GetCount(), m_threadCount, m_activeThreadCount);

	if (m_taskList.GetCount() >= m_maxTaskCount)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcTheradPool::Execute, task_count : %d, out of range", m_taskList.GetCount());

		return AtlHresultFromWin32(RPC_S_SERVER_TOO_BUSY);
	}

	HRESULT hr = m_taskList.AddTail(pHandler);
	if (FAILED(hr))
		return hr;

	SetEvent(m_hExeEvent);

	if (m_activeThreadCount >= m_threadCount - 1 && m_threadCount < m_maxThreadCount)
	{
		static LONG _threadCount = 0;
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::Execute, CreateThread(%d)", InterlockedIncrement(&_threadCount));

		DWORD threadId = 0;
		HANDLE hThread = CreateThread(nullptr, 0, _WorkProc, this, 0, &threadId);
		if (!hThread)
		{
			return AtlHresultFromLastError();
		}

		hr = m_threads.Add(hThread, threadId);
		if (hr != S_OK)
			return hr;

		_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcTheadPool::Execute - CreateThread end");
	}

	return S_OK;
}

void RpcThreadPool::_ExitThread()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::_ExitThread");

	DWORD threadId = GetCurrentThreadId();
	HANDLE hThread = m_threads.Remove(threadId);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadPool::_ExitThread end, ThreadCount : %d, ActiveThreadCount : %d, hThread : %x", m_threadCount, m_activeThreadCount, hThread);
}