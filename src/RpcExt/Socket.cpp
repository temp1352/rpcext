#include "stdafx.h"
#include "Socket.h"

#ifndef _NO_GLOBAL_SOCKET_STARTUP
static SockInit _sockInit;
#endif

Socket::Socket()
{
	m_socket = INVALID_SOCKET;

	m_hEventConnect = nullptr;
	m_hEventRead = nullptr;
	m_hEventWrite = nullptr;

#ifdef _FD_CLOSE

	m_hEventClose = nullptr;

#endif

	m_dwSocketTimeout = 10000;
}

Socket::~Socket()
{
	Close();
}

void Socket::Close()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs, true);

	if (m_hEventConnect)
	{
		WSACloseEvent(m_hEventConnect);
		m_hEventConnect = nullptr;
	}

	if (m_hEventRead)
	{
		WSACloseEvent(m_hEventRead);
		m_hEventRead = nullptr;
	}

	if (m_hEventWrite)
	{
		WSACloseEvent(m_hEventWrite);
		m_hEventWrite = nullptr;
	}

#ifdef _FD_CLOSE

	if (m_hEventClose)
	{
		WSACloseEvent(m_hEventClose);
		m_hEventClose = nullptr;
	}

#endif

	if (m_socket != INVALID_SOCKET)
	{
		BOOL bDontLinger = FALSE;
		setsockopt(m_socket, SOL_SOCKET, SO_DONTLINGER, (LPCSTR)&bDontLinger, sizeof(BOOL));
		closesocket(m_socket);

		m_socket = INVALID_SOCKET;
	}
}

bool Socket::Create(const ADDRINFOT* pAI, WORD wFlags)
{
	return Create(pAI->ai_family, pAI->ai_socktype, pAI->ai_protocol, wFlags);
}

bool Socket::Create(int af, int st, int proto, WORD wFlags)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs, true);

	if (m_socket != INVALID_SOCKET)
	{
		WSASetLastError(WSAEALREADY);

		return false; // Must close this socket first
	}

	m_socket = WSASocket(af, st, proto, nullptr, 0, wFlags | WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	m_hEventRead = CreateEvent(FD_READ, TRUE);
	if (!m_hEventRead)
	{
		return false;
	}

	m_hEventWrite = CreateEvent(FD_WRITE, TRUE);
	if (!m_hEventWrite)
	{
		return false;
	}

	m_hEventConnect = CreateEvent(FD_CONNECT, FALSE);
	if (!m_hEventConnect)
	{
		return false;
	}

#ifdef _FD_CLOSE

	m_hEventClose = CreateEvent(FD_CLOSE, FALSE);
	if (!m_hEventClose)
	{
		return false;
	}

#endif

	return true;
}

bool Socket::Connect(LPCTSTR addr, unsigned short port, DWORD timeout)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs, true);

	if (m_socket != INVALID_SOCKET)
		return true;

	CSocketAddr address;
	int ret = address.FindAddr(addr, port, 0, PF_UNSPEC, SOCK_STREAM, 0);
	if (ret != ERROR_SUCCESS)
	{
		SetLastError(ret);
		return false;
	}

	bool bRet = true;
	ADDRINFOT* pAI = address.GetAddrInfoList();
	while (pAI)
	{
		bRet = Connect(pAI, timeout);
		if (bRet)
			break;

		if (m_socket)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		pAI = pAI->ai_next;
	}

	return bRet;
}

bool Socket::Connect(const ADDRINFOT* pAI, DWORD timeout)
{
	if (m_socket == INVALID_SOCKET && !Create(pAI))
		return false;

	return Connect((SOCKADDR*)pAI->ai_addr, (int)pAI->ai_addrlen, timeout);
}

