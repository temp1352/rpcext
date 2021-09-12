#include "StdAfx.h"
#include "CallContext.h"

#include "RpcExt.h"

#include "AppContextMap.h"
#include "StubBufferMap.h"

RpcCallContext::RpcCallContext(RpcConnection* pConnection)
{
	ATLASSERT(pConnection);

	m_init = false;

	m_pConnection = pConnection;
	m_pAccessToken = nullptr;

	m_pHeadNode = nullptr;
}

RpcCallContext::~RpcCallContext(void)
{
	ComApi::Release(&m_pAccessToken);
}

STDMETHODIMP RpcCallContext::QueryInterface(REFIID riid, void** ppv)
{
	if (!ppv)
		return E_POINTER;

	if (InlineIsEqualGUID(riid, __uuidof(IRpcCallContext)))
	{
		*ppv = static_cast<IRpcCallContext*>(this);
		AddRef();

		return S_OK;
	}
	else if (InlineIsEqualGUID(riid, IID_IUnknown))
	{
		*ppv = static_cast<IUnknown*>(this);
		AddRef();

		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) RpcCallContext::AddRef()
{
	return m_pConnection->AddRef();
}

STDMETHODIMP_(ULONG) RpcCallContext::Release()
{
	return m_pConnection->Release();
}

HRESULT RpcCallContext::Initialize()
{
	HRESULT hr = m_cs.Init();
	if (hr != S_OK)
		return hr;

	m_init = true;

	return hr;
}

void RpcCallContext::Uninitialize()
{
	if (!m_init)
		return;

	m_init = false;

	RemoveAll();

	m_cs.Term();
}

HRESULT RpcCallContext::Connect(REFCLSID rclsid, REFIID riid, OID oid, DWORD flags, SC_AUTH_INFO* pInfo, OID* poid, BSTR* pRefreshToken)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::Connect");

	CComPtr<ClassObject> spClassObject;
	HRESULT hr = _rpcExt.m_classObjectMap.Lookup(rclsid, &spClassObject);
	if (hr != S_OK)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcCallContext::Connect - m_classObjectMap.Lookup failed");
		return E_FAIL;
	}

	if (spClassObject->m_usage & RE_CU_AUTHORIZATION)
	{
		hr = Logon(spClassObject->GetContext(), flags, pInfo, pRefreshToken);
		if (FAILED(hr))
			return hr;
	}

	CComPtr<StubManager> spStubManager;

	if (oid == 0 || _rpcExt.m_stubMgrList.Lookup(oid, &spStubManager) != S_OK)
	{
		CComPtr<IUnknown> spUnkServer;
		hr = CreateInstance(spClassObject, riid, &spUnkServer, poid);
		if (hr != S_OK)
			return hr;

		hr = _rpcExt.m_stubMgrList.CreateStubManager(*poid, spUnkServer, spClassObject->m_aptType, GetCurrentThreadId(), &spStubManager);
		if (hr != S_OK)
			return hr;
	}

	hr = Add(spStubManager);
	if (FAILED(hr))
		return hr;

	hr = spStubManager->RemQueryInterface(riid);

	return hr;
}

