#include "StdAfx.h"
#include "EventManager.h"

#include "RpcExt.h"

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
	Clear();
}

HRESULT EventManager::Initialize()
{
	return S_OK;
}

void EventManager::Uninitialize()
{
	Clear();
}

void EventManager::SetEvent(int index, RPC_COOKIE cookie)
{
	m_cs.Lock();

	if ((ULONG)index < m_aEvent.GetCount() && m_aEvent[index].cookie == cookie)
	{
		::SetEvent(m_aEvent[index].hEvent);
	}

	m_cs.Unlock();
}

int EventManager::GetEvent(RPC_COOKIE cookie)
{
	m_cs.Lock();

	int index = 0;
	ULONG count = m_aEvent.GetCount();
	for (; index < count; ++index)
	{
		if (m_aEvent[index].bLock == FALSE)
		{
			m_aEvent[index].cookie = cookie;
			m_aEvent[index].bLock = TRUE;

			break;
		}
	}

	if (index == count)
	{
		_Event event;
		event.cookie = cookie;
		event.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		event.bLock = TRUE;

		try
		{
			index = m_aEvent.Add(event);
		}
		catch (CAtlException e)
		{
			e;
			CloseHandle(event.hEvent);
		}
	}

	m_cs.Unlock();

	return index;
}

void EventManager::ReleaseEvent(int index)
{
	m_cs.Lock();

	if ((ULONG)index < m_aEvent.GetCount())
	{
		m_aEvent[index].cookie = 0;
		m_aEvent[index].bLock = FALSE;
	}

	m_cs.Unlock();
}

HANDLE EventManager::GetHandle(int index)
{
	m_cs.Lock();

	HANDLE h = (ULONG)index < m_aEvent.GetCount() ? m_aEvent[index].hEvent : nullptr;

	m_cs.Unlock();

	return h;
}

void EventManager::Clear()
{
	m_cs.Lock();

	ULONG count = m_aEvent.GetCount();
	for (ULONG i = 0; i < count; ++i)
	{
		if (m_aEvent[i].hEvent)
		{
			CloseHandle(m_aEvent[i].hEvent);
		}
	}

	m_aEvent.RemoveAll();

	m_cs.Unlock();
}

RpcAutoEvent::RpcAutoEvent(RPC_COOKIE cookie)
{
	m_eventId = _rpcExt.m_eventManager.GetEvent(cookie);
}

RpcAutoEvent::~RpcAutoEvent()
{
	if (m_eventId != RPC_INVALID_EVENTID)
	{
		_rpcExt.m_eventManager.ReleaseEvent(m_eventId);
	}
}