bool Socket::Connect(const SOCKADDR* psa, int len, DWORD timeout)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs, true);

	if (m_hEventConnect == nullptr)
		return false;

	if (timeout == 0)
		timeout = m_dwSocketTimeout;

	bool bRet = true;

	// if you try to connect the socket without
	// creating it first it's reasonable to automatically
	// try the create for you.
	if (m_socket == INVALID_SOCKET)
		return false;

	if (WSAConnect(m_socket, psa, len, nullptr, nullptr, nullptr, nullptr))
	{
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError != WSAEWOULDBLOCK)
		{
			bRet = false;
		}
		else
		{
			dwLastError = WaitForSingleObject(m_hEventConnect, timeout);
			if (dwLastError == WAIT_OBJECT_0)
			{
				// make sure there were no connection errors.
				WSANETWORKEVENTS wse;
				ZeroMemory(&wse, sizeof(wse));
				WSAEnumNetworkEvents(m_socket, nullptr, &wse);
				if (wse.iErrorCode[FD_CONNECT_BIT] != 0)
				{
					WSASetLastError(wse.iErrorCode[FD_CONNECT_BIT]);
					bRet = false;
				}
			}
			else
			{
				bRet = false;
			}
		}
	}

	return bRet;
}

bool Socket::Accept(SOCKET socket)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs, true);

	if (m_socket != INVALID_SOCKET)
		return false;

	m_socket = accept(socket, nullptr, nullptr);
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	m_hEventRead = CreateEvent(FD_READ, TRUE);
	m_hEventWrite = CreateEvent(FD_WRITE, TRUE);

	if (!m_hEventRead || !m_hEventWrite)
	{
		return false;
	}

#ifdef _FD_CLOSE

	m_hEventClose = CreateEvent(FD_CLOSE, FALSE);
	if (!m_hEventClose)
	{
		m_dwLastError = ::GetLastError();
		return false;
	}

#endif

	return true;
}

bool Socket::Accept2(SOCKET socket)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs, true);

	if (m_socket != INVALID_SOCKET)
		return false;

	m_socket = socket;
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	m_hEventRead = CreateEvent(FD_READ, TRUE);
	m_hEventWrite = CreateEvent(FD_WRITE, TRUE);

	if (!m_hEventRead || !m_hEventWrite)
	{
		return false;
	}

#ifdef _FD_CLOSE

	m_hEventClose = CreateEvent(FD_CLOSE, FALSE);
	if (!m_hEventClose)
	{
		return false;
	}

#endif

	return true;
}

bool Socket::Attach(SOCKET socket)
{
	if (m_socket != INVALID_SOCKET)
		return false;

	m_socket = socket;

	m_hEventConnect = CreateEvent(FD_CONNECT, TRUE);
	if (m_hEventConnect == nullptr)
		return false;

	m_hEventRead = CreateEvent(FD_READ, TRUE);
	m_hEventWrite = CreateEvent(FD_WRITE, TRUE);

	if (!m_hEventRead || !m_hEventWrite)
	{
		return false;
	}

#ifdef _FD_CLOSE

	m_hEventClose = CreateEvent(FD_CLOSE, FALSE);
	if (!m_hEventClose)
	{
		return false;
	}

#endif

	return true;
}

WSAEVENT Socket::CreateEvent(long lNetworkEvents, BOOL bInitialState)
{
	WSAEVENT hEvent = WSACreateEvent();
	if (!hEvent)
		return nullptr;

	if (bInitialState)
	{
		if (!WSASetEvent(hEvent))
		{
			WSACloseEvent(hEvent);
			return nullptr;
		}
	}

	if (WSAEventSelect(m_socket, hEvent, lNetworkEvents) == SOCKET_ERROR)
	{
		WSACloseEvent(hEvent);
		return nullptr;
	}

	return hEvent;
}

bool Socket::IsReading()
{
	return WaitForSingleObject(m_hEventRead, 0) == WAIT_ABANDONED;
}

bool Socket::IsWriting()
{
	return WaitForSingleObject(m_hEventWrite, 0) == WAIT_TIMEOUT;
}

#ifdef _FD_CLOSE

