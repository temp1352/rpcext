#pragma once

#include "ObjRef.h"
#include "ChannelBuffer.h"
#include "ClientConnection.h"

class __declspec(uuid("064B0C34-1854-447d-90D4-34400E554633"))
	ProxyManager : public CComObjectRootEx<CComMultiThreadModel>,
				   //public IMarshal,
				   public IClientSecurity,
				   public IRpcOptions,
				   public IRpcProxyInfo,
				   public IRpcConnectionSink
{
public:
	ProxyManager();
	~ProxyManager();

	static HRESULT _CreateInstance(REFIID riid, void** ppv);

	ULONG _Release();
	ULONG InternalRelease();

	BEGIN_COM_MAP(ProxyManager)
		COM_INTERFACE_ENTRY(ProxyManager)
		//COM_INTERFACE_ENTRY(IMarshal)
		COM_INTERFACE_ENTRY(IClientSecurity)
		COM_INTERFACE_ENTRY(IRpcOptions)
		COM_INTERFACE_ENTRY(IRpcProxyInfo)
		COM_INTERFACE_ENTRY(IRpcConnectionSink)
		COM_INTERFACE_ENTRY_FUNC_BLIND(0, _QueryInterface)
	END_COM_MAP()

	static HRESULT __stdcall _QueryInterface(void* pv, REFIID riid, LPVOID* ppv, DWORD_PTR dw);

	HRESULT Create(REFIID riid, ConnectInfo* pConnInfo);

	HRESULT Connect(REFCLSID rclsid, RE_CONNECTINFO* pConnInfo, REFIID riid, void** ppv);

	HRESULT GetProxy(REFIID riid, void** ppv);
	HRESULT CreateProxy(REFIID riid, void** ppv);
	HRESULT LookupProxy(REFIID riid, void** ppv);

	void Close();

	STDMETHOD(GetSessionId)(LPWSTR buff, ULONG size);
	STDMETHOD(GetConnection)(REFIID riid, void** ppv);

	STDMETHOD(OnConnected)(RPC_CONNECT_RESP* pResp);
	STDMETHOD(OnDisconnected)();

	STDMETHOD(GetUnmarshalClass)(REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags, CLSID* pCid);
	STDMETHOD(GetMarshalSizeMax)(REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags, DWORD* pSize);

	STDMETHOD(MarshalInterface)(IStream* pStm, REFIID riid, void* pv, DWORD dwDestContext, void* pvDestContext, DWORD mshlflags);
	STDMETHOD(UnmarshalInterface)(IStream* pStm, REFIID riid, void** ppv);

	STDMETHOD(ReleaseMarshalData)(IStream* pStm);

	STDMETHOD(DisconnectObject)(DWORD dwReserved);

	STDMETHOD(QueryBlanket)(IUnknown* pProxy, DWORD* pAuthnSvc, DWORD* pAuthzSvc, OLECHAR** pServerPrincName, DWORD* pAuthnLevel, DWORD* pImpLevel, void** pAuthInfo, DWORD* pCapabilites);
	STDMETHOD(SetBlanket)(IUnknown* pProxy, DWORD dwAuthnSvc, DWORD dwAuthzSvc, OLECHAR* pServerPrincName, DWORD dwAuthnLevel, DWORD dwImpLevel, void* pAuthInfo, DWORD dwCapabilities);

	STDMETHOD(CopyProxy)(IUnknown* pProxy, IUnknown** ppCopy);

#if _MSC_VER < 1600

	STDMETHOD(Set)(IUnknown* pPrx, DWORD dwProperty, ULONG_PTR dwValue);
	STDMETHOD(Query)(IUnknown* pPrx, DWORD dwProperty, ULONG_PTR* pdwValue);

#else

	STDMETHOD(Set)(IUnknown* pPrx, RPCOPT_PROPERTIES dwProperty, ULONG_PTR dwValue);
	STDMETHOD(Query)(IUnknown* pPrx, RPCOPT_PROPERTIES dwProperty, ULONG_PTR* pdwValue);

#endif

public:
	GUID m_cid;
	RPC_SID m_sid;
	OID m_oid;

	int m_port;
	char m_hostName[32];

	RpcChannelBuffer* m_pFirstChannel;

	RpcConnection* m_pConnection;
};
