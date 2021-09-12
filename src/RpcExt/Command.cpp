#include "StdAfx.h"
#include "Command.h"

#include "RpcBuffer.h"

#include "RpcExt.h"
#include "ErrorInfo.h"

#include "ScBase/Crypt.h"

RpcCommand::RpcCommand(RpcCommandType type)
	: m_type(type)
{
	m_cookie = 0;
	m_eventId = RPC_INVALID_EVENTID;

	m_expireTime = 0;
}

RpcCommand::~RpcCommand()
{
	//AtlTrace("RpcCommand::~RpcCommand\n");
}

int RpcCommand::_defaultExpireSpan = 60000;

void RpcCommand::Serialize(RpcBuffer* pBuffer)
{
	pBuffer->WriteInt32(m_type);
	pBuffer->WriteInt32(m_cookie);
	pBuffer->WriteInt32(m_eventId);
}

void RpcCommand::Deserialize(RpcBuffer* pBuffer)
{
	m_cookie = pBuffer->ReadInt32();
	m_eventId = pBuffer->ReadInt32();
}

void RpcCommand::SetExpire(int timeSpan)
{
	if (timeSpan == -1)
	{
		timeSpan = _defaultExpireSpan;
	}

	m_expireTime = GetTickCount64() + timeSpan;
}

// RpcRespCommand

RpcRespCommand::RpcRespCommand(RpcCommandType type)
	: RpcCommand(type)
{
	m_hRes = E_UNEXPECTED;
	m_pErrorInfo = nullptr;
}

RpcRespCommand::~RpcRespCommand()
{
	ComApi::Release(&m_pErrorInfo);
}

void RpcRespCommand::Serialize(RpcBuffer* pBuffer)
{
	__super::Serialize(pBuffer);

	pBuffer->WriteInt32(m_hRes);

	SerializeErrorInfo(pBuffer);
}

void RpcRespCommand::Deserialize(RpcBuffer* pBuffer)
{
	__super::Deserialize(pBuffer);

	m_hRes = pBuffer->ReadInt32();

	DeserializeErrorInfo(pBuffer);
}

HRESULT RpcRespCommand::SerializeErrorInfo(RpcBuffer* pBuffer)
{
	if (m_pErrorInfo)
	{
		pBuffer->WriteInt16(1);

		GUID guid = GUID_NULL;
		m_pErrorInfo->GetGUID(&guid);
		pBuffer->WriteGUID(guid);

		DWORD context = 0;
		m_pErrorInfo->GetHelpContext(&context);
		pBuffer->WriteInt32(context);

		CComBSTR source;
		m_pErrorInfo->GetSource(&source);
		pBuffer->WriteString(source);

		CComBSTR description;
		m_pErrorInfo->GetDescription(&description);
		pBuffer->WriteString(description);

		CComBSTR helpFile;
		m_pErrorInfo->GetHelpFile(&helpFile);
		pBuffer->WriteString(helpFile);
	}
	else
	{
		pBuffer->WriteInt16(0);
	}

	return 0;
}

HRESULT RpcRespCommand::DeserializeErrorInfo(RpcBuffer* pBuffer)
{
	short n = pBuffer->ReadInt16();
	if (n > 0 && !m_pErrorInfo)
	{
		ErrorInfo::_CreateInstance(pBuffer, __uuidof(IErrorInfo), (void**)&m_pErrorInfo);
	}

	return 0;
}

void RpcRespCommand::CacheError()
{
	ComApi::Release(&m_pErrorInfo);

	auto hr = ::GetErrorInfo(0, &m_pErrorInfo);
}

void RpcRespCommand::ReportError()
{
	if (m_pErrorInfo)
	{
		SetErrorInfo(0, m_pErrorInfo);
	}
}

// RpcBindingRequest

RpcBindingRequest::RpcBindingRequest()
	: RpcCommand(RPC_BINDING)
{
	m_wMajorVer = 1;
	m_wMinorVer = 0;

	m_flags = 0;

	m_iid = IID_NULL;
	m_clsid = CLSID_NULL;
	m_oid = 0;

	m_cid = GUID_NULL;
	m_sid = GUID_NULL;
}

