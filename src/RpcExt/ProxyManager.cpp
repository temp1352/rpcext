#include "StdAfx.h"
#include "ProxyManager.h"

#include "RpcExt.h"
#include "Marshal.h"
#include "InterfaceMap.h"

// {c3fcc19e-a970-11d2-8b5a-00a0c9b7c9c4}
static const IID IID_IManagedObject = { 0xc3fcc19e, 0xa970, 0x11d2, { 0x8b, 0x5a, 0x00, 0xa0, 0xc9, 0xb7, 0xc9, 0xc4 } };

// {0000001b-0000-0000-C000-000000000046}
static const IID IID_IdentityUnmarshal = { 0x0000001b, 0x0000, 0x0000, { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };

// {334D391F-0E79-3B15-C9FF-EAC65DD07C42}
static const IID IID_IMarshalUnknown = { 0x334D391F, 0x0E79, 0x3B15, { 0xC9, 0xFF, 0xEA, 0xC6, 0x5D, 0xD0, 0x7C, 0x42 } };

// {94EA2B94-E9CC-49E0-C0FF-EE64CA8F5B90}
static const IID IID_IMarshalUnknown2 = { 0x94EA2B94, 0xE9CC, 0x49E0, { 0xC0, 0xFF, 0xEE, 0x64, 0xCA, 0x8F, 0x5B, 0x90 } };

//#define PROXY_CHECK_MEM

#ifdef PROXY_CHECK_MEM
static _CrtMemState state;
#endif

// ProxyManager

ProxyManager::ProxyManager()
{
#ifdef PROXY_CHECK_MEM

	_CrtMemCheckpoint(&state);

#endif

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManager::ProxyManager, this : 0x%08x", this);

	m_cid = GUID_NULL;
	m_sid = GUID_NULL;
	m_oid = 0;
	m_port = 0;
	memset(m_hostName, 0, sizeof(m_hostName));

	m_pFirstChannel = nullptr;

	m_pConnection = nullptr;
}

ProxyManager::~ProxyManager()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManager::~ProxyManager, this : 0x%08x", this);

#ifdef PROXY_CHECK_MEM

	_CrtMemDumpAllObjectsSince(&state);

#endif
}

HRESULT ProxyManager::_CreateInstance(REFIID riid, void** ppv)
{
	CComObject<ProxyManager>* p = nullptr;
	HRESULT hr = CComObject<ProxyManager>::CreateInstance(&p);
	if (SUCCEEDED(hr))
	{
		p->AddRef();

		_rpcExt.m_proxyMgrList.Add(p);

		hr = p->QueryInterface(riid, ppv);

		p->Release();
	}

	return hr;
}

ULONG ProxyManager::_Release()
{
	return __super::InternalRelease();
}

ULONG ProxyManager::InternalRelease()
{
	ULONG ref = __super::InternalRelease();
	if (ref == 1)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManager::InternalRelease, ref = 1");

		_rpcExt.m_proxyMgrList.Remove(this);
	}
	else if (ref == 0)
	{
		Close();
	}

	return ref;
}

