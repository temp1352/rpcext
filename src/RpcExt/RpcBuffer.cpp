#include "StdAfx.h"
#include "RpcBuffer.h"

#include "Command.h"
#include "Socket.h"

RpcBuffer::RpcBuffer()
{
	m_pBuffer = nullptr;
	m_size = 0;
	m_allocSize = 0;
	m_cursor = 0;
}

RpcBuffer::~RpcBuffer()
{
	Clear();
}

static char _rpcFlag[] = "rpcx";
const int _flagLen = 4;
const int _headerLen = _flagLen + sizeof(LONG);

HRESULT RpcBuffer::Send(Socket* pSocket)
{
	DWORD cbWritten = 0;
	DWORD cbSended = 0;

	while (cbSended < m_size)
	{
		if (!pSocket->Write(m_pBuffer + cbSended, m_size - cbSended, &cbWritten))
			return pSocket->GetHResult();

		cbSended += cbWritten;
	}

	return S_OK;
}

HRESULT RpcBuffer::Recv(Socket* pSocket)
{
	DWORD read = 0;
	BYTE buffer[_headerLen];
	if (!pSocket->Read(buffer, _headerLen, &read))
		return pSocket->GetHResult();

	if (strncmp((char*)buffer, _rpcFlag, _flagLen) != 0)
		return E_FAIL;

	ULONG size = _headerLen + *(ULONG*)(buffer + _flagLen);
	HRESULT hr = Alloc(size);
	if (FAILED(hr))
		return hr;

	memcpy(m_pBuffer, buffer, _headerLen);
	m_size = _headerLen;

	while (m_size < size)
	{
		read = 0;

		if (!pSocket->Read(m_pBuffer + m_size, size - m_size, &read))
			return pSocket->GetHResult();

		m_size += read;
	}

	return S_OK;
}

HRESULT RpcBuffer::Write(RpcCommand* pCmd)
{
	ATLASSERT(pCmd);

	try
	{
		ULONG start = m_cursor;

		WriteBytes(_rpcFlag, _flagLen);
		WriteInt32(0);

		pCmd->Serialize(this);

		*(ULONG*)(m_pBuffer + start + _flagLen) = m_cursor - start - _headerLen;

		m_cursor = start;
	}
	catch (CAtlException e)
	{
		return e;
	}

	return S_OK;
}

HRESULT RpcBuffer::Read(RpcCommand* pCmd)
{
	if (m_size - m_cursor < _headerLen)
		return S_FALSE;

	LPBYTE pBuffer = m_pBuffer + m_cursor;
	if (strncmp((char*)pBuffer, _rpcFlag, _flagLen) != 0)
		return S_FALSE;

	ULONG len = *(ULONG*)(pBuffer + _flagLen);
	if (len > m_size - m_cursor - _headerLen)
		return S_FALSE;

	ULONG start = m_cursor;
	m_cursor += _headerLen;

	try
	{
		DWORD type = ReadInt32();

		if (type != pCmd->m_type)
			return E_COMMAND_ERROR;

		pCmd->Deserialize(this);
	}
	catch (CAtlException e)
	{
		m_cursor = start;

		return e;
	}

	if (m_cursor == m_size)
	{
		m_size = 0;
		m_cursor = 0;
	}
	else if (m_cursor > m_size / 2)
	{
		m_size -= m_cursor;
		memcpy(m_pBuffer, m_pBuffer + m_cursor, m_size);
		m_cursor = 0;
	}

	return S_OK;
}

HRESULT RpcBuffer::Read(RpcCommand** ppCmd)
{
	ATLASSERT(ppCmd);
	ATLASSERT(*ppCmd == nullptr);

	if (m_size - m_cursor < _headerLen)
		return S_FALSE;

	LPBYTE pBuffer = m_pBuffer + m_cursor;
	if (strncmp((char*)pBuffer, _rpcFlag, _flagLen) != 0)
		return S_FALSE;

	ULONG len = *(ULONG*)(pBuffer + _flagLen);

	if (len > m_size - m_cursor)
		return S_FALSE;

	ULONG start = m_cursor;
	m_cursor += _headerLen;

	try
	{
		DWORD type = ReadInt32();

		*ppCmd = RpcCommandFactory::_Create((RpcCommandType)type, true);

		(*ppCmd)->Deserialize(this);
	}
	catch (CAtlException e)
	{
		m_cursor = start;

		if (*ppCmd)
		{
			(*ppCmd)->Release();
		}

		return e;
	}

	if (m_cursor == m_size)
	{
		m_size = 0;
		m_cursor = 0;
	}
	else if (m_cursor > m_size / 2)
	{
		m_size -= m_cursor;
		memcpy(m_pBuffer, m_pBuffer + m_cursor, m_size);
		m_cursor = 0;
	}

	return S_OK;
}

