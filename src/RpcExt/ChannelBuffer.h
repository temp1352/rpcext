#pragma once

#include "Command.h"

class ProxyManager;
class RpcConnection;

enum
{
	RPC_PROTOCOL_VERSION = 0,
};

class __declspec(uuid("70C7D4DF-6B19-4972-A770-42CAD96FB047"))
	RpcChannelBuffer : public CComObjectRootEx<CComMultiThreadModel>,
					   public IRpcChannelBuffer3
{
public:
	friend ProxyManager;

	RpcChannelBuffer();
	~RpcChannelBuffer();

#ifdef _DEBUG

	ULONG InternalAddRef();
	ULONG InternalRelease();

#endif

	HRESULT Connect(REFIID riid, ProxyManager* pProxyManager, RpcConnection* pConnection, IRpcProxyBuffer* pProxyBuffer);
	void Disconnect();

	BEGIN_COM_MAP(RpcChannelBuffer)
		COM_INTERFACE_ENTRY(RpcChannelBuffer)
		COM_INTERFACE_ENTRY(IRpcChannelBuffer)
		COM_INTERFACE_ENTRY(IRpcChannelBuffer2)
		COM_INTERFACE_ENTRY(IRpcChannelBuffer3)
	END_COM_MAP()

	STDMETHOD(GetBuffer)(RPCOLEMESSAGE* pMessage, REFIID riid);

	STDMETHOD(SendReceive)(RPCOLEMESSAGE* pMessage, ULONG* pStatus);

	STDMETHOD(FreeBuffer)(RPCOLEMESSAGE* pMessage);

	STDMETHOD(GetDestCtx)(DWORD* pdwDestContext, void** ppvDestContext);

	STDMETHOD(IsConnected)();

	STDMETHOD(GetProtocolVersion)(DWORD* pdwVersion);

	STDMETHOD(Send)(RPCOLEMESSAGE* pMsg, ULONG* pulStatus);
	STDMETHOD(Receive)(RPCOLEMESSAGE* pMsg, ULONG ulSize, ULONG* pulStatus);

	STDMETHOD(Cancel)(RPCOLEMESSAGE* pMsg);

	STDMETHOD(GetCallContext)(RPCOLEMESSAGE* pMsg, REFIID riid, void** pInterface);

	STDMETHOD(GetDestCtxEx)(RPCOLEMESSAGE* pMsg, DWORD* pdwDestContext, void** ppvDestContext);

	STDMETHOD(GetState)(RPCOLEMESSAGE* pMsg, DWORD* pState);

	STDMETHOD(RegisterAsync)(RPCOLEMESSAGE* pMsg, IAsyncManager* pAsyncMgr);

protected:
	IID m_iid;

	ProxyManager* m_pProxyManager;
	RpcConnection* m_pConnection;

	IRpcProxyBuffer* m_pProxyBuffer;

	RpcChannelBuffer* m_pNext;
};
