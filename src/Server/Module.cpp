#include "pch.h"
#include "Module.h"

#include "RpcShell.h"
#include "RegisterClassObjects.h"

HRESULT ServerModule::InitializeCom() throw()
{
	return RE_Initialize(COINIT_MULTITHREADED);
}

void ServerModule::UninitializeCom() throw()
{
	RE_Uninitialize();
}

HRESULT ServerModule::RegisterClassObjects(DWORD dwClsContext, DWORD dwFlags)
{
	return RE_RegisterClassObjects(&_AtlComModule, dwClsContext, dwFlags, 0);
}

HRESULT ServerModule::RevokeClassObjects() throw()
{
	return RE_RevokeClassObjects(&_AtlComModule);
}

HRESULT ServerModule::PreMessageLoop(int showCmd) throw()
{
	HRESULT hr = __super::PreMessageLoop(showCmd);
	if (FAILED(hr))
		return hr;

#ifdef _DEBUG
	hr = RE_RegisterTypeLib(LIBID_Server);
#endif

	hr = RE_Listen(5000);

	return hr;
}

HRESULT ServerModule::PostMessageLoop() throw()
{
	RE_StopListening();

	return __super::PostMessageLoop();
}

ServerModule _AtlModule;

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

