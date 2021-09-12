#pragma once

#include "Settings.h"
#include "ProcReplacement.h"
#include "ThreadStorage.h"
#include "AppContextMap.h"
#include "ClassObjectMap.h"
#include "RpcServer.h"
#include "ConnectionMap.h"
#include "ProxyManagerList.h"
#include "StubManagerList.h"
#include "StubBufferMap.h"
#include "PsFactoryBuffer.h"
#include "MarshaledInterfaceList.h"
#include "ThreadPool.h"
#include "EventManager.h"

class RE_API RpcExt
{
public:
	RpcExt();
	~RpcExt();

	HRESULT Initialize(DWORD coInit);
	void Uninitialize();

	HRESULT _Initialize();
	void _Uninitialize();

private:
	LONG m_initCount;

public:
	CComAutoCriticalSection m_cs;

	RpcSettings m_settings;

	ProcReplacement m_procRepl;

	RpcThreadStorage m_threadStg;

	AppContextMap m_appContextMap;
	ClassObjectMap m_classObjectMap;

	RpcServer m_rpcServer;

	RpcConnectionMap m_connectionMap;

	ProxyManagerList m_proxyMgrList;
	StubManagerList m_stubMgrList;

	MarshaledInterfaceList m_miList;
	RpcStubBufferMap m_stubBufMap;

	RpcThreadPool m_threadPool;

	EventManager m_eventManager;
};

extern RpcExt _rpcExt;
