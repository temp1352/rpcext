#pragma once

#include "Sid.h"

class SC_API ScAce
{
public:
	ScAce(IScSid* pSid, DWORD allowed, DWORD denied);
	ScAce(const ScAce& source);
	~ScAce();

	ScAce& operator=(const ScAce& source);

	HRESULT GetAccountName(BSTR* pName, SID_NAME_USE* pNameUse = nullptr);

	IScSid* GetSid() { return pSid; }

protected:
	IScSid* pSid;

public:
	DWORD allowed;
	DWORD denied;
};

class SC_API __declspec(uuid("D591CFBC-2538-46E4-9AC3-3D651E34E6E1"))
	ScAcl : public CComObjectRootEx<CComSingleThreadModel>,
			public IUnknown
{
public:
	ScAcl();
	~ScAcl();

	static HRESULT _CreateInstance(REFIID riid, void** ppv);

	BEGIN_COM_MAP(ScAcl)
		COM_INTERFACE_ENTRY(ScAcl)
		//COM_INTERFACE_ENTRY(IScAceList)
	END_COM_MAP()

	void Swap(ScAcl& other);

	HRESULT Query(HKEY hKeyParent, LPCWSTR keyName);
	HRESULT Save(HKEY hKeyParent, LPCWSTR keyName);

	HRESULT List(SECURITY_DESCRIPTOR* psd);

	HRESULT Insert(SID* pSid, ACCESS_MASK mask, bool bAllowed);
	HRESULT Insert(IScSid* pSid, DWORD allowed, DWORD denied);

	HRESULT Add(SID* pSid, ACCESS_MASK mask, bool bAllowed);
	HRESULT Add(IScSid* pSid, DWORD allowed, DWORD denied);

	ScAce* Lookup(PSID pSid);
	ScAce* Lookup(IScSid* pSid);

	void Clear();

	HRESULT AccessCheck(HANDLE hToken, DWORD desiredAccess, DWORD* pAllowed, DWORD* pDenied);
	HRESULT AccessCheck(IScSidGroup* pGroup, DWORD desiredAccess, DWORD* pAllowed, DWORD* pDenied);

	ULONG GetCount() { return m_count; }
	ScAce& operator[](ULONG i) { return m_pAces[i]; }

private:
	ULONG m_ref;

protected:
	ULONG m_count;
	ULONG m_allocSize;

	ScAce* m_pAces;
};