#include "StdAfx.h"
#include "ClientConnection.h"

#include "RpcExt.h"

RpcClientConnection::RpcClientConnection()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::RpcClientConnection, this : #%08x", this);

	m_flags |= RPC_CF_COMPLETION_ROUTINE;

	m_hThread = nullptr;
	m_hConnEvent = nullptr;
	m_hConnCompEvent = nullptr;
	m_hSendEvent = nullptr;

	m_pHeadSink = nullptr;

	m_clsid = CLSID_NULL;
	m_iid = IID_NULL;

	m_oid = 0;

	m_options = 0;
	m_timeout = _rpcExt.m_settings.connection.connectTimeout;
	m_expireSpan = _rpcExt.m_settings.connection.expireSpan;
	m_port = 0;
	m_connectCount = 0;

	m_hConnectRes = HRESULT_FROM_WIN32(WSAENOTCONN);

	m_requesting = 0;
	m_expireTime = 0;

	m_hash = 0;
	m_pNext = nullptr;
}

RpcClientConnection::~RpcClientConnection()
{
	Close();
}

HRESULT RpcClientConnection::_CreateInstance(IRpcConnectionSink* pSink, RpcClientConnection** ppConnection)
{
	CComObject<RpcClientConnection>* pConnection = nullptr;
	HRESULT hr = CComObject<RpcClientConnection>::CreateInstance(&pConnection);
	if (FAILED(hr))
		return hr;

	pConnection->AddRef();

	if (pSink)
	{
		pConnection->Advise(pSink, FALSE);
	}

	*ppConnection = pConnection;

	return hr;
}

#ifdef _DEBUG

ULONG RpcClientConnection::InternalAddRef()
{
	ULONG ref = __super::InternalAddRef();

	return ref;
}

ULONG RpcClientConnection::InternalRelease()
{
	ULONG ref = __super::InternalRelease();

	return ref;
}

#endif

int _GetPort(int port, LPCWSTR appName)
{
	if (port > 0 || !appName)
		return port;

	WCHAR keyName[MAX_PATH] = L"";
	RE_GetRegKeyName(appName, keyName, MAX_PATH);

	RegKey key;
	LONG res = key.Open(HKEY_LOCAL_MACHINE, keyName, KEY_READ);
	if (res == ERROR_SUCCESS)
	{
		res = key.QueryDWORDValue(L"Port", (DWORD&)port);
	}

	return port;
}

HRESULT RpcClientConnection::Create(REFCLSID rclsid, REFIID riid, const RE_CONNECTINFO* pConnInfo)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Create(rclsid, riid, pConnInfo)");

	m_clsid = rclsid;
	m_iid = riid;

	m_options = pConnInfo->options;
	m_timeout = pConnInfo->timeout ? pConnInfo->timeout : _rpcExt.m_settings.connection.connectTimeout;
	m_expireSpan = pConnInfo->expireSpan ? pConnInfo->expireSpan : _rpcExt.m_settings.connection.expireSpan;

	m_port = _GetPort(pConnInfo->port, pConnInfo->appName);

	m_serverName = pConnInfo->serverName;
	m_domain = pConnInfo->domain;
	m_referer = pConnInfo->referer;
	m_params = pConnInfo->params;

	m_sessionId = pConnInfo->sessionId;
	m_authCode = pConnInfo->auth_code;
	m_userName = pConnInfo->userName;
	m_password = pConnInfo->password;

	HRESULT hr = Initialize();
	if (FAILED(hr))
		return hr;

	if (m_options & RE_CF_ASYNC)
		return E_PENDING;

	DWORD wait = WaitForSingleObject(m_hConnCompEvent, m_timeout);
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Create(rclsid, riid, pConnInfo), wait : %d", wait);
	if (wait == WAIT_OBJECT_0)
	{
		hr = m_hConnectRes;
		m_errorInfo.Report();
	}
	else if (wait == WAIT_TIMEOUT)
	{
		hr = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
	}
	else
	{
		hr = E_FAIL;
	}

	if (FAILED(hr))
	{
		Close();
	}

	return hr;
}

