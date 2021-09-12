#include "StdAfx.h"
#include "ObjRef.h"

#include "Marshal.h"

// ObjRef

ObjRef::ObjRef()
{
	m_signature = 0x574f454d; // "MEOW"
	m_type = _OBJ_STANDARD;
	m_iid = IID_NULL;
}

ObjRef::~ObjRef()
{
}

HRESULT ObjRef::_CreateInstanceFromBuffer(LPBYTE& pBuffer, ObjRef** ppObjRef)
{
	ATLASSERT(pBuffer);
	ATLASSERT(ppObjRef);

	LPBYTE p = pBuffer;
	if (*(DWORD*)p != 0x574f454d)
	{
		_logFile.Write(LogCategory::Misc, LogLevel::Details, LogOption::Std, "ObjRef::_CreateInstanceFromBuffer - *(DWORD*)p != 0x574f454d");
		return TYPE_E_INVDATAREAD;
	}

	DWORD type = *(DWORD*)(p + sizeof(DWORD));
	switch (type)
	{
	case _OBJ_STANDARD:
		*ppObjRef = new StdObjRef;
		break;

	case _OBJ_HANDLER:
		*ppObjRef = new HandlerObjRef;
		break;

	case _OBJ_CUSTOM:
		*ppObjRef = new CustomObjRef;
	}

	if (!*ppObjRef)
		return E_OUTOFMEMORY;

	HRESULT hr = (*ppObjRef)->Read(p);
	if (hr != S_OK)
	{
		delete *ppObjRef;
		*ppObjRef = nullptr;
	}

	pBuffer = p;

	return hr;
}

HRESULT ObjRef::Read(LPBYTE& pBuffer)
{
	m_signature = *(DWORD*)pBuffer;
	pBuffer += sizeof(DWORD);

	m_type = *(DWORD*)pBuffer;
	pBuffer += sizeof(DWORD);

	m_iid = *(IID*)pBuffer;
	pBuffer += sizeof(IID);

	return S_OK;
}

HRESULT ObjRef::Write(LPBYTE& pBuffer)
{
	*(DWORD*)pBuffer = m_signature;
	pBuffer += sizeof(DWORD);

	*(DWORD*)pBuffer = m_type;
	pBuffer += sizeof(DWORD);

	*(IID*)pBuffer = m_iid;
	pBuffer += sizeof(IID);

	return S_OK;
}

// StdObjRef

StdObjRef::StdObjRef()
{
	memset(&m_std, 0, sizeof(STDOBJREF));

	m_psaResAddr = nullptr;
	m_size = 0;
}

StdObjRef::~StdObjRef()
{
	if (m_psaResAddr)
	{
		free(m_psaResAddr);
	}
}

HRESULT StdObjRef::Read(LPBYTE& pBuffer)
{
	HRESULT hr = __super::Read(pBuffer);

	memcpy(&m_std, pBuffer, sizeof(STDOBJREF));
	pBuffer += sizeof(STDOBJREF);

	hr = ReadResAddr(pBuffer);

	return hr;
}

HRESULT StdObjRef::ReadResAddr(LPBYTE& pBuffer)
{
	USHORT wNumEntries = *(USHORT*)pBuffer;

	m_size = wNumEntries * sizeof(WCHAR) + sizeof(USHORT) * 2;
	if (m_size)
	{
		m_psaResAddr = (DUALSTRINGARRAY*)malloc(m_size);
		if (!m_psaResAddr)
			return E_OUTOFMEMORY;

		memcpy(m_psaResAddr, pBuffer, m_size);
	}

	pBuffer += m_size;

#ifdef _DEBUG
	/*_logFile.Write(3, "ReadResAddr");
	_logFile.WriteBinary(3, m_psaResAddr, m_size);
	_logFile.Write(m_psaResAddr, m_size);*/
#endif

	return S_OK;
}

HRESULT StdObjRef::Write(LPBYTE& pBuffer)
{
	HRESULT hr = __super::Write(pBuffer);

	memcpy(pBuffer, &m_std, sizeof(STDOBJREF));
	pBuffer += sizeof(STDOBJREF);

	if (m_psaResAddr)
		memcpy(pBuffer, m_psaResAddr, m_size);
	else
		*(DWORD*)pBuffer = 0;

	return hr;
}

// HandlerObjRef

HandlerObjRef::HandlerObjRef()
{
	m_type = _OBJ_HANDLER;
	m_clsid = CLSID_NULL;
}

HandlerObjRef::~HandlerObjRef()
{
}

HRESULT HandlerObjRef::Read(LPBYTE& pBuffer)
{
	HRESULT hr = __super::Read(pBuffer);

	memcpy(&m_std, pBuffer, sizeof(STDOBJREF));
	pBuffer += sizeof(STDOBJREF);

	memcpy(&m_clsid, pBuffer, sizeof(CLSID));
	pBuffer += sizeof(CLSID);

	hr = ReadResAddr(pBuffer);

	return hr;
}

HRESULT HandlerObjRef::Write(LPBYTE& pBuffer)
{
	HRESULT hr = __super::Write(pBuffer);

	memcpy(pBuffer, &m_std, sizeof(STDOBJREF));
	pBuffer += sizeof(STDOBJREF);

	*(CLSID*)pBuffer = m_clsid;

	if (m_psaResAddr)
		memcpy(pBuffer, m_psaResAddr, m_size);
	else
		*(DWORD*)pBuffer = 0;

	return hr;
}

