#include "pch.h"
#include "UserService.h"

// UserService

UserService::UserService()
{

}

STDMETHODIMP UserService::Hello(LPCWSTR name, BSTR* pResult)
{
	if (!pResult)
		return E_POINTER;

	if (!name || !*name)
		return E_INVALIDARG;

	static LPCWSTR hello = L"Hello, ";
	static int helloLen = wcslen(hello);
	int nameLen = wcslen(name);

	*pResult = SysAllocStringLen(hello, helloLen + nameLen);
	wcscpy_s(*pResult + helloLen, nameLen + 1, name);

	return S_OK;
}