HRESULT RpcClientConnection::Create(OID oid, REFIID riid, const RE_CONNECTINFO* pConnInfo)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Create(oid, riid, pConnInfo)");

	m_oid = oid;
	m_iid = riid;

	m_options = pConnInfo->options;
	m_timeout = pConnInfo->timeout ? pConnInfo->timeout : _rpcExt.m_settings.connection.connectTimeout;
	m_expireSpan = pConnInfo->expireSpan ? pConnInfo->expireSpan : _rpcExt.m_settings.connection.expireSpan;

	m_port = _GetPort(pConnInfo->port, pConnInfo->appName);

	m_serverName = pConnInfo->serverName;
	m_domain = pConnInfo->domain;
	m_referer = pConnInfo->referer;
	m_params = pConnInfo->params;

	m_sessionId = pConnInfo->sessionId;
	m_authCode = pConnInfo->auth_code;
	m_userName = pConnInfo->userName;
	m_password = pConnInfo->password;

	HRESULT hr = Initialize();
	if (FAILED(hr))
		return hr;

	if (m_options & RE_CF_ASYNC)
		return E_PENDING;

	DWORD wait = WaitForSingleObject(m_hConnCompEvent, m_timeout);
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Create(oid, riid, pConnInfo), wait : %d", wait);
	if (wait == WAIT_OBJECT_0)
	{
		hr = m_hConnectRes;
		m_errorInfo.Report();
	}
	else if (wait == WAIT_TIMEOUT)
	{
		hr = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
	}
	else
	{
		hr = E_FAIL;
	}

	if (FAILED(hr))
	{
		Close();
	}

	return hr;
}

static DWORD WINAPI _ThreadProc(LPVOID pv)
{
	try
	{
		auto pThis = (RpcClientConnection*)pv;

		pThis->ThreadProc();
	}
	catch (CXtlException e)
	{
		_logFile.WriteV(LogCategory::Test, LogLevel::Error, LogOption::Std, e.m_hr, L"RpcClientConnection::_ThreadProc - catch(CXtlException), source: %ws, message : %ws", e.m_source, e.m_message);
	}
	catch (CAtlException e)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, e.m_hr, L"RpcClientConnection::_ThreadProc, catch(CAtlException)");
	}
	catch (...)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, GetLastError(), L"RpcClientConnection::_ThreadProc, catch(...)");
	}

	return 0;
}

HRESULT RpcClientConnection::Initialize()
{
	HRESULT hr = __super::Initialize();
	if (FAILED(hr))
		return hr;

	hr = CoCreateGuid(&m_cid);

	hr = _rpcExt.m_connectionMap.Insert(this);

	m_hConnEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_hConnEvent)
		return AtlHresultFromLastError();

	m_hConnCompEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (!m_hConnCompEvent)
		return AtlHresultFromLastError();

	m_hSendEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_hSendEvent)
		return AtlHresultFromLastError();

	DWORD threadId = 0;
	m_hThread = CreateThread(nullptr, 0, _ThreadProc, this, 0, &threadId);
	if (!m_hThread)
		return AtlHresultFromLastError();

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Initialize, CreateThread, id : %d", threadId);

	return S_OK;
}

void RpcClientConnection::Close()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Close");

	_rpcExt.m_connectionMap.Remove(this);

	Lock();
	
	auto pSinkNode = m_pHeadSink;
	while (pSinkNode)
	{
		auto pTemp = pSinkNode;
		pSinkNode = pSinkNode->pNext;
		if (pTemp->addRef)
		{
			pTemp->pSink->Release();
		}

		delete pTemp;
	}
	m_pHeadSink = nullptr;

	if (m_hTermEvent)
	{
		SetEvent(m_hTermEvent);
	}

	if (m_hThread)
	{
		Unlock();
		DWORD wait = WaitForSingleObject(m_hThread, 60000);
		Lock();

		if (wait != WAIT_OBJECT_0)
		{
			DWORD dwExitCode = 0;
			GetExitCodeThread(m_hThread, &dwExitCode);
			if (dwExitCode == STILL_ACTIVE)
			{
				TerminateThread(m_hThread, 0);
				_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Close, TerminateThread");
			}
		}

		CloseHandle(m_hThread);
		m_hThread = nullptr;
	}

	if (m_hConnEvent)
	{
		CloseHandle(m_hConnEvent);
		m_hConnEvent = nullptr;
	}

	if (m_hConnCompEvent)
	{
		CloseHandle(m_hConnCompEvent);
		m_hConnCompEvent = nullptr;
	}

	if (m_hSendEvent)
	{
		CloseHandle(m_hSendEvent);
		m_hSendEvent = nullptr;
	}

	Unlock();

	__super::Close();
}

