#pragma once

// GuidMap

class GuidMap
{
public:
	struct _Node
	{
		UINT hash;
		GUID key;
		IUnknown* pUnk;
		_Node* pNext;
	};

	typedef _Node* _Position;

	GuidMap();
	~GuidMap();

	HRESULT Insert(REFGUID key, IUnknown* pUnk, _Position* pPos = nullptr);
	HRESULT Lookup(REFGUID key, REFIID riid, void** ppv);

	HRESULT Remove(REFGUID key, REFIID riid = IID_NULL, void** ppv = nullptr);
	HRESULT Remove(_Position pos);

	HRESULT InitBins();

	void Clear();
	void _Clear();

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

protected:
	CComAutoCriticalSection m_cs;

	ULONG m_count;
	ULONG m_binCount;
	_Node** m_ppBins;
};
