#pragma once

class RpcStubBuffer;

class MarshaledInterfaceList
{
public:
	struct _Node
	{
		IID iid;
		IUnknown* pUnkServer;
		RpcStubBuffer* pBuffer;

		_Node* pNext;
	};

	MarshaledInterfaceList();
	~MarshaledInterfaceList();

	HRESULT Initialize();
	void Uninitialize();

	_Node* Insert(REFIID riid, IUnknown* pUnkServer);

	_Node* Lookup(REFIID riid, IUnknown* pUnkServer);

	void Remove(_Node* pNode);
	void Clear();

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

protected:
	_Node* m_pHead;

	CComAutoCriticalSection m_cs;
};