bool Socket::IsClosed()
{
	if (WaitForSingleObject(m_hEventClose, 0) == WAIT_OBJECT_0)
	{
		WSANETWORKEVENTS wse;
		ZeroMemory(&wse, sizeof(wse));
		WSAEnumNetworkEvents(m_socket, m_hEventClose, &wse);

		if ((wse.lNetworkEvents & FD_CLOSE) && wse.iErrorCode[FD_CLOSE_BIT] == 0)
		{
			return true;
		}
	}

	return false;
}

#endif

bool Socket::Read(unsigned char* pBuff, DWORD dwSize, DWORD* pcbRead)
{
	if (Read(pBuff, &dwSize))
	{
		if (pcbRead)
			*pcbRead = dwSize;

		return true;
	}

	return false;
}

bool Socket::Read(unsigned char* pBuff, DWORD* pdwSize, DWORD timeout)
{
	if (timeout == 0)
		timeout = m_dwSocketTimeout;

	if (WaitForSingleObject(m_hEventRead, 0) == WAIT_ABANDONED)
	{
		WSASetLastError(WSAEINPROGRESS);

		return false; // another read on is blocking this socket
	}

	bool bRet = true;
	WSABUF buff;
	buff.buf = (char*)pBuff;
	buff.len = *pdwSize;

	*pdwSize = 0;

	DWORD dwFlags = 0;

	// protect against re-entrency
	m_csRead.Lock();

	WSAOVERLAPPED o;
	ZeroMemory(&o, sizeof(o));
	o.hEvent = m_hEventRead;
	WSAResetEvent(o.hEvent);

	WSASetLastError(0);

	if (WSARecv(m_socket, &buff, 1, pdwSize, &dwFlags, &o, 0))
	{
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError != WSA_IO_PENDING)
		{
			bRet = false;
		}
	}

	// wait for the read to complete
	if (bRet)
	{
		DWORD dwWait = WaitForSingleObject(o.hEvent, timeout);
		if (dwWait == WAIT_OBJECT_0)
		{
			dwFlags = 0;
			bRet = WSAGetOverlappedResult(m_socket, &o, pdwSize, FALSE, &dwFlags);
		}
		else if (dwWait == WAIT_TIMEOUT)
		{
			CancelIo(&o);

			WSASetLastError(ERROR_TIMEOUT);

			bRet = false;
		}
		else
		{
			CancelIo(&o);

			bRet = false;
		}
	}

	m_csRead.Unlock();

	return bRet;
}

bool Socket::Write(const unsigned char* pBuffIn, DWORD dwSize, DWORD* pcbWritten)
{
	if (Write(pBuffIn, &dwSize))
	{
		if (pcbWritten)
			*pcbWritten = dwSize;

		return true;
	}

	return false;
}

bool Socket::Write(const unsigned char* pBuffIn, DWORD* pdwSize, DWORD timeout)
{
	WSABUF buff;
	buff.buf = (char*)pBuffIn;
	buff.len = *pdwSize;

	return Write(&buff, 1, pdwSize, timeout);
}

bool Socket::Write(WSABUF* pBuffers, int count, DWORD* pdwSize, DWORD timeout)
{
	*pdwSize = 0;

	if (timeout == 0)
		timeout = m_dwSocketTimeout;

	// make sure we aren't already writing
	if (WaitForSingleObject(m_hEventWrite, 0) == WAIT_TIMEOUT)
	{
		WSASetLastError(WSAEINPROGRESS);

		return false; // another write on is blocking this socket
	}

	bool bRet = true;

	m_csWrite.Lock();

	WSAOVERLAPPED o;
	ZeroMemory(&o, sizeof(o));
	o.hEvent = m_hEventWrite;
	WSAResetEvent(o.hEvent);

	WSASetLastError(0);

	if (WSASend(m_socket, pBuffers, count, pdwSize, 0, &o, 0))
	{
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError != WSA_IO_PENDING)
		{
			bRet = false;
		}
	}

	// wait for write to complete
	if (bRet)
	{
		DWORD dwWait = WaitForSingleObject(m_hEventWrite, timeout);
		if (dwWait == WAIT_OBJECT_0)
		{
			DWORD dwFlags = 0;
			bRet = WSAGetOverlappedResult(m_socket, &o, pdwSize, FALSE, &dwFlags);
		}
		else if (dwWait == WAIT_TIMEOUT)
		{
			CancelIo(&o);
			WSASetLastError(ERROR_TIMEOUT);

			bRet = false;
		}
		else
		{
			CancelIo(&o);

			bRet = false;
		}
	}

	m_csWrite.Unlock();

	return bRet;
}