void RpcClientConnection::ThreadProc()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::ThreadProc");

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	ConnectProc();

	HANDLE hEvents[] = { m_hConnEvent, m_hTermEvent };
	while (true)
	{
		MainProc();

		DWORD wait = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
		if (wait == WAIT_OBJECT_0)
		{
			ConnectProc();
		}
		else if (wait == WAIT_OBJECT_0 + 1)
		{
			_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::ThreadProc, stop, break");

			break;
		}
		else
		{
			break;
		}
	}

	CoUninitialize();

	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::ThreadProc - end");
}

void RpcClientConnection::MainProc()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::MainProc");

	PostRecv();

	DWORD timeout = _rpcExt.m_settings.connection.idle;
	HANDLE hEvents[] = { m_hSendEvent, m_hTermEvent };

	while (m_flags & RPC_CF_CONNECTED)
	{
		DWORD wait = WSAWaitForMultipleEvents(2, hEvents, FALSE, timeout, TRUE);
		if (wait == WAIT_OBJECT_0)
		{
			SendNext();
		}
		else if (wait == WAIT_OBJECT_0 + 1)
		{
			break;
		}
		else if (wait == WAIT_TIMEOUT)
		{
			if (!OnIdle())
				break;
		}
		else
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::MainProc, wait : %d", wait);
		}
	}

	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::MainProc - end");
}

void RpcClientConnection::ConnectProc()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::ConnectProc - oid : %I64u", m_oid);

	CComObjectStack<RpcBindingRequest> cmd;

	cmd.m_oid = m_oid;
	cmd.m_clsid = m_clsid;
	cmd.m_iid = m_iid;
	cmd.m_cid = m_cid;

	cmd.m_domain = m_domain;
	cmd.m_referer = m_referer;
	cmd.m_params = m_params;

	GuidUtil::FromString(m_sessionId.m_str, &cmd.m_sid);

	cmd.m_authCode = m_authCode;
	cmd.m_refreshToken = m_refreshToken;

	cmd.m_userName = m_userName;
	cmd.m_password = m_password;

	if (m_options & RE_CF_INTEGRATED_SECURITY)
	{
		cmd.m_flags |= RPC_BF_INTEGRATED_SECURITY;
	}

	for (;; ++m_connectCount)
	{
		BOOL completed = FALSE;

		HRESULT hr = Connect(&cmd, &completed);
		_logFile.WriteV(LogCategory::Comm, LogLevel::Error, LogOption::Std, "Connect (%d) : ServerName : %s, Port : %d, hr : %x", m_connectCount + 1, m_serverName ? CW2A(m_serverName) : "", m_port, hr);

		InterlockedExchange(&m_hConnectRes, hr);

		if (completed || !(m_options & RE_CF_AUTO_RECONNECT) || (m_connectCount == 0 && (m_options & RE_CF_FIRST_CONNECT_ONE)))
		{
			SetEvent(m_hConnCompEvent);

			break;
		}

		DWORD wait = WaitForSingleObject(m_hTermEvent, _rpcExt.m_settings.connection.connectInterval);
		if (wait == WAIT_OBJECT_0)
		{
			_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::ConnectProc, Wait StopEvent, break");

			break;
		}
	}

	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::ConnectProc - end");
}

static WCHAR _localhost[] = L"localhost";

