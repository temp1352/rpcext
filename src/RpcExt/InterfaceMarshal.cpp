#include "StdAfx.h"
#include "InterfaceMarshal.h"

#include "RpcExt.h"
#include "ObjRef.h"
#include "MemStream.h"
#include "Marshal.h"
#include "InterfaceMap.h"

static char _marshalInterface[] = "CoMarshalInterface";
static char _unmarshalInterface[] = "CoUnmarshalInterface";

HRESULT InterfaceMarshal::Initialize()
{
	_rpcExt.m_procRepl.AddProc(_marshalInterface, _PROC_STATIC, (FARPROC)MarshalInterface);
	_rpcExt.m_procRepl.AddProc(_unmarshalInterface, _PROC_STATIC, (FARPROC)UnmarshalInterface);

	return S_OK;
}

struct _ObjRef
{
	DWORD dwSignature;
	DWORD type;
	IID iid;
};

struct _CustomObjRef
{
	CLSID clsid;
	ULONG cbExtension;
	ULONG nSize;
};

void _Seek(LPSTREAM pStm, DWORD dwOrigin, int nMove)
{
	LARGE_INTEGER dlibMove;
	dlibMove.QuadPart = (LONGLONG)nMove;
	pStm->Seek(dlibMove, dwOrigin, nullptr);
}

HRESULT WINAPI InterfaceMarshal::MarshalInterface(LPSTREAM pStm, REFIID riid, LPUNKNOWN pUnk,
	DWORD dwDestContext, LPVOID pvDestContext, DWORD mshlflags)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "InterfaceMarshal::MarshalInterface");

	if (dwDestContext != MSHCTX_DIFFERENTMACHINE)
		return CoMarshalInterface(pStm, riid, pUnk, dwDestContext, pvDestContext, mshlflags);

	CComQIPtr<IMarshal> spMarshal = pUnk;
	if (spMarshal)
	{
		//_ObjRef objRef = { 0x574f454d, _OBJ_CUSTOM, riid};
		//_CustomObjRef custObjRef = { CLSID_NULL, 0, 0 };
		//return spMarshal->MarshalInterface(pStm, riid, pUnk, dwDestContext, pvDestContext, mshlflags);
		return CoMarshalInterface(pStm, riid, pUnk, dwDestContext, pvDestContext, mshlflags);
	}

	HRESULT hr = MarshalEx(pStm, riid, pUnk, dwDestContext, pvDestContext, mshlflags);

	return hr;
}

HRESULT WINAPI InterfaceMarshal::MarshalEx(LPSTREAM pStm, REFIID riid, LPUNKNOWN pUnk,
	DWORD dwDestContext, LPVOID pvDestContext, DWORD mshlflags)
{
	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		CComBSTR itfName;
		_interfaceMap.GetInterfaceName(riid, &itfName);

		_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, L"InterfaceMarshal::MarshalEx, interface : %ws", itfName);
	}

	IUnknown* pUnkServer = nullptr;
	HRESULT hr = pUnk->QueryInterface(IID_IUnknown, (void**)&pUnkServer);
	if (hr != S_OK)
		return hr;

	_rpcExt.m_miList.Lock();
	MarshaledInterfaceList::_Node* pNode = _rpcExt.m_miList.Insert(riid, pUnkServer);
	if (pNode == nullptr)
	{
		_rpcExt.m_miList.Unlock();
		pUnkServer->Release();

		return E_OUTOFMEMORY;
	}

	WriteStream stream;
	hr = CoMarshalInterface(&stream, riid, pUnk, dwDestContext, pvDestContext, mshlflags);
	if (hr == S_OK)
	{
		static const int _offset = sizeof(DWORD) + sizeof(DWORD) + sizeof(IID);
		const BYTE* pData = stream.GetData();
		auto pObjRef = (STDOBJREF*)(pData + _offset);

		if (pNode && pNode->pBuffer)
		{
			pNode->pBuffer->m_ipid = pObjRef->ipid;
			_rpcExt.m_stubBufMap.Insert(pNode->pBuffer);
		}

		pStm->Write(pData, stream.GetSize(), nullptr);
	}

	_rpcExt.m_miList.Clear();
	_rpcExt.m_miList.Unlock();

	return hr;
}

HRESULT WINAPI InterfaceMarshal::UnmarshalInterface(LPSTREAM pStm, REFIID riid, LPVOID* ppv)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "InterfaceMarshal::UnarshalInterface");

	_ObjRef objRef;
	HRESULT hr = pStm->Read(&objRef, sizeof(_ObjRef), nullptr);
	if (objRef.type != _OBJ_CUSTOM)
	{
		_Seek(pStm, SEEK_CUR, -(int)sizeof(_ObjRef));

		return CoUnmarshalInterface(pStm, riid, ppv);
	}

	_CustomObjRef custObjRef;
	hr = pStm->Read(&custObjRef, sizeof(_CustomObjRef), nullptr);
	if (!InlineIsEqualGUID(custObjRef.clsid, __uuidof(MarshalClassFactory)))
	{
		_Seek(pStm, SEEK_CUR, -(int)sizeof(_ObjRef) - (int)sizeof(_CustomObjRef));

		return CoUnmarshalInterface(pStm, riid, ppv);
	}

	return UnmarshalEx(pStm, objRef.iid, ppv);
}

HRESULT WINAPI InterfaceMarshal::UnmarshalEx(LPSTREAM pStm, REFIID riid, LPVOID* ppv)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "InterfaceMarshal::UnmarshalEx");

	ConnectInfo connInfo;
	HRESULT hr = connInfo.Read(pStm);
	if (FAILED(hr))
		return hr;

	if (_logFile.m_outLevel >= LogLevel::Details)
	{
		CComBSTR itfName;
		_interfaceMap.GetInterfaceName(riid, &itfName);

		_logFile.WriteV(LogCategory::Misc, LogLevel::Details, LogOption::Std, L"InterfaceMarshal::UnmarshalEx, interface : %ws, hostName : %ws, port : %d, sid : %ws", itfName, connInfo.m_hostName, connInfo.m_port, GuidString(connInfo.m_sid).m_str);
	}

	CComPtr<ProxyManager> spProxyManager;
	hr = _rpcExt.m_proxyMgrList.GetProxyManager(connInfo.m_cid, connInfo.m_oid, &spProxyManager);
	if (hr != S_OK)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "InterfaceMarshal::UnmarshalEx - ProxyManager::_CreateInstance");

		hr = ProxyManager::_CreateInstance(__uuidof(ProxyManager), (void**)&spProxyManager);
		if (hr != S_OK)
			return hr;

		hr = spProxyManager->Create(riid, &connInfo);
	}

	hr = spProxyManager->GetProxy(riid, ppv);

	return hr;
}