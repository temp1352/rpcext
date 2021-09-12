#pragma once

#include "ProxyManager.h"

class ProxyManagerList
{
public:
	struct _Node
	{
		UINT hash;
		ProxyManager* pManager;
		_Node* pNext;
	};

	ProxyManagerList();
	~ProxyManagerList();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT GetProxyManager(REFGUID cid, OID oid, ProxyManager** ppManager);

	HRESULT Add(ProxyManager* pManager);
	BOOL Remove(ProxyManager* pManager);

	void RemoveAll();

	HRESULT InitBins();

protected:
	CComAutoDeleteCriticalSection m_cs;

	ULONG m_count;
	ULONG m_binCount;
	_Node** m_ppBins;
	_Node* m_pCache;
};