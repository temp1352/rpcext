#include "StdAfx.h"
#include "Connection.h"

#include "RpcExt.h"
#include "InterfaceMap.h"
#include "SecurityContext.h"
#include "ThreadWindow.h"

RpcConnection::RpcConnection()
	: m_callContext(this)
{
	m_flags = 0;
	m_aid = 0;

	m_privateRef = 0;

	m_testCount = 0;
	m_nextTestTime = GetTickCount64() + _rpcExt.m_settings.connection.testInterval;

	m_hTermEvent = nullptr;

	m_cid = GUID_NULL;
	m_sid = GUID_NULL;
}

RpcConnection::~RpcConnection()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::~RpcConnection, this : #%08x, aid : %I64u", this, m_aid);
}

HRESULT RpcConnection::FinalConstruct()
{
	return S_OK;
}

void RpcConnection::FinalRelease()
{
}

ULONG RpcConnection::InternalAddRef()
{
	DWORD ref = __super::InternalAddRef();

	return ref;
}

ULONG RpcConnection::InternalRelease()
{
	ULONG ref = __super::InternalRelease();
	if (ref == 0)
	{
		Close();
	}

	return ref;
}

ULONG RpcConnection::_Release()
{
	return __super::InternalRelease();
}

DWORD RpcConnection::AddPrivateRef(LONG ref)
{
	return InterlockedExchangeAdd(&m_privateRef, ref);
}

DWORD RpcConnection::ReleasePrivateRef(LONG ref)
{
	ref = InterlockedExchangeAdd(&m_privateRef, -ref);
	if (ref <= 0)
	{
		Close();
	}

	return ref;
}

HRESULT RpcConnection::Initialize()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::Initialize");

	HRESULT hr = m_callContext.Initialize();
	if (hr != S_OK)
		return hr;

	m_hTermEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (!m_hTermEvent)
		return AtlHresultFromLastError();

	return S_OK;
}

void RpcConnection::Close()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::Close, this : #%08x, aid : %I64u", this, m_aid);

	Lock();

	if (m_hTermEvent)
	{
		CloseHandle(m_hTermEvent);
		m_hTermEvent = nullptr;
	}

	Unlock();

	m_socket.Close();
	m_callContext.Uninitialize();
}

void RpcConnection::Term()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::Term, this : #%08x, aid : %I64u", this, m_aid);

	if (m_hTermEvent)
	{
		SetEvent(m_hTermEvent);
	}
}

HRESULT RpcConnection::RpcConnection::HealthCheck()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::HealthCheck, this : #%08x, aid : %I64u", this, m_aid);

	return IsConnected() ? S_OK : HRESULT_FROM_WIN32(ERROR_GRACEFUL_DISCONNECT);
}

typedef void (RpcConnection::*_CommandHandler)(RpcCommand*);

void RpcConnection::OnReceiveCompletion(RpcCommand* pCmd)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnReceiveCompletion, this : #%08x, aid : %I64u, type : %08x, cookie : %d, eventid : %d", this, m_aid, pCmd->m_type, pCmd->m_cookie, pCmd->m_eventId);

	static _CommandHandler _requestHandler[] = {
		nullptr,
		(_CommandHandler)&RpcConnection::OnBinding,
		(_CommandHandler)&RpcConnection::OnAuthenticate,
		(_CommandHandler)&RpcConnection::OnInvoke,
		(_CommandHandler)&RpcConnection::OnQueryState,
		(_CommandHandler)&RpcConnection::OnQueryInterface,
		(_CommandHandler)&RpcConnection::OnActiveTest,
		(_CommandHandler)&RpcConnection::OnFree,
	};

	static _CommandHandler _respHandler[] = {
		nullptr,
		(_CommandHandler)&RpcConnection::OnBindingResp,
		(_CommandHandler)&RpcConnection::OnAuthenticateResp,
		(_CommandHandler)&RpcConnection::OnInvokeResp,
		(_CommandHandler)&RpcConnection::OnQueryStateResp,
		(_CommandHandler)&RpcConnection::OnQueryInterfaceResp,
		(_CommandHandler)&RpcConnection::OnActiveTestResp,
		(_CommandHandler)&RpcConnection::OnFreeResp,
	};

	if ((pCmd->m_type & 0x80000000) == 0)
	{
		(this->*_requestHandler[pCmd->m_type])(pCmd);
	}
	else
	{
		(this->*_respHandler[pCmd->m_type - 0x80000000])(pCmd);
	}
}

void RpcConnection::UpdateNextTestTime(ULONGLONG currentTime)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::UpdateTestTime, this : #%08x, aid : %I64u, m_nextTestTime : %I64u, m_testCount : %d", this, m_aid, m_nextTestTime, m_testCount);

	if (currentTime == 0)
	{
		currentTime = GetTickCount64();
	}

	InterlockedExchange64((LONG64*)&m_nextTestTime, currentTime + _rpcExt.m_settings.connection.testInterval);
	InterlockedExchange(&m_testCount, 0);
}