void RpcBindingRequest::Serialize(RpcBuffer* pBuffer)
{
	__super::Serialize(pBuffer);

	pBuffer->WriteInt16(m_wMinorVer);
	pBuffer->WriteInt16(m_wMinorVer);

	pBuffer->WriteInt32(m_flags);

	pBuffer->WriteBytes(&m_iid, sizeof(IID));
	pBuffer->WriteBytes(&m_clsid, sizeof(CLSID));

	pBuffer->WriteInt64(m_oid);

	pBuffer->WriteGUID(m_cid);
	pBuffer->WriteGUID(m_sid);

	pBuffer->WriteString(m_domain);
	pBuffer->WriteString(m_referer);
	pBuffer->WriteString(m_params);

	pBuffer->WriteString(m_authCode);
	pBuffer->WriteString(m_refreshToken);

	ExchangeAccountInfo(pBuffer, TRUE);
}

void RpcBindingRequest::Deserialize(RpcBuffer* pBuffer)
{
	__super::Deserialize(pBuffer);

	m_wMajorVer = pBuffer->ReadInt16();
	m_wMinorVer = pBuffer->ReadInt16();

	m_flags = pBuffer->ReadInt32();

	pBuffer->ReadBytes(&m_iid, sizeof(IID));
	pBuffer->ReadBytes(&m_clsid, sizeof(CLSID));

	m_oid = pBuffer->ReadInt64();

	pBuffer->ReadGUID(&m_cid);
	pBuffer->ReadGUID(&m_sid);

	pBuffer->ReadBSTR(&m_domain);
	pBuffer->ReadBSTR(&m_referer);
	pBuffer->ReadBSTR(&m_params);

	pBuffer->ReadBSTR(&m_authCode);
	pBuffer->ReadBSTR(&m_refreshToken);

	ExchangeAccountInfo(pBuffer, FALSE);
}

static int _GetLength(const TCHAR* pch)
{
	return (*pch - '0') * 10 + (*(pch + 1) - '0');
}

void RpcBindingRequest::ExchangeAccountInfo(RpcBuffer* pBuffer, BOOL bWrite)
{
	ScCrypt crypt;
	HRESULT hr = crypt.Initialize(_T("&!ixwo01~#@"));
	if (FAILED(hr))
	{
		XTL_THROW(hr);
	}

	if (bWrite)
	{
		CString str, str2;
		str.Format(L"%Ts%Ts%02d%02d",
			m_userName ? m_userName.m_str : L"",
			m_password ? m_password.m_str : L"",
			m_userName.Length(), m_password.Length());
		hr = crypt.Encrypt(str, str2);
		if (FAILED(hr))
		{
			XTL_THROW(hr);
		}

		pBuffer->WriteString(str2);
	}
	else
	{
		CComBSTR str;
		pBuffer->ReadBSTR(&str);

		CString str2;
		hr = crypt.Decrypt(str, str.Length(), str2);
		if (FAILED(hr))
		{
			XTL_THROW(hr);
		}

		int len = str2.GetLength();
		if (len < 4)
		{
			XTL_THROW(E_FAIL);
		}

		int len1 = _GetLength(str2.GetString() + len - 4);
		int len2 = _GetLength(str2.GetString() + len - 2);

		if (len != len1 + len2 + 4)
		{
			XTL_THROW(E_FAIL);
		}

		m_userName = str2.Left(len1);
		m_password = str2.Mid(len1, len2);
	}
}

// RpcBindingResp

RpcBindingResp::RpcBindingResp()
	: RpcRespCommand(RPC_BINDING_RESP)
{
	m_sid = GUID_NULL;
	m_oid = 0;
	m_aid = 0;
}

void RpcBindingResp::Serialize(RpcBuffer* pBuffer)
{
	__super::Serialize(pBuffer);

	pBuffer->WriteGUID(m_sid);
	pBuffer->WriteInt64(m_oid);
	pBuffer->WriteInt64(m_aid);

	pBuffer->WriteString(m_refreshToken);
}

