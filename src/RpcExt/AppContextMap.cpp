#include "stdafx.h"
#include "AppContextMap.h"

#include "ScBase/Global.h"

AppContext::AppContext()
{
	m_appId = GUID_NULL;

	m_pHeadChannel = nullptr;
}

AppContext::~AppContext()
{
	Close();
}

HRESULT AppContext::Initialize(REFGUID appId)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContext::Initialize");

	CComPtr<IScAccessChannel> spChannel;
	HRESULT hr = ScGlobal::CreateAccessChannel(appId, __uuidof(IScAccessChannel), (void**)&spChannel);
	if (SUCCEEDED(hr))
	{
		hr = AddChannel(GUID_NULL, spChannel);
	}

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContext::Initialize end, hr = #%08x", hr);

	return S_OK;
}

void AppContext::Close()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContext::Close");

	Lock();

	auto pNode = m_pHeadChannel;
	while (pNode)
	{
		auto pTemp = pNode;
		pNode = pNode->pNext;

		pTemp->pChannel->Release();
		delete pTemp;
	}

	m_pHeadChannel = nullptr;

	Unlock();
}

HRESULT AppContext::AddChannel(REFGUID typeId, IScAccessChannel* pChannel)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContext::AddChannel");

	if (!pChannel)
		return E_POINTER;

	ObjectLock lock(this);

	auto pNode = m_pHeadChannel;
	for (; pNode; pNode = pNode->pNext)
	{
		if (InlineIsEqualGUID(pNode->typeId, typeId))
		{
			ComApi::Assign(&pNode->pChannel, pChannel);

			return S_OK;
		}
	}

	pNode = new _SecurityChannelNode;
	if (!pNode)
		return E_OUTOFMEMORY;

	pNode->typeId = typeId;
	pNode->pChannel = ComApi::AddRef(pChannel);

	pNode->pNext = m_pHeadChannel;
	m_pHeadChannel = pNode;

	return S_OK;
}

HRESULT AppContext::GetChannle(REFGUID typeId, IScAccessChannel** ppChannel)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContext::GetChannle");

	ObjectLock lock(this);

	auto pNode = m_pHeadChannel;
	for (; pNode; pNode = pNode->pNext)
	{
		if (InlineIsEqualGUID(pNode->typeId, typeId))
		{
			if (*ppChannel)
			{
				*ppChannel = pNode->pChannel;
				(*ppChannel)->AddRef();
			}

			return S_OK;
		}
	}

	return S_FALSE;
}

HRESULT AppContext::RemoveChannel(REFGUID typeId)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContext::RemoveChannel");

	ObjectLock lock(this);

	auto ppNode = &m_pHeadChannel;
	for (; *ppNode; ppNode = &(*ppNode)->pNext)
	{
		if (InlineIsEqualGUID((*ppNode)->typeId, typeId))
		{
			auto pNode = *ppNode;
			*ppNode = pNode->pNext;

			pNode->pChannel->Release();
			delete pNode;

			return S_OK;
		}
	}

	return S_FALSE;
}

HRESULT AppContext::Logon(SC_AUTH_INFO* pInfo, SC_AUTH_RESP* pResp)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContext::Logon");

	ObjectLock lock(this);

	HRESULT hr = E_FAIL;

	auto pNode = m_pHeadChannel;
	for (; pNode; pNode = pNode->pNext)
	{
		hr = pNode->pChannel->Logon(pInfo, pResp);

		if (HRESULT_CODE(hr) != ERROR_LOGON_FAILURE)
			break;
	}

	return hr;
}

HRESULT AppContext::AccessCheck(HANDLE hToken, IScAccessToken** ppToken)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContext::AccessCheck");

	ObjectLock lock(this);

	HRESULT hr = E_FAIL;

	auto pNode = m_pHeadChannel;
	for (; pNode; pNode = pNode->pNext)
	{
		hr = pNode->pChannel->AccessCheck(hToken, ppToken);
		if (SUCCEEDED(hr))
			break;
	}

	return hr;
}

// AppContextMap

AppContextMap::AppContextMap()
{
}

AppContextMap::~AppContextMap()
{
}

HRESULT AppContextMap::Initialize()
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContextMap::Initialize");

	return S_OK;
}

void AppContextMap::Uninitialize()
{
	m_map.Clear();

	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContextMap::Uninitialize");
}

HRESULT AppContextMap::RegisterChannel(REFGUID appId, REFGUID typeId, IScAccessChannel* pChannel)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContextMap::RegisterChannel");

	if (!pChannel)
		return E_POINTER;

	CComPtr<AppContext> spContext;
	HRESULT hr = Insert(appId, &spContext);
	if (SUCCEEDED(hr))
	{
		hr = spContext->AddChannel(typeId, pChannel);
	}

	return hr;
}

HRESULT AppContextMap::RevokeChannel(REFGUID appId, REFGUID typeId)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContextMap::RevokeChannel");

	CComPtr<AppContext> spContext;
	HRESULT hr = Remove(appId, &spContext);
	if (hr == S_OK)
	{
		hr = spContext->RemoveChannel(typeId);
	}

	return hr;
}

HRESULT AppContextMap::Insert(REFGUID appId, AppContext** ppContext)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContextMap::Insert");

	HRESULT hr = m_map.Lookup(appId, __uuidof(AppContext), (void**)ppContext);
	if (hr == S_OK)
		return hr;

	CComObject<AppContext>* pContext = nullptr;
	hr = CComObject<AppContext>::CreateInstance(&pContext);
	if (FAILED(hr))
		return hr;

	pContext->AddRef();

	hr = pContext->Initialize(appId);
	if (FAILED(hr))
	{
		pContext->Release();

		return hr;
	}

	hr = m_map.Insert(appId, pContext);
	if (FAILED(hr))
	{
		pContext->Release();

		return hr;
	}

	if (ppContext)
	{
		*ppContext = pContext;
	}
	else
	{
		pContext->Release();
	}

	return S_OK;
}

HRESULT AppContextMap::Lookup(REFGUID appId, AppContext** ppContext)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContextMap::Lookup");

	return m_map.Lookup(appId, __uuidof(AppContext), (void**)ppContext);
}

HRESULT AppContextMap::Remove(REFGUID appId, AppContext** ppContext)
{
	_logFile.Write(LogCategory::Misc, LogLevel::General, LogOption::Std, "AppContextMap::Remove");

	return m_map.Remove(appId, __uuidof(AppContext), (void**)ppContext);
}
