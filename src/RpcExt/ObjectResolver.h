#pragma once

#include "ObjRef.h"

class ObjectResolver
{
public:
	static HRESULT Resolve(IID iid, IUnknown* pUnkServer, StdObjRef* pObjRef);
	static BOOL IsResolving();

protected:
	static CComAutoCriticalSection s_cs;
	static BOOL s_bResolving;
};
