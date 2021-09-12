#include "StdAfx.h"
#include "RpcExt.h"

#include "Apartment.h"
#include "Marshal.h"
#include "GetClassObjectDelegate.h"
#include "InterfaceMarshal.h"
#include "InterfaceMap.h"

RpcExt::RpcExt()
{
	m_initCount = 0;
}

RpcExt::~RpcExt()
{
}

HRESULT RpcExt::Initialize(DWORD coInit)
{
	HRESULT hr = 0;

	if (InterlockedIncrement(&m_initCount) == 1)
	{
		hr = _Initialize();
		if (FAILED(hr))
			return hr;
	}

	switch (coInit)
	{
	case COINIT_MULTITHREADED:
		hr = m_threadStg.InitThreadInfo(APTTYPE_MTA);
		break;

	case COINIT_APARTMENTTHREADED:
		hr = m_threadStg.InitThreadInfo(APTTYPE_STA);
		break;

	default:

		APTTYPE apttype = Apartment::GetType();
		hr = m_threadStg.InitThreadInfo(apttype);
	}

	return hr;
}

void RpcExt::Uninitialize()
{
	m_threadStg.UninitThreadInfo();

	if (InterlockedDecrement(&m_initCount) == 0)
	{
		_Uninitialize();
	}
}

#define RPC_EXT_CHECK_MEM

#ifdef RPC_EXT_CHECK_MEM
static _CrtMemState state;
#endif

HRESULT RpcExt::_Initialize()
{
#ifdef RPC_EXT_CHECK_MEM

	_CrtMemCheckpoint(&state);

#endif

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	HRESULT hr = m_procRepl.Initialize();
	if (FAILED(hr))
		return hr;

	hr = GetClassObjectDelegate::Initialize();

	hr = InterfaceMarshal::Initialize();

	hr = MarshalClassFactory::RegisterClassObject();
	if (FAILED(hr))
		return hr;

	hr = m_threadStg.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_rpcServer.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_appContextMap.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_classObjectMap.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_proxyMgrList.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_stubMgrList.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_miList.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_connectionMap.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_stubBufMap.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_threadPool.Initialize();
	if (FAILED(hr))
		return hr;

	hr = m_eventManager.Initialize();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void RpcExt::_Uninitialize()
{
	if (m_initCount == 0)
		return;

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	m_eventManager.Uninitialize();
	m_threadPool.Uninitialize();
	m_stubBufMap.Uninitialize();
	m_miList.Uninitialize();
	m_stubMgrList.Uninitialize();
	m_proxyMgrList.Uninitialize();
	m_connectionMap.Uninitialize();
	m_rpcServer.Uninitialize();
	m_classObjectMap.Uninitialize();
	m_appContextMap.Uninitialize();
	m_threadStg.Uninitialize();
	m_procRepl.Uninitialize();

	_procList.Clear();
	_interfaceMap.Clear();

	MarshalClassFactory::RevokeClassObject();

#ifdef RPC_EXT_CHECK_MEM

	_CrtMemDumpAllObjectsSince(&state);

#endif
}