// CustomObjRef

CustomObjRef::CustomObjRef()
{
	m_type = _OBJ_CUSTOM;
	m_clsid = CLSID_NULL;
	m_extension = 0;
	m_size = 0;
	m_pData = nullptr;
}

CustomObjRef::~CustomObjRef()
{
	if (m_pData)
	{
		free(m_pData);
	}
}

HRESULT CustomObjRef::Read(LPBYTE& pBuffer)
{
	HRESULT hr = __super::Read(pBuffer);

	memcpy(&m_clsid, pBuffer, sizeof(CLSID));
	pBuffer += sizeof(CLSID);

	m_extension = *(ULONG*)pBuffer;
	pBuffer += sizeof(ULONG);

	m_size = *(ULONG*)pBuffer;
	pBuffer += sizeof(ULONG);

	if (m_size)
	{
		m_pData = (LPBYTE)malloc(m_size);
		if (!m_pData)
			return E_OUTOFMEMORY;

		memcpy(m_pData, pBuffer, m_size);
	}

	return S_OK;
}

HRESULT CustomObjRef::Write(LPBYTE& pBuffer)
{
	HRESULT hr = __super::Write(pBuffer);

	*(CLSID*)pBuffer = m_clsid;
	pBuffer += sizeof(CLSID);

	*(ULONG*)pBuffer = m_extension;
	pBuffer += sizeof(ULONG);

	*(ULONG*)pBuffer = m_size;
	pBuffer += sizeof(ULONG);

	if (m_pData)
	{
		memcpy(pBuffer, m_pData, m_size);
	}

	return hr;
}

// ConnectInfo

ConnectInfo::ConnectInfo()
{
	m_cid = GUID_NULL;
	m_sid = GUID_NULL;
	m_oid = 0;
	memset(m_hostName, 0, sizeof(m_hostName));
	m_port = 0;
}

HRESULT ConnectInfo::Write(LPBYTE& pBuffer)
{
	*(GUID*)pBuffer = m_cid;
	pBuffer += sizeof(GUID);

	*(RPC_SID*)pBuffer = m_sid;
	pBuffer += sizeof(RPC_SID);

	*(OID*)pBuffer = m_oid;
	pBuffer += sizeof(OID);

	*(int*)pBuffer = m_port;
	pBuffer += sizeof(int);

	memcpy(pBuffer, m_hostName, sizeof(m_hostName));
	pBuffer += sizeof(m_hostName);

	return S_OK;
}

HRESULT ConnectInfo::Write(IStream* pStream)
{
	HRESULT hr = pStream->Write(&m_cid, sizeof(GUID), nullptr);
	if (FAILED(hr))
		return hr;

	hr = pStream->Write(&m_sid, sizeof(RPC_SID), nullptr);
	if (FAILED(hr))
		return hr;

	hr = pStream->Write(&m_oid, sizeof(OID), nullptr);
	if (FAILED(hr))
		return hr;

	hr = pStream->Write(&m_port, sizeof(int), nullptr);
	if (FAILED(hr))
		return hr;

	hr = pStream->Write(m_hostName, sizeof(m_hostName), nullptr);

	return hr;
}

HRESULT ConnectInfo::Read(IStream* pStream)
{
	HRESULT hr = pStream->Read(&m_cid, sizeof(GUID), nullptr);
	if (FAILED(hr))
		return hr;

	hr = pStream->Read(&m_sid, sizeof(RPC_SID), nullptr);
	if (FAILED(hr))
		return hr;

	hr = pStream->Read(&m_oid, sizeof(OID), nullptr);
	if (FAILED(hr))
		return hr;

	hr = pStream->Read(&m_port, sizeof(int), nullptr);
	if (FAILED(hr))
		return hr;

	hr = pStream->Read(m_hostName, sizeof(m_hostName), nullptr);

	return hr;
}

// ExtObjRef

ExtObjRef::ExtObjRef(StdObjRef* pObjRef)
{
	m_type = _OBJ_CUSTOM;
	m_iid = pObjRef->m_iid;
	m_clsid = pObjRef->m_type == _OBJ_HANDLER ? static_cast<HandlerObjRef*>(pObjRef)->m_clsid : __uuidof(MarshalClassFactory);
	m_extension = 0;
	m_size = pObjRef->GetLength() - sizeof(CLSID) - sizeof(ULONG) - sizeof(DWORD);

	m_connInfo.m_cid = GUID_NULL;
	m_connInfo.m_sid = GUID_NULL;
	m_connInfo.m_oid = pObjRef->m_std.oid;
}

HRESULT ExtObjRef::Write(LPBYTE& pBuffer)
{
	HRESULT hr = __super::Write(pBuffer);

	*(CLSID*)pBuffer = m_clsid;
	pBuffer += sizeof(CLSID);

	*(ULONG*)pBuffer = m_extension;
	pBuffer += sizeof(ULONG);

	*(ULONG*)pBuffer = m_size;
	pBuffer += sizeof(ULONG);

	m_connInfo.Write(pBuffer);

	return hr;
}