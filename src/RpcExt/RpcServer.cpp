#include "StdAfx.h"
#include "RpcServer.h"

#include "RpcExt.h"

RpcServer::RpcServer()
{
	m_state = RE_SS_STOPED;
	m_port = 0;

	m_maxThreads = 30;
	m_maxConnections = 1000;

	m_hIOCP = nullptr;
	m_hStopEvent = nullptr;
}

RpcServer::~RpcServer()
{
}

HRESULT RpcServer::Initialize()
{
	HRESULT hr = m_cs.Init();
	if (FAILED(hr))
		return hr;

	//  103, 131, 163, 211, 257, 331, 409, 521, 647, 821
	// 1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209

	hr = m_acceptList.Initialize(211);
	if (FAILED(hr))
		return hr;

	hr = m_connList.Initialize(3251);

	return hr;
}

void RpcServer::Uninitialize()
{
	m_connList.Term();
	m_acceptList.Term();

	m_cs.Term();
}

ULONG RpcServer::GetMaxThreads()
{
	m_cs.Lock();

	ULONG maxThreads = m_maxThreads;

	m_cs.Unlock();

	return maxThreads;
}

HRESULT RpcServer::SetMaxThreads(ULONG maxThreads)
{
	return 0;
}

ULONG RpcServer::GetMaxConnections()
{
	m_cs.Lock();

	ULONG maxConnections = m_maxConnections;

	m_cs.Unlock();

	return maxConnections;
}

HRESULT RpcServer::SetMaxConnections(ULONG maxConnections)
{
	return 0;
}

HRESULT RpcServer::Startup(LPCWSTR appName)
{
	if (!appName || *appName == '\0')
		return E_INVALIDARG;

	WCHAR keyName[MAX_PATH] = L"";
	RE_GetRegKeyName(appName, keyName, MAX_PATH);

	RegKey key;
	LONG res = key.Open(HKEY_LOCAL_MACHINE, keyName, KEY_READ);
	if (res != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(res);

	DWORD port = 0;
	res = key.QueryDWORDValue(L"Port", port);
	if (res != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(res);

	res = key.QueryDWORDValue(L"ThreadLimit", m_maxThreads);
	res = key.QueryDWORDValue(L"ConnectionLimit", m_maxConnections);

	return Startup(port);
}

HRESULT RpcServer::Startup(LONG port)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::Start - port : %d, maxTherads : %d, maxConnections : %d", port, m_maxThreads, m_maxConnections);

	CComCritSecLock<CComCriticalSection> lock(m_cs);

	if (m_state == RE_SS_RUNNING)
		return S_FALSE;

	m_port = port;

	m_hStopEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (!m_hStopEvent)
		return AtlHresultFromLastError();

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!m_hIOCP)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, GetLastError(), L"RpcServer::Startup - CreateIoCompletionPort failed");

		return AtlHresultFromLastError();
	}

	if (!m_socket.Create(m_port, false) || !m_socket.Listen(m_maxConnections))
		return AtlHresultFromLastError();

	if (!CreateIoCompletionPort((HANDLE)(SOCKET)m_socket, m_hIOCP, (ULONG_PTR)this, 0))
	{
		return AtlHresultFromLastError();
	}

	HANDLE hThread = CreateThread(nullptr, 0, _IdleProc, this, 0, nullptr);
	if (!hThread)
		return AtlHresultFromLastError();

	m_threads.Add(hThread);

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	int threadCount = si.dwNumberOfProcessors * 2;
	for (int i = 0; i < threadCount; ++i)
	{
		HANDLE hThread = CreateThread(nullptr, 0, _WorkProc, this, 0, nullptr);
		if (!hThread)
			return AtlHresultFromLastError();

		m_threads.Add(hThread);
	}

	for (int i = 0; i < threadCount; ++i)
	{
		HRESULT hr = NewConnection();
		if (FAILED(hr))
			return hr;
	}

	m_state = RE_SS_RUNNING;

	return S_OK;
}

HRESULT RpcServer::Stop()
{
	AtlTrace("RpcServer::Stop\n");

	CComCritSecLock<CComCriticalSection> lock(m_cs);

	if (m_state == RE_SS_STOPED)
		return S_FALSE;

	if (m_hStopEvent)
	{
		SetEvent(m_hStopEvent);

		m_threads.PostQueuedCompletionStatus(m_hIOCP, 0, NULL, nullptr);

		m_threads.WaitAll(60000);
		m_threads.Close();
		m_threads.RemoveAll();

		CloseHandle(m_hStopEvent);
		m_hStopEvent = nullptr;
	}

	if (m_hIOCP)
	{
		CloseHandle(m_hIOCP);
		m_hIOCP = nullptr;
	}

	m_socket.Close();
	m_connList.CloseAll();
	m_acceptList.CloseAll();

	m_state = RE_SS_STOPED;

	return S_OK;
}

