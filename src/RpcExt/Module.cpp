#include "stdafx.h"
#include "resource.h"

#include "RpcExt.h"
#include "Marshal.h"
#include "PsFactoryBuffer.h"

class RpcExtModule : public CAtlDllModuleT<RpcExtModule>
{
public:
	DECLARE_LIBID(LIBID_RpcExt)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_APPID, "{973AABC8-ADBE-44CF-8ADF-AED30230F085}")
};

RpcExtModule _AtlModule;

// DLL 入口点
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		_logFile.Term();
		_rpcExt._Uninitialize();
	}

	return _AtlModule.DllMain(dwReason, lpReserved);
}

// 用于确定 DLL 是否可由 OLE 卸载
STDAPI DllCanUnloadNow()
{
	return _AtlModule.DllCanUnloadNow();
}

// 返回一个类工厂以创建所请求类型的对象
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	if (InlineIsEqualGUID(rclsid, __uuidof(MarshalClassFactory)))
		return MarshalClassFactory::GetClassObject(riid, ppv);

	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - 将项添加到系统注册表
STDAPI DllRegisterServer()
{
	// 注册对象、类型库和类型库中的所有接口
	HRESULT hr = _AtlModule.DllRegisterServer();
	if (FAILED(hr))
		return hr;

	/*hr = PsFactoryBuffer::RegisterServer();
	if(hr != S_OK)
		return hr;*/

	hr = MarshalClassFactory::RegisterServer();
	if (hr != S_OK)
		return hr;

	return hr;
}

// DllUnregisterServer - 将项从系统注册表中移除
STDAPI DllUnregisterServer()
{
	HRESULT hr = 0;

	hr = MarshalClassFactory::UnregisterServer();

	/*hr = PsFactoryBuffer::UnregisterServer();*/

	hr = _AtlModule.DllUnregisterServer();

	return hr;
}
