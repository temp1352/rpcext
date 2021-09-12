#include "StdAfx.h"
#include "Resolver.h"

#include "dcom.h"

Resolver::Resolver()
{
}

Resolver::~Resolver()
{
}

void __RPC_FAR* __RPC_USER midl_user_allocate(size_t len)
{
	return (malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR* ptr)
{
	free(ptr);
}

HRESULT _GetConnectInfo(LPBYTE pBuffer, BSTR* pDoMain)
{
	short n = *(short*)pBuffer;

	return 0;
}

HRESULT _Test(OXID* pOxid)
{
	RPC_WSTR pszStringBinding = nullptr;
	RPC_STATUS status = RpcStringBindingCompose((RPC_WSTR)L"00000142-0000-0000-C000-000000000046", (RPC_WSTR)L"ncacn_ip_tcp", nullptr, (RPC_WSTR)L"135", nullptr, &pszStringBinding);
	if (status != RPC_S_OK)
		return 0;

	handle_t hRpc = 0;
	status = RpcBindingFromStringBinding(pszStringBinding, &hRpc);
	if (status != RPC_S_OK)
		return 0;

	WCHAR szProtseqs[] = L"ncacn_ip_tcp";
	USHORT cProtseqs = wcslen(szProtseqs);
	DUALSTRINGARRAY* pBindings = nullptr;
	IPID ipid = GUID_NULL;
	DWORD dwAuth = 0;
	/*error_status_t s = ResolveOxid(hRpc, pOxid, cProtseqs, (USHORT*)szProtseqs, &pBindings, &ipid, &dwAuth);

	error_status_t s2 = ResolveOxid(hRpc, pOxid, 0, nullptr, &pBindings, &ipid, &dwAuth);*/

	/*IPID ipid = IID_NULL;
	IID iid = IID_NULL;
	HRESULT hr = 0;
	MInterfacePointer * pPointer = nullptr;
	status = RemQueryInterface2(hRpc, ipid, 1, &iid, &hr, &pPointer);*/

	return 0;
}