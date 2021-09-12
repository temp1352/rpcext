#include "StdAfx.h"
#include "ChannelBuffer.h"

#include "RpcExt.h"
#include "ProxyManager.h"
#include "ClientConnection.h"

#include "InterfaceMap.h"

RpcChannelBuffer::RpcChannelBuffer()
{
	m_iid = IID_NULL;

	m_pProxyManager = nullptr;
	m_pConnection = nullptr;
	m_pProxyBuffer = nullptr;

	m_pNext = nullptr;
}

RpcChannelBuffer::~RpcChannelBuffer()
{
	//AtlTrace("RpcChannelBuffer::~RpcChannelBuffer\n");
}

#ifdef _DEBUG

ULONG RpcChannelBuffer::InternalAddRef()
{
	DWORD ref = __super::InternalAddRef();

	return ref;
}

ULONG RpcChannelBuffer::InternalRelease()
{
	DWORD ref = __super::InternalRelease();

	return ref;
}

#endif

HRESULT RpcChannelBuffer::Connect(REFIID riid, ProxyManager* pProxyManager, RpcConnection* pConnection, IRpcProxyBuffer* pProxyBuffer)
{
	Lock();

	HRESULT hr = pProxyBuffer->Connect(this);
	if (hr == S_OK)
	{
		m_iid = riid;
		m_pProxyManager = pProxyManager;
		m_pConnection = pConnection;
		m_pProxyBuffer = pProxyBuffer;

		pConnection->AddRef();
		pProxyBuffer->AddRef();
	}

	Unlock();

	return hr;
}

void RpcChannelBuffer::Disconnect()
{
	Lock();

	if (m_pProxyBuffer)
	{
		m_pProxyBuffer->Disconnect();
		m_pProxyBuffer->Release();
		m_pProxyBuffer = nullptr;
	}

	if (m_pConnection)
	{
		m_pConnection->Release();
		m_pConnection = nullptr;
	}

	m_pProxyManager = nullptr;

	Unlock();
}

STDMETHODIMP RpcChannelBuffer::GetBuffer(RPCOLEMESSAGE* pMessage, REFIID riid)
{
	//_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcChannelBuffer::GetBuffer");

	RPC_MESSAGE* pRpcMsg = (RPC_MESSAGE*)pMessage;

	if (pMessage->cbBuffer > 0)
	{
		pMessage->Buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pMessage->cbBuffer);
		if (!pMessage->Buffer)
			return E_OUTOFMEMORY;
	}
	else
	{
		pMessage->Buffer = nullptr;
	}

	return S_OK;
}

STDMETHODIMP RpcChannelBuffer::SendReceive(RPCOLEMESSAGE* pMessage, ULONG* pStatus)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcChannelBuffer::SendReceive, m_pProxyManager : #%08x, m_pConnection : #%08x", m_pProxyManager, m_pConnection);

	if (!pMessage || !pStatus)
		return E_INVALIDARG;

	ObjectLock lock(this);

	if (!m_pProxyManager || !m_pConnection)
		return E_UNEXPECTED;

	HRESULT hr = m_pConnection->HealthCheck();
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, hr, "RpcChannelBuffer::SendReceive, m_pConnection->HealthCheck() return failed");

		return AtlHresultFromWin32(RPC_S_SERVER_UNAVAILABLE);
	}

	auto pRpcMsg = (RPC_MESSAGE*)pMessage;

	CComPtr<RpcInvokeRequest> spInvCmd(new CComObject<RpcInvokeRequest>());
	if (!spInvCmd)
		return E_OUTOFMEMORY;

	spInvCmd->m_cookie = m_pConnection->CreateCookie();

	CStringA strItf, strMethod;

	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		_interfaceMap.GetMethodName(m_iid, pMessage->iMethod, strItf, strMethod);
		_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, "RpcChannelBuffer::SendReceive, interface : %s, method : %s, rpcFlags : %x, cbBuffer : %d, cookie : %d",
			strItf, strMethod, pMessage->rpcFlags, pMessage->cbBuffer, spInvCmd->m_cookie);
	}

	RpcAutoEvent autoEvent(spInvCmd->m_cookie);

	spInvCmd->m_eventId = autoEvent.m_eventId;
	spInvCmd->m_oid = m_pProxyManager->m_oid;
	spInvCmd->m_iid = m_iid;
	spInvCmd->m_message.Attach(pRpcMsg);

	hr = m_pConnection->m_callContext.ProcessInterfacePointers(&spInvCmd->m_message, &spInvCmd->m_itfPtrCount);

	CComPtr<RpcInvokeResp> spRespCmd;
	hr = m_pConnection->SendReceive(spInvCmd, &spRespCmd);
	if (hr == S_OK && spRespCmd)
	{
		hr = spRespCmd->m_hRes;
		if (FAILED(hr) && _logFile.m_outLevel >= LogLevel::Details)
		{
			if (pMessage->Buffer)
			{
				_logFile.WriteBinaryV(LogCategory::Misc, LogLevel::Details, LogOption::Std, pMessage->Buffer, pMessage->cbBuffer, "invoke buffer");
			}
		}

		spRespCmd->m_message.Detach(pRpcMsg);

		spRespCmd->ReportError();
	}
	else if (SUCCEEDED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, "RpcChannelBuffer::SendReceive, The remote procedure call failed, hr = #%x", hr);
		hr = HRESULT_FROM_WIN32(RPC_S_CALL_FAILED);
	}

	if (pStatus)
	{
		*pStatus = 0;
	}

	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, "RpcChannelBuffer::SendReceive return - oid : %I64u, interface : %s, method : %s, rpcFlags : %x, cbBuffer : %d, cookie : %d, hr = 0x%08x",
			spInvCmd->m_oid, strItf, strMethod, pMessage->rpcFlags, pMessage->cbBuffer, spInvCmd->m_cookie, hr);
	}

	return hr;
}