void RpcBindingResp::Deserialize(RpcBuffer* pBuffer)
{
	__super::Deserialize(pBuffer);

	pBuffer->ReadGUID(&m_sid);
	m_oid = pBuffer->ReadInt64();
	m_aid = pBuffer->ReadInt64();

	pBuffer->ReadBSTR(&m_refreshToken);
}

// RpcAuthRequest

RpcAuthRequest::RpcAuthRequest()
	: RpcCommand(RPC_AUTHENTICATE)
{
}

// RpcAuthResp

RpcAuthResp::RpcAuthResp()
	: RpcRespCommand(RPC_AUTHENTICATE_RESP)
{
}

// RpcInvokeRequest

RpcInvokeRequest::RpcInvokeRequest()
	: RpcCommand(RPC_INVOKE)
{
	m_oid = 0;
	m_iid = IID_NULL;
	m_itfPtrCount = 0;
}

ULONG _CheckSum(BYTE* p, int len)
{
	ULONG sum = 0;

	for (; len > 0; --len)
	{
		sum += *p++;
	}

	return sum;
}

void RpcInvokeRequest::Serialize(RpcBuffer* pBuffer)
{
	__super::Serialize(pBuffer);

	pBuffer->WriteInt64(m_oid);

	pBuffer->WriteGUID(m_iid);

	pBuffer->WriteInt32(m_itfPtrCount);

	m_message.Write(pBuffer);

	pBuffer->WriteInt32(_CheckSum((BYTE*)m_message.Buffer, m_message.BufferLength));
}

void RpcInvokeRequest::Deserialize(RpcBuffer* pBuffer)
{
	__super::Deserialize(pBuffer);

	m_oid = pBuffer->ReadInt64();

	pBuffer->ReadGUID(&m_iid);

	m_itfPtrCount = pBuffer->ReadInt32();

	m_message.Read(pBuffer);

	if (pBuffer->ReadInt32() != _CheckSum((BYTE*)m_message.Buffer, m_message.BufferLength))
	{
		XTL_THROW(E_COMMAND_ERROR);
	}
}

// RpcInvokeResp

RpcInvokeResp::RpcInvokeResp()
	: RpcRespCommand(RPC_INVOKE_RESP)
{
	m_iid = IID_NULL;

	m_itfPtrCount = 0;
}

RpcInvokeResp::~RpcInvokeResp()
{
}

void RpcInvokeResp::Serialize(RpcBuffer* pBuffer)
{
	__super::Serialize(pBuffer);

	pBuffer->WriteGUID(m_iid);

	pBuffer->WriteInt32(m_itfPtrCount);

	m_message.Write(pBuffer);
}

void RpcInvokeResp::Deserialize(RpcBuffer* pBuffer)
{
	__super::Deserialize(pBuffer);

	pBuffer->ReadGUID(&m_iid);

	m_itfPtrCount = pBuffer->ReadInt32();

	m_message.Read(pBuffer);
}

// RpcStateRequest

RpcStateRequest::RpcStateRequest()
	: RpcCommand(RPC_QUERY_STATE)
{
}

void RpcStateRequest::Serialize(RpcBuffer* pBuffer)
{
	__super::Serialize(pBuffer);

	pBuffer->WriteInt32(m_invokeCookie);
}

void RpcStateRequest::Deserialize(RpcBuffer* pBuffer)
{
	__super::Deserialize(pBuffer);

	m_invokeCookie = pBuffer->ReadInt32();
}

// RpcStateResp

RpcStateResp::RpcStateResp()
	: RpcRespCommand(RPC_QUERY_STATE_RESP)
{
}

// RpcQIRequest

RpcQIRequest::RpcQIRequest()
	: RpcCommand(RPC_QUERY_INTERFACE)
{
	m_oid = 0;
	m_iid = IID_NULL;
}

void RpcQIRequest::Serialize(RpcBuffer* pBuffer)
{
	__super::Serialize(pBuffer);

	pBuffer->WriteInt64(m_oid);

	pBuffer->WriteGUID(m_iid);
}

