#pragma once

#include "SimpleObjectMap.h"

class RpcCallContext;
class RpcThreadStorage;

class __declspec(uuid("664E94DD-3702-46e2-A001-E10F099BF61B"))
	RpcThreadInfo : public CComObjectRootEx<CComMultiThreadModel>,
					public IUnknown
{
public:
	friend RpcThreadStorage;

	RpcThreadInfo();
	~RpcThreadInfo();

	BEGIN_COM_MAP(RpcThreadInfo)
		COM_INTERFACE_ENTRY(RpcThreadInfo)
	END_COM_MAP()

	HRESULT Initialize(APTTYPE type);
	void Uninitialize();

	RpcCallContext* GetContext();
	void SetContext(RpcCallContext* pContext);

public:
	DWORD m_threadId;
	APTTYPE m_aptType;

	HWND m_hWnd;

protected:
	RpcCallContext* m_pContext;

	SimpleObjectMap m_valueMap;

	RpcThreadInfo* m_pNext;
};
