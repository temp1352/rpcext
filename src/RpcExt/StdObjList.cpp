#include "StdAfx.h"
#include "StdObjList.h"

StdObjList::StdObjList()
{
	m_pHead = nullptr;
	m_pTail = nullptr;

	m_count = 0;
}

StdObjList::~StdObjList()
{
	Clear();
}

HRESULT StdObjList::Add(LPBYTE pStart, StdObjRef* pObjRef)
{
	_Node* pNode = new _Node(pStart, pObjRef);
	if (!pNode)
		return E_OUTOFMEMORY;

	if (m_pTail)
		m_pTail->pNext = pNode;
	else
		m_pHead = pNode;

	m_pTail = pNode;

	++m_count;

	return S_OK;
}

StdObjRef* StdObjList::GetNext(POSITION& pos, LPBYTE& pStart)
{
	_Node* pNode = (_Node*)pos;

	pos = (POSITION)pNode->pNext;

	pStart = pNode->pStart;

	return pNode->pObjRef;
}

void StdObjList::Clear()
{
	_Node* pNode = m_pHead;
	while (pNode)
	{
		_Node* pTemp = pNode;
		pNode = pNode->pNext;

		delete pTemp;
	}

	m_pHead = nullptr;
	m_pTail = nullptr;

	m_count = 0;
}