void RpcConnection::CleanRecvQueue(ULONGLONG currentTime)
{
	m_recvBox.queue.Lock();

	//_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::CleanRecvQueue, m_recvBox.queue.Count : %d", m_recvBox.queue._GetCount());

	RpcCommand* pCmd = nullptr;
	RpcQueue::_Node* pCurr = nullptr;
	RpcQueue::_Node* pNode = m_recvBox.queue._GetHead();
	while (pNode)
	{
		pCurr = pNode;
		pNode = pNode->pNext;

		pCmd = pCurr->pCmd;

		if ((currentTime > pCmd->m_expireTime && currentTime - pCmd->m_expireTime < 0x7f000000) || (pCmd->m_expireTime - currentTime > 0x7f000000))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::CleanRecvQueue, Remove, type : %x", pCmd->m_type);

			m_recvBox.queue._Remove(pCurr);
		}
	}

	m_recvBox.queue.Unlock();
}

void RpcConnection::ActiveTest()
{
	CComPtr<RpcActiveRequest> spCmd(new CComObject<RpcActiveRequest>);
	if (spCmd)
	{
		QueueSend(spCmd);
	}
}

HRESULT RpcConnection::Disconnect()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::Disconnect, this : #%08x, aid : %I64u", this, m_aid);

	m_socket.Close();

	m_recvBox.queue.RemoveAll();
	m_callContext.RemoveAll();

	OnDisconnected();

	return 0;
}

HRESULT RpcConnection::InitAuth()
{
	Credentials credentials;
	HRESULT hr = credentials.Acquire(SECPKG_CRED_OUTBOUND);
	if (FAILED(hr))
		return hr;

	CSecBuffer inBuff, outBuff;
	hr = inBuff.Allocate();
	if (FAILED(hr))
		return hr;

	hr = outBuff.Allocate();
	if (FAILED(hr))
		return hr;

	SecurityContext securityContext(&credentials);
	hr = securityContext.Initialize(nullptr, nullptr, &outBuff);
	if (FAILED(hr))
		return hr;

	hr = outBuff.Send(&m_socket);
	if (FAILED(hr))
		return hr;

	for (;;)
	{
		hr = inBuff.Recv(&m_socket);
		if (FAILED(hr))
			break;

		outBuff.cbBuffer = SEC_BUF_SIZE;

		hr = securityContext.Initialize(nullptr, &inBuff, &outBuff);
		if (FAILED(hr) || hr == S_OK)
			break;

		hr = outBuff.Send(&m_socket);
		if (FAILED(hr))
			break;
	}

	return hr;
}

HRESULT RpcConnection::AcceptAuth(HANDLE* phToken)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::AcceptAuth");

	CSecPkgInfo pkgInfo;
	HRESULT hr = pkgInfo.Query(nullptr);
	if (FAILED(hr))
		return hr;

	DWORD cbMaxMessage = pkgInfo->cbMaxToken;

	CSecBuffer inBuff, outBuff;
	hr = outBuff.Allocate(cbMaxMessage);
	if (FAILED(hr))
		return hr;

	Credentials credentials;
	hr = credentials.Acquire(SECPKG_CRED_INBOUND);
	if (FAILED(hr))
		return hr;

	BOOL fDone = FALSE;
	BOOL fNewConversation = TRUE;
	SecurityContext securityContext(&credentials);

	while (!fDone)
	{
		hr = inBuff.Recv(&m_socket);
		if (FAILED(hr))
			return hr;

		hr = securityContext.Accept(fNewConversation, &inBuff, &outBuff);
		if (FAILED(hr))
			return hr;

		if (hr == S_OK)
		{
			fDone = TRUE;
		}

		hr = outBuff.Send(&m_socket);
		if (FAILED(hr))
			return hr;

		fNewConversation = FALSE;
	}

	hr = securityContext.Impersonate();
	if (FAILED(hr))
		return hr;

	if (!::OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, false, phToken))
	{
		hr = AtlHresultFromLastError();
	}

	securityContext.Revert();

	return hr;
}

DWORD _WaitForMultipleObjects(DWORD count, CONST HANDLE* pHandles, DWORD milliseconds)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "_WaitForMultipleObjects");

	auto startTime = GetTickCount64();

	for (;;)
	{
		DWORD wait = MsgWaitForMultipleObjects(count, pHandles, FALSE, milliseconds, QS_ALLEVENTS);
		if (wait == WAIT_OBJECT_0 + count)
		{
			MSG msg = { 0 };
			while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				//_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "DispatchMessage");

				if (msg.message == WM_QUIT)
				{
					PostQuitMessage(msg.wParam);

					return wait;
				}

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);

				//_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "DispatchMessage - end");
			}

			if (milliseconds != INFINITE)
			{
				auto currentTime = GetTickCount64();
				milliseconds -= currentTime - startTime;
				startTime = currentTime;
			}
		}
		else
		{
			return wait;
		}
	}
}

const DWORD _sendTimeout = 20000;