HRESULT RpcClientConnection::Connect(RpcBindingRequest* pCmd, BOOL* pCompleted)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Connect");

	if (!pCmd)
		return E_INVALIDARG;

	if (m_socket.IsConnected())
	{
		m_socket.Close();
	}

	RpcBuffer buffer;
	HRESULT hr = buffer.Write(pCmd);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcClientConnection::Connect - buffer.Write");
		return hr;
	}

	hr = Connect(&buffer);
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcClientConnection::Connect - Connect(&buffer)");
		return hr;
	}

	if (m_options & RE_CF_INTEGRATED_SECURITY)
	{
		hr = InitAuth();
		if (FAILED(hr))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcClientConnection::Connect - InitAuth");
			return hr;
		}
	}

	hr = buffer.Recv(&m_socket);
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcClientConnection::Connect - m_buffer.Recv");
		return hr;
	}

	CComObjectStack<RpcBindingResp> respCmd;
	hr = buffer.Read(&respCmd);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, "RpcClientConnection::Connect - Read failed, this : #%08x, hr = #%08x", this, hr);

		return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
	}

	*pCompleted = TRUE;

	hr = respCmd.m_hRes;
	if (hr != S_OK)
	{
		m_errorInfo.Set(respCmd.GetErrorInfo());

		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcClientConnection::Connect - Validate failed, this : #%08x, hr = #%08x", this, hr);

		return hr;
	}

	m_aid = respCmd.m_aid;
	m_sid = respCmd.m_sid;
	m_oid = respCmd.m_oid;
	m_refreshToken = std::move(respCmd.m_refreshToken);

	OnConnected();

	RPC_CONNECT_RESP resp;
	resp.sid = respCmd.m_sid;
	resp.oid = respCmd.m_oid;

	Lock();
	auto pSinkNode = m_pHeadSink;
	for (; pSinkNode; pSinkNode = pSinkNode->pNext)
	{
		pSinkNode->pSink->OnConnected(&resp);
	}
	Unlock();

	return hr;
}

HRESULT RpcClientConnection::Connect(RpcBuffer* pBuffer)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, L"RpcClientConnection::Connect2 - serverName : %ws, port : %d", m_serverName, m_port);

	if (m_socket.IsConnected())
	{
		m_socket.Close();
	}

	LPCWSTR addr = m_serverName;
	if (!addr || _wcsicmp(addr, L".") == 0)
	{
		addr = _localhost;
	}

	CSocketAddr address;
	int ret = address.FindAddr(addr, m_port, 0, PF_UNSPEC, SOCK_STREAM, 0);
	if (ret != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(ret);

	HRESULT hr = E_FAIL;
	ADDRINFOT* pAI = address.GetAddrInfoList();
	for (int i = 1; pAI; ++i)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Connect - m_socket.Connect, i : %d", i);
		if (m_socket.Connect(pAI, m_timeout))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Connect - pBuffer->Send, i : %d", i);

			pBuffer->Seek(RPC_BP_BEGIN, 0);

			hr = pBuffer->Send(&m_socket);
			if (SUCCEEDED(hr))
			{
				break;
			}
		}

		hr = m_socket.GetHResult();

		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Connect - m_socket.Close, i : %d", i);
		m_socket.Close();

		pAI = pAI->ai_next;
	}

	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, L"RpcClientConnection::Connect - failed");
	}

	return hr;
}

STDMETHODIMP RpcClientConnection::HealthCheck()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::HealthCheck - hr : #%08x", m_hConnectRes);

	if (m_hConnectRes == S_OK && (m_flags & RPC_CF_CONNECTED) != 0)
		return S_OK;

	if (!(m_options & RE_CF_AUTO_RECONNECT))
	{
		_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::HealthCheck - no RE_CF_AUTO_RECONNECT");
		return HRESULT_FROM_WIN32(ERROR_CONNECTION_ABORTED);
	}

	SetEvent(m_hConnEvent);

	DWORD wait = WaitForSingleObject(m_hConnCompEvent, m_timeout);
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::HealthCheck - wait : %d", wait);
	if (wait == WAIT_OBJECT_0)
	{
		m_errorInfo.Report();

		return m_hConnectRes;
	}
	else if (wait == WAIT_TIMEOUT)
	{
		return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
	}
	else
	{
		return RPC_E_DISCONNECTED;
	}
}

