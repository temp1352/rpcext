#include "StdAfx.h"
#include "StubBufferMap.h"

#include "RpcExt.h"
#include "HashMap.h"
#include "InterfaceMap.h"

RpcStubBuffer::RpcStubBuffer()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::RpcStubBuffer");

	m_ref = 0;
	m_hash = 0;

	m_conected = false;

	m_pRawBuffer = nullptr;
	m_pUnkServer = nullptr;

	m_iid = IID_NULL;
	m_ipid = IID_NULL;

	m_pNext = nullptr;
}

RpcStubBuffer::~RpcStubBuffer()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::~RpcStubBuffer, this : 0x%08x, m_pRawBuffer = 0x%08x", this, m_pRawBuffer);

	ComApi::Release(&m_pUnkServer);
	
	if (m_pRawBuffer)
	{
		if (m_conected)
		{
			m_pRawBuffer->Disconnect();
		}

		m_pRawBuffer->Release();
	}
}

HRESULT RpcStubBuffer::CreateInstance(REFIID riid, IUnknown* pUnkServer, IRpcStubBuffer* pRawBuffer, IRpcStubBuffer** ppStubBuffer)
{
	MarshaledInterfaceList::_Node* pNode = _rpcExt.m_miList.Lookup(riid, pUnkServer);
	if (!pNode)
	{
		*ppStubBuffer = pRawBuffer;
		pRawBuffer->AddRef();

		return S_OK;
	}

	auto p = new RpcStubBuffer;
	if (!p)
		return E_OUTOFMEMORY;

	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		CStringA strItf;
		_interfaceMap.GetInterfaceName(riid, strItf);

		_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, "RpcStubBuffer::CreateInstance, interface : %s", strItf);
	}

	pNode->pBuffer = ComApi::AddRef(p);

	p->Lock();

	pRawBuffer->AddRef();
	pUnkServer->AddRef();

	p->m_pRawBuffer = pRawBuffer;
	p->m_pUnkServer = pUnkServer;

	p->m_iid = riid;
	p->m_conected = true;

	HRESULT hr = p->QueryInterface(IID_IRpcStubBuffer, (void**)ppStubBuffer);

	p->Unlock();

	return hr;
}

STDMETHODIMP RpcStubBuffer::QueryInterface(REFIID riid, void** ppv)
{
	if (!ppv)
		return E_POINTER;

	if (InlineIsEqualGUID(riid, __uuidof(RpcStubBuffer)))
	{
		*ppv = this;
		AddRef();

		return S_OK;
	}
	else if (InlineIsEqualGUID(riid, IID_IRpcStubBuffer) ||
			 InlineIsEqualGUID(riid, IID_IUnknown))
	{
		*ppv = static_cast<IRpcStubBuffer*>(this);
		AddRef();

		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) RpcStubBuffer::AddRef()
{
	ULONG ref = InterlockedIncrement(&m_ref);

	return ref;
}

STDMETHODIMP_(ULONG) RpcStubBuffer::Release()
{
	ULONG ref = _Release();

	if (ref == 1)
	{
		_rpcExt.m_stubBufMap.Remove(this);
	}

	return ref;
}

ULONG RpcStubBuffer::_Release()
{
	ULONG ref = InterlockedDecrement(&m_ref);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

STDMETHODIMP RpcStubBuffer::Connect(IUnknown* pUnkServer)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::Connect, this : %08x", this);

	if (!pUnkServer)
		return E_POINTER;

	Lock();

	HRESULT hr = E_FAIL;

	if (pUnkServer == m_pUnkServer)
	{
		hr = m_pRawBuffer->Connect(pUnkServer);
		if (SUCCEEDED(hr))
		{
			m_conected = true;
		}
	}
	else
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::Connect, this : %08x, pUnkServer != m_pUnkServer", this);
	}

	Unlock();

	return hr;
}

STDMETHODIMP_(void) RpcStubBuffer::Disconnect()
{
	Lock();

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::Disconnect, this : #%08x", this);

	m_pRawBuffer->Disconnect();

	m_conected = false;

	Unlock();
}

STDMETHODIMP RpcStubBuffer::Invoke(RPCOLEMESSAGE* prpcmsg, IRpcChannelBuffer* pRpcChannelBuffer)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::Invoke, this : #%08x", this);

	Lock();

	HRESULT hr = E_FAIL;

	if (!m_conected)
	{
		hr = m_pRawBuffer->Connect(m_pUnkServer);
		if (FAILED(hr))
		{
			Unlock();

			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcStubBuffer::Invoke, this : #%08x, conne", this);

			return hr;
		}

		m_conected = true;
	}

	hr = m_pRawBuffer->Invoke(prpcmsg, pRpcChannelBuffer);

	Unlock();

	return hr;
}

STDMETHODIMP_(IRpcStubBuffer*) RpcStubBuffer::IsIIDSupported(REFIID riid)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::IsIIDSupported");

	Lock();

	IRpcStubBuffer* pBuffer = m_pRawBuffer->IsIIDSupported(riid);

	Unlock();

	return pBuffer;
}

