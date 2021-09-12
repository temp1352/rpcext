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

// DLL ��ڵ�
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

// ����ȷ�� DLL �Ƿ���� OLE ж��
STDAPI DllCanUnloadNow()
{
	return _AtlModule.DllCanUnloadNow();
}

// ����һ���๤���Դ������������͵Ķ���
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	if (InlineIsEqualGUID(rclsid, __uuidof(MarshalClassFactory)))
		return MarshalClassFactory::GetClassObject(riid, ppv);

	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - ������ӵ�ϵͳע���
STDAPI DllRegisterServer()
{
	// ע��������Ϳ�����Ϳ��е����нӿ�
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

// DllUnregisterServer - �����ϵͳע������Ƴ�
STDAPI DllUnregisterServer()
{
	HRESULT hr = 0;

	hr = MarshalClassFactory::UnregisterServer();

	/*hr = PsFactoryBuffer::UnregisterServer();*/

	hr = _AtlModule.DllUnregisterServer();

	return hr;
}
