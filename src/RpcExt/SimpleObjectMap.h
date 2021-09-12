#pragma once

// SimpleObjectMap

class SimpleObjectMap
{
public:
	SimpleObjectMap();
	~SimpleObjectMap();

	struct _Node
	{
		UINT hash;
		GUID guid;
		IUnknown* pUnk;

		_Node* pNext;
	};

	HRESULT Insert(REFGUID rguid, IUnknown* pUnk);
	HRESULT Lookup(REFGUID rguid, REFIID riid, void** ppv);

	HRESULT Remove(REFGUID rguid);

	_Node* GetNode(REFGUID rguid);

	UINT Hash(REFGUID guid);

	HRESULT InitBins();

	void Clear();

protected:
	ULONG m_binCount;
	_Node** m_ppBins;
};
