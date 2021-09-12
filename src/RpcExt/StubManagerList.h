#pragma once

#include "StubManager.h"

class RpcConnection;

// StubManagerList

class StubManagerList
{
public:
	struct _Node
	{
		OID oid;
		StubManager* pStubManager;
		_Node* pNext;
	};

	StubManagerList();
	~StubManagerList();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT CreateStubManager(OID oid, IUnknown* pUnk, APTTYPE type, DWORD dwThreadId, StubManager** ppStubManager);
	void ReleaseStubManager(StubManager* pStubManager);

	HRESULT Insert(StubManager* pStubManager);
	HRESULT Lookup(OID oid, StubManager** ppStubManager);

	HRESULT Remove(OID oid);

	HRESULT InitBins();

	void Clear();
	void _Clear();

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

protected:
	bool m_init;

	CComAutoDeleteCriticalSection m_cs;

	ULONG m_binCount;
	_Node** m_ppBins;
};