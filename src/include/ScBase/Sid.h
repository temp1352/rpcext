#pragma once

#include "ScApi.h"

class __declspec(uuid("E7483D11-BB4B-4E1E-A2EA-4912B42E0567"))
	ScSid : public CComObjectRootEx<CComSingleThreadModel>,
			public IScSid
{
public:
	ScSid();
	~ScSid();

	static HRESULT _Create(PSID pSid, REFIID riid, void** ppv);
	static HRESULT _Create(LPCWSTR accountName, REFIID riid, void** ppv);

	BEGIN_COM_MAP(ScSid)
		COM_INTERFACE_ENTRY(ScSid)
		COM_INTERFACE_ENTRY(IScSid)
	END_COM_MAP()

	bool operator==(const PSID pSid);
	bool operator==(const ScSid& other);

	HRESULT Init(PSID pSid);
	HRESULT Init(LPCWSTR accountName);

	BOOL Equal(PSID pSid);
	int Compare(PSID pSid);

	STDMETHOD_(PSID, GetPSID)() { return (SID*)m_buffer; }
	STDMETHOD_(ULONG, GetLength)() { return m_length; }

	STDMETHOD_(BOOL, Equal)(const IScSid* pSid);
	STDMETHOD_(int, Compare)(const IScSid* pSid);

	STDMETHOD(GetAccountName)(BSTR* pName, SID_NAME_USE* pNameUse = nullptr);

	static HRESULT _GetAccountName(PSID pSid, BSTR* pName, SID_NAME_USE* pNameUse = nullptr);

protected:
	ULONG m_length;
	BYTE m_buffer[SECURITY_MAX_SID_SIZE];
};