void RpcQIRequest::Deserialize(RpcBuffer* pBuffer)
{
	__super::Deserialize(pBuffer);

	m_oid = pBuffer->ReadInt64();

	pBuffer->ReadGUID(&m_iid);
}

// RpcQIResp

RpcQIResp::RpcQIResp()
	: RpcRespCommand(RPC_QUERY_INTERFACE_RESP)
{
}

// RpcActiveRequest

RpcActiveRequest::RpcActiveRequest()
	: RpcCommand(RPC_ACTIVE_TEST)
{
}

// RpcActiveResp

RpcActiveResp::RpcActiveResp()
	: RpcCommand(RPC_ACTIVE_TEST_RESP)
{
}

// RpcFreeRequest

RpcFreeRequest::RpcFreeRequest()
	: RpcCommand(RPC_FREE)
{
	m_oid = 0;
}

void RpcFreeRequest::Serialize(RpcBuffer* pBuffer)
{
	__super::Serialize(pBuffer);

	pBuffer->WriteInt64(m_oid);
}

void RpcFreeRequest::Deserialize(RpcBuffer* pBuffer)
{
	__super::Deserialize(pBuffer);

	m_oid = pBuffer->ReadInt64();
}

// RpcFreeResp

RpcFreeResp::RpcFreeResp()
	: RpcRespCommand(RPC_FREE_RESP)
{
	AtlTrace("RpcFreeResp::RpcFreeResp\n");
}

RpcFreeResp::~RpcFreeResp()
{
	AtlTrace("RpcFreeResp::~RpcFreeResp\n");
}

template <class T>
RpcCommand* _CreateCommand()
{
	return new CComObject<T>;
}

struct _RpcCommandEntry
{
	DWORD type;
	RpcCommand* (*pfnCreate)();
} _entries[] = {
	{ RPC_BINDING,	_CreateCommand<RpcBindingRequest> },
	{ RPC_BINDING_RESP,	_CreateCommand<RpcBindingResp> },
	{ RPC_AUTHENTICATE,	_CreateCommand<RpcAuthRequest> },
	{ RPC_AUTHENTICATE_RESP,	_CreateCommand<RpcAuthResp> },
	{ RPC_INVOKE,	_CreateCommand<RpcInvokeRequest> },
	{ RPC_INVOKE_RESP,	_CreateCommand<RpcInvokeResp> },
	{ RPC_QUERY_STATE,	_CreateCommand<RpcStateRequest> },
	{ RPC_QUERY_STATE_RESP,	_CreateCommand<RpcStateResp> },
	{ RPC_QUERY_INTERFACE,	_CreateCommand<RpcQIRequest> },
	{ RPC_QUERY_INTERFACE_RESP,	_CreateCommand<RpcQIResp> },
	{ RPC_ACTIVE_TEST,	_CreateCommand<RpcActiveRequest> },
	{ RPC_ACTIVE_TEST_RESP,	_CreateCommand<RpcActiveResp> },
	{ RPC_FREE,	_CreateCommand<RpcFreeRequest> },
	{ RPC_FREE_RESP,	_CreateCommand<RpcFreeResp> },
};

_RpcCommandEntry* _GetCommandEntry(DWORD type)
{
	for (int i = 0; i < _countof(_entries); ++i)
	{
		if (type == _entries[i].type)
		{
			return &_entries[i];
		}
	}

	return nullptr;
}

RpcCommand* RpcCommandFactory::_Create(RpcCommandType type, bool addRef)
{
	_RpcCommandEntry* pEntry = _GetCommandEntry(type);
	if (!pEntry)
	{
		_logFile.WriteV(LogCategory::Misc, LogLevel::Error, LogOption::Std, "RpcCommandFactory::_Create error, type : %d", type);

		XTL_THROW(E_COMMAND_ERROR);

		return nullptr;
	}

	RpcCommand* p = pEntry->pfnCreate();
	if (p == nullptr)
	{
		XTL_THROW(E_OUTOFMEMORY);

		return nullptr;
	}

	if (addRef)
	{
		p->AddRef();
	}

	return p;
}