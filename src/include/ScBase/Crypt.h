#pragma once

#include "ScApi.h"
#include <atlcrypt.h>
#include <atlenc.h>

class SC_API ScCrypt
{
public:
	ScCrypt();

	HRESULT Initialize(LPCTSTR text = nullptr);

	HRESULT Encrypt(LPCSTR text, DWORD len, CStringA& result);
	HRESULT Decrypt(LPCSTR text, DWORD len, CStringA& result);

	HRESULT Encrypt(LPCTSTR text, DWORD len, CString& result);
	HRESULT Decrypt(LPCTSTR text, DWORD len, CString& result);

	HRESULT Encrypt(const CString& text, CString& result);
	HRESULT Decrypt(const CString& text, CString& result);

	HRESULT HexEncode(const BYTE* srcData, int nSrcLen, CStringA& result);
	HRESULT HexDecode(const BYTE* srcData, int nSrcLen, CStringA& result);

protected:
	CCryptProv m_prov;
	CCryptMD5Hash m_hash;
	CCryptDerivedKey m_key;
};