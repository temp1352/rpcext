#include <iostream>

#include <RpcShell.h>
#include <Server_i.h>
#include <Server_i.c>

#include <atlbase.h>
#include <atlcomcli.h>
using namespace ATL;

HRESULT Hello(LPCWSTR name, BSTR* pResult);

int main(int argc, char* argv[])
{
	std::cout << "please input name or exit" << std::endl;

	char input[129] = "";
	

	RE_Initialize(COINIT_APARTMENTTHREADED);

	for (;;)
	{
		std::cin >> input;
		if (!strcmp(input, "exit"))
		{
			break;
		}

		CComBSTR result;
		HRESULT hr = Hello(CA2W(input), &result);
		if (SUCCEEDED(hr))
		{
			printf("result : %s\n", CW2A(result).m_psz);
		}
		else
		{
			printf("error - hr :#%08x\n", hr);
		}
	}

	RE_Uninitialize();
}

HRESULT Hello(LPCWSTR name, BSTR* pResult)
{
	RE_CONNECTINFO connInfo;
	memset(&connInfo, 0, sizeof(RE_CONNECTINFO));
	connInfo.options = RE_CF_ALWAYS_EX;
	connInfo.port = 5000;
	connInfo.serverName = L"localhost";

	CComPtr<IUserService> spService;
	HRESULT hr = RE_CreateInstance(CLSID_UserService, nullptr, CLSCTX_ALL, &connInfo, IID_IUserService, (void**)&spService);
	if (SUCCEEDED(hr))
	{
		hr = spService->Hello(name, pResult);
	}

	return hr;
}