HRESULT RpcConnection::AuthRequest()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::AuthRequest, this : #%08x", this);

	CComPtr<RpcAuthRequest> spRequest(new CComObject<RpcAuthRequest>());
	if (!spRequest)
		return E_OUTOFMEMORY;

	spRequest->m_cookie = CreateCookie();

	RpcAutoEvent autoEvent(spRequest->m_cookie);
	spRequest->m_eventId = autoEvent.m_eventId;

	HANDLE hReqEvent = _rpcExt.m_eventManager.GetHandle(spRequest->m_eventId);
	ResetEvent(hReqEvent);

	HRESULT hr = QueueSend(spRequest);
	if (FAILED(hr))
		return hr;

	int sendCount = 1;

	HANDLE hEvents[] = { m_hTermEvent, hReqEvent };

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::AuthRequest, this : #%08x, aid : %I64u, eventId : %d, cookie : %d, hEvent : %x", this, m_aid, spRequest->m_eventId, spRequest->m_cookie, hReqEvent);

	for (;;)
	{
		DWORD wait = _WaitForMultipleObjects(2, hEvents, _sendTimeout);
		if (wait == WAIT_OBJECT_0)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcConnection::AuthRequest, Stop, this : #%08x, aid : %I64u", this, m_aid);

			return AtlHresultFromWin32(RPC_S_CALL_FAILED);
		}
		else if (wait == WAIT_OBJECT_0 + 1)
		{
			return m_recvBox.queue.Lookup(RPC_AUTHENTICATE_RESP, spRequest->m_cookie, nullptr, TRUE);
		}
		else if (wait == WAIT_TIMEOUT)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::AuthRequest, this : #%08x, aid : %I64u, sendCount : %d", this, m_aid, sendCount);

			if (sendCount > 3)
			{
				_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::AuthRequest, Wait timeout, The remote procedure call failed, this : #%08x, aid : %I64u", this, m_aid);

				return AtlHresultFromWin32(RPC_S_CALL_FAILED);
			}

			hr = QueueSend(spRequest);
			if (FAILED(hr))
				break;

			++sendCount;
		}
		else
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcConnection::AuthRequest - Wait failed, this : #%08x, aid : %I64u, wait : %d", this, m_aid, wait);

			return AtlHresultFromWin32(wait);
		}
	}

	return hr;
}

HRESULT RpcConnection::SendReceive(RpcInvokeRequest* pInvoke, RpcInvokeResp** ppResp)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::SendReceive, this : #%08x, aid : %I64u, cookie : %d, eventid : %d", this, m_aid, pInvoke->m_cookie, pInvoke->m_eventId);

	HANDLE hInvokeEvent = _rpcExt.m_eventManager.GetHandle(pInvoke->m_eventId);
	ResetEvent(hInvokeEvent);

	HRESULT hr = QueueSend(pInvoke);
	if (FAILED(hr))
		return hr;

	HANDLE hEvents[] = { m_hTermEvent, hInvokeEvent };
	for (;;)
	{
		DWORD wait = _WaitForMultipleObjects(2, hEvents, _sendTimeout);
		if (wait == WAIT_OBJECT_0) // stop
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::SendReceive - Stop, this : #%08x, aid : %I64u", this, m_aid);

			return AtlHresultFromWin32(RPC_S_CALL_FAILED);
		}
		else if (wait == WAIT_OBJECT_0 + 1)
		{
			HRESULT hr = m_recvBox.queue.Lookup(RPC_INVOKE_RESP, pInvoke->m_cookie, (RpcCommand**)ppResp, TRUE);
			if (hr != S_OK)
				_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcConnection::SendReceive - Lookup, this : #%08x, aid : %I64u, hr = 0x%08x, eventid : %d, hInvokeEvent : %x", this, m_aid, hr, pInvoke->m_eventId, hInvokeEvent);

			return hr;
		}
		else if (wait == WAIT_TIMEOUT)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::SendReceive - Timeout, this : #%08x, aid : %I64u", this, m_aid);

			break;
		}
		else
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, "RpcConnection::SendReceiv - Wait failed, this : #%08x, aid : %I64u, wait : %d", this, m_aid, wait);

			return AtlHresultFromWin32(wait);
		}
	}

	return GetInvokeState(pInvoke->m_cookie, hInvokeEvent, ppResp);
}

