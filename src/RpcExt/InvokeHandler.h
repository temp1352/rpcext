#pragma once

#include "Command.h"
#include "RpcHandler.h"

class RpcStub;
class StubManager;
class RpcConnection;

// InvokeHandler

class InvokeHandler : public CComObjectRootEx<CComMultiThreadModel>,
					  public IRpcHandler,
					  public IRpcChannelBuffer3
{
public:
	friend StubManager;

	InvokeHandler();
	~InvokeHandler();

	BEGIN_COM_MAP(InvokeHandler)
		COM_INTERFACE_ENTRY(IRpcHandler)
		COM_INTERFACE_ENTRY(IRpcChannelBuffer)
		COM_INTERFACE_ENTRY(IRpcChannelBuffer2)
		COM_INTERFACE_ENTRY(IRpcChannelBuffer3)
	END_COM_MAP()

	HRESULT Init(RpcConnection* pConnection, StubManager* pStubManager, IRpcStubBuffer* pStubBuffer, RpcInvokeRequest* pInvCmd);

	STDMETHOD(Execute)(_ExecuteContext* pContext);

	HRESULT OnInvoke(RpcInvokeResp* pResp);
	HRESULT Invoke(RPCOLEMESSAGE* pMessage);

	static LRESULT OnInvokeProc(LPARAM lParam);
	static HRESULT _Invoke(IRpcStubBuffer* pStubBuffer, IRpcChannelBuffer* pChannelBuffer, RPCOLEMESSAGE* pMessage);

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
	RpcConnection* m_pConnection;
	StubManager* m_pStubManager;

	IRpcStubBuffer* m_pStubBuffer;

	RpcInvokeRequest* m_pInvCmd;
};