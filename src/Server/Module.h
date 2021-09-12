#pragma once

#include "framework.h"
#include "resource.h"
#include "Server_i.h"

class ServerModule : public ATL::CAtlExeModuleT< ServerModule >
{
public:
	DECLARE_LIBID(LIBID_Server)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SERVER, "{d56bdf05-fd5d-4b69-bb60-54cd8d7c4df6}")

	static HRESULT InitializeCom() throw();
	static void UninitializeCom() throw();

	HRESULT RegisterClassObjects(DWORD dwClsContext, DWORD dwFlags);
	HRESULT RevokeClassObjects() throw();

	HRESULT PreMessageLoop(int nShowCmd) throw();
	HRESULT PostMessageLoop() throw();
};