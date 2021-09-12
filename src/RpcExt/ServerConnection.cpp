#include "StdAfx.h"
#include "ServerConnection.h"

#include "RpcExt.h"

ULONG_PTR _GetAid()
{
	static LONG64 _aid = 0;

	ULONG64 aid = InterlockedIncrement64(&_aid);
	if (aid == 0)
	{
		aid = InterlockedIncrement64(&_aid);
	}

	return aid;
}

RpcServerConnection::RpcServerConnection()
{
	m_hIOCP = nullptr;

	m_pServer = nullptr;

	m_aid = _GetAid();

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServerConnection::RpcServerConnection - this : #%08x, aid : %I64u", this, m_aid);
}

RpcServerConnection::~RpcServerConnection()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServerConnection::~RpcServerConnection - this : #%08x, aid : %I64u", this, m_aid);
}

HRESULT RpcServerConnection::_CreateInstance(RpcServerConnection** ppConnection)
{
	CComObject<RpcServerConnection>* p = nullptr;
	HRESULT hr = CComObject<RpcServerConnection>::CreateInstance(&p);
	if (SUCCEEDED(hr))
	{
		p->AddRef();

		*ppConnection = p;
	}

	return hr;
}

ULONG RpcServerConnection::InternalAddRef()
{
	DWORD ref = __super::InternalAddRef();

	return ref;
}

ULONG RpcServerConnection::InternalRelease()
{
	DWORD ref = __super::InternalRelease();

	return ref;
}

HRESULT RpcServerConnection::Reset()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServerConnection::Reset - m_pServer : #%x", m_pServer);

	__super::Close();

	return Create(m_pServer);
}

HRESULT RpcServerConnection::Create(RpcServer* pServer)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServerConnection::Create, this : #%08x, aid : %I64u, pServer : #%x, iocp : #%x", this, m_aid, pServer, pServer ? pServer->m_hIOCP : 0);

	if (!pServer)
		return E_UNEXPECTED;

	HRESULT hr = __super::Initialize();
	if (FAILED(hr))
		return hr;

	m_pServer = pServer;
	m_hIOCP = pServer->m_hIOCP;

	if (!m_socket.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP, WSA_FLAG_OVERLAPPED))
	{
		DWORD lastError = WSAGetLastError();

		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, lastError, "RpcServerConnection::Create - AcceptEx, error : %d", lastError);

		return HRESULT_FROM_WIN32(lastError);
	}

	DWORD bytes = 0;

	ULONG BUF_SIZE = 1024;
	hr = m_recvBox.buffer.Alloc(BUF_SIZE);
	if (FAILED(hr))
		return hr;

	if (!pServer->m_socket.AcceptEx(m_socket, m_recvBox.buffer.GetBuffer(), BUF_SIZE, &bytes, &m_recvBox.overlapped))
	{
		DWORD lastError = WSAGetLastError();
		if (lastError != WSA_IO_PENDING)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, lastError, "RpcServerConnection::Create - AcceptEx, error : %d", lastError);

			return HRESULT_FROM_WIN32(lastError);
		}
	}

	return S_OK;
}

HRESULT RpcServerConnection::Accept(DWORD receiveDataLength)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServerConnection::Accept, this : #%08x, aid : %I64u, iocp = #%x", this, m_aid, m_hIOCP);

	SOCKADDR_IN* localSockaddr = nullptr;
	SOCKADDR_IN* remoteSockaddr = nullptr;

	ListenSocket::GetAcceptExSockAddrs(m_recvBox.buffer.GetBuffer(), m_recvBox.buffer.GetAllocSize(), &localSockaddr, &remoteSockaddr);

	HANDLE h = CreateIoCompletionPort((HANDLE)(SOCKET)m_socket, m_hIOCP, m_aid, 0);
	if (!h)
	{
		return AtlHresultFromLastError();
	}

	if (receiveDataLength > 0)
	{
		OnReceived(0, receiveDataLength);

		return S_OK;
	}
	else
	{
		return PostRecv();
	}
}

bool RpcServerConnection::OnIdle()
{
	if (!__super::OnIdle())
		return false;

	return WaitForSingleObject(m_hTermEvent, 0) != WAIT_OBJECT_0;
}

HRESULT RpcServerConnection::QueueSend(RpcCommand* pCmd)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcServerConnection::QueueSend, this : #%08x, aid : %I64u, cookie : %d, eventId : %d", this, m_aid, pCmd->m_cookie, pCmd->m_eventId);

	HRESULT hr = m_sendBox.queue.AddTail(pCmd);
	if (SUCCEEDED(hr))
	{
		if (!::PostQueuedCompletionStatus(m_hIOCP, 0, m_aid, SEND_OVERLAPPED))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcServerConnection::QueueSend, this : #%08x, aid : %I64u, #error = #%x", this, m_aid, GetLastError());
		}
	}

	return hr;
}

void RpcServerConnection::Close()
{
	Term();

	__super::Close();
}

void RpcServerConnection::Term()
{
	__super::Term();

	Lock();

	if (m_pServer)
	{
		::PostQueuedCompletionStatus(m_pServer->m_hIOCP, 0, m_aid, CLOSE_OVERLAPPED);

		m_pServer = nullptr;
	}

	Unlock();
}
