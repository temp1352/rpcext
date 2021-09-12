#pragma once

#include "StdObjList.h"
#include "RpcBuffer.h"

class RpcBuffer;
class RpcConnection;

// RpcMessage

class RpcMessage : public RPC_MESSAGE
{
public:
	RpcMessage();
	~RpcMessage();

	void Read(RpcBuffer* pBuffer);
	void Write(RpcBuffer* pBuffer);

	void Attach(RPC_MESSAGE* pMessage);
	void Detach(RPC_MESSAGE* pMessage);

	void Free();

	HRESULT GetObjList(StdObjList* pObjList);
};