HRESULT __stdcall ProxyManager::_QueryInterface(void* pv, REFIID riid, LPVOID* ppv, DWORD_PTR dw)
{
	auto pThis = (ProxyManager*)pv;

	HRESULT hr = pThis->LookupProxy(riid, ppv);
	if (hr == S_OK)
		return hr;

	if (InlineIsEqualGUID(riid, IID_IManagedObject))
		return E_NOINTERFACE;

	if (InlineIsEqualGUID(riid, IID_IMarshal) || InlineIsEqualGUID(riid, IID_IMarshal2) || InlineIsEqualGUID(riid, IID_IStdMarshalInfo) || InlineIsEqualGUID(riid, IID_INoMarshal) || InlineIsEqualGUID(riid, IID_IFastRundown) || InlineIsEqualGUID(riid, IID_IdentityUnmarshal) || InlineIsEqualGUID(riid, IID_IMarshalUnknown) || InlineIsEqualGUID(riid, IID_IMarshalUnknown2))
		return E_NOINTERFACE;

	pThis->Lock();

	hr = pThis->m_pConnection->RemQueryInterface(pThis->m_oid, riid);
	if (hr == S_OK)
	{
		hr = pThis->CreateProxy(riid, ppv);
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	pThis->Unlock();

	return hr;
}

HRESULT ProxyManager::Create(REFIID riid, ConnectInfo* pConnInfo)
{
	ATLASSERT(pConnInfo);

	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		CStringA itfName;
		_interfaceMap.GetInterfaceName(riid, itfName);
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManager::Create, interface : %s, oid : %I64u", itfName, pConnInfo->m_oid);
	}

	ObjectLock lock(this);

	ATLASSERT(!m_pConnection);
	if (m_pConnection)
		return S_OK;

	m_cid = pConnInfo->m_cid;
	m_sid = pConnInfo->m_sid;
	m_oid = pConnInfo->m_oid;
	m_port = pConnInfo->m_port;
	strcpy_s(m_hostName, _countof(m_hostName), pConnInfo->m_hostName);

	HRESULT hr = _rpcExt.m_connectionMap.Lookup(m_cid, (RpcClientConnection**)&m_pConnection);
	_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, L"ProxyManager::Create -  _rpcExt.m_proxyMgrList.GetConnection");
	if (hr != S_OK)
	{
		hr = _rpcExt.m_rpcServer.GetConnection(pConnInfo->m_cid, &m_pConnection);
	}

	if (hr == S_OK)
	{
		m_pConnection->AddPrivateRef();
		return hr;
	}

	RpcClientConnection* pConnection = nullptr;
	hr = RpcClientConnection::_CreateInstance(static_cast<IRpcConnectionSink*>(this), &pConnection);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, L"ProxyManager::Create - RpcClientConnection::_CreateInstance");
		return hr;
	}

	pConnection->AddPrivateRef();
	m_pConnection = pConnection;

	USES_CONVERSION;

	RE_CONNECTINFO connInfo;
	memset(&connInfo, 0, sizeof(RE_CONNECTINFO));
	connInfo.options = 0;
	connInfo.port = pConnInfo->m_port;
	connInfo.serverName = A2W(pConnInfo->m_hostName);

	hr = pConnection->Create(pConnInfo->m_oid, riid, &connInfo);
	if (FAILED(hr) && hr != E_PENDING)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, L"ProxyManager::Create - pConnection->Create");
		return hr;
	}

	return S_OK;
}

HRESULT ProxyManager::Connect(REFCLSID rclsid, RE_CONNECTINFO* pConnInfo, REFIID riid, void** ppv)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManager::Connect");

	Lock();
	if (m_pConnection)
	{
		Unlock();
		return S_FALSE;
	}

	RpcClientConnection* pConnection = nullptr;
	HRESULT hr = RpcClientConnection::_CreateInstance(static_cast<IRpcConnectionSink*>(this), &pConnection);
	if (FAILED(hr))
	{
		Unlock();
		return hr;
	}

	pConnection->AddPrivateRef();
	m_pConnection = pConnection;

	Unlock();

	hr = pConnection->Create(rclsid, riid, pConnInfo);
	if (FAILED(hr) && hr != E_PENDING)
		return hr;

	if (InlineIsEqualGUID(riid, IID_IUnknown))
		hr = QueryInterface(riid, ppv);
	else
		hr = CreateProxy(riid, ppv);

	if (FAILED(hr))
		return hr;

	Lock();
	m_port = pConnInfo->port;
	Unlock();

	return hr;
}

STDMETHODIMP ProxyManager::GetSessionId(LPWSTR buff, ULONG size)
{
	int len = GuidUtil::ToString(m_sid, GuidFormat::NoBrace, buff, size);

	return S_OK;
}

STDMETHODIMP ProxyManager::GetConnection(REFIID riid, void** ppv)
{
	ObjectLock lock(this);

	return m_pConnection ? m_pConnection->QueryInterface(riid, ppv) : E_NOINTERFACE;
}

STDMETHODIMP ProxyManager::OnConnected(RPC_CONNECT_RESP* pResp)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManager::OnBinding - oid : %I64u", pResp->oid);

	Lock();

	m_cid = pResp->cid;
	m_sid = pResp->sid;
	m_oid = pResp->oid;

	Unlock();

	return S_OK;
}

STDMETHODIMP ProxyManager::OnDisconnected()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManager::OnDisconnected - cid : %s, sid : %s, oid : %I64u", GuidStringA(m_cid).m_str, GuidStringA(m_sid).m_str, m_oid);

	return S_OK;
}

HRESULT ProxyManager::GetProxy(REFIID riid, void** ppv)
{
	ObjectLock lock(this);

	HRESULT hr = LookupProxy(riid, ppv);
	if (hr == S_OK)
		return hr;

	hr = CreateProxy(riid, ppv);

	return hr;
}

HRESULT ProxyManager::CreateProxy(REFIID riid, void** ppv)
{
	ObjectLock lock(this);

	CComPtr<IRpcProxyBuffer> spProxyBuffer;
	HRESULT hr = PsFactoryBuffer::_CreateProxy(_GetRawUnknown(), riid, &spProxyBuffer, ppv);
	if (hr != S_OK)
		return hr;

	CComObject<RpcChannelBuffer>* pChannelBuffer = nullptr;
	hr = CComObject<RpcChannelBuffer>::CreateInstance(&pChannelBuffer);
	if (FAILED(hr))
		return hr;

	pChannelBuffer->AddRef();

	hr = pChannelBuffer->Connect(riid, this, m_pConnection, spProxyBuffer);
	if (FAILED(hr))
	{
		pChannelBuffer->Release();
		return hr;
	}

	pChannelBuffer->m_pNext = m_pFirstChannel;
	m_pFirstChannel = pChannelBuffer;

	return hr;
}