HRESULT RpcCallContext::Logon(AppContext* pContext, DWORD flags, SC_AUTH_INFO* pInfo, BSTR* pRefreshToken)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, L"RpcCallContext::Logon - aid : %I64u, user : %ws", m_pConnection->m_aid, pInfo->userName);

	if (!pContext)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcCallContext::Logon - !pContext");
		return E_FAIL;
	}

	ATLASSERT(!m_pAccessToken);
	if (m_pAccessToken)
		return S_OK;

	HRESULT hr = 0;

	if (flags & RPC_BF_INTEGRATED_SECURITY)
	{
		CHandle token;
		hr = m_pConnection->AcceptAuth(&token.m_h);
		if (hr != S_OK)
			return E_ACCESSDENIED;

		hr = pContext->AccessCheck(token, &m_pAccessToken);
		if (SUCCEEDED(hr))
		{
			token.Detach();
		}
	}
	else
	{
		SC_AUTH_RESP resp;
		memset(&resp, 0, sizeof(resp));
		hr = pContext->Logon(pInfo, &resp);
		if (SUCCEEDED(hr))
		{
			*pRefreshToken = resp.refreshToken;
			m_pAccessToken = resp.pToken;
		}
	
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, L"RpcCallContext::Login - pContext->Logon, sid : %ws, clsid : %ws, domain : %ws, authCode : %ws, refreshToken : %ws, userName : %ws, refreshToken : %ws, pToken : #%08x", GuidString(pInfo->sid).m_str, GuidString(pInfo->clsid).m_str, pInfo->domain, pInfo->authCode, pInfo->refreshToken, pInfo->userName, resp.refreshToken, resp.pToken);
	}

	return hr;
}

STDMETHODIMP RpcCallContext::Authentication(HANDLE* phToken)
{
	if (!phToken)
		return E_POINTER;

	HRESULT hr = m_pConnection->AuthRequest();
	if (FAILED(hr))
		return hr;

	m_pConnection->SyncMode();

	hr = m_pConnection->AcceptAuth(phToken);

	m_pConnection->AsyncMode();
	m_pConnection->PostRecv();

	return hr;
}

extern HRESULT _Test(OXID* pOxid);

HRESULT RpcCallContext::CreateInstance(ClassObject* pClassObject, REFIID riid, IUnknown** ppUnk, OID* poid)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::CreateInstance");

	CComPtr<IUnknown> spUnkServer;
	HRESULT hr = pClassObject->CreateInstance(IID_IUnknown, (void**)&spUnkServer);
	if (FAILED(hr))
		return hr;

	/*StdObjRef objRef;
	hr = ObjectResolver::Resolve(riid, spUnkServer, &objRef);
	*poid = objRef.m_std.oid;*/

	*poid = (OID)spUnkServer.p;
	*ppUnk = spUnkServer.Detach();

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::CreateInstance, oid : %I64u", *poid);

	return S_OK;
}

HRESULT RpcCallContext::GetAccessToken(IScAccessToken** ppToken)
{
	if (!ppToken)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::GetAccessToken - E_POINTER");
		return E_POINTER;
	}

	*ppToken = ComApi::AddRef(m_pAccessToken);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::GetAccessToken - aid : %I64u, token : #%08x", m_pConnection->m_aid, *ppToken);

	return *ppToken ? S_OK : S_FALSE;
}

HRESULT RpcCallContext::ProcessInterfacePointers(RpcMessage* pMsg, LONG* pItfPtrCount)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::ProcessInterfacePointers");

	*pItfPtrCount = 0;

	StdObjList objList;
	HRESULT hr = pMsg->GetObjList(&objList);
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::ProcessInterfacePointers - GetObjList, hr :#%08x", hr);
		return hr;
	}

	POSITION pos = objList.GetHeadPosition();
	while (pos)
	{
		LPBYTE pStart = nullptr;
		StdObjRef* pObjRef = objList.GetNext(pos, pStart);

		CComPtr<RpcStubBuffer> spStubBuffer;
		hr = _rpcExt.m_stubBufMap.Lookup(pObjRef->m_std.ipid, &spStubBuffer);
		if (hr != S_OK)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcCallContext::ProcessInterfacePointers, m_stubBufMap.Lookup failed, 检查MarchalInterface的接口指针是否正确，有可能是接口方法定义时缺少iid_is(riid), hr : %x, ipid = ", hr);
			_logFile.WriteBinary(LogCategory::Misc, LogLevel::General, LogOption::Std, &pObjRef->m_std.ipid, sizeof(IPID));

			return hr;
		}

		hr = CreateStub(spStubBuffer->GetServer(), spStubBuffer, pObjRef, pStart);
		if (FAILED(hr))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::ProcessInterfacePointers - CreateSub, hr : #%08x", hr);
			return hr;
		}
	}

	*pItfPtrCount = objList.GetCount();

	return S_OK;
}

