#include "StdAfx.h"
#include "StubManager.h"

#include "RpcExt.h"
#include "ThreadWindow.h"

RpcStub::RpcStub()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStub::RpcStub");

	m_pStubManager = nullptr;

	m_iid = IID_NULL;

	m_pStubBuffer = nullptr;

	m_pNext = nullptr;
}

RpcStub::~RpcStub()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStub::~RpcStub");

	if (m_pStubBuffer)
	{
		m_pStubBuffer->Release();
	}
}

HRESULT RpcStub::Init(StubManager* pStubManager, REFIID riid, IRpcStubBuffer* pBuffer)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStub::Init");

	m_pStubManager = pStubManager;

	m_iid = riid;

	pBuffer->AddRef();
	m_pStubBuffer = pBuffer;

	return S_OK;
}

void RpcStub::Close()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcStub::Close");

	if (m_pStubBuffer)
	{
		//m_pStubBuffer->Disconnect();

		m_pStubBuffer->Release();
		m_pStubBuffer = nullptr;
	}
}

// StubManager

StubManager::StubManager()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::StubManager");

	m_privateRef = 0;

	m_createThreadId = 0;
	m_aptType = APTTYPE_STA;

	m_pUnk = nullptr;

	m_pFirstStub = nullptr;
}

StubManager::~StubManager()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::~StubManager, this : 0x%08x", this);

	Close();
}

HRESULT StubManager::_CreateInstance(REFIID riid, void** ppv)
{
	CComObject<StubManager>* p = nullptr;
	HRESULT hr = CComObject<StubManager>::CreateInstance(&p);
	if (SUCCEEDED(hr))
	{
		p->AddRef();
		hr = p->QueryInterface(riid, ppv);
		p->Release();
	}

	return hr;
}

HRESULT __stdcall StubManager::_QueryInterface(void* pv, REFIID riid, LPVOID* ppv, DWORD_PTR dw)
{
	return E_NOINTERFACE;
}

HRESULT StubManager::Create(OID oid, IUnknown* pUnk, APTTYPE type, DWORD threadId)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Create, oid : %I64u", oid);

	Lock();

	m_oid = oid;

	m_aptType = type;
	m_createThreadId = threadId;

	m_pUnk = pUnk;

	Safe_AddRef();

	Unlock();

	return S_OK;
}

HRESULT StubManager::CreateStub(REFIID riid, IRpcStubBuffer* pStubBuffer)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::CreateStub");

	HRESULT hr = GetStub(riid, nullptr);
	if (hr != S_OK)
	{
		hr = _CreateStub(riid, pStubBuffer, nullptr);
	}

	return hr;
}

HRESULT StubManager::_CreateStub(REFIID riid, IRpcStubBuffer* pStubBuffer, RpcStub** ppStub)
{
	CComObject<RpcStub>* pStub = nullptr;
	HRESULT hr = CComObject<RpcStub>::CreateInstance(&pStub);
	if (FAILED(hr))
		return hr;

	pStub->AddRef();
	pStub->Init(this, riid, pStubBuffer);

	Lock();

	pStub->m_pNext = m_pFirstStub;
	m_pFirstStub = pStub;

	if (ppStub)
	{
		*ppStub = pStub;
		pStub->AddRef();
	}

	Unlock();

	return hr;
}

HRESULT StubManager::GetStub(REFIID riid, RpcStub** ppStub)
{
	HRESULT hr = S_FALSE;

	Lock();

	RpcStub* pNode = m_pFirstStub;
	while (pNode)
	{
		if (InlineIsEqualGUID(pNode->m_iid, riid))
		{
			if (ppStub)
			{
				*ppStub = pNode;
				pNode->AddRef();
			}

			hr = S_OK;

			break;
		}

		pNode = pNode->m_pNext;
	}

	Unlock();

	return hr;
}

void StubManager::Close()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Close, this : 0x%08x, m_pUnk = 0x%08x", this, m_pUnk);

	Lock();

	RpcStub* pNode = m_pFirstStub;
	while (pNode)
	{
		pNode->Close();

		RpcStub* pTemp = pNode;
		pNode = pNode->m_pNext;
		pTemp->Release();
	}

	m_pFirstStub = nullptr;

	if (m_pUnk)
	{
		HRESULT hr = CoDisconnectObject(m_pUnk, 0);

		DWORD ref = Safe_Release();

		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "CoDisconnectObject, Safe_Release, ref : %d", ref);

		m_pUnk = nullptr;
	}

	Unlock();
}

