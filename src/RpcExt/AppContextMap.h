#pragma once

class AppContextMap;

class ATL_NO_VTABLE __declspec(uuid("EE40EDB3-023C-4B46-8A07-9103425D676B"))
	AppContext : public CComObjectRootEx<CComMultiThreadModel>,
				 public IUnknown
{
public:
	friend AppContextMap;

	AppContext();
	~AppContext();

	BEGIN_COM_MAP(AppContext)
		COM_INTERFACE_ENTRY(AppContext)
		COM_INTERFACE_ENTRY(IUnknown)
	END_COM_MAP()

	HRESULT Initialize(REFGUID appId);
	void Close();

	struct _SecurityChannelNode
	{
		GUID typeId;
		IScAccessChannel* pChannel;
		_SecurityChannelNode* pNext;
	};

	HRESULT AddChannel(REFGUID typeId, IScAccessChannel* pChannel);
	HRESULT GetChannle(REFGUID typeId, IScAccessChannel** ppChannel);
	HRESULT RemoveChannel(REFGUID typeId);

	HRESULT Logon(SC_AUTH_INFO* pInfo, SC_AUTH_RESP* pResp);
	HRESULT AccessCheck(HANDLE hToken, IScAccessToken** ppToken);

protected:
	GUID m_appId;

	_SecurityChannelNode* m_pHeadChannel;
};

#include "GuidMap.h"

// AppContextMap

class AppContextMap
{
public:
	AppContextMap();
	~AppContextMap();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT RegisterChannel(REFGUID appId, REFGUID typeId, IScAccessChannel* pChannel);
	HRESULT RevokeChannel(REFGUID appId, REFGUID typeId);

	HRESULT Insert(REFGUID appId, AppContext** ppContext);
	HRESULT Lookup(REFGUID appId, AppContext** ppContext);

	HRESULT Remove(REFGUID appId, AppContext** ppContext = nullptr);

protected:
	GuidMap m_map;
};