HRESULT RpcCallContext::CreateStub(IUnknown* pUnkServer, IRpcStubBuffer* pStubBuffer, StdObjRef* pObjRef, LPBYTE pStart)
{
	OID oid = (OID)pUnkServer; //pObjRef->m_std.oid;

	CComPtr<StubManager> spStubManager;
	HRESULT hr = GetStubManager(oid, &spStubManager);
	if (hr != S_OK)
	{
		CComPtr<RpcThreadInfo> spThreadInfo;
		hr = _rpcExt.m_threadStg.GetThreadInfo(GetCurrentThreadId(), &spThreadInfo);
		if (hr != S_OK)
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcCallContext::CreateStub - GetThreadInfo, hr : %x", hr);
			return hr;
		}

		hr = _rpcExt.m_stubMgrList.CreateStubManager(oid, pUnkServer, spThreadInfo->m_aptType, spThreadInfo->m_threadId, &spStubManager);
		if (FAILED(hr))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcCallContext::CreateStub - CreateStubManager, hr : %x", hr);
			return hr;
		}

		hr = Add(spStubManager);
		if (FAILED(hr))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcCallContext::CreateStub - Add, hr : %x", hr);
			return hr;
		}
	}

	hr = spStubManager->CreateStub(pObjRef->m_iid, pStubBuffer);
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcCallContext::CreateStub - CreateStub, hr : %x, iid : %s", hr, GuidStringA(pObjRef->m_iid).m_str);
		return hr;
	}

	ExtObjRef extObjRef(pObjRef);
	auto& connInfo = extObjRef.m_connInfo;
	connInfo.m_cid = m_pConnection->m_cid;
	connInfo.m_sid = m_pConnection->m_sid;
	connInfo.m_oid = oid;

	extObjRef.Write(pStart);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::CreateStub - hr : #%08x, aid : %I64u, sid : %s, oid : %I64u", hr, m_pConnection->m_aid, GuidStringA(connInfo.m_sid).m_str, connInfo.m_oid);

	return hr;
}

HRESULT RpcCallContext::Invoke(RpcInvokeRequest* pCmd)
{
	CComPtr<StubManager> spStubManager;
	HRESULT hr = GetStubManager(pCmd->m_oid, &spStubManager);
	if (hr == S_OK)
	{
		hr = spStubManager->Invoke(m_pConnection, pCmd);
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "RpcCallContext::Invoke, hr : %x", hr);
	}
	else
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcCallContext::Invoke, GetStubManager failed, aid : %I64u, sid : %s, oid : %I64d", m_pConnection->m_aid, GuidStringA(m_pConnection->m_sid).m_str, pCmd->m_oid);
	}

	if (FAILED(hr))
	{
		CComPtr<RpcInvokeResp> spRespCmd(new CComObject<RpcInvokeResp>);
		spRespCmd->m_cookie = pCmd->m_cookie;
		spRespCmd->m_eventId = pCmd->m_eventId;
		spRespCmd->m_hRes = hr;
		spRespCmd->m_iid = pCmd->m_iid;
		spRespCmd->CacheError();

		m_pConnection->QueueSend(spRespCmd);
	
		pCmd->SetExpire(-1);
	}

	return hr;
}

HRESULT RpcCallContext::QueryInterface(RpcQIRequest* pCmd)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::QueryInterface - oid : %I64u", pCmd->m_oid);

	CComPtr<StubManager> spStubManager;
	HRESULT hr = GetStubManager(pCmd->m_oid, &spStubManager);
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcCallContext::QueryInterface - GetSubManager, hr : %x", hr);
	}
	else
	{
		hr = spStubManager->RemQueryInterface(pCmd->m_iid);
		if (FAILED(hr))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcCallContext::QueryInterface - RemQueryInterface, hr : %x", hr);
		}
	}

	m_cs.Lock();

	CComPtr<RpcQIResp> spRespCmd(new CComObject<RpcQIResp>);
	spRespCmd->m_cookie = pCmd->m_cookie;
	spRespCmd->m_eventId = pCmd->m_eventId;
	spRespCmd->m_hRes = hr;
	m_pConnection->QueueSend(spRespCmd);

	m_cs.Unlock();

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::QueryInterface end, hr : %x", hr);

	return S_OK;
}