HRESULT StubManager::Invoke(RpcConnection* pConnection, RpcInvokeRequest* pCmd)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Invoke");

	CComPtr<RpcStub> spStub;
	HRESULT hr = GetStub(pCmd->m_iid, &spStub);
	if (hr != S_OK)
	{
		hr = _RemQueryInterface(pCmd->m_iid, &spStub);
		if (hr != S_OK)
		{
			_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Invoke, iid error");

			return E_IID_ERROR;
		}
	}

	CComObject<InvokeHandler>* pHandler = nullptr;
	hr = CComObject<InvokeHandler>::CreateInstance(&pHandler);
	if (FAILED(hr))
		return hr;

	pHandler->AddRef();

	hr = pHandler->Init(pConnection, this, spStub->m_pStubBuffer, pCmd);

	hr = _rpcExt.m_threadPool.Execute(pHandler);

	pHandler->Release();

	return hr;
}

HRESULT StubManager::RemQueryInterface(REFIID riid)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::RemQueryInterface");

	if (InlineIsEqualGUID(riid, IID_NULL))
		return E_INVALIDARG;

	if (InlineIsEqualGUID(riid, IID_IUnknown))
		return S_OK;

	HRESULT hr = GetStub(riid, nullptr);
	if (hr == S_OK)
		return hr;

	hr = _RemQueryInterface(riid, nullptr);

	return hr;
}

HRESULT StubManager::_RemQueryInterface(REFIID riid, RpcStub** ppStub)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::_RemQueryInterface");

	CComPtr<IUnknown> spUnk;
	HRESULT hr = Safe_QueryInterface(riid, (void**)&spUnk);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::_RemQueryInterface - Safe_QueryInterface, hr = #%x", hr);

		return hr;
	}

	ObjectLock lock(this);

	CComPtr<IRpcStubBuffer> spStubBuffer;
	hr = PsFactoryBuffer::_CreateStub(riid, m_pUnk, &spStubBuffer);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::_RemQueryInterface - _CreateSub, hr = #%x", hr);

		return hr;
	}

	hr = spStubBuffer->Connect(m_pUnk);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::_RemQueryInterface - Connect, hr = #%x", hr);

		return hr;
	}

	hr = _CreateStub(riid, spStubBuffer, ppStub);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::_RemQueryInterface - _CreateSub, hr = #%x", hr);
	}

	return hr;
}

HRESULT StubManager::Safe_QueryInterface(REFIID riid, void** ppv)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Safe_QueryInterface");

	ObjectLock lock(this);

	if (!m_pUnk)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_QueryInterface, m_pUnk = nullptr");

		return E_FAIL;
	}

	if (m_aptType == APTTYPE_MTA || m_createThreadId == GetCurrentThreadId())
	{
		HRESULT hr = m_pUnk->QueryInterface(riid, ppv);
		if (FAILED(hr))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_QueryInterface - m_pUnk->QueryInterface, hr = #%x", hr);
		}

		return hr;
	}

	CComPtr<RpcThreadInfo> spThreadInfo;
	HRESULT hr = _rpcExt.m_threadStg.GetThreadInfo(m_createThreadId, &spThreadInfo);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_QueryInterface - GetThreadInfo, hr = #%x", hr);

		return hr;
	}

	CAutoPtr<_QueryInterfaceParam> spParam(new _QueryInterfaceParam);
	if (!spParam)
		return E_OUTOFMEMORY;

	spParam->pUnk = m_pUnk;
	spParam->iid = riid;
	spParam->ppv = ppv;
	spParam->hr = E_PENDING;
	spParam->hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!spParam->hEvent)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_QueryInterface - CreateEvent failed");

		return AtlHresultFromLastError();
	}

	PostMessage(spThreadInfo->m_hWnd, RPC_WM_INVOKE, (WPARAM)Safe_OnQueryInterface, (LPARAM)spParam.m_p);
	DWORD wait = WaitForSingleObject(spParam->hEvent, INFINITE);

	CloseHandle(spParam->hEvent);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Safe_QueryInterface, wait : %d", wait);

	if (wait == WAIT_OBJECT_0)
	{
		return spParam->hr;
	}

	return E_UNEXPECTED;
}

HRESULT StubManager::Safe_OnQueryInterface(LPARAM lParam)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Safe_OnQueryInterface");

	auto pParam = (_QueryInterfaceParam*)lParam;
	if (pParam)
	{
		pParam->hr = pParam->pUnk->QueryInterface(pParam->iid, pParam->ppv);

		SetEvent(pParam->hEvent);
	}

	return E_UNEXPECTED;
}

