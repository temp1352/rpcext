#pragma once

#include "resource.h" 

#include "Server_i.h"

// UserService

class ATL_NO_VTABLE UserService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<UserService, &CLSID_UserService>,
	public IUserService
{
public:
	UserService();

	DECLARE_REGISTRY_RESOURCEID(106)

	BEGIN_COM_MAP(UserService)
		COM_INTERFACE_ENTRY(IUserService)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
	STDMETHOD(Hello)(LPCWSTR name, BSTR* pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(UserService), UserService)
