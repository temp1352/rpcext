#include "StdAfx.h"
#include "ErrorInfo.h"

#include "RpcBuffer.h"

ErrorInfo::ErrorInfo()
{
	m_guid = GUID_NULL;
	m_context = 0;
}

ErrorInfo::~ErrorInfo()
{
}

HRESULT ErrorInfo::_CreateInstance(RpcBuffer* pBuffer, REFIID riid, void** ppv)
{
	CComObject<ErrorInfo>* p = nullptr;
	HRESULT hr = CComObject<ErrorInfo>::CreateInstance(&p);
	if (SUCCEEDED(hr))
	{
		p->AddRef();

		hr = p->Load(pBuffer);
		if (SUCCEEDED(hr))
		{
			hr = p->QueryInterface(riid, ppv);
		}

		p->Release();
	}

	return hr;
}

HRESULT ErrorInfo::Load(RpcBuffer* pBuffer)
{
	pBuffer->ReadGUID(&m_guid);
	m_context = pBuffer->ReadInt32();
	pBuffer->ReadBSTR(&m_source);
	pBuffer->ReadBSTR(&m_description);
	pBuffer->ReadBSTR(&m_helpFile);

	return 0;
}

STDMETHODIMP ErrorInfo::GetGUID(GUID* pGUID)
{
	if (!pGUID)
		return E_POINTER;

	ObjectLock lock(this);

	*pGUID = m_guid;

	return S_OK;
}

STDMETHODIMP ErrorInfo::GetSource(BSTR* pBstrSource)
{
	if (!pBstrSource)
		return E_POINTER;

	ObjectLock lock(this);

	return m_source.CopyTo(pBstrSource);
}

STDMETHODIMP ErrorInfo::GetDescription(BSTR* pBstrDescription)
{
	if (!pBstrDescription)
		return E_POINTER;

	ObjectLock lock(this);

	return m_description.CopyTo(pBstrDescription);
}

STDMETHODIMP ErrorInfo::GetHelpFile(BSTR* pBstrHelpFile)
{
	if (!pBstrHelpFile)
		return E_POINTER;

	ObjectLock lock(this);

	return m_helpFile.CopyTo(pBstrHelpFile);
}

STDMETHODIMP ErrorInfo::GetHelpContext(DWORD* pdwHelpContext)
{
	if (!pdwHelpContext)
		return E_POINTER;

	ObjectLock lock(this);

	*pdwHelpContext = m_context;

	return S_OK;
}

ErrorInfoBuffer::ErrorInfoBuffer()
{
	m_pInfo = nullptr;
}

ErrorInfoBuffer::~ErrorInfoBuffer()
{
	ComApi::Release(&m_pInfo);
}

void ErrorInfoBuffer::Set(IErrorInfo* pInfo)
{
	m_cs.Lock();

	ComApi::Assign(&m_pInfo, pInfo);

	m_cs.Unlock();
}

void ErrorInfoBuffer::Report()
{
	m_cs.Lock();

	if (m_pInfo)
	{
		SetErrorInfo(0, m_pInfo);
	}

	m_cs.Unlock();
}
