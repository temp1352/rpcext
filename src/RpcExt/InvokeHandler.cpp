#include "StdAfx.h"
#include "InvokeHandler.h"

#include "RpcExt.h"
#include "StubManager.h"
#include "Connection.h"
#include "ThreadWindow.h"

#include "InterfaceMap.h"

// CInvkeWorker

InvokeHandler::InvokeHandler()
{
	m_pConnection = nullptr;
	m_pStubManager = nullptr;
	m_pStubBuffer = nullptr;
	m_pInvCmd = nullptr;
}

InvokeHandler::~InvokeHandler()
{
	if (m_pInvCmd)
	{
		m_pInvCmd->Release();
	}

	if (m_pStubBuffer)
	{
		m_pStubBuffer->Release();
	}

	if (m_pStubManager)
	{
		m_pStubManager->Release();
	}

	if (m_pConnection)
	{
		m_pConnection->ReleasePrivateRef();
		m_pConnection->Release();
	}
}

HRESULT InvokeHandler::Init(RpcConnection* pConnection, StubManager* pStubManager, IRpcStubBuffer* pStubBuffer, RpcInvokeRequest* pInvCmd)
{
	Lock();

	pConnection->AddPrivateRef();
	pConnection->AddRef();
	pStubManager->AddRef();
	pStubBuffer->AddRef();
	pInvCmd->AddRef();

	m_pConnection = pConnection;
	m_pStubManager = pStubManager;
	m_pStubBuffer = pStubBuffer;
	m_pInvCmd = pInvCmd;

	Unlock();

	return S_OK;
}

STDMETHODIMP InvokeHandler::Execute(_ExecuteContext* pContext)
{
	ObjectLock lock(this);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "InvokeHandler::Execute - begin, m_pConnection : #%08x, aid : %I64u, m_pStubManager = #%08x, m_pStubBuffer = #%08x",
		m_pConnection, m_pConnection->m_aid, m_pStubManager, m_pStubBuffer);

	CComPtr<RpcInvokeResp> spRespCmd(new CComObject<RpcInvokeResp>);
	if (!spRespCmd)
		return E_OUTOFMEMORY;

	CComPtr<RpcThreadInfo> spThreadInfo;
	HRESULT hr = _rpcExt.m_threadStg.GetThreadInfo(&spThreadInfo);
	if (hr == S_OK)
	{
		spThreadInfo->SetContext(&m_pConnection->m_callContext);
	}
	else
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, hr, "InvokeHandler::Execute - GetThreadInfo error");
	}

	spRespCmd->m_cookie = m_pInvCmd->m_cookie;
	spRespCmd->m_eventId = m_pInvCmd->m_eventId;
	spRespCmd->m_iid = m_pInvCmd->m_iid;

	hr = OnInvoke(spRespCmd);

	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "InvokeHandler::Execute - QueueSend, connection : #%08x, aid : %I64u",
			m_pConnection, m_pConnection->m_aid);
	}

	hr = m_pConnection->QueueSend(spRespCmd);
	m_pInvCmd->SetExpire(-1);

	if (spThreadInfo)
	{
		spThreadInfo->SetContext(nullptr);
	}

	return hr;
}

