#include "StdAfx.h"
#include "ThreadInfo.h"

#include "ThreadWindow.h"

RpcThreadInfo::RpcThreadInfo()
{
	m_aptType = APTTYPE_MTA;
	m_hWnd = nullptr;

	m_pContext = nullptr;
	m_pNext = nullptr;
}

RpcThreadInfo::~RpcThreadInfo()
{
}

HRESULT RpcThreadInfo::Initialize(APTTYPE type)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcTheadInfo::Initialize, APTTYPE : %d", type);

	m_threadId = GetCurrentThreadId();

	m_aptType = type;
	if (m_aptType == APTTYPE_STA)
	{
		m_hWnd = RpcThreadWindow::Create();
		if (!m_hWnd)
			return AtlHresultFromLastError();
	}

	return S_OK;
}

void RpcThreadInfo::Uninitialize()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadInfo::Uninitialize");

	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);

		m_hWnd = nullptr;
	}
}

RpcCallContext* RpcThreadInfo::GetContext()
{
	return m_pContext;
}

void RpcThreadInfo::SetContext(RpcCallContext* pContext)
{
	m_pContext = pContext;
}