STDMETHODIMP RpcClientConnection::Advise(IRpcConnectionSink* pSink, BOOL addRef)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Advise");

	ObjectLock lock(this);

	auto ppNode = &m_pHeadSink;
	for (; *ppNode; ppNode = &(*ppNode)->pNext);

	*ppNode = new _SinkNode;
	if (!*ppNode)
	{
		return E_OUTOFMEMORY;
	}

	if (addRef)
	{
		pSink->AddRef();
	}

	(*ppNode)->addRef = addRef;
	(*ppNode)->pSink = pSink;
	(*ppNode)->pNext = nullptr;

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, S_OK, "RpcClientConnection::Advise - end");

	return S_OK;
}

STDMETHODIMP RpcClientConnection::Unadvise(IRpcConnectionSink* pSink)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::Unadvise");

	ObjectLock lock(this);

	auto ppNode = &m_pHeadSink;
	for (; *ppNode; ppNode = &(*ppNode)->pNext)
	{
		if ((*ppNode)->pSink == pSink)
			break;
	}

	auto pNode = *ppNode;
	if (!pNode)
	{
		return S_FALSE;
	}

	*ppNode = pNode->pNext;

	if (pNode->addRef)
	{
		pNode->pSink->Release();
	}

	delete pNode;

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, S_OK, "RpcClientConnection::Unadvise - end");

	return S_OK;
}

STDMETHODIMP_(ULONG) RpcClientConnection::GetConnectCount()
{
	return m_connectCount;
}

STDMETHODIMP RpcClientConnection::GetLastConnectResult()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::GetLastConnectResult - hr : #%08x", m_hConnectRes);

	return m_hConnectRes;
}

HRESULT RpcClientConnection::SendReceive(RpcInvokeRequest* pInvoke, RpcInvokeResp** ppResp)
{
	BOOL call = pInvoke->m_type == RPC_INVOKE || pInvoke->m_type == RPC_QUERY_INTERFACE;
	if (call)
	{
		InterlockedIncrement(&m_requesting);
	}

	HRESULT hr = __super::SendReceive(pInvoke, ppResp);

	if (call)
	{
		InterlockedDecrement(&m_requesting);
		InterlockedExchange64((LONG64*)&m_expireTime, GetTickCount64() + m_expireSpan);
	}

	return hr;
}

HRESULT RpcClientConnection::QueueSend(RpcCommand* pCmd)
{
	HRESULT hr = m_sendBox.queue.AddTail(pCmd);
	if (SUCCEEDED(hr))
	{
		SetEvent(m_hSendEvent);
	}

	return hr;
}

void RpcClientConnection::OnConnected()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::OnConnected - aid : %I64u, sid : %s, oid : %I64u", m_aid, GuidStringA(m_sid).m_str, m_oid);

	__super::OnConnected();

	InterlockedExchange64((LONG64*)&m_expireTime, GetTickCount64() + m_expireSpan);
}

void RpcClientConnection::OnDisconnected()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::OnDisconnected - aid : %I64u, sid : %s, oid : %I64u", m_aid, GuidStringA(m_sid).m_str, m_oid);

	__super::OnDisconnected();

	InterlockedExchange64((LONG64*)&m_expireTime, 0);

	Lock();

	ResetEvent(m_hConnCompEvent);
	m_hConnectRes = HRESULT_FROM_WIN32(WSAESHUTDOWN);

	auto pSinkNode = m_pHeadSink;
	for (; pSinkNode; pSinkNode = pSinkNode->pNext)
	{
		pSinkNode->pSink->OnDisconnected();
	}

	if ((m_options & RE_CF_AUTO_RECONNECT) && !(m_options & RE_CF_AUTO_DISCONNECT) && m_pHeadSink)
	{
		SetEvent(m_hConnEvent);
	}

	Unlock();
}

bool RpcClientConnection::OnIdle()
{
	if (m_requesting == 0 && (m_options & RE_CF_AUTO_DISCONNECT) && IsConnected() && m_expireTime > 0)
	{
		auto currentTime = GetTickCount64();
		if (currentTime >= m_expireTime)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcClientConnection::OnIdle - Disconnect");

			Disconnect();

			return true;
		}
	}

	return __super::OnIdle();
}
