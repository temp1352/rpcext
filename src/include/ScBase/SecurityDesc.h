#pragma once

#include "Sid.h"

class TempAce
{
public:
	TempAce();
	~TempAce();

	HRESULT Create(ScSid* pSid, DWORD dwAccessMask, bool allowed, BYTE aceFlags);

	void* GetAce() { return m_pAce; }
	ULONG GetLength() { return m_length; }

protected:
	void* m_pAce;
	ULONG m_length;
};

class ScDacl
{
public:
	ScDacl();
	~ScDacl();

	HRESULT Init(ULONG length);

	HRESULT AddAllowedAce(ScSid* pSid, ACCESS_MASK dwAccessMask, BYTE aceFlags = 0);
	HRESULT AddDeniedAce(ScSid* pSid, ACCESS_MASK dwAccessMask, BYTE aceFlags = 0);
	HRESULT AddAce(LPVOID pAce, ULONG length);

	void Read(Reader& r);
	void Write(Writer& w);

	ACL* GetAcl() { return m_pAcl; }
	ULONG GetLength() { return m_length; }

protected:
	ACL* m_pAcl;
	ULONG m_length;
};

class ScSecurityDesc
{
public:
	ScSecurityDesc();
	~ScSecurityDesc();

	HRESULT Init();
	HRESULT SetDacl(ScDacl& dacl, bool bDefaulted);

	PSECURITY_DESCRIPTOR GetPSD() { return m_psd; }

protected:
	PSECURITY_DESCRIPTOR m_psd;
};