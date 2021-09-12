#pragma once

enum RPC_BUFF_POS
{
	RPC_BP_BEGIN,
	RPC_BP_CURRENT,
	RPC_BP_END,
};

class Socket;
class RpcCommand;

class RpcBuffer
{
public:
	RpcBuffer();
	~RpcBuffer();

	HRESULT Send(Socket* pSocket);
	HRESULT Recv(Socket* pSocket);

	HRESULT Write(RpcCommand* pCmd);
	HRESULT Read(RpcCommand* pCmd);
	HRESULT Read(RpcCommand** ppCmd);

	BYTE* _Read(DWORD size);
	BYTE* _Write(DWORD size);

	BYTE ReadByte()
	{
		return *_Read(1);
	}

	void WriteByte(BYTE b)
	{
		*(BYTE*)_Write(1) = b;
	}

	short ReadInt16()
	{
		//return ntohs(*(short*)_Read(2));
		return *(short*)_Read(2);
	}

	void WriteInt16(short s)
	{
		//*(short*)_Write(2) = htons(s);
		*(short*)_Write(2) = s;
	}

	int ReadInt32()
	{
		//return ntohl(*(int*)_Read(4));
		return *(int*)_Read(4);
	}

	void WriteInt32(int l)
	{
		//*(int*)_Write(4) = htonl(l);
		*(int*)_Write(4) = l;
	}

	__int64 ReadInt64()
	{
		ULARGE_INTEGER v;
		v.HighPart = ReadInt32();
		v.LowPart = ReadInt32();
		return v.QuadPart;
	}

	void WriteInt64(__int64 n)
	{
		ULARGE_INTEGER v;
		v.QuadPart = n;
		WriteInt32(v.HighPart);
		WriteInt32(v.LowPart);
	}

	void ReadBytes(void* pBuff, DWORD size);
	void WriteBytes(void* pBuff, DWORD size);

	void ReadGUID(LPGUID lpguid);
	void WriteGUID(REFGUID rguid);

	void ReadBSTR(BSTR* pbstr);
	void WriteString(LPCWSTR str, int len = -1);

	HRESULT Alloc(ULONG size);

	void SetSize(ULONG size, bool bAppend);
	void Seek(RPC_BUFF_POS pos, LONG offset);

	void Clear();

	BYTE* GetBuffer() { return m_pBuffer; }
	BYTE* GetCurrent() { return m_pBuffer + m_cursor; }

	ULONG GetCursor() { return m_cursor; }
	ULONG GetSize() { return m_size; }
	ULONG GetAllocSize() { return m_allocSize; }
	ULONG GetAvailable() { return m_size - m_cursor; }

protected:
	BYTE* m_pBuffer;

	ULONG m_size;
	ULONG m_allocSize;
	ULONG m_cursor;
};