ULONG StubManager::Safe_AddRef()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Safe_AddRef");

	ObjectLock lock(this);

	if (!m_pUnk)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_AddRef, m_pUnk = nullptr");

		return E_FAIL;
	}

	if (m_aptType == APTTYPE_MTA || m_createThreadId == GetCurrentThreadId())
	{
		HRESULT hr = m_pUnk->AddRef();
		if (FAILED(hr))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_AddRef, hr = #%x", hr);
		}

		return hr;
	}

	CComPtr<RpcThreadInfo> spThreadInfo;
	HRESULT hr = _rpcExt.m_threadStg.GetThreadInfo(m_createThreadId, &spThreadInfo);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_AddRef - GetThreadInfo, hr = #%x", hr);

		return hr;
	}

	CAutoPtr<_AddRefParam> spParam(new _AddRefParam);
	if (!spParam)
		return E_OUTOFMEMORY;

	spParam->pUnk = m_pUnk;
	spParam->ref = 0;
	spParam->hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!spParam->hEvent)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_AddRef - CreateEvent failed");

		return AtlHresultFromLastError();
	}

	PostMessage(spThreadInfo->m_hWnd, RPC_WM_INVOKE, (WPARAM)Safe_OnAddRef, (LPARAM)spParam.m_p);
	DWORD wait = WaitForSingleObject(spParam->hEvent, INFINITE);

	CloseHandle(spParam->hEvent);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Safe_AddRef, wait : %d", wait);

	if (wait == WAIT_OBJECT_0)
	{
		return spParam->ref;
	}

	return E_UNEXPECTED;
}

ULONG StubManager::Safe_OnAddRef(LPARAM lParam)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Safe_OnAddRef");

	auto pParam = (_AddRefParam*)lParam;
	if (pParam)
	{
		pParam->ref = pParam->pUnk->AddRef();

		SetEvent(pParam->hEvent);
	}

	return E_UNEXPECTED;
}

ULONG StubManager::Safe_Release()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Safe_Release");

	ObjectLock lock(this);

	if (!m_pUnk)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_Release, m_pUnk = nullptr");

		return E_FAIL;
	}

	if (m_aptType == APTTYPE_MTA || m_createThreadId == GetCurrentThreadId())
	{
		HRESULT hr = m_pUnk->Release();
		if (FAILED(hr))
		{
			_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_Release - m_pUnk->Release, hr = #%d", hr);
		}

		return hr;
	}

	CComPtr<RpcThreadInfo> spThreadInfo;
	HRESULT hr = _rpcExt.m_threadStg.GetThreadInfo(m_createThreadId, &spThreadInfo);
	if (hr != S_OK)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_Release - GetThreadInfo, hr = #%x", hr);

		return hr;
	}

	CAutoPtr<_AddRefParam> spParam(new _AddRefParam);
	if (!spParam)
		return E_OUTOFMEMORY;

	spParam->pUnk = m_pUnk;
	spParam->ref = 0;
	spParam->hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!spParam->hEvent)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::Error, LogOption::Std, "StubManager::Safe_Release - CreateEvent failed");

		return AtlHresultFromLastError();
	}

	PostMessage(spThreadInfo->m_hWnd, RPC_WM_INVOKE, (WPARAM)Safe_OnRelease, (LPARAM)spParam.m_p);
	DWORD wait = WaitForSingleObject(spParam->hEvent, INFINITE);

	CloseHandle(spParam->hEvent);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Safe_Release, wait : %d", wait);

	if (wait == WAIT_OBJECT_0)
	{
		return spParam->ref;
	}

	return E_UNEXPECTED;
}

ULONG StubManager::Safe_OnRelease(LPARAM lParam)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "StubManager::Safe_OnRelease");

	_AddRefParam* pParam = (_AddRefParam*)lParam;
	if (pParam)
	{
		pParam->ref = pParam->pUnk->Release();

		SetEvent(pParam->hEvent);
	}

	return E_UNEXPECTED;
}

STDMETHODIMP StubManager::QueryBlanket(DWORD* pAuthnSvc, DWORD* pAuthzSvc, OLECHAR** pServerPrincName,
	DWORD* pAuthnLevel, DWORD* pImpLevel, void** pPrivs, DWORD* pCapabilities)
{
	return E_NOTIMPL;
}

STDMETHODIMP StubManager::ImpersonateClient()
{
	return E_NOTIMPL;
}

STDMETHODIMP StubManager::RevertToSelf()
{
	return E_NOTIMPL;
}

STDMETHODIMP_(BOOL) StubManager::IsImpersonating()
{
	return FALSE;
}
