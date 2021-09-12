#include "StdAfx.h"
#include "ObjectResolver.h"

#include "MemStream.h"

CComAutoCriticalSection ObjectResolver::s_cs;
BOOL ObjectResolver::s_bResolving = FALSE;

HRESULT _Resolve(IID iid, IUnknown* pUnkServer, StdObjRef* pObjRef)
{
	CComPtr<IMarshal> spMarshal;
	HRESULT hr = CoGetStandardMarshal(iid, pUnkServer, MSHCTX_DIFFERENTMACHINE, nullptr, MSHLFLAGS_NORMAL, &spMarshal);
	if (FAILED(hr))
		return hr;

	WriteStream wStream;
	hr = spMarshal->MarshalInterface(&wStream, iid, pUnkServer, MSHCTX_DIFFERENTMACHINE, nullptr, MSHLFLAGS_NORMAL);
	if (FAILED(hr))
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcStubBuffer::ResolveIPID, MarshalInterface, hr : %x", hr);
		return hr;
	}

	LPBYTE lpData = const_cast<BYTE*>(wStream.GetData());
	hr = pObjRef->Read(lpData);

	ReadStream rStream(wStream.GetData(), wStream.GetSize());
	//spMarshal->ReleaseMarshalData(&rStream);

	return hr;
}

HRESULT ObjectResolver::Resolve(IID iid, IUnknown* pUnkServer, StdObjRef* pObjRef)
{
	CComCritSecLock<CComAutoCriticalSection> lock(s_cs);

	s_bResolving = TRUE;

	_Resolve(iid, pUnkServer, pObjRef);

	s_bResolving = FALSE;

	return S_OK;
}

BOOL ObjectResolver::IsResolving()
{
	s_cs.Lock();

	BOOL bResolving = s_bResolving;

	s_cs.Unlock();

	return bResolving;
}