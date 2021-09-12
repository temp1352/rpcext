#pragma once

#include "ObjRef.h"

class StdObjList
{
public:
	struct _Node
	{
		LPBYTE pStart;
		StdObjRef* pObjRef;

		_Node* pNext;

		_Node(LPBYTE pStart, StdObjRef* pObjRef)
		{
			this->pStart = pStart;
			this->pObjRef = pObjRef;
			pNext = nullptr;
		}

		~_Node()
		{
			delete pObjRef;
		}
	};

	StdObjList();
	~StdObjList();

	HRESULT Add(LPBYTE pStart, StdObjRef* pObjRef);

	POSITION GetHeadPosition() { return (POSITION)m_pHead; }

	StdObjRef* GetNext(POSITION& pos, LPBYTE& pStart);

	void Clear();

	ULONG GetCount() { return m_count; }

protected:
	_Node* m_pHead;
	_Node* m_pTail;

	ULONG m_count;
};
