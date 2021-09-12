#pragma once

class AnonymousUser
{
public:
	AnonymousUser();
	~AnonymousUser();

	static HRESULT OpenToken(HANDLE* phToken);
	static HRESULT GetAuthInfo(CString& user, CString& password);

	static HRESULT Register();
	static HRESULT Unregister();

	static HRESULT CreateUser(LPCWSTR user, LPCWSTR password);
	static void RandomPassword(LPWSTR result, DWORD size);

	static HRESULT BuildAuthInfo(LPCWSTR user, LPCWSTR pPassword, CString& authInfo);
	static HRESULT ParseAuthInfo(LPCWSTR authInfo, CString& user, CString& password);
};
