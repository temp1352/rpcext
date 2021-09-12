#pragma once

#include "RpcShell_i.h"

#include "RpcDef.h"
#include "RpcMessage.h"

// RpcCommand

class __declspec(uuid("2F09EB85-E1AA-4444-8DD7-2E8E14E03B5F"))
	RpcCommand : public CComObjectRootEx<CComMultiThreadModel>,
				 public IUnknown
{
public:
	RpcCommand(RpcCommandType type);
	virtual ~RpcCommand();

	BEGIN_COM_MAP(RpcCommand)
		COM_INTERFACE_ENTRY(RpcCommand)
	END_COM_MAP()

	virtual void Serialize(RpcBuffer* pBuffer);
	virtual void Deserialize(RpcBuffer* pBuffer);

	void SetExpire(int timeSpan);

public:
	const RpcCommandType m_type;

	RPC_COOKIE m_cookie;
	RPC_EVENTID m_eventId;

	ULONGLONG m_expireTime;

	static int _defaultExpireSpan;
};

// RpcRespCommand

class RpcRespCommand : public RpcCommand
{
public:
	RpcRespCommand(RpcCommandType type);
	~RpcRespCommand();

	virtual void Serialize(RpcBuffer* pBuffer);
	virtual void Deserialize(RpcBuffer* pBuffer);

	HRESULT SerializeErrorInfo(RpcBuffer* pBuffer);
	HRESULT DeserializeErrorInfo(RpcBuffer* pBuffer);

	void CacheError();
	void ReportError();

	IErrorInfo* GetErrorInfo() { return m_pErrorInfo; }

public:
	HRESULT m_hRes;
protected:
	IErrorInfo* m_pErrorInfo;
};

// RpcBindingRequest

enum RPC_BINDING_FLAG
{
	RPC_BF_INTEGRATED_SECURITY = 0x01,
};

class RpcBindingRequest : public RpcCommand
{
public:
	RpcBindingRequest();

	virtual void Serialize(RpcBuffer* pBuffer);
	virtual void Deserialize(RpcBuffer* pBuffer);

	virtual void ExchangeAccountInfo(RpcBuffer* pBuffer, BOOL bWrite);

public:
	WORD m_wMajorVer;
	WORD m_wMinorVer;

	DWORD m_flags;

	IID m_iid;
	CLSID m_clsid;

	OID m_oid;

	GUID m_cid;
	RPC_SID m_sid;

	CComBSTR m_domain;
	CComBSTR m_referer;
	CComBSTR m_params;

	CComBSTR m_authCode;
	CComBSTR m_refreshToken;

	CComBSTR m_userName;
	CComBSTR m_password;
};

// RpcBindingResp

class RpcBindingResp : public RpcRespCommand
{
public:
	RpcBindingResp();

	virtual void Serialize(RpcBuffer* pBuffer);
	virtual void Deserialize(RpcBuffer* pBuffer);

public:
	RPC_SID m_sid;
	OID m_oid;
	ULONG m_aid;

	CComBSTR m_refreshToken;
};

// RpcAuthRequest

class RpcAuthRequest : public RpcCommand
{
public:
	RpcAuthRequest();
};

// RpcAuthResp

class RpcAuthResp : public RpcRespCommand
{
public:
	RpcAuthResp();
};

// RpcInvokeRequest

class RpcInvokeRequest : public RpcCommand
{
public:
	RpcInvokeRequest();

	virtual void Serialize(RpcBuffer* pBuffer);
	virtual void Deserialize(RpcBuffer* pBuffer);

public:
	OID m_oid;
	IID m_iid;

	LONG m_itfPtrCount;

	RpcMessage m_message;
};

// RpcInvokeResp

class RpcInvokeResp : public RpcRespCommand
{
public:
	RpcInvokeResp();
	~RpcInvokeResp();

	virtual void Serialize(RpcBuffer* pBuffer);
	virtual void Deserialize(RpcBuffer* pBuffer);

public:
	IID m_iid;

	LONG m_itfPtrCount;

	RpcMessage m_message;
};

// RpcStateRequest

class RpcStateRequest : public RpcCommand
{
public:
	RpcStateRequest();

	virtual void Serialize(RpcBuffer* pBuffer);
	virtual void Deserialize(RpcBuffer* pBuffer);

public:
	RPC_COOKIE m_invokeCookie;
};

// RpcStateResp

class RpcStateResp : public RpcRespCommand
{
public:
	RpcStateResp();
};

// RpcQIRequest

class RpcQIRequest : public RpcCommand
{
public:
	RpcQIRequest();

	virtual void Serialize(RpcBuffer* pBuffer);
	virtual void Deserialize(RpcBuffer* pBuffer);

public:
	OID m_oid;
	IID m_iid;
};

// RpcQIResp

class RpcQIResp : public RpcRespCommand
{
public:
	RpcQIResp();
};

// RpcActiveRequest

class RpcActiveRequest : public RpcCommand
{
public:
	RpcActiveRequest();
};

// CRpcActiveTestResp

class RpcActiveResp : public RpcCommand
{
public:
	RpcActiveResp();
};

// RpcFreeRequest

class RpcFreeRequest : public RpcCommand
{
public:
	RpcFreeRequest();

	virtual void Serialize(RpcBuffer* pBuffer);
	virtual void Deserialize(RpcBuffer* pBuffer);

public:
	OID m_oid;
};

// RpcFreeResp

class RpcFreeResp : public RpcRespCommand
{
public:
	RpcFreeResp();
	~RpcFreeResp();
};

class RpcCommandFactory
{
public:
	static RpcCommand* _Create(RpcCommandType type, bool addRef);
};