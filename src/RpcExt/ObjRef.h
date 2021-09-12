#pragma once

#include "dcom.h"
#include "RpcDef.h"

enum _ObjType
{
	_OBJ_STANDARD = 1,
	_OBJ_HANDLER = 2,
	_OBJ_CUSTOM = 4,
};

// ObjRef

class ObjRef
{
public:
	ObjRef();
	virtual ~ObjRef();

	static HRESULT _CreateInstanceFromBuffer(LPBYTE& pBuffer, ObjRef** ppObjRef);

	virtual HRESULT Read(LPBYTE& pBuffer);
	virtual HRESULT Write(LPBYTE& pBuffer);

	virtual ULONG GetLength() { return sizeof(DWORD) + sizeof(DWORD) + sizeof(IID); }

public:
	DWORD m_signature;
	DWORD m_type;
	IID m_iid;
};

// StdObjRef

class StdObjRef : public ObjRef
{
public:
	StdObjRef();
	~StdObjRef();

	virtual HRESULT Read(LPBYTE& pBuffer);
	HRESULT ReadResAddr(LPBYTE& pBuffer);

	virtual HRESULT Write(LPBYTE& pBuffer);

	virtual ULONG GetLength() { return __super::GetLength() + sizeof(STDOBJREF) + m_size; }

public:
	STDOBJREF m_std;

	DUALSTRINGARRAY* m_psaResAddr;

	ULONG m_size;
};

// HandlerObjRef

class HandlerObjRef : public StdObjRef
{
public:
	HandlerObjRef();
	~HandlerObjRef();

	virtual HRESULT Read(LPBYTE& pBuffer);
	virtual HRESULT Write(LPBYTE& pBuffer);

	virtual ULONG GetLength() { return __super::GetLength() + sizeof(CLSID); }

public:
	CLSID m_clsid;
};

// CustomObjRef

class CustomObjRef : public ObjRef
{
public:
	CustomObjRef();
	~CustomObjRef();

	virtual HRESULT Read(LPBYTE& pBuffer);
	virtual HRESULT Write(LPBYTE& pBuffer);

	virtual ULONG GetLength() { return __super::GetLength() + sizeof(CLSID) + sizeof(ULONG) + sizeof(ULONG) + m_size; }

public:
	CLSID m_clsid;
	ULONG m_extension;
	ULONG m_size;
	BYTE* m_pData;
};

class ConnectInfo
{
public:
	ConnectInfo();

	HRESULT Write(LPBYTE& pBuffer);
	HRESULT Write(IStream* pStream);
	HRESULT Read(IStream* pStream);

	GUID m_cid;
	RPC_SID m_sid;
	OID m_oid;

	int m_port;
	char m_hostName[32];
};

class ExtObjRef : public ObjRef
{
public:
	ExtObjRef(StdObjRef* pObjRef);

	virtual HRESULT Write(LPBYTE& pBuffer);

	virtual ULONG GetLength() { return __super::GetLength() + sizeof(CLSID) + sizeof(ULONG) + m_size; }

public:
	CLSID m_clsid;
	ULONG m_extension;
	ULONG m_size;

	ConnectInfo m_connInfo;
};
