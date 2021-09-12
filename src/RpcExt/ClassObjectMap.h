#pragma once

class AppContext;
class ClassObjectMap;

class ATL_NO_VTABLE __declspec(uuid("3F037533-BC47-45e9-AACA-65988E88E978"))
	ClassObject : public CComObjectRootEx<CComMultiThreadModel>,
				  public IUnknown
{
public:
	friend ClassObjectMap;

	ClassObject();
	~ClassObject();

	BEGIN_COM_MAP(ClassObject)
		COM_INTERFACE_ENTRY(ClassObject)
		COM_INTERFACE_ENTRY(IUnknown)
	END_COM_MAP()

	HRESULT Init(REFCLSID rclsid, DWORD clsContext, DWORD flags, DWORD usage, LPUNKNOWN lpUnk);

	static HRESULT GetAppID(REFCLSID rclsid, LPGUID lpGuid);

	HRESULT CreateInstance(REFIID riid, void** ppv);
	static LRESULT OnCreateInstance(LPARAM lParam);

	AppContext* GetContext() { return m_pAppContext; }

	struct _CreateInstanceParam
	{
		IClassFactory* pFactory;
		IID iid;
		void** ppv;
		HRESULT hr;
		HANDLE hEvent;
	};

	DWORD m_threadId;
	APTTYPE m_aptType;

	CLSID m_clsid;
	DWORD m_clsContext;
	DWORD m_regFlags; // REGCLS
	DWORD m_register;
	DWORD m_usage;

protected:
	IUnknown* m_pUnk;
	AppContext* m_pAppContext;
};

#include "GuidMap.h"

// ClassObjectMap

class ClassObjectMap
{
public:
	ClassObjectMap();
	~ClassObjectMap();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT Insert(REFCLSID clsid, LPUNKNOWN lpUnk, DWORD clsContext, DWORD regFlags, DWORD usage, DWORD* pCookie);
	HRESULT Lookup(REFCLSID rclsid, ClassObject** ppObject);

	HRESULT Remove(DWORD cookie);
	HRESULT Remove(REFCLSID rclsid);

protected:
	GuidMap m_map;
};