DWORD RpcServer::WorkProc()
{
	DWORD transfered = 0;
	ULONG_PTR pKey = NULL;
	OVERLAPPED* pOverlapped = nullptr;

	while (WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
	{
		transfered = 0;
		pKey = NULL;
		pOverlapped = nullptr;

		_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::WorkProc - GetQueuedCompletionStatus");

		SetLastError(0);

		BOOL status = GetQueuedCompletionStatus(m_hIOCP, &transfered, (PULONG_PTR)&pKey, &pOverlapped, INFINITE);
		DWORD error = GetLastError();
		if (!status)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::WorkProc - GetQueuedCompletionStatus failed, #error : %d, pKey : #%08x", error, pKey);

			if (error == ERROR_NETNAME_DELETED)
			{
				if (pKey == (ULONG_PTR)this)
				{
					auto pConn = (RpcServerConnection*)((ULONG_PTR)pOverlapped - offsetof(RpcServerConnection, m_recvBox.overlapped));

					pConn->Reset();

					_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::WorkProc - ERROR_NETNAME_DELETED, pConn->Reset(), acceptList.Count : %d", m_acceptList.GetCount());
				}
				else
				{
					m_connList.Remove(pKey);
					_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::WorkProc - ERROR_NETNAME_DELETED, m_connList.Remove(#%08x)", pKey);
				}
			}

			continue;
		}

		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::WorkProc - GetQueuedCompletionStatus, pKey = #%x, pOverlapped = #%x", pKey, pOverlapped);

		if (pKey == (ULONG_PTR)this)
		{
			_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::WorkProc - GetQueuedCompletionStatus - Accept, connList.Count : %d", m_connList.GetCount());

			auto pConn = (RpcServerConnection*)((ULONG_PTR)pOverlapped - offsetof(RpcServerConnection, m_recvBox.overlapped));

			if (m_connList.GetCount() < m_maxConnections)
			{
				m_connList.Add(pConn);
				pConn->Accept(transfered);
				m_acceptList.Remove(pConn);

				NewConnection();
			}
			else
			{
				pConn->Reset();
			}
		}
		else if (pKey)
		{
			CComPtr<RpcServerConnection> spConn;

			if (m_connList.GetConnection(pKey, false, (RpcConnection**)&spConn) == S_OK)
			{
				if (pOverlapped == &spConn->m_recvBox.overlapped)
				{
					spConn->OnReceived(error, transfered);
				}
				else if (pOverlapped == &spConn->m_sendBox.overlapped)
				{
					spConn->OnSended(error, transfered);
				}
				else if (pOverlapped == SEND_OVERLAPPED)
				{
					spConn->SendNext();
				}
				else if (pOverlapped == CLOSE_OVERLAPPED)
				{
					m_connList.Remove(spConn);
				}
				else
				{
					_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::WorkProc - Overlapped is unknown");
				}
			}
			else
			{
				_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::WorkProc - m_connList.GetConnection failed");
			}
		}
	}

	m_state = RE_SS_STOPED;

	return 0;
}

DWORD WINAPI RpcServer::_WorkProc(LPVOID pv)
{
	try
	{
		auto pThis = (RpcServer*)pv;

		return pThis->WorkProc();
	}
	catch (CXtlException e)
	{
		_logFile.WriteV(LogCategory::Test, LogLevel::Error, LogOption::Std, e.m_hr, L"RpcServer::_WorkProc - catch(CXtlException), source: %ws, message : %ws", e.m_source, e.m_message);
	}
	catch (CAtlException e)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, e.m_hr, L"RpcServer::_WorkProc, catch(CAtlException)");
	}
	catch (...)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, GetLastError(), L"RpcServer::_WorkProc, catch(...)");
	}

	return 0;
}

DWORD RpcServer::IdleProc()
{
	DWORD timeout = 5000;

	for (;;)
	{
		DWORD wait = ::WaitForSingleObject(m_hStopEvent, timeout);
		if (wait == WAIT_TIMEOUT)
		{
			_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, L"RpcServer::IdleProc - WAIT_TIMEOUT");

			m_connList.Lock();

			ConnectionList::_Node* pNode = m_connList.GetHead();
			while (pNode)
			{
				if (!pNode->pConn->OnIdle())
				{
					ConnectionList::_Node* pTemp = pNode;
					pNode = pNode->pNext;

					m_connList.Remove(pTemp);
				}
				else
				{
					pNode = pNode->pNext;
				}
			}

			m_connList.Unlock();
		}
		else if (wait == WAIT_OBJECT_0)
		{
			_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, L"RpcServer::IdleProc - WAIT_OBJECT_0");

			break;
		}
	}

	return 0;
}

DWORD WINAPI RpcServer::_IdleProc(LPVOID pv)
{
	try
	{
		auto pThis = (RpcServer*)pv;

		return pThis->IdleProc();
	}
	catch (CXtlException e)
	{
		_logFile.WriteV(LogCategory::Test, LogLevel::General, LogOption::Std, e.m_hr, L"RpcServer::_IdleProc - catch(CXtlException), source: %ws, message : %ws", e.m_source, e.m_message);
	}
	catch (CAtlException e)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, e.m_hr, L"RpcServer::_IdleProc, catch(CAtlException)");
	}
	catch (...)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, GetLastError(), L"RpcServer::_IdleProc, catch(...)");
	}

	return 0;
}

HRESULT RpcServer::NewConnection()
{
	static LONG _count = 0;

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::NewConnection(%d), ConnectionCount : %d", InterlockedIncrement(&_count), m_connList.GetCount());

	/*if(m_connList.GetCount() >= m_maxConnections)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServer::NewConnection, ConnectionCount > MaxConnections");

		return S_FALSE;
	}*/

	CComPtr<RpcServerConnection> spConn;
	HRESULT hr = RpcServerConnection::_CreateInstance(&spConn);
	if (FAILED(hr))
		return hr;

	hr = m_acceptList.Add(spConn);
	if (FAILED(hr))
		return hr;

	hr = spConn->Create(this);
	if (FAILED(hr))
	{
		m_acceptList.Remove(spConn);
	}

	return hr;
}

HRESULT RpcServer::GetConnection(RPC_SID sid, RpcConnection** ppConnection)
{
	return m_connList.GetConnection(sid, ppConnection);
}

void RpcServer::OnClose(RpcServerConnection* pConnection)
{
	m_connList.Remove(pConnection);
}