bool Socket::AsyncRead(unsigned char* pBuff, DWORD dwSize, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	WSABUF buff;
	buff.buf = (char*)pBuff;
	buff.len = dwSize;

	return AsyncRead(&buff, 1, lpOverlapped, lpCompletionRoutine);
}

bool Socket::AsyncRead(WSABUF* pBuffers, int count, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	// protect against re-entrency
	m_csRead.Lock();

	if (WaitForSingleObject(m_hEventRead, 0) == WAIT_ABANDONED)
	{
		WSASetLastError(WSAEINPROGRESS);

		m_csRead.Unlock();

		return false; // another write on is blocking this socket
	}

	bool bRet = true;
	DWORD dwSize = 0;
	DWORD dwFlags = 0;

	ZeroMemory(lpOverlapped, sizeof(WSAOVERLAPPED));
	lpOverlapped->hEvent = m_hEventRead;
	WSAResetEvent(lpOverlapped->hEvent);

	WSASetLastError(0);

	if (WSARecv(m_socket, pBuffers, count, &dwSize, &dwFlags, lpOverlapped, lpCompletionRoutine))
	{
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError != WSA_IO_PENDING)
		{
			WSASetEvent(lpOverlapped->hEvent);

			bRet = false;
		}
	}

	m_csRead.Unlock();

	return bRet;
}

bool Socket::AsyncWrite(const unsigned char* pBuffIn, DWORD dwSize, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	WSABUF buff;
	buff.buf = (char*)pBuffIn;
	buff.len = dwSize;

	return AsyncWrite(&buff, 1, lpOverlapped, lpCompletionRoutine);
}

bool Socket::AsyncWrite(WSABUF* pBuffers, int count, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	m_csWrite.Lock();

	// make sure we aren't already writing
	if (WaitForSingleObject(m_hEventWrite, 0) == WAIT_TIMEOUT)
	{
		m_csWrite.Unlock();

		WSASetLastError(WSAEINPROGRESS);

		return false; // another write on is blocking this socket
	}

	bool bRet = true;
	DWORD dwSize = 0;

	ZeroMemory(lpOverlapped, sizeof(WSAOVERLAPPED));
	lpOverlapped->hEvent = m_hEventWrite;
	WSAResetEvent(lpOverlapped->hEvent);

	WSASetLastError(0);

	if (WSASend(m_socket, pBuffers, count, &dwSize, 0, lpOverlapped, lpCompletionRoutine))
	{
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError != WSA_IO_PENDING)
		{
			WSASetEvent(lpOverlapped->hEvent);

			bRet = false;
		}
	}

	m_csWrite.Unlock();

	return bRet;
}

bool Socket::WaitEventRead()
{
	bool bRet = false;

	m_csRead.Lock();

	if (WaitForSingleObject(m_hEventRead, 0) != WAIT_ABANDONED)
	{
		WSAResetEvent(m_hEventRead);

		bRet = true;
	}

	m_csRead.Unlock();

	return bRet;
}

void Socket::SetEventRead()
{
	m_csRead.Lock();

	WSASetEvent(m_hEventRead);

	m_csRead.Unlock();
}

