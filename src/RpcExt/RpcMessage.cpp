#include "StdAfx.h"
#include "RpcMessage.h"

#include "InterfaceMap.h"
#include "RpcBuffer.h"

// RpcMessage

RpcMessage::RpcMessage()
{
	memset(this, 0, sizeof(RPC_MESSAGE));
}

RpcMessage::~RpcMessage()
{
	Free();
}

void RpcMessage::Read(RpcBuffer* pBuffer)
{
	Free();

	this->DataRepresentation = pBuffer->ReadInt32();

	this->BufferLength = pBuffer->ReadInt32();
	if (this->BufferLength)
	{
		this->Buffer = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, this->BufferLength);
		if (!this->Buffer)
		{
			XTL_THROW(E_OUTOFMEMORY);
		}

		pBuffer->ReadBytes(this->Buffer, this->BufferLength);
	}

	this->ProcNum = pBuffer->ReadInt32();
}

void RpcMessage::Write(RpcBuffer* pBuffer)
{
	pBuffer->WriteInt32(this->DataRepresentation);
	pBuffer->WriteInt32(this->BufferLength);

	if (this->Buffer)
	{
		pBuffer->WriteBytes(this->Buffer, this->BufferLength);
	}

	pBuffer->WriteInt32(this->ProcNum);
}

void RpcMessage::Attach(RPC_MESSAGE* pMessage)
{
	ATLASSERT(pMessage);

	Free();

	*(RPC_MESSAGE*)this = *pMessage;

	pMessage->Buffer = nullptr;
	pMessage->BufferLength = 0;
}

void RpcMessage::Detach(RPC_MESSAGE* pMessage)
{
	if (pMessage->Buffer)
	{
		HeapFree(GetProcessHeap(), 0, pMessage->Buffer);
	}

	*pMessage = *this;

	this->Buffer = nullptr;
	this->BufferLength = 0;
}

void RpcMessage::Free()
{
	if (this->Buffer)
	{
		HeapFree(GetProcessHeap(), 0, this->Buffer);

		this->Buffer = nullptr;
	}

	this->BufferLength = 0;
}

static const int _minMIPointerLen = sizeof(ULONG) * 2 + sizeof(IID) + sizeof(STDOBJREF) + sizeof(DUALSTRINGARRAY);

HRESULT RpcMessage::GetObjList(StdObjList* pObjList)
{
	//_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcMessage::GetObjList - length : %d", BufferLength);

	BYTE* pCur = (BYTE*)Buffer;
	ULONG len = BufferLength;

	if (!pCur || len < _minMIPointerLen)
	{
		//_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcMessage::GetObjList - len : %d", len);
		return S_FALSE;
	}

	auto pEnd = pCur + len - _minMIPointerLen;
	while (pCur < pEnd)
	{
		if (pCur[0] == 'M' && pCur[1] == 'E' && pCur[2] == 'O' && pCur[3] == 'W')
		{
			LPBYTE pStart = pCur;

			CAutoPtr<ObjRef> spObjRef;
			HRESULT hr = ObjRef::_CreateInstanceFromBuffer(pCur, &spObjRef.m_p);
			if (FAILED(hr))
			{
				_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcMessage::GetObjList - ObjRef::_CreateInstanceFromBuffer, hr : #%08x, cur : %d, %02x %02x %02x %02x", hr, pCur - Buffer, pCur[0], pCur[1], pCur[2], pCur[3]);

				return hr;
			}

			if (_logFile.m_outLevel > 1)
			{
				CStringA strItf;
				_interfaceMap.GetInterfaceName(spObjRef->m_iid, strItf);

				_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcMessage::GetObjList, iid : %s, type : %d", strItf, spObjRef->m_type);
			}

			if (spObjRef->m_type == _OBJ_STANDARD || spObjRef->m_type == _OBJ_HANDLER)
			{
				hr = pObjList->Add(pStart, (StdObjRef*)spObjRef.m_p);
				if (FAILED(hr))
				{
					_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcMessage::GetObjList - pObjList->Add, hr : %08x", hr);
					return hr;
				}

				spObjRef.Detach();
			}
		}
		else
		{
			++pCur;
		}
	}

	return S_OK;
}
