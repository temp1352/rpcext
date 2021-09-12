#pragma once

#include "ScApi.h"
#include "SidGroup.h"
#include <ContainedObject.h>

class __declspec(uuid("9422A9A9-E5A7-45B0-93BB-935C0DB29CB5"))
	ScAccessToken : public CComObjectRootEx<CComMultiThreadModel>,
					public IScAccessToken
{
public:
	ScAccessToken();
	~ScAccessToken();

	static HRESULT _CreateInstance(HANDLE hToken, REFIID riid, void** ppv);

	BEGIN_COM_MAP(ScAccessToken)
		COM_INTERFACE_ENTRY(ScAccessToken)
		COM_INTERFACE_ENTRY(IScAccessToken)
	END_COM_MAP()

	HRESULT Initialize(HANDLE hToken);
	HRESULT UpdateGroups(HANDLE hToken);

	STDMETHOD_(SC_ACCOUNT_TYPE, GetAccountType)();
	STDMETHOD_(LONG, GetUserName)(LPWSTR pBuffer, DWORD size);

	STDMETHOD(Impersonate)();
	STDMETHOD(Revert)();

	STDMETHOD(AccessCheck)(IScDescriptor* pDescriptor, DWORD desiredAccess, DWORD* pAllowed, DWORD* pDenied);

protected:
	HANDLE m_hToken;

	CComBSTR m_userName;

	ContainedObjectT<SidGroup> m_sidGroup;
};

class __declspec(uuid("7BC2C0CE-040E-485D-96BB-102BE95F8742"))
	ScAccessChannel : public CComObjectRootEx<CComMultiThreadModel>,
					  public IScAccessChannel
{
public:
	ScAccessChannel();
	~ScAccessChannel();

	static HRESULT _CreateInstance(REFGUID appId, REFIID riid, void** ppv);

	BEGIN_COM_MAP(ScAccessChannel)
		COM_INTERFACE_ENTRY(ScAccessChannel)
		COM_INTERFACE_ENTRY(IScAccessChannel)
	END_COM_MAP()

	HRESULT Initialize(REFGUID appId);

	STDMETHOD(Logon)(SC_AUTH_INFO* pInfo, SC_AUTH_RESP* pResp);
	STDMETHOD(AccessCheck)(HANDLE hToken, IScAccessToken** ppToken);

	HRESULT AccessCheck(HANDLE hToken);

	SECURITY_DESCRIPTOR* GetSecurityDescriptor() { return m_psd; }

protected:
	ContainedObjectT<ScAcl> m_acl;

	SECURITY_DESCRIPTOR* m_psd;
};