HRESULT RpcConnection::GetInvokeState(RPC_COOKIE invokeCookie, HANDLE hInvokeEvent, RpcInvokeResp** ppResp)
{
	CComPtr<RpcStateRequest> spStateCmd(new CComObject<RpcStateRequest>());
	if (!spStateCmd)
		return E_OUTOFMEMORY;

	spStateCmd->m_cookie = CreateCookie();

	RpcAutoEvent autoEvent(spStateCmd->m_cookie);
	spStateCmd->m_eventId = autoEvent.m_eventId;
	spStateCmd->m_invokeCookie = invokeCookie;

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::GetInvokeState, this : #%08x, aid : %I64u, cookie : %d, eventid : %d", this, m_aid, spStateCmd->m_cookie, spStateCmd->m_eventId);

	HANDLE hStateEvent = _rpcExt.m_eventManager.GetHandle(spStateCmd->m_eventId);
	ResetEvent(hStateEvent);

	HRESULT hr = QueueSend(spStateCmd);
	if (FAILED(hr))
		return hr;

	int sendCount = 1;

	HANDLE hEvents[] = { m_hTermEvent, hInvokeEvent, hStateEvent };

	for (;;)
	{
		DWORD wait = _WaitForMultipleObjects(3, hEvents, _sendTimeout);
		if (wait == WAIT_OBJECT_0)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::GetInvokeState, Stop, this : #%08x, aid : %I64u", this, m_aid);

			return AtlHresultFromWin32(RPC_S_CALL_FAILED);
		}
		else if (wait == WAIT_OBJECT_0 + 1)
		{
			return m_recvBox.queue.Lookup(RPC_INVOKE_RESP, invokeCookie, (RpcCommand**)ppResp, TRUE);
		}
		else if (wait == WAIT_OBJECT_0 + 2)
		{
			CComPtr<RpcStateResp> spStateResp;
			hr = m_recvBox.queue.Lookup(RPC_QUERY_STATE_RESP, spStateCmd->m_cookie, (RpcCommand**)&spStateResp, TRUE);
			if (hr != S_OK)
			{
				_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcConnection::GetInvokeState - m_recvBox.queue.Lookup, this : #%08x, aid : %I64u, hr != S_OK, The remote procedure call failed", this, m_aid);

				return AtlHresultFromWin32(RPC_S_CALL_FAILED);
			}

			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::GetInvokeState, this : #%08x, aid : %I64u, hr = #%x", this, m_aid, spStateResp->m_hRes);

			if (spStateResp->m_hRes != S_OK)
			{
				_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcConnection::GetInvokeState - m_recvBox.queue.Lookup, spStateResp->m_hRes != S_OK, The remote procedure call failed");

				return AtlHresultFromWin32(RPC_S_CALL_FAILED);
			}

			sendCount = 0;

			ResetEvent(hStateEvent);
		}
		else if (wait == WAIT_TIMEOUT)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::GetInvokeState, this : #%08x, aid : %I64u, sendCount : %d", this, m_aid, sendCount);

			if (sendCount > 3)
			{
				_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcConnection::GetInvokeState - Wait timeout, The remote procedure call failed, this : #%08x, aid : %I64u", this, m_aid);

				return AtlHresultFromWin32(RPC_S_CALL_FAILED);
			}

			hr = QueueSend(spStateCmd);
			if (FAILED(hr))
				return hr;

			++sendCount;
		}
		else
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, "RpcConnection::GetInvokeState, Wait failed, this : #%08x, aid : %I64u, wait : %d", this, m_aid, wait);

			return AtlHresultFromWin32(wait);
		}
	}

	return hr;
}

HRESULT RpcConnection::RemQueryInterface(OID oid, REFIID riid)
{
	CStringA itfName;

	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		_interfaceMap.GetInterfaceName(riid, itfName);
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::RemQueryInterface, this : #%08x, aid : %I64u, oid : %I64u, interface : %s", this, m_aid, oid, itfName);
	}

	CComPtr<RpcQIRequest> spQICmd(new CComObject<RpcQIRequest>());
	if (!spQICmd)
		return E_OUTOFMEMORY;

	spQICmd->m_cookie = CreateCookie();

	RpcAutoEvent autoEvent(spQICmd->m_cookie);
	spQICmd->m_eventId = autoEvent.m_eventId;
	spQICmd->m_oid = oid;
	spQICmd->m_iid = riid;

	HANDLE hQIEvent = _rpcExt.m_eventManager.GetHandle(spQICmd->m_eventId);
	ResetEvent(hQIEvent);

	HRESULT hr = QueueSend(spQICmd);
	if (FAILED(hr))
		return hr;

	int sendCount = 1;

	HANDLE hEvents[] = { m_hTermEvent, hQIEvent };

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::RemQueryInterface, this : #%08x, aid : %I64u, eventId : %d, cookie : %d, interface : %s, hEvent : %x", this, m_aid, spQICmd->m_eventId, spQICmd->m_cookie, itfName, hQIEvent);

	for (;;)
	{
		DWORD wait = _WaitForMultipleObjects(2, hEvents, _sendTimeout);
		if (wait == WAIT_OBJECT_0)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcConnection::RemQueryInterface, Stop, this : #%08x, aid : %I64u, interface : %s", this, m_aid, itfName);

			return AtlHresultFromWin32(RPC_S_CALL_FAILED);
		}
		else if (wait == WAIT_OBJECT_0 + 1)
		{
			return m_recvBox.queue.Lookup(RPC_QUERY_INTERFACE_RESP, spQICmd->m_cookie, nullptr, TRUE);
		}
		else if (wait == WAIT_TIMEOUT)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::RemQueryInterface, this : #%08x, aid : %I64u, interface : %s, sendCount : %d", this, m_aid, itfName, sendCount);

			if (sendCount > 3)
			{
				_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::RemQueryInterface, Wait timeout, The remote procedure call failed, this : #%08x, aid : %I64u, interface : %s", this, m_aid, itfName);

				return AtlHresultFromWin32(RPC_S_CALL_FAILED);
			}

			hr = QueueSend(spQICmd);
			if (FAILED(hr))
				break;

			++sendCount;
		}
		else
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, "RpcConnection::RemQueryInterface - Wait failed, this : #%08x, aid : %I64u, interface : %s, wait : %d", this, m_aid, itfName, wait);

			return AtlHresultFromWin32(wait);
		}
	}

	return hr;
}