HRESULT InvokeHandler::OnInvoke(RpcInvokeResp* pResp)
{
	//_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "InvokeHandler::OnInvoke");

	CStringA itfName, methodName;
	auto pMsg = (RPCOLEMESSAGE*)&m_pInvCmd->m_message;

	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		_interfaceMap.GetMethodName(m_pInvCmd->m_iid, pMsg->iMethod, itfName, methodName);

		_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, "InvokeHandler::Invoke - iMethod : %d, interface : %s, method : %s, rpcFlags : %x, cbBuffer : %d, cookie : %d", pMsg->iMethod,
			itfName, methodName, pMsg->rpcFlags, pMsg->cbBuffer, m_pInvCmd->m_cookie);
	}

	SetErrorInfo(0, nullptr);

	LPVOID pBuffer = m_pInvCmd->m_message.Buffer;

	HRESULT hr = Invoke(pMsg);
	pResp->m_hRes = hr;
	pResp->CacheError();

	_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, hr, "InvokeHandler::Invoke - Invoke finish, interface : %s, method : %s, cookie : %d, hr : %x",
		itfName, methodName, pResp->m_cookie, hr);

	if (pBuffer != m_pInvCmd->m_message.Buffer)
	{
		HeapFree(GetProcessHeap(), 0, pBuffer);
	}

	if (FAILED(hr))
	{
		if (_logFile.m_outLevel >= LogLevel::Details && pMsg->Buffer)
		{
			_logFile.WriteBinaryV(LogCategory::Misc, LogLevel::Details, LogOption::Std, pMsg->Buffer, pMsg->cbBuffer, "invoke buffer");
		}

		return hr;
	}

	m_pInvCmd->m_message.Detach(&pResp->m_message);
	hr = m_pConnection->m_callContext.ProcessInterfacePointers(&pResp->m_message, &pResp->m_itfPtrCount);

	if (FAILED(hr) && _logFile.m_outLevel >= LogLevel::Details)
	{
		pMsg = (RPCOLEMESSAGE*)&pResp->m_message;

		_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, "InvokeHandler::Invoke - ProcessInterfacePointers, interface : %s, method : %s, cookie : %d, cbBuffer : %d, hr : #%08x",
			itfName, methodName, pResp->m_cookie, pMsg->cbBuffer, hr);

		if (pMsg->cbBuffer)
		{
			_logFile.WriteBinaryV(LogCategory::Misc, LogLevel::Details, LogOption::Std, pMsg->Buffer, pMsg->cbBuffer, "ProcessInterfacePointers failed, buffer : ");
		}
	}

	return hr;
}

struct _InvokeParam
{
	IRpcStubBuffer* pStubBuffer;
	IRpcChannelBuffer* pChannelBuffer;
	RPCOLEMESSAGE* pMessage;
	HRESULT hr;
	HANDLE hEvent;
};

HRESULT InvokeHandler::Invoke(RPCOLEMESSAGE* pMessage)
{
	if (m_pStubManager->m_aptType == APTTYPE_MTA)
	{
		return _Invoke(m_pStubBuffer, this, pMessage);
	}

	CComPtr<RpcThreadInfo> spThreadInfo;
	HRESULT hr = _rpcExt.m_threadStg.GetThreadInfo(m_pStubManager->m_createThreadId, &spThreadInfo);
	if (hr != S_OK)
		return hr;

	CAutoPtr<_InvokeParam> spParam(new _InvokeParam);
	if (!spParam)
		return E_OUTOFMEMORY;

	spParam->pStubBuffer = m_pStubBuffer;
	spParam->pChannelBuffer = this;
	spParam->pMessage = pMessage;
	spParam->hr = E_FAIL;
	spParam->hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!spParam->hEvent)
		return AtlHresultFromLastError();

	PostMessage(spThreadInfo->m_hWnd, RPC_WM_INVOKE, (WPARAM)OnInvokeProc, (LPARAM)spParam.m_p);

	DWORD wait = WaitForSingleObject(spParam->hEvent, INFINITE);

	CloseHandle(spParam->hEvent);

	if (wait == WAIT_OBJECT_0)
	{
		return spParam->hr;
	}

	return E_UNEXPECTED;
}

LRESULT InvokeHandler::OnInvokeProc(LPARAM lParam)
{
	_InvokeParam* pParam = (_InvokeParam*)lParam;

	pParam->hr = _Invoke(pParam->pStubBuffer, pParam->pChannelBuffer, pParam->pMessage);

	SetEvent(pParam->hEvent);

	return 0;
}

