#pragma once

#include "Sid.h"
#include "Acl.h"

class SC_API __declspec(uuid("5401E7C4-4384-46E7-B33B-4F5E3A07C07C"))
	SidGroup : public CComObjectRootEx<CComSingleThreadModel>,
			   public IScSidGroup
{
public:
	SidGroup();
	~SidGroup();

	static HRESULT _CreateInstance(REFIID riid, void** ppv);

	BEGIN_COM_MAP(SidGroup)
		COM_INTERFACE_ENTRY(SidGroup)
		COM_INTERFACE_ENTRY(IScSidGroup)
	END_COM_MAP()

	HRESULT Init(PTOKEN_GROUPS pGroups);

	STDMETHOD(Insert)(PSID pSid);
	STDMETHOD(Insert)(IScSid* pSid);
	STDMETHOD(IsMember)(IScSid* pSid);

	void Clear();

	HRESULT AccessCheck(ScAcl* pList, DWORD desiredAccess, DWORD* pAllowed, DWORD* pDenied);

protected:
	HRESULT Alloc(ULONG nSize, bool bGrow = true);
	int Find(const IScSid* pSid);

protected:
	CComAutoCriticalSection m_cs;

	ULONG m_count;
	ULONG m_allocSize;
	IScSid** m_ppSids;
};