HRESULT RpcConnection::FreeObject(OID oid)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::FreeObject, oid : %I64u, this : #%08x, aid : %I64u", oid, this, m_aid);

	if (!m_socket.IsConnected())
		return S_FALSE;

	CComPtr<RpcFreeRequest> spFreeCmd(new CComObject<RpcFreeRequest>());
	if (!spFreeCmd)
		return E_OUTOFMEMORY;

	spFreeCmd->m_cookie = CreateCookie();

	RpcAutoEvent autoEvent(spFreeCmd->m_cookie);
	spFreeCmd->m_eventId = autoEvent.m_eventId;
	spFreeCmd->m_oid = oid;

	HANDLE hFreeEvent = _rpcExt.m_eventManager.GetHandle(spFreeCmd->m_eventId);
	ResetEvent(hFreeEvent);

	HRESULT hr = QueueSend(spFreeCmd);
	if (FAILED(hr))
		return hr;

	HANDLE hEvents[] = { m_hTermEvent, hFreeEvent };

	DWORD wait = _WaitForMultipleObjects(2, hEvents, _sendTimeout);
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcConnection::FreeObject, this : #%08x, aid : %I64u, wait : %d", this, m_aid, wait);
	if (wait == WAIT_OBJECT_0)
	{
		hr = AtlHresultFromWin32(RPC_S_CALL_FAILED);

		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcConnection::FreeObject, Stop, this : #%08x, aid : %I64u", this, m_aid);
	}
	else if (wait == WAIT_OBJECT_0 + 1)
	{
		hr = m_recvBox.queue.Lookup(RPC_INVOKE_RESP, spFreeCmd->m_cookie, nullptr, TRUE);
	}
	else
	{
		hr = AtlHresultFromWin32(wait);
	}

	if (FAILED(hr) && m_privateRef == 0)
	{
		Disconnect();
	}

	return hr;
}

DWORD RpcConnection::CreateCookie()
{
	static LONG _cookie = 0;

	return InterlockedIncrement(&_cookie);
}

HRESULT RpcConnection::QueueSend(RpcCommand* pCmd)
{
	HRESULT hr = m_sendBox.queue.AddTail(pCmd);

	return hr;
}

void RpcConnection::SyncMode()
{
	InterlockedExchange(&m_flags, m_flags | RPC_CF_SYNC_MODE);
}

void RpcConnection::AsyncMode()
{
	InterlockedExchange(&m_flags, m_flags & ~RPC_CF_SYNC_MODE);
}

HRESULT RpcConnection::SendNext(bool lock)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::SendNext, this : #%08x, aid : %I64u, lock : %d", this, m_aid, (int)lock);

	if (lock)
	{
		m_socket.m_csWrite.Lock();

		if (WaitForSingleObject(m_socket.m_hEventWrite, 0) != WAIT_OBJECT_0)
		{
			m_socket.m_csWrite.Unlock();

			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::SendNext - WaitForSingleObject failed, this : #%08x, aid : %I64u, lock : %d", this, m_aid, (int)lock);

			return S_FALSE;
		}
	}

	WSAResetEvent(m_socket.m_hEventWrite);

	// 当发送缓冲区剩余字节为0时才发送，不为0表示上个发送消息还未发送完成，连续5次失败后，即使上个发送
	// 消息未完成，强制清空，继续发送新消息
	if (m_sendBox.failedCount >= 5 || m_sendBox.buffer.GetAvailable() == 0)
	{
		CComPtr<RpcCommand> spCmd;
		if (m_sendBox.queue.RemoveHead(&spCmd) != S_OK)
		{
			WSASetEvent(m_socket.m_hEventWrite);

			if (lock)
			{
				m_socket.m_csWrite.Unlock();
			}

			return S_FALSE;
		}

		m_sendBox.buffer.SetSize(0, false);
		m_sendBox.buffer.Write(spCmd);

		m_sendBox.failedCount = 0;

		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::SendNext - PacketData, this : #%08x, aid : %I64u, type = #%x, cookie : %d, eventid : %d, len : %d", this, m_aid, spCmd->m_type, spCmd->m_cookie, spCmd->m_eventId, m_sendBox.buffer.GetAvailable());
	}
	else if (m_sendBox.buffer.GetAvailable() == 0)
	{
		WSASetEvent(m_socket.m_hEventWrite);

		if (lock)
		{
			m_socket.m_csWrite.Unlock();
		}

		return S_FALSE;
	}

	HRESULT hr = 0;
	DWORD dwSent = 0;
	WSABUF buf = { m_sendBox.buffer.GetAvailable(), (char*)m_sendBox.buffer.GetCurrent() };
	if (!m_socket._AsyncWrite(&buf, 1, &dwSent, &m_sendBox.overlapped, (m_flags & RPC_CF_COMPLETION_ROUTINE) ? SendCompletion : nullptr))
	{
		DWORD lastError = m_socket.GetLastError();

		_logFile.WriteV(LogCategory::Comm, LogLevel::Error, LogOption::Std, "RpcConnection::SetNext - _AsyncWrite, this : #%08x, aid : %I64u, #error : %d", this, m_aid, lastError);

		if (lastError == WSAECONNRESET)
		{
			Term();
		}

		++m_sendBox.failedCount;

		hr = HRESULT_FROM_WIN32(lastError);
	}
	else
	{
		ATLASSERT(m_sendBox.test1 == m_sendBox.test2);

		++m_sendBox.test1;
	}

	if (lock)
	{
		m_socket.m_csWrite.Unlock();
	}

	return hr;
}