HRESULT RpcCallContext::Free(RpcFreeRequest* pCmd)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::Free - oid : %I64u", pCmd->m_oid);

	CComPtr<RpcFreeResp> spRespCmd(new CComObject<RpcFreeResp>);
	if (!spRespCmd)
		return E_OUTOFMEMORY;

	spRespCmd->m_cookie = pCmd->m_cookie;
	spRespCmd->m_eventId = pCmd->m_eventId;

	m_cs.Lock();
	m_pConnection->QueueSend(spRespCmd);
	m_cs.Unlock();

	CComPtr<StubManager> spStubManager;
	HRESULT hr = GetStubManager(pCmd->m_oid, &spStubManager);
	if (hr == S_OK)
	{
		Remove(spStubManager);

		_rpcExt.m_stubMgrList.ReleaseStubManager(spStubManager);
	}
	
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::Free end, hr : %x", hr);

	return hr;
}

HRESULT RpcCallContext::GetStubManager(OID oid, StubManager** ppStubManager)
{
	ATLASSERT(ppStubManager);

	m_cs.Lock();

	auto pNode = m_pHeadNode;
	while (pNode)
	{
		if (pNode->pStubManager->m_oid == oid)
		{
			*ppStubManager = pNode->pStubManager;
			(*ppStubManager)->AddRef();

			m_cs.Unlock();

			return S_OK;
		}

		pNode = pNode->pNext;
	}

	m_cs.Unlock();

	return E_OID_ERROR;
}

HRESULT RpcCallContext::Add(StubManager* pStubManager)
{
	ATLASSERT(pStubManager);

	auto pNode = new _StubMgrNode;
	if (!pNode)
		return E_OUTOFMEMORY;

	pStubManager->AddRef();

	pNode->pStubManager = pStubManager;

	m_cs.Lock();

	pNode->pNext = m_pHeadNode;
	m_pHeadNode = pNode;

	m_pConnection->AddPrivateRef();

	m_cs.Unlock();

	return S_OK;
}

void RpcCallContext::Remove(StubManager* pStubManager)
{
	m_cs.Lock();

	_StubMgrNode* pPrev = nullptr;
	_StubMgrNode* pNode = m_pHeadNode;

	while (pNode)
	{
		if (pNode->pStubManager == pStubManager)
			break;

		pPrev = pNode;
		pNode = pNode->pNext;
	}

	if (pPrev)
		pPrev->pNext = pNode->pNext;
	else
		m_pHeadNode = pNode->pNext;

	pNode->pNext = nullptr;
	pNode->pStubManager->Release();

	delete pNode;

	m_pConnection->ReleasePrivateRef();

	m_cs.Unlock();
}

void RpcCallContext::RemoveAll()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::RemoveAll");

	m_cs.Lock();

	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::RemoveAll, Locked");

	LONG ref = 0;

	_StubMgrNode* pNode = m_pHeadNode;
	while (pNode)
	{
		_StubMgrNode* pTemp = pNode;

		pNode = pNode->pNext;

		pTemp->pStubManager->Release();
		delete pTemp;

		_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::RemoveAll, delete");

		++ref;
	}

	m_pHeadNode = nullptr;

	m_cs.Unlock();

	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::RemoveAll, ReleasePrivateRef");
	if (ref != 0)
	{
		m_pConnection->ReleasePrivateRef(ref);
	}

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcCallContext::RemoveAll - end, ref : %d", ref);
}
