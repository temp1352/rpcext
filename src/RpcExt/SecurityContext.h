#pragma once

#include <Sspi.h>

enum
{
	SEC_BUF_SIZE = 12000,
};

class Socket;

class CSecBuffer : public SecBuffer
{
public:
	CSecBuffer();
	~CSecBuffer();

	HRESULT Allocate(ULONG size = SEC_BUF_SIZE);
	void Free();

	HRESULT Send(Socket* pSocket);
	HRESULT Recv(Socket* pSocket);
};

class CSecPkgInfo
{
public:
	CSecPkgInfo();
	~CSecPkgInfo();

	HRESULT Query(LPCWSTR name);

	operator SecPkgInfo*() const throw()
	{
		return p;
	}

	SecPkgInfo& operator*() const
	{
		return *p;
	}

	PSecPkgInfo operator->()
	{
		return p;
	}

	PSecPkgInfo p;
};

class Credentials
{
public:
	Credentials();
	~Credentials();

	HRESULT Acquire(ULONG fCredentialUse);
	void Free();

public:
	CredHandle m_hCred;

	TimeStamp m_lifeTime;
};

class SecurityContext
{
public:
	SecurityContext(Credentials* pCred);
	~SecurityContext();

	HRESULT Initialize(LPCWSTR target, CSecBuffer* inBuff, CSecBuffer* outBuff);
	HRESULT Accept(BOOL fNewConversation, CSecBuffer* inBuff, CSecBuffer* outBuff);

	void Delete();

	HRESULT QueryAttributes(ULONG ulAttribute, void* pBuffer);
	HRESULT Revert();

	HRESULT Impersonate();

	HRESULT Encrypt(PBYTE pBuff, ULONG cbBuff, BYTE** ppOutput, ULONG* pcbOutput, ULONG cbSecurityTrailer);
	LPBYTE Decrypt(PBYTE pBuff, LPDWORD pcbBuff, ULONG cbSecurityTrailer);

	PBYTE Verify(PBYTE pBuffer, LPDWORD pcbBuff, ULONG cbMaxSignature);

protected:
	Credentials* m_pCred;

	_SecHandle m_hContext;

	TimeStamp m_lifeTime;
};