bool Socket::_AsyncRead(WSABUF* pBuffers, int count, LPDWORD lpRecvd, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	bool bRet = true;
	DWORD dwFlags = 0;

	WSASetLastError(0);

	if (WSARecv(m_socket, pBuffers, count, lpRecvd, &dwFlags, lpOverlapped, lpCompletionRoutine))
	{
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError != WSA_IO_PENDING)
		{
			WSASetEvent(m_hEventRead);

			bRet = false;
		}
	}

	return bRet;
}

bool Socket::WaitEventWrite()
{
	bool bRet = false;

	m_csWrite.Lock();

	if (WaitForSingleObject(m_hEventWrite, 0) != WAIT_TIMEOUT)
	{
		WSAResetEvent(m_hEventWrite);

		bRet = true;
	}

	m_csWrite.Unlock();

	return bRet;
}

void Socket::SetEventWrite()
{
	m_csWrite.Lock();

	WSASetEvent(m_hEventWrite);

	m_csWrite.Unlock();
}

bool Socket::_AsyncWrite(WSABUF* pBuffers, int count, LPDWORD lpSent, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	bool bRet = true;

	WSASetLastError(0);

	if (WSASend(m_socket, pBuffers, count, lpSent, 0, lpOverlapped, lpCompletionRoutine))
	{
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError != WSA_IO_PENDING)
		{
			WSASetEvent(m_hEventWrite);

			bRet = false;
		}
	}

	return bRet;
}

typedef BOOL(WINAPI* _CancelIoExProc)(HANDLE hFile, LPOVERLAPPED lpOverlapped);

_CancelIoExProc _GetCancelIoExProc()
{
	HINSTANCE hInst = LoadLibrary(L"Kernel32.dll");
	if (hInst)
		return (_CancelIoExProc)GetProcAddress(hInst, "CancelIoEx");

	return nullptr;
}

bool Socket::CancelIo(LPOVERLAPPED lpOverlapped)
{
	static _CancelIoExProc proc = _GetCancelIoExProc();

	return proc != nullptr ? proc((HANDLE)m_socket, lpOverlapped) : ::CancelIo((HANDLE)m_socket);
}

DWORD Socket::GetSocketTimeout() throw()
{
	return m_dwSocketTimeout;
}

DWORD Socket::SetSocketTimeout(DWORD dwNewTimeout) throw()
{
	DWORD dwOldTimeout = m_dwSocketTimeout;
	m_dwSocketTimeout = dwNewTimeout;
	return dwOldTimeout;
}

// CListSocket

ListenSocket::ListenSocket()
{
	m_socket = INVALID_SOCKET;
	m_dwLastError = 0;

	m_hEventAccept = nullptr;
}

LPFN_ACCEPTEX ListenSocket::s_lpfnAcceptEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS ListenSocket::s_lpfnGetAcceptExSockAddrs = nullptr;

ListenSocket::~ListenSocket()
{
	Close();
}

bool ListenSocket::Create(UINT socketPort, bool createEvent)
{
	m_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		m_dwLastError = WSAGetLastError();
		return false;
	}

	if (!Bind(nullptr, socketPort))
	{
		m_dwLastError = WSAGetLastError();
		Close();

		return false;
	}

	if (createEvent)
	{
		m_hEventAccept = WSACreateEvent();
		if (!m_hEventAccept)
			return false;

		if (WSAEventSelect(m_socket, m_hEventAccept, FD_ACCEPT) == SOCKET_ERROR)
		{
			return false;
		}
	}

	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
	DWORD dwBytes = 0;

	if (!s_lpfnAcceptEx)
	{
		if (WSAIoctl(m_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx,
				sizeof(GuidAcceptEx), &s_lpfnAcceptEx, sizeof(s_lpfnAcceptEx), &dwBytes, nullptr, nullptr) == SOCKET_ERROR)
		{
			m_dwLastError = WSAGetLastError();

			return false;
		}
	}

	if (!s_lpfnGetAcceptExSockAddrs)
	{
		if (WSAIoctl(m_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidGetAcceptExSockAddrs,
				sizeof(GuidGetAcceptExSockAddrs), &s_lpfnGetAcceptExSockAddrs, sizeof(s_lpfnGetAcceptExSockAddrs), &dwBytes, nullptr, nullptr) == SOCKET_ERROR)
		{
			m_dwLastError = WSAGetLastError();

			return false;
		}
	}

	return true;
}

bool ListenSocket::Bind(LPCTSTR addr, unsigned short port)
{
	//CSocketAddr address;
	//// Find address information
	//if ((m_dwLastError = address.FindAddr(addr, port, 0, PF_UNSPEC, SOCK_STREAM, 0)) != ERROR_SUCCESS)
	//{
	//	return false;
	//}

	//ADDRINFOT *pAI = address.GetAddrInfo();
	//if(bind(m_socket, (SOCKADDR*)pAI->ai_addr, (int)pAI->ai_addrlen) == SOCKET_ERROR)
	//{
	//	m_dwLastError = WSAGetLastError();
	//	return false;
	//}

	//return true;

	USES_CONVERSION_EX;

	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	LPSTR ascii;
	if (addr != nullptr)
	{
		ascii = T2A_EX((LPTSTR)addr, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
		if (ascii == nullptr)
		{
			// OUT OF MEMORY
			WSASetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return FALSE;
		}
	}
	else
	{
		ascii = nullptr;
	}

	sockAddr.sin_family = AF_INET;

	if (ascii == nullptr)
	{
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		DWORD result = inet_addr(ascii);
		if (result == INADDR_NONE)
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
		sockAddr.sin_addr.s_addr = result;
	}

	sockAddr.sin_port = htons((u_short)port);

	return bind(m_socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;
}

bool ListenSocket::Listen(int nConnectionBacklog)
{
	if (listen(m_socket, nConnectionBacklog) == SOCKET_ERROR)
	{
		m_dwLastError = WSAGetLastError();

		return false;
	}

	return true;
}

void ListenSocket::Close()
{
	if (m_hEventAccept)
	{
		WSACloseEvent(m_hEventAccept);
		m_hEventAccept = nullptr;
	}

	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

BOOL ListenSocket::AcceptEx(SOCKET sAcceptSocket, PVOID lpOutputBuffer, DWORD dwReceiveDataLength,
	PDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped)
{
	return AcceptEx(sAcceptSocket, lpOutputBuffer, dwReceiveDataLength - ((sizeof(SOCKADDR_IN) + 16) * 2),
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, lpdwBytesReceived, lpOverlapped);
}

BOOL ListenSocket::AcceptEx(SOCKET sAcceptSocket, PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength,
	DWORD dwRemoteAddressLength, LPDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped)
{
	return s_lpfnAcceptEx ? s_lpfnAcceptEx(m_socket, sAcceptSocket, lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength,
								dwRemoteAddressLength, lpdwBytesReceived, lpOverlapped)
						  : FALSE;
}

void ListenSocket::GetAcceptExSockAddrs(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, SOCKADDR_IN** localSockaddr, SOCKADDR_IN** remoteSockaddr)
{
	int localLen = sizeof(SOCKADDR_IN), remoteLen = sizeof(SOCKADDR_IN);

	GetAcceptExSockAddrs(lpOutputBuffer, dwReceiveDataLength - ((sizeof(SOCKADDR_IN) + 16) * 2),
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, (LPSOCKADDR*)localSockaddr, &localLen, (LPSOCKADDR*)remoteSockaddr, &remoteLen);
}

void ListenSocket::GetAcceptExSockAddrs(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength,
	sockaddr** LocalSockaddr, LPINT LocalSockaddrLength, sockaddr** RemoteSockaddr, LPINT RemoteSockaddrLength)
{
	if (s_lpfnGetAcceptExSockAddrs)
	{
		s_lpfnGetAcceptExSockAddrs(lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength,
			LocalSockaddr, LocalSockaddrLength, RemoteSockaddr, RemoteSockaddrLength);
	}
}