STDMETHODIMP RpcChannelBuffer::FreeBuffer(RPCOLEMESSAGE* pMessage)
{
	//_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcChannelBuffer::FreeBuffer");

	if (pMessage->Buffer)
	{
		HeapFree(GetProcessHeap(), 0, pMessage->Buffer);
	}

	return S_OK;
}

STDMETHODIMP RpcChannelBuffer::GetDestCtx(DWORD* pdwDestContext, void** ppvDestContext)
{
	//_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcChannelBuffer::GetDestCtx");

	if (!pdwDestContext)
		return E_POINTER;

	*pdwDestContext = MSHCTX_DIFFERENTMACHINE;
	*ppvDestContext = nullptr;

	return S_OK;
}

STDMETHODIMP RpcChannelBuffer::IsConnected()
{
	//_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcChannelBuffer::IsConnected\n");

	HRESULT hr = S_FALSE;

	Lock();

	if (m_pConnection)
	{
		hr = m_pConnection->IsConnected() ? S_OK : S_FALSE;
	}

	Unlock();

	return S_OK;
}

STDMETHODIMP RpcChannelBuffer::GetProtocolVersion(DWORD* pdwVersion)
{
	if (!pdwVersion)
		return E_POINTER;

	*pdwVersion = RPC_PROTOCOL_VERSION;

	return S_OK;
}

STDMETHODIMP RpcChannelBuffer::Send(RPCOLEMESSAGE* pMsg, ULONG* pulStatus)
{
	return E_NOTIMPL;
}

STDMETHODIMP RpcChannelBuffer::Receive(RPCOLEMESSAGE* pMsg, ULONG ulSize, ULONG* pulStatus)
{
	return E_NOTIMPL;
}

STDMETHODIMP RpcChannelBuffer::Cancel(RPCOLEMESSAGE* pMsg)
{
	return E_NOTIMPL;
}

STDMETHODIMP RpcChannelBuffer::GetCallContext(RPCOLEMESSAGE* pMsg, REFIID riid, void** pInterface)
{
	return E_NOTIMPL;
}

STDMETHODIMP RpcChannelBuffer::GetDestCtxEx(RPCOLEMESSAGE* pMsg, DWORD* pdwDestContext, void** ppvDestContext)
{
	return E_NOTIMPL;
}

STDMETHODIMP RpcChannelBuffer::GetState(RPCOLEMESSAGE* pMsg, DWORD* pState)
{
	return E_NOTIMPL;
}

STDMETHODIMP RpcChannelBuffer::RegisterAsync(RPCOLEMESSAGE* pMsg, IAsyncManager* pAsyncMgr)
{
	return E_NOTIMPL;
}