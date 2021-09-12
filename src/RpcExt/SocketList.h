#pragma once

class SocketList
{
public:
	SocketList();
	~SocketList();

	struct _SocketNode
	{
		SOCKET socket;
		_SocketNode* pNext;
	};

	HRESULT AddTail(SOCKET socket);
	SOCKET RemoveHead();

	ULONG GetCount();

	void Clear();

protected:
	ULONG m_count;

	_SocketNode* m_pHead;
	_SocketNode* m_pTail;

	CComAutoCriticalSection m_cs;
};