BYTE* RpcBuffer::_Read(DWORD size)
{
	if (size > m_size - m_cursor)
	{
		XTL_THROW(HRESULT_FROM_WIN32(ERROR_BUFFER_ALL_ZEROS));
	}

	BYTE* pData = m_pBuffer + m_cursor;
	m_cursor += size;

	return pData;
}

static ULONG _BUFFER_SIZE = 1024;

BYTE* RpcBuffer::_Write(DWORD size)
{
	ULONG totalSiz = m_cursor + size;
	if (m_size < totalSiz)
	{
		ULONG allocSize = totalSiz * 2 + 1;
		if (allocSize < _BUFFER_SIZE)
		{
			allocSize = _BUFFER_SIZE;
		}

		HRESULT hr = Alloc(allocSize);
		if (hr != S_OK)
		{
			XTL_THROW(hr);
		}

		m_size = totalSiz;
	}

	BYTE* pData = m_pBuffer + m_cursor;
	m_cursor += size;

	return pData;
}

void RpcBuffer::ReadBytes(void* pBuff, DWORD size)
{
	ATLASSERT(pBuff);

	BYTE* p = _Read(size);

	memcpy(pBuff, p, size);
}

void RpcBuffer::WriteBytes(void* pBuff, DWORD size)
{
	BYTE* p = _Write(size);

	memcpy(p, pBuff, size);
}

void RpcBuffer::ReadGUID(LPGUID lpguid)
{
	ReadBytes(lpguid, sizeof(GUID));
}

void RpcBuffer::WriteGUID(REFGUID rguid)
{
	WriteBytes((void*)&rguid, sizeof(GUID));
}

void RpcBuffer::ReadBSTR(BSTR* pbstr)
{
	ATLASSERT(pbstr);

	int len = ReadInt32();
	if (len <= 0)
		return;

	if (*pbstr)
	{
		SysFreeString(*pbstr);
	}

	*pbstr = SysAllocStringLen(nullptr, len);
	if (*pbstr == nullptr)
	{
		XTL_THROW(HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY));
	}

	ReadBytes(*pbstr, len * sizeof(WCHAR));
}

void RpcBuffer::WriteString(LPCWSTR str, int len)
{
	if (len == -1)
	{
		len = str ? wcslen(str) : 0;
	}

	WriteInt32(len);

	if (str)
	{
		WriteBytes((LPVOID)str, len * sizeof(WCHAR));
	}
}

HRESULT RpcBuffer::Alloc(ULONG size)
{
	if (m_pBuffer == nullptr)
	{
		m_pBuffer = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
		if (m_pBuffer == nullptr)
			return E_OUTOFMEMORY;

		m_allocSize = size;
	}
	else if (m_allocSize < size)
	{
		auto pBuffer = (BYTE*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_pBuffer, size);
		if (pBuffer == nullptr)
			return E_OUTOFMEMORY;

		m_allocSize = size;

		m_pBuffer = pBuffer;
	}

	return S_OK;
}

void RpcBuffer::SetSize(ULONG size, bool bAppend)
{
	if (bAppend)
		m_size += size;
	else
		m_size = size;

	if (m_size > m_allocSize)
	{
		m_size = m_allocSize;
	}

	if (m_cursor > m_size)
	{
		m_cursor = m_size;
	}
}

void RpcBuffer::Seek(RPC_BUFF_POS pos, LONG offset)
{
	switch (pos)
	{
	case RPC_BP_BEGIN:
		m_cursor = offset;
		break;

	case RPC_BP_CURRENT:
		m_cursor += offset;
		break;

	case RPC_BP_END:
		m_cursor = m_size + offset;
		break;
	}

	if (m_cursor < 0 || m_cursor > m_size)
		while (0)
			;
}

void RpcBuffer::Clear()
{
	if (m_pBuffer)
	{
		HeapFree(GetProcessHeap(), 0, m_pBuffer);

		m_pBuffer = nullptr;

		m_allocSize = 0;
		m_size = 0;
		m_cursor = 0;
	}
}
