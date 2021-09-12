#pragma once

class RpcStubBufferList;
class RpcStubBufferMap;

class __declspec(uuid("D61F6732-4960-4738-AA27-B27AB4769CD3"))
	RpcStubBuffer : public IRpcStubBuffer
{
public:
	friend RpcStubBufferList;
	friend RpcStubBufferMap;

	RpcStubBuffer();
	~RpcStubBuffer();

	static HRESULT CreateInstance(REFIID riid, IUnknown* pUnkServer, IRpcStubBuffer* pRawBuffer, IRpcStubBuffer** ppStubBuffer);

	STDMETHOD(QueryInterface)(REFIID riid, void** ppv);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	ULONG _Release();

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

	STDMETHOD(Connect)(IUnknown* pUnkServer);

	STDMETHOD_(void, Disconnect)();

	STDMETHOD(Invoke)(RPCOLEMESSAGE* prpcmsg, IRpcChannelBuffer* pRpcChannelBuffer);

	STDMETHOD_(IRpcStubBuffer*, IsIIDSupported)(REFIID riid);

	STDMETHOD_(ULONG, CountRefs)();

	STDMETHOD(DebugServerQueryInterface)(void** ppv);

	STDMETHOD_(void, DebugServerRelease)(void* pv);

	IUnknown* GetServer() { return m_pUnkServer; }

private:
	ULONG m_ref;

	UINT m_hash;

	bool m_conected;

public:
	IID m_iid;
	IPID m_ipid;

protected:
	IUnknown* m_pUnkServer;

	IRpcStubBuffer* m_pRawBuffer;

	CComAutoCriticalSection m_cs;

	RpcStubBuffer* m_pNext;
};

// RpcStubBufferMap

class RpcStubBufferMap
{
public:
	RpcStubBufferMap();
	~RpcStubBufferMap();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT Insert(RpcStubBuffer* pStubBuffer);
	HRESULT Lookup(const IPID& ipid, RpcStubBuffer** ppStubBuffer);

	BOOL Remove(RpcStubBuffer* pStubBuffer);

	HRESULT InitBins();

	void Clear();
	void _Clear();

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

protected:
	CComAutoCriticalSection m_cs;

	ULONG m_binCount;
	RpcStubBuffer** m_ppBins;
};