HRESULT ProxyManager::LookupProxy(REFIID riid, void** ppv)
{
	ObjectLock lock(this);

	RpcChannelBuffer* pNode = m_pFirstChannel;
	while (pNode)
	{
		if (InlineIsEqualGUID(riid, pNode->m_iid))
		{
			return pNode->m_pProxyBuffer->QueryInterface(riid, ppv);
		}

		pNode = pNode->m_pNext;
	}

	return E_NOINTERFACE;
}

void ProxyManager::Close()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "ProxyManager::Close");

	Lock();

	RpcChannelBuffer* pNode = m_pFirstChannel;
	while (pNode)
	{
		RpcChannelBuffer* pTemp = pNode;

		pNode = pNode->m_pNext;

		pTemp->Disconnect();
		pTemp->Release();
	}

	m_pFirstChannel = nullptr;

	if (m_pConnection)
	{
		m_pConnection->Unadvise(this);
		m_pConnection->ReleasePrivateRef();
		m_pConnection->FreeObject(m_oid);
		m_pConnection->Release();
		m_pConnection = nullptr;
	}

	Unlock();
}

STDMETHODIMP ProxyManager::GetUnmarshalClass(REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags, CLSID* pCid)
{
	*pCid = __uuidof(MarshalClassFactory);

	return S_OK;
}

STDMETHODIMP ProxyManager::GetMarshalSizeMax(REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags, DWORD* pSize)
{
	if (!pSize)
		return E_POINTER;

	*pSize = sizeof(ConnectInfo);

	return 0;
}

STDMETHODIMP ProxyManager::MarshalInterface(IStream* pStm, REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags)
{
	if (!pStm)
		return E_POINTER;

	ConnectInfo connInfo;
	connInfo.m_cid = m_cid;
	connInfo.m_sid = m_sid;
	connInfo.m_oid = m_oid;
	connInfo.m_port = m_port;
	strcpy_s(connInfo.m_hostName, _countof(connInfo.m_hostName), m_hostName);

	HRESULT hr = connInfo.Write(pStm);

	return hr;
}

STDMETHODIMP ProxyManager::UnmarshalInterface(IStream* pStm, REFIID riid, void** ppv)
{
	return 0;
}

STDMETHODIMP ProxyManager::ReleaseMarshalData(IStream* pStm)
{
	return 0;
}

STDMETHODIMP ProxyManager::DisconnectObject(DWORD dwReserved)
{
	AtlTrace("ProxyManager::DisconnectObject\n");

	return 0;
}

STDMETHODIMP ProxyManager::QueryBlanket(IUnknown* pProxy, DWORD* pAuthnSvc, DWORD* pAuthzSvc, OLECHAR** pServerPrincName,
	DWORD* pAuthnLevel, DWORD* pImpLevel, void** pAuthInfo, DWORD* pCapabilites)
{
	return E_NOTIMPL;
}

STDMETHODIMP ProxyManager::SetBlanket(IUnknown* pProxy, DWORD dwAuthnSvc, DWORD dwAuthzSvc, OLECHAR* pServerPrincName,
	DWORD dwAuthnLevel, DWORD dwImpLevel, void* pAuthInfo, DWORD dwCapabilities)
{
	return E_NOTIMPL;
}

STDMETHODIMP ProxyManager::CopyProxy(IUnknown* pProxy, IUnknown** ppCopy)
{
	return E_NOTIMPL;
}

#if _MSC_VER < 1600

STDMETHODIMP ProxyManager::Set(IUnknown* pPrx, DWORD dwProperty, ULONG_PTR dwValue)
{
	return E_NOTIMPL;
}

STDMETHODIMP ProxyManager::Query(IUnknown* pPrx, DWORD dwProperty, ULONG_PTR* pdwValue)
{
	return E_NOTIMPL;
}

#else

STDMETHODIMP ProxyManager::Set(IUnknown* pPrx, RPCOPT_PROPERTIES dwProperty, ULONG_PTR dwValue)
{
	return E_NOTIMPL;
}

STDMETHODIMP ProxyManager::Query(IUnknown* pPrx, RPCOPT_PROPERTIES dwProperty, ULONG_PTR* pdwValue)
{
	return E_NOTIMPL;
}

#endif