HRESULT RpcConnection::PostRecv(bool lock)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::PostRecv, this : #%08x, aid : %I64u, lock : %d", this, m_aid, (int)lock);

	m_socket.m_csRead.Lock();

	if (lock)
	{
		if (WaitForSingleObject(m_socket.m_hEventRead, 0) != WAIT_OBJECT_0)
		{
			m_socket.m_csRead.Unlock();

			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::PostRecv - WaitForSingleObject failed, this : #%08x, aid : %I64u", this, m_aid);

			return S_FALSE;
		}
	}

	WSAResetEvent(m_socket.m_hEventRead);

	const ULONG BUF_SIZE = 4096;
	ULONG size = m_recvBox.buffer.GetSize();
	HRESULT hr = m_recvBox.buffer.Alloc(size + BUF_SIZE);
	if (FAILED(hr))
	{
		m_socket.m_csRead.Unlock();
		WSASetEvent(m_socket.m_hEventRead);

		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::PostRecv - Alloc failed, this : #%08x, aid : %I64u", this, m_aid);

		return hr;
	}

	DWORD recved = 0;
	WSABUF buf = { BUF_SIZE, (char*)m_recvBox.buffer.GetBuffer() + size };
	if (!m_socket._AsyncRead(&buf, 1, &recved, &m_recvBox.overlapped, (m_flags & RPC_CF_COMPLETION_ROUTINE) ? RecvCompletion : nullptr))
	{
		DWORD lastError = m_socket.GetLastError();

		_logFile.WriteV(LogCategory::Comm, LogLevel::Error, LogOption::Std, "RpcConnection::PostRecv - _AsyncRead, this : #%08x, aid : %I64u, #error : %d", this, m_aid, lastError);

		if (lastError == WSAECONNRESET)
		{
			Term();
		}

		hr = HRESULT_FROM_WIN32(lastError);
	}

	m_socket.m_csRead.Unlock();

	return hr;
}

void RpcConnection::OnSended(DWORD error, DWORD transferred)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnSended, error : %d, transferred : %d, this : #%08x, aid : %I64u", error, transferred, this, m_aid);

	m_socket.m_csWrite.Lock();

	ATLASSERT(++m_sendBox.test2 == m_sendBox.test1);
	if (m_sendBox.test2 != m_sendBox.test1)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnSended, this : #%08x, aid : %I64u, test1 : %d, test2 : %d", this, m_aid, m_sendBox.test1, m_sendBox.test2);
	}

	if (error == WSAECONNRESET || (error == ERROR_SUCCESS && transferred == 0))
	{
		Term();

		m_socket.m_csWrite.Unlock();

		return;
	}

	if (error == ERROR_SUCCESS)
	{
		UpdateNextTestTime();

		m_sendBox.buffer.Seek(RPC_BP_CURRENT, transferred);
	}
	else
	{
		++m_sendBox.failedCount;
	}

	SendNext(false);

	m_socket.m_csWrite.Unlock();
}

void RpcConnection::OnReceived(DWORD error, DWORD transferred)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnReceived, this : #%08x, aid : %I64u, error : %d, transferred : %d", this, m_aid, error, transferred);

	if (error == WSAECONNRESET || (error == ERROR_SUCCESS && transferred == 0))
	{
		Term();

		return;
	}

	if (error == ERROR_SUCCESS && transferred > 0)
	{
		UpdateNextTestTime();

		m_recvBox.buffer.SetSize(transferred, true);

		RpcCommand* pCmd = nullptr;
		while (m_recvBox.buffer.Read(&pCmd) == S_OK)
		{
			OnReceiveCompletion(pCmd);

			if (pCmd->m_type == RPC_AUTHENTICATE_RESP)
			{
				Sleep(500);
			}

			pCmd->Release();
			pCmd = nullptr;
		}
	}

	if ((m_flags & RPC_CF_SYNC_MODE) == 0)
	{
		PostRecv(false);
	}
}

