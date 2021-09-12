#include "StdAfx.h"
#include "MarshaledInterfaceList.h"

#include "StubBufferMap.h"

MarshaledInterfaceList::MarshaledInterfaceList()
{
	m_pHead = nullptr;
}

MarshaledInterfaceList::~MarshaledInterfaceList()
{
	Clear();
}

HRESULT MarshaledInterfaceList::Initialize()
{
	return S_OK;
}

void MarshaledInterfaceList::Uninitialize()
{
}

MarshaledInterfaceList::_Node* MarshaledInterfaceList::Insert(REFIID riid, IUnknown* pUnkServer)
{
	ATLASSERT(pUnkServer);

	_Node* pNode = new _Node;
	if (!pNode)
		return nullptr;

	pNode->iid = riid;
	pNode->pUnkServer = pUnkServer;
	pNode->pBuffer = nullptr;

	pNode->pNext = m_pHead;

	m_pHead = pNode;

	return pNode;
}

MarshaledInterfaceList::_Node* MarshaledInterfaceList::Lookup(REFIID riid, IUnknown* pUnkServer)
{
	_Node* pNode = m_pHead;
	while (pNode)
	{
		if (InlineIsEqualGUID(pNode->iid, riid) && pNode->pUnkServer == pUnkServer)
			return pNode;

		pNode = pNode->pNext;
	}

	return nullptr;
}

void MarshaledInterfaceList::Remove(_Node* pDelete)
{
	ATLASSERT(pDelete);

	_Node* pNode = m_pHead;
	_Node* pPrev = nullptr;
	while (pNode)
	{
		if (pNode == pDelete)
			break;

		pPrev = pNode;
		pNode = pNode->pNext;
	}

	if (!pNode)
		return;

	if (pPrev)
		pPrev->pNext = pNode->pNext;
	else
		m_pHead = pNode->pNext;

	pNode->pUnkServer->Release();

	if (pNode->pBuffer)
		pNode->pBuffer->Release();

	delete pNode;
}

void MarshaledInterfaceList::Clear()
{
	_Node* pNode = m_pHead;
	while (pNode)
	{
		_Node* pTemp = pNode;
		pNode = pNode->pNext;

		pTemp->pUnkServer->Release();

		if (pTemp->pBuffer)
			pTemp->pBuffer->Release();

		delete pTemp;
	}

	m_pHead = nullptr;
}