HRESULT InvokeHandler::_Invoke(IRpcStubBuffer* pStubBuffer, IRpcChannelBuffer* pChannelBuffer, RPCOLEMESSAGE* pMessage)
{
	try
	{
		return pStubBuffer->Invoke(pMessage, pChannelBuffer);
	}
	catch (CXtlException e)
	{
		_logFile.WriteV(LogCategory::Test, LogLevel::Error, LogOption::Std, e.m_hr, L"InvokeHandler::_Invoke - catch(CXtlException), source: %ws, message : %ws", e.m_source, e.m_message);
		return e;
	}
	catch (CAtlException e)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, e.m_hr, L"InvokeHandler::_Invoke, catch(CAtlException)");
		return e;
	}
	catch (...)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, GetLastError(), L"InvokeHandler::_Invoke, catch(...)");
		return E_FAIL;
	}
}

STDMETHODIMP InvokeHandler::GetBuffer(RPCOLEMESSAGE* pMessage, REFIID riid)
{
	//AtlTrace("InvokeHandler::GetBuffer\n");

	//RPC_MESSAGE * pRpcMsg = (RPC_MESSAGE*)pMessage;

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

STDMETHODIMP InvokeHandler::SendReceive(RPCOLEMESSAGE* pMessage, ULONG* pStatus)
{
	//_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "InvokeHandler::SendReceive");

	if (!pMessage || !pStatus)
		return E_INVALIDARG;

	if (pStatus)
	{
		*pStatus = 0;
	}

	return S_OK;
}

STDMETHODIMP InvokeHandler::FreeBuffer(RPCOLEMESSAGE* pMessage)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "InvokeHandler::SendReceive - cbBuffer : %d", pMessage->cbBuffer);

	if (pMessage->Buffer)
	{
		HeapFree(GetProcessHeap(), 0, pMessage->Buffer);

		pMessage->Buffer = nullptr;
		pMessage->cbBuffer = 0;
	}

	return S_OK;
}

STDMETHODIMP InvokeHandler::GetDestCtx(DWORD* pdwDestContext, void** ppvDestContext)
{
	//AtlTrace("InvokeHandler::GetDestCtx\n");

	if (!pdwDestContext)
		return E_POINTER;

	*pdwDestContext = MSHCTX_DIFFERENTMACHINE;
	*ppvDestContext = nullptr;

	return S_OK;
}

STDMETHODIMP InvokeHandler::IsConnected()
{
	//AtlTrace("InvokeHandler::IsConnected\n");

	return S_OK;
}

STDMETHODIMP InvokeHandler::GetProtocolVersion(DWORD* pdwVersion)
{
	if (!pdwVersion)
		return E_POINTER;

	*pdwVersion = RPC_PROTOCOL_VERSION;

	return S_OK;
}

STDMETHODIMP InvokeHandler::Send(RPCOLEMESSAGE* pMsg, ULONG* pulStatus)
{
	return E_NOTIMPL;
}

STDMETHODIMP InvokeHandler::Receive(RPCOLEMESSAGE* pMsg, ULONG ulSize, ULONG* pulStatus)
{
	return E_NOTIMPL;
}

STDMETHODIMP InvokeHandler::Cancel(RPCOLEMESSAGE* pMsg)
{
	return E_NOTIMPL;
}

STDMETHODIMP InvokeHandler::GetCallContext(RPCOLEMESSAGE* pMsg, REFIID riid, void** pInterface)
{
	return E_NOTIMPL;
}

STDMETHODIMP InvokeHandler::GetDestCtxEx(RPCOLEMESSAGE* pMsg, DWORD* pdwDestContext, void** ppvDestContext)
{
	return E_NOTIMPL;
}

STDMETHODIMP InvokeHandler::GetState(RPCOLEMESSAGE* pMsg, DWORD* pState)
{
	return E_NOTIMPL;
}

STDMETHODIMP InvokeHandler::RegisterAsync(RPCOLEMESSAGE* pMsg, IAsyncManager* pAsyncMgr)
{
	return E_NOTIMPL;
}