void CALLBACK RpcConnection::SendCompletion(DWORD error, DWORD transferred, LPWSAOVERLAPPED pOverlapped, DWORD flags)
{
	auto pThis = (RpcConnection*)((LPBYTE)pOverlapped - offsetof(RpcConnection, m_sendBox.overlapped));

	pThis->OnSended(error, transferred);
}

void CALLBACK RpcConnection::RecvCompletion(DWORD error, DWORD transferred, LPWSAOVERLAPPED pOverlapped, DWORD flags)
{
	auto pThis = (RpcConnection*)((LPBYTE)pOverlapped - offsetof(RpcConnection, m_recvBox.overlapped));

	pThis->OnReceived(error, transferred);
}

void RpcConnection::OnConnected()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnConnected, this : #%08x, aid : %I64u", this, m_aid);

	InterlockedExchange((LONG*)&m_flags, m_flags | RPC_CF_CONNECTED);

	UpdateNextTestTime();
}

void RpcConnection::OnDisconnected()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnDisconnected, this : #%08x, aid : %I64u", this, m_aid);

	InterlockedExchange((LONG*)&m_flags, m_flags & ~RPC_CF_CONNECTED);
}

bool RpcConnection::OnIdle()
{
	//_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnIdle, this : 0x%08x", this);

	auto currentTime = GetTickCount64();

	CleanRecvQueue(currentTime);

	if (currentTime >= m_nextTestTime)
	{
		ULONG testCount = InterlockedIncrement(&m_testCount);

		if (testCount <= _rpcExt.m_settings.connection.testCount)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnIdle - ActiveTest, this : #%08x, aid : %I64u, currentTime : %d, m_nextTestTime : %I64u, m_testCount : %d", this, m_aid, currentTime, m_nextTestTime, testCount);

			ActiveTest();
		}
		else
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnIdle - Disconnect, this : #%08x, aid : %I64u, currentTime : %d, m_nextTestTime : %I64u, m_testCount : %d", this, m_aid, currentTime, m_nextTestTime, testCount);

			Disconnect();

			return false;
		}
	}

	return true;
}

void RpcConnection::OnBinding(RpcBindingRequest* pCmd)
{
	m_cid = pCmd->m_cid;

	if (pCmd->m_sid == GUID_NULL)
	{
		auto _ = CoCreateGuid(&pCmd->m_sid);
	}

	SC_AUTH_INFO authInfo;
	authInfo.sid = pCmd->m_sid;
	authInfo.clsid = pCmd->m_clsid;
	authInfo.domain = pCmd->m_domain;
	authInfo.referer = pCmd->m_referer;
	authInfo.params = pCmd->m_params;
	authInfo.authCode = pCmd->m_authCode;
	authInfo.refreshToken = pCmd->m_refreshToken;
	authInfo.userName = pCmd->m_userName;
	authInfo.password = pCmd->m_password;

	OID oid = pCmd->m_oid;
	CComBSTR refreshToken;
	HRESULT hr = m_callContext.Connect(pCmd->m_clsid, pCmd->m_iid, oid, pCmd->m_flags, &authInfo, &oid, &refreshToken);
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnBinding - m_callContext.Connect, this : #%08x, aid : %I64u, hr : %x, oid : %I64u", this, m_aid, hr, oid);

	CComPtr<RpcBindingResp> spResp(new CComObject<RpcBindingResp>);
	if (spResp)
	{
		spResp->m_hRes = hr;
		spResp->m_oid = oid;
		spResp->m_aid = m_aid;
		spResp->m_sid = m_sid = authInfo.sid;
		spResp->m_refreshToken = std::move(refreshToken);

		spResp->CacheError();

		QueueSend(spResp);

		OnConnected();
	}
}

void RpcConnection::OnBindingResp(RpcBindingResp* pCmd)
{
}

void RpcConnection::OnAuthenticate(RpcAuthRequest* pCmd)
{
	CComPtr<RpcAuthResp> spResp(new CComObject<RpcAuthResp>);
	if (!spResp)
		return;

	spResp->m_cookie = pCmd->m_cookie;
	spResp->m_eventId = pCmd->m_eventId;
	spResp->m_hRes = S_OK;

	RpcBuffer buffer;
	HRESULT hr = buffer.Write(spResp);
	if (FAILED(hr))
		return;

	hr = buffer.Send(&m_socket);
	if (FAILED(hr))
		return;

	InitAuth();
}

void RpcConnection::OnAuthenticateResp(RpcAuthResp* pCmd)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnAuthenticateResp, evnetId : %d cookie : %d, hEvent : %x", pCmd->m_eventId, pCmd->m_cookie,
		_rpcExt.m_eventManager.GetHandle(pCmd->m_eventId));

	pCmd->SetExpire(-1);
	m_recvBox.queue.AddTail(pCmd);

	_rpcExt.m_eventManager.SetEvent(pCmd->m_eventId, pCmd->m_cookie);
}

void RpcConnection::OnInvoke(RpcInvokeRequest* pCmd)
{
	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		CStringA itfName, methodName;
		_interfaceMap.GetMethodName(pCmd->m_iid, pCmd->m_message.ProcNum, itfName, methodName);

		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnInvoke, this : #%08x, aid : %I64u, cookie : %d, eventId : %d, procNum : %d, interface : %s, method : %s, threadPool.TaskCount : %d", this, m_aid, pCmd->m_cookie, pCmd->m_eventId, pCmd->m_message.ProcNum, itfName, methodName, _rpcExt.m_threadPool.m_taskList.GetCount());
	}

	if (m_flags & RPC_CF_CONNECTED)
	{
		pCmd->m_expireTime = GetTickCount64() + 0x7f000000;
		m_recvBox.queue.AddTail(pCmd);

		m_callContext.Invoke(pCmd);
	}
}

void RpcConnection::OnInvokeResp(RpcInvokeResp* pCmd)
{
	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		CStringA itfName, methodName;
		_interfaceMap.GetMethodName(pCmd->m_iid, pCmd->m_message.ProcNum, itfName, methodName);

		_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, "RpcConnection::OnInvokeResp, this : #%08x, cookie : %d, eventid : %d, procNum : %d, interface : %s, method : %s, result : 0x%08x, ", this, pCmd->m_cookie, pCmd->m_eventId, pCmd->m_message.ProcNum,
			itfName, methodName, pCmd->m_hRes);
	}

	pCmd->SetExpire(-1);
	m_recvBox.queue.AddHead(pCmd);

	_rpcExt.m_eventManager.SetEvent(pCmd->m_eventId, pCmd->m_cookie);
}

void RpcConnection::OnQueryState(RpcStateRequest* pCmd)
{
	HRESULT hr = m_recvBox.queue.Lookup(RPC_INVOKE, pCmd->m_invokeCookie, nullptr, FALSE);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnQueryState, cookie : %d, eventid : %d, invoke_cookie : %d, hRes : %x, m_recvBox.queue.Count : %d, threadPool.TaskCount : %d",
			pCmd->m_cookie, pCmd->m_eventId, pCmd->m_invokeCookie, hr, m_recvBox.queue.GetCount(), _rpcExt.m_threadPool.m_taskList.GetCount());
	}

	CComPtr<RpcStateResp> spResp(new CComObject<RpcStateResp>);
	if (spResp)
	{
		spResp->m_cookie = pCmd->m_cookie;
		spResp->m_eventId = pCmd->m_eventId;
		spResp->m_hRes = hr;

		QueueSend(spResp);
	}
}

void RpcConnection::OnQueryStateResp(RpcStateResp* pCmd)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnQueryStateResp, cookie : %d", pCmd->m_cookie);

	pCmd->SetExpire(-1);
	m_recvBox.queue.AddTail(pCmd);

	_rpcExt.m_eventManager.SetEvent(pCmd->m_eventId, pCmd->m_cookie);
}

void RpcConnection::OnQueryInterface(RpcQIRequest* pCmd)
{
	if (_logFile.m_outLevel > 1)
	{
		CStringA strItf;
		_interfaceMap.GetInterfaceName(pCmd->m_iid, strItf);
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnQueryInterface, iid : %s", strItf);
	}

	m_callContext.QueryInterface(pCmd);
}

void RpcConnection::OnQueryInterfaceResp(RpcQIResp* pCmd)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnQueryInterfaceResp, evnetId : %d cookie : %d, hEvent : %x", pCmd->m_eventId, pCmd->m_cookie,
		_rpcExt.m_eventManager.GetHandle(pCmd->m_eventId));

	pCmd->SetExpire(-1);
	m_recvBox.queue.AddTail(pCmd);

	_rpcExt.m_eventManager.SetEvent(pCmd->m_eventId, pCmd->m_cookie);
}

void RpcConnection::OnActiveTest(RpcActiveRequest* pCmd)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnActiveTest, this : #%08x, aid : %I64u", this, m_aid);

	CComPtr<RpcActiveResp> spResp(new CComObject<RpcActiveResp>);
	if (spResp)
	{
		QueueSend(spResp);
	}
}

void RpcConnection::OnActiveTestResp(RpcActiveResp* pResp)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnActiveTestResp, this : #%08x, aid : %I64u", this, m_aid);
}

void RpcConnection::OnFree(RpcFreeRequest* pCmd)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnFree");

	m_callContext.Free(pCmd);
}

void RpcConnection::OnFreeResp(RpcFreeResp* pCmd)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcConnection::OnFreeResp");

	pCmd->m_expireTime = GetTickCount64() + 5000;
	m_recvBox.queue.AddTail(pCmd);

	_rpcExt.m_eventManager.SetEvent(pCmd->m_eventId, pCmd->m_cookie);
}
