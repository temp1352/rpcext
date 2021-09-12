#include "stdafx.h"
#include "SecurityContext.h"

#include "Socket.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Secur32.lib")

CSecBuffer::CSecBuffer()
{
	cbBuffer = 0;
	BufferType = SECBUFFER_TOKEN;
	pvBuffer = nullptr;
}

CSecBuffer::~CSecBuffer()
{
	if (pvBuffer)
	{
		free(pvBuffer);
	}
}

HRESULT CSecBuffer::Allocate(ULONG nSize)
{
	BYTE* pBuff = (BYTE*)malloc(nSize);
	if (!pBuff)
		return E_OUTOFMEMORY;

	memset(pBuff, 0, nSize);

	if (pvBuffer)
	{
		free(pvBuffer);
	}

	cbBuffer = nSize;
	pvBuffer = pBuff;

	return S_OK;
}

void CSecBuffer::Free()
{
	if (pvBuffer)
	{
		free(pvBuffer);
		pvBuffer = nullptr;

		cbBuffer = 0;
	}
}

static char _secFlag[] = "secd";
const int _flagLen = 4;
const int _headerLen = _flagLen + sizeof(LONG);

HRESULT CSecBuffer::Send(Socket* pSocket)
{
	char buff[_headerLen];
	strcpy_s(buff, _secFlag);
	*(ULONG*)(buff + _flagLen) = cbBuffer;

	DWORD cbWritten = 0;
	if (!pSocket->Write((LPBYTE)buff, _headerLen, &cbWritten))
		return pSocket->GetHResult();

	DWORD cbSended = 0;

	while (cbSended < cbBuffer)
	{
		if (!pSocket->Write((LPBYTE)pvBuffer + cbSended, cbBuffer - cbSended, &cbWritten))
			return pSocket->GetHResult();

		cbSended += cbWritten;
	}

	return S_OK;
}

HRESULT CSecBuffer::Recv(Socket* pSocket)
{
	DWORD cbRead = 0;
	BYTE buffer[_headerLen];
	if (!pSocket->Read(buffer, _headerLen, &cbRead))
		return pSocket->GetHResult();

	if (strncmp((char*)buffer, _secFlag, _flagLen) != 0)
		return E_FAIL;

	ULONG length = *(ULONG*)(buffer + _flagLen);
	HRESULT hr = Allocate(length);
	if (FAILED(hr))
		return hr;

	length = 0;

	while (length < cbBuffer)
	{
		cbRead = 0;

		if (!pSocket->Read((LPBYTE)pvBuffer + length, cbBuffer - length, &cbRead))
			return pSocket->GetHResult();

		length += cbRead;
	}

	return S_OK;
}

// CSecPkgInfo

CSecPkgInfo::CSecPkgInfo()
{
	p = nullptr;
}

CSecPkgInfo::~CSecPkgInfo()
{
	if (p)
	{
		FreeContextBuffer(p);
	}
}

static WCHAR _szPackage[] = L"Negotiate";

HRESULT CSecPkgInfo::Query(LPCWSTR name)
{
	if (p)
	{
		FreeContextBuffer(p);
		p = nullptr;
	}

	if (!name)
	{
		name = _szPackage;
	}

	return QuerySecurityPackageInfo((LPWSTR)name, &p);
}

// Credentials

Credentials::Credentials()
{
	m_hCred.dwLower = m_hCred.dwUpper = 0;
	m_lifeTime.LowPart = m_lifeTime.HighPart = 0;
}

Credentials::~Credentials()
{
	FreeCredentialHandle(&m_hCred);
}

HRESULT Credentials::Acquire(ULONG fCredentialUse)
{
	HRESULT hr = AcquireCredentialsHandle(NULL, _szPackage, fCredentialUse,
		NULL, NULL, NULL, NULL, &m_hCred, &m_lifeTime);
	if (FAILED(hr))
		return hr;

	return 0;
}

void Credentials::Free()
{
	FreeCredentialHandle(&m_hCred);
	m_hCred.dwLower = m_hCred.dwUpper = 0;
}

// SecurityContext

SecurityContext::SecurityContext(Credentials* pCred)
{
	m_pCred = pCred;

	m_hContext.dwLower = m_hContext.dwUpper = 0;
	m_lifeTime.LowPart = m_lifeTime.HighPart = 0;
}

SecurityContext::~SecurityContext()
{
	DeleteSecurityContext(&m_hContext);
}

HRESULT SecurityContext::Initialize(LPCWSTR pszTarget, CSecBuffer* inBuff, CSecBuffer* outBuff)
{
	SecBufferDesc outBuffDesc;
	outBuffDesc.ulVersion = 0;
	outBuffDesc.cBuffers = 1;
	outBuffDesc.pBuffers = outBuff;

	HRESULT hr = S_OK;
	ULONG contextAttributes = 0;

	if (inBuff)
	{
		SecBufferDesc inBuffDesc;
		inBuffDesc.ulVersion = 0;
		inBuffDesc.cBuffers = 1;
		inBuffDesc.pBuffers = inBuff;

		hr = InitializeSecurityContext(&m_pCred->m_hCred, &m_hContext, (LPWSTR)pszTarget, ISC_REQ_CONFIDENTIALITY, 0, SECURITY_NATIVE_DREP,
			&inBuffDesc, 0, &m_hContext, &outBuffDesc, &contextAttributes, &m_lifeTime);
	}
	else
	{
		hr = InitializeSecurityContext(&m_pCred->m_hCred, nullptr, (LPWSTR)pszTarget, ISC_REQ_CONFIDENTIALITY, 0, SECURITY_NATIVE_DREP,
			NULL, 0, &m_hContext, &outBuffDesc, &contextAttributes, &m_lifeTime);
	}

	if ((hr == SEC_I_COMPLETE_NEEDED) || (hr == SEC_I_COMPLETE_AND_CONTINUE))
	{
		hr = CompleteAuthToken(&m_hContext, &outBuffDesc);
		if (FAILED(hr))
		{
			return hr;
		}
	}

	return ((hr == SEC_I_CONTINUE_NEEDED) || (hr == SEC_I_COMPLETE_AND_CONTINUE)) ? S_FALSE : S_OK;
}

HRESULT SecurityContext::Accept(BOOL fNewConversation, CSecBuffer* inBuff, CSecBuffer* outBuff)
{
	SecBufferDesc outBuffDesc;
	outBuffDesc.ulVersion = 0;
	outBuffDesc.cBuffers = 1;
	outBuffDesc.pBuffers = outBuff;

	SecBufferDesc inBuffDesc;
	inBuffDesc.ulVersion = 0;
	inBuffDesc.cBuffers = 1;
	inBuffDesc.pBuffers = inBuff;

	ULONG attribs = 0;

	HRESULT hr = AcceptSecurityContext(&m_pCred->m_hCred, fNewConversation ? nullptr : &m_hContext, &inBuffDesc, attribs,
		SECURITY_NATIVE_DREP, &m_hContext, &outBuffDesc, &attribs, &m_lifeTime);

	if (FAILED(hr))
		return hr;

	if ((hr == SEC_I_COMPLETE_NEEDED) || (hr == SEC_I_COMPLETE_AND_CONTINUE))
	{
		hr = CompleteAuthToken(&m_hContext, &outBuffDesc);
		if (FAILED(hr))
			return hr;
	}

	return ((hr == SEC_I_CONTINUE_NEEDED) || (hr == SEC_I_COMPLETE_AND_CONTINUE)) ? S_FALSE : S_OK;
}

void SecurityContext::Delete()
{
	DeleteSecurityContext(&m_hContext);
	m_hContext.dwLower = m_hContext.dwUpper = 0;
}

HRESULT SecurityContext::QueryAttributes(ULONG ulAttribute, void* pBuff)
{
	return QueryContextAttributes(&m_hContext, ulAttribute, pBuff);
}

HRESULT SecurityContext::Revert()
{
	return RevertSecurityContext(&m_hContext);
}

HRESULT SecurityContext::Impersonate()
{
	return ImpersonateSecurityContext(&m_hContext);
}

HRESULT SecurityContext::Encrypt(PBYTE pBuff, ULONG cbBuff, BYTE** ppOutput, ULONG* pcbOutput, ULONG cbSecurityTrailer)
{
	ULONG SigBufferSize = cbSecurityTrailer;

	*ppOutput = (PBYTE)malloc(SigBufferSize + cbBuff + sizeof(DWORD));

	SecBuffer secBuff[2];
	secBuff[0].cbBuffer = SigBufferSize;
	secBuff[0].BufferType = SECBUFFER_TOKEN;
	secBuff[0].pvBuffer = *ppOutput + sizeof(DWORD);

	secBuff[1].cbBuffer = cbBuff;
	secBuff[1].BufferType = SECBUFFER_DATA;
	secBuff[1].pvBuffer = pBuff;

	SecBufferDesc buffDesc;
	buffDesc.ulVersion = 0;
	buffDesc.cBuffers = 2;
	buffDesc.pBuffers = secBuff;

	ULONG ulQop = 0;
	HRESULT hr = EncryptMessage(&m_hContext, ulQop, &buffDesc, 0);
	if (SUCCEEDED(hr))
	{
		*((DWORD*)*ppOutput) = secBuff[0].cbBuffer;

		memcpy(*ppOutput + secBuff[0].cbBuffer + sizeof(DWORD), pBuff, cbBuff);

		*pcbOutput = cbBuff + secBuff[0].cbBuffer + sizeof(DWORD);
	}

	return hr;
}

LPBYTE SecurityContext::Decrypt(PBYTE pBuff, LPDWORD pcbBuff, ULONG cbSecurityTrailer)
{
	DWORD SigBufferSize = *((DWORD*)pBuff);
	PBYTE pSigBuffer = (LPBYTE)pBuff + sizeof(DWORD);
	PBYTE pDataBuffer = pSigBuffer + SigBufferSize;

	SecBuffer secBuff[2];
	secBuff[0].cbBuffer = SigBufferSize;
	secBuff[0].BufferType = SECBUFFER_TOKEN;
	secBuff[0].pvBuffer = pSigBuffer;

	secBuff[1].cbBuffer = *pcbBuff - SigBufferSize - sizeof(DWORD);
	secBuff[1].BufferType = SECBUFFER_DATA;
	secBuff[1].pvBuffer = pDataBuffer;

	SecBufferDesc buffDesc;
	buffDesc.ulVersion = 0;
	buffDesc.cBuffers = 2;
	buffDesc.pBuffers = secBuff;

	ULONG ulQop = 0;
	HRESULT hr = DecryptMessage(&m_hContext, &buffDesc, 0, &ulQop);
	if (FAILED(hr))
	{
		SetLastError(hr);

		return nullptr;
	}

	return pDataBuffer;
}

PBYTE SecurityContext::Verify(PBYTE pBuff, LPDWORD pcbBuff, ULONG cbMaxSignature)
{
	PBYTE pSigBuffer = pBuff;
	PBYTE pDataBuffer = pBuff + cbMaxSignature;

	*pcbBuff = *pcbBuff - (cbMaxSignature);

	SecBuffer secBuff[2];
	secBuff[0].cbBuffer = cbMaxSignature;
	secBuff[0].BufferType = SECBUFFER_TOKEN;
	secBuff[0].pvBuffer = pSigBuffer;

	secBuff[1].cbBuffer = *pcbBuff;
	secBuff[1].BufferType = SECBUFFER_DATA;
	secBuff[1].pvBuffer = pDataBuffer;

	SecBufferDesc buffDesc;
	buffDesc.ulVersion = 0;
	buffDesc.cBuffers = 2;
	buffDesc.pBuffers = secBuff;

	ULONG ulQop = 0;
	HRESULT hr = VerifySignature(&m_hContext, &buffDesc, 0, &ulQop);
	if (FAILED(hr))
	{
		SetLastError(hr);

		return nullptr;
	}

	return pDataBuffer;
}