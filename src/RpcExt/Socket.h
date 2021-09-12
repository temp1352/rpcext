#pragma once

#include <atlsocket.h>

#define ATL_WINSOCK_VER MAKELONG(2, 0)

// One of these objects can be created globally to turn
// on the socket stuff at CRT startup and shut it down
// on CRT term.
class SockInit
{
public:
	SockInit() throw()
	{
		m_dwErr = WSAEFAULT;
		Init();
	}

	bool Init()
	{
		if (!IsStarted())
		{
			m_dwErr = WSAStartup(ATL_WINSOCK_VER, &m_stData);
		}

		return m_dwErr == 0;
	}

	bool GetIPAddr(CString& strAddr)
	{
		in_addr addr;
		if (GetIPAddr(addr))
		{
			strAddr = inet_ntoa(addr);
			return true;
		}

		return false;
	}

	bool GetIPAddr(in_addr& addr)
	{
		char szName[MAX_PATH];
		if (::gethostname(szName, sizeof(szName)) == 0)
		{
			PHOSTENT hostinfo = ::gethostbyname(szName);
			if (hostinfo)
			{
				addr = *(in_addr*)hostinfo->h_addr_list[0];
				return true;
			}
		}

		return false;
	}

	bool IsStarted() { return m_dwErr == 0; }

	~SockInit() throw()
	{
		if (!m_dwErr)
		{
			WSACleanup();
		}
	}

	WSADATA m_stData;
	DWORD m_dwErr;
};

//#define _FD_CLOSE

class Socket
{
public:
	Socket() throw();
	~Socket() throw();

	operator SOCKET() throw() { return m_socket; }

	BOOL IsConnected() { return m_socket != INVALID_SOCKET; }

	bool Create(const ADDRINFOT* pAI, WORD wFlags = 0) throw();
	bool Create(int af, int st, int proto, WORD wFlags = 0) throw();
	bool Connect(LPCTSTR addr, unsigned short port, DWORD timeout = 0) throw();
	bool Connect(const SOCKADDR* psa, int len, DWORD timeout = 0) throw();
	bool Connect(const ADDRINFOT* pAI, DWORD timeout = 0) throw();

	void Close() throw();

	bool Accept(SOCKET socket);
	bool Accept2(SOCKET socket);
	bool Attach(SOCKET socket);

	WSAEVENT CreateEvent(long lNetworkEvents, BOOL bInitialState);

	bool IsReading();
	bool IsWriting();

#ifdef _FD_CLOSE

	bool IsClosed();

#endif

	bool Read(unsigned char* pBuff, DWORD dwSize, DWORD* pcbRead);
	bool Read(unsigned char* pBuff, DWORD* pdwSize, DWORD timeout = 0);

	bool Write(const unsigned char* pBufferIn, DWORD dwSize, DWORD* pcbWritten);
	bool Write(const unsigned char* pBuffIn, DWORD* pdwSize, DWORD timeout = 0);
	bool Write(WSABUF* pBuffers, int count, DWORD* pdwSize, DWORD timeout = 0);

	bool AsyncRead(unsigned char* pBuff, DWORD dwSize, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	bool AsyncRead(WSABUF* pBuffers, int count, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

	bool AsyncWrite(const unsigned char* pBuffIn, DWORD dwSize, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	bool AsyncWrite(WSABUF* pBuffers, int count, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

	bool WaitEventRead();
	void SetEventRead();
	bool _AsyncRead(WSABUF* pBuffers, int count, LPDWORD lpRecvd, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

	bool WaitEventWrite();
	void SetEventWrite();
	bool _AsyncWrite(WSABUF* pBuffers, int count, LPDWORD lpSent, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

	bool CancelIo(LPOVERLAPPED lpOverlapped);

	DWORD GetSocketTimeout() throw();
	DWORD SetSocketTimeout(DWORD dwNewTimeout) throw();

	DWORD GetLastError() { return WSAGetLastError(); }
	HRESULT GetHResult() { return HRESULT_FROM_WIN32(GetLastError()); }

public:
	SOCKET m_socket;

	DWORD m_dwSocketTimeout;

	WSAEVENT m_hEventConnect;
	WSAEVENT m_hEventRead;
	WSAEVENT m_hEventWrite;

#ifdef _FD_CLOSE

	WSAEVENT m_hEventClose;

#endif

	CComAutoCriticalSection m_cs;

	CComAutoCriticalSection m_csRead;
	CComAutoCriticalSection m_csWrite;
};

class ListenSocket
{
public:
	ListenSocket();
	~ListenSocket();

	operator SOCKET() throw() { return m_socket; }

	bool Create(UINT socketPort, bool createEvent);

	bool Bind(LPCTSTR addr, unsigned short port);
	bool Listen(int nConnectionBacklog = 30);
	void Close();

	BOOL AcceptEx(SOCKET sAcceptSocket, PVOID lpOutputBuffer, DWORD dwReceiveDataLength,
		PDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped);

	BOOL AcceptEx(SOCKET sAcceptSocket, PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength,
		DWORD dwRemoteAddressLength, LPDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped);

	static void GetAcceptExSockAddrs(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, SOCKADDR_IN** localSockaddr, SOCKADDR_IN** remoteSockaddr);

	static void GetAcceptExSockAddrs(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength,
		sockaddr** LocalSockaddr, LPINT LocalSockaddrLength, sockaddr** RemoteSockaddr, LPINT RemoteSockaddrLength);

	void ResetEvent() { WSAResetEvent(m_hEventAccept); }
	HANDLE GetAcceptEvent() { return m_hEventAccept; }

	DWORD GetLastError() { return m_dwLastError; }

protected:
	SOCKET m_socket;

	DWORD m_dwLastError;
	HANDLE m_hEventAccept;

	static LPFN_ACCEPTEX s_lpfnAcceptEx;
	static LPFN_GETACCEPTEXSOCKADDRS s_lpfnGetAcceptExSockAddrs;
};