STDMETHODIMP_(ULONG) RpcStubBuffer::CountRefs()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::CountRefs");

	Lock();

	ULONG ref = m_pRawBuffer->CountRefs();

	Unlock();

	return ref;
}

STDMETHODIMP RpcStubBuffer::DebugServerQueryInterface(void** ppv)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::DebugServerQueryInterface");

	Lock();

	HRESULT hr = m_pRawBuffer->DebugServerQueryInterface(ppv);

	Unlock();

	return hr;
}

STDMETHODIMP_(void) RpcStubBuffer::DebugServerRelease(void* pv)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBuffer::DebugServerRelease");

	Lock();

	m_pRawBuffer->DebugServerRelease(pv);

	Unlock();
}

// RpcStubBufferMap

RpcStubBufferMap::RpcStubBufferMap()
{
	m_binCount = 0;
	m_ppBins = nullptr;
}

RpcStubBufferMap::~RpcStubBufferMap()
{
	_Clear();
}

HRESULT RpcStubBufferMap::Initialize()
{
	return S_OK;
}

void RpcStubBufferMap::Uninitialize()
{
	Clear();
}

HRESULT RpcStubBufferMap::Insert(RpcStubBuffer* pStubBuffer)
{
	if (!pStubBuffer)
		return E_POINTER;

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	HRESULT hr = Lookup(pStubBuffer->m_ipid, nullptr);
	if (hr == S_OK)
		return S_FALSE;

	if (!m_ppBins)
	{
		hr = InitBins();
		if (hr != S_OK)
			return hr;
	}

	pStubBuffer->m_hash = RpcHashMap::Hash(pStubBuffer->m_ipid);

	int binIdx = pStubBuffer->m_hash % m_binCount;

	pStubBuffer->m_pNext = m_ppBins[binIdx];
	m_ppBins[binIdx] = ComApi::AddRef(pStubBuffer);

	//_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBufferMap::Insert, hash : %d, binIdx : %d, ipid = ", pStubBuffer->m_hash, binIdx);
	//_logFile.WriteBinary(LogCategory::Misc, LogLevel::General, LogOption::Std, &pStubBuffer->m_ipid, sizeof(IPID));

	return S_OK;
}

HRESULT RpcStubBufferMap::Lookup(const IPID& ipid, RpcStubBuffer** ppStubBuffer)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins || !m_binCount)
		return S_FALSE;

	UINT hash = RpcHashMap::Hash(ipid);
	int binIdx = hash % m_binCount;

	//_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStubBufferMap::Lookup, hash : %d, binIdx : %d, ipid = ", hash, binIdx);
	//_logFile.WriteBinary(LogCategory::Misc, LogLevel::General, LogOption::Std, &ipid, sizeof(IPID));

	RpcStubBuffer* pNode = m_ppBins[binIdx];
	while (pNode)
	{
		if (pNode->m_hash == hash && InlineIsEqualGUID(pNode->m_ipid, ipid))
		{
			if (ppStubBuffer)
			{
				*ppStubBuffer = ComApi::AddRef(pNode);
			}

			return S_OK;
		}

		pNode = pNode->m_pNext;
	}

	return S_FALSE;
}

BOOL RpcStubBufferMap::Remove(RpcStubBuffer* pStubBuffer)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	if (!m_ppBins || !m_binCount)
		return FALSE;

	int binIdx = pStubBuffer->m_hash % m_binCount;
	;

	RpcStubBuffer* pNode = m_ppBins[binIdx];
	RpcStubBuffer* pPrev = nullptr;

	while (pNode)
	{
		if (pNode == pStubBuffer)
		{
			break;
		}

		pPrev = pNode;
		pNode = pNode->m_pNext;
	}

	if (pNode == nullptr)
		return FALSE;

	if (pPrev)
		pPrev->m_pNext = pNode->m_pNext;
	else
		m_ppBins[binIdx] = pNode->m_pNext;

	pStubBuffer->m_hash = 0;
	pStubBuffer->m_pNext = nullptr;
	pStubBuffer->Release();

	return TRUE;
}

HRESULT RpcStubBufferMap::InitBins()
{
	if (!m_ppBins)
	{
		ULONG binCount = RpcHashMap::BinSize(100);

		m_ppBins = (RpcStubBuffer**)malloc(binCount * sizeof(RpcStubBuffer*));
		if (!m_ppBins)
		{
			SetLastError(ERROR_OUTOFMEMORY);
			return E_OUTOFMEMORY;
		}

		memset(m_ppBins, 0, binCount * sizeof(RpcStubBuffer*));

		m_binCount = binCount;
	}

	return S_OK;
}

void RpcStubBufferMap::Clear()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	_Clear();
}

void RpcStubBufferMap::_Clear()
{
	if (m_ppBins)
	{
		for (ULONG i = 0; i < m_binCount; ++i)
		{
			RpcStubBuffer *pNode = m_ppBins[i], *pTemp;
			while (pNode)
			{
				pTemp = pNode;
				pNode = pNode->m_pNext;

				pTemp->m_hash = 0;
				pTemp->m_pNext = nullptr;
				pTemp->_Release();
			}
		}

		free(m_ppBins);
		m_ppBins = nullptr;

		m_binCount = 0;
	}
}