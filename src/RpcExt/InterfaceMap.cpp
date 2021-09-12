#include "StdAfx.h"
#include "InterfaceMap.h"

// Interface

Interface::Interface()
{
	m_hash = 0;
	m_iid = IID_NULL;
	m_pNext = nullptr;
}

Interface::~Interface()
{
}

HRESULT Interface::Init(REFIID riid)
{
	m_iid = riid;

	WCHAR name[64];
	if (StringFromGUID2(riid, name, 64))
	{
		m_name = name;
	}

	return S_OK;
}

HRESULT Interface::Load(ITypeInfo* pTypeInfo, TYPEATTR* pTypeAttr)
{
	m_iid = pTypeAttr->guid;

	HRESULT hr = pTypeInfo->GetDocumentation(-1, &m_name, nullptr, nullptr, nullptr);
	if (hr != S_OK)
		return hr;

	for (int i = 0; i < pTypeAttr->cImplTypes; ++i)
	{
		HREFTYPE hRefType = 0;
		hr = pTypeInfo->GetRefTypeOfImplType(i, &hRefType);
		if (FAILED(hr))
			break;

		CComPtr<ITypeInfo> spRefTypeInfo;
		hr = pTypeInfo->GetRefTypeInfo(hRefType, &spRefTypeInfo);
		if (hr == S_OK)
		{
			TYPEATTR* pTypeAttr2 = nullptr;
			hr = spRefTypeInfo->GetTypeAttr(&pTypeAttr2);
			if (hr == S_OK)
			{
				hr = LoadFuncs(spRefTypeInfo, pTypeAttr2->cFuncs);
				spRefTypeInfo->ReleaseTypeAttr(pTypeAttr2);
			}
		}
	}

	hr = LoadFuncs(pTypeInfo, pTypeAttr->cFuncs);

	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "Interface::Load, interface : %ws, functions : %d", m_name, m_aFunc.GetCount());

	return S_OK;
}

HRESULT Interface::LoadFuncs(ITypeInfo* pTypeInfo, LONG count)
{
	for (int i = 0; i < count; ++i)
	{
		FUNCDESC* pFuncDesc = nullptr;
		HRESULT hr = pTypeInfo->GetFuncDesc(i, &pFuncDesc);
		if (hr == S_OK)
		{
			AddFunc(pFuncDesc, pTypeInfo);

			pTypeInfo->ReleaseFuncDesc(pFuncDesc);
		}
	}

	return S_OK;
}

HRESULT Interface::AddFunc(FUNCDESC* pFuncDesc, ITypeInfo* pTypeInfo)
{
	CComBSTR name;
	HRESULT hr = pTypeInfo->GetDocumentation(pFuncDesc->memid, &name, nullptr, nullptr, nullptr);
	if (hr != S_OK)
		return hr;

	CAutoPtr<FuncDesc> spFunc(new FuncDesc);
	if (!spFunc)
		return E_OUTOFMEMORY;

	spFunc->memid = pFuncDesc->memid;
	spFunc->invkind = pFuncDesc->invkind;
	spFunc->name = std::move(name);
	spFunc->index = m_aFunc.GetCount();

	try
	{
		m_aFunc.Add(spFunc);
	}
	catch (CAtlException e)
	{
		return e;
	}

	return S_FALSE;
}

FuncDesc* Interface::SafeGetFunc(ULONG index)
{
	if (index >= m_aFunc.GetCount())
		return nullptr;

	return m_aFunc[index];
}

// InterfaceMap

InterfaceMap::InterfaceMap()
{
	m_count = 0;
	m_binCount = 0;
	m_ppBins = nullptr;
}

InterfaceMap::~InterfaceMap()
{
	Clear();
}

HRESULT InterfaceMap::LoadInterfaces(LPCWSTR fileName)
{
	CComPtr<ITypeLib> spTypeLib;
	HRESULT hr = LoadTypeLib(fileName, &spTypeLib);
	if (hr == S_OK)
	{
		hr = LoadInterfaces(spTypeLib);
	}

	return hr;
}

HRESULT InterfaceMap::LoadInterfaces(REFGUID libid)
{
	CComPtr<ITypeLib> spTypeLib;
	HRESULT hr = LoadRegTypeLib(libid, 1, 0, 0, &spTypeLib);
	if (hr == S_OK)
	{
		hr = LoadInterfaces(spTypeLib);
	}

	return hr;
}

HRESULT InterfaceMap::LoadInterfaces(ITypeLib* pTypeLib)
{
	HRESULT hr = S_FALSE;

	ULONG count = pTypeLib->GetTypeInfoCount();
	for (ULONG i = 0; i < count; ++i)
	{
		CComPtr<ITypeInfo> spTypeInfo;
		hr = pTypeLib->GetTypeInfo(i, &spTypeInfo);
		if (hr != S_OK)
			return hr;

		CComBSTR name;
		hr = spTypeInfo->GetDocumentation(-1, &name, nullptr, nullptr, nullptr);

		TYPEATTR* pTypeAttr = nullptr;
		HRESULT hr = spTypeInfo->GetTypeAttr(&pTypeAttr);
		if (hr == S_OK)
		{
			if (pTypeAttr->typekind == TKIND_INTERFACE || pTypeAttr->typekind == TKIND_DISPATCH)
			{
				CAutoPtr<Interface> spInterface(new Interface);
				if (!spInterface)
				{
					spTypeInfo->ReleaseTypeAttr(pTypeAttr);
					return E_OUTOFMEMORY;
				}

				hr = spInterface->Load(spTypeInfo, pTypeAttr);
				if (hr == S_OK)
				{
					Insert(spInterface.Detach());
				}
			}

			spTypeInfo->ReleaseTypeAttr(pTypeAttr);
		}
	}

	return S_OK;
}

HRESULT InterfaceMap::Insert(Interface* pInterface)
{
	ATLASSERT(pInterface);

	if (!m_ppBins)
	{
		HRESULT hr = InitBins();
		if (hr != S_OK)
			return hr;
	}

	pInterface->m_hash = Hash(pInterface->m_iid);

	int binIdx = pInterface->m_hash % m_binCount;

	pInterface->m_pNext = m_ppBins[binIdx];
	m_ppBins[binIdx] = pInterface;

	++m_count;

	return S_OK;
}

Interface* InterfaceMap::GetInterface(REFIID riid)
{
	Interface* pInterface = Lookup(riid);
	if (!pInterface)
	{
		pInterface = new Interface;
		if (pInterface)
		{
			pInterface->Init(riid);

			HRESULT hr = Insert(pInterface);
			if (FAILED(hr))
			{
				delete pInterface;

				return nullptr;
			}
		}
	}

	return pInterface;
}

Interface* InterfaceMap::Lookup(REFIID riid)
{
	if (!m_ppBins || !m_binCount)
		return nullptr;

	UINT hash = Hash(riid);
	int binIdx = hash % m_binCount;

	Interface* pNode = m_ppBins[binIdx];
	while (pNode)
	{
		if (pNode->m_hash == hash && InlineIsEqualGUID(pNode->m_iid, riid))
		{
			return pNode;
		}

		pNode = pNode->m_pNext;
	}

	return nullptr;
}

HRESULT InterfaceMap::GetInterfaceName(REFIID riid, BSTR* pName)
{
	ATLASSERT(pName);

	Interface* pInterface = GetInterface(riid);
	if (pInterface)
	{
		*pName = pInterface->m_name.Copy();

		return S_OK;
	}

	return E_FAIL;
}

HRESULT InterfaceMap::GetMethodName(REFIID riid, int methodId, BSTR* pItfName, BSTR* pMethodName)
{
	ATLASSERT(pItfName);
	ATLASSERT(pMethodName);

	Interface* pInterface = GetInterface(riid);
	if (pInterface)
	{
		*pItfName = pInterface->m_name.Copy();

		FuncDesc* pFuncDesc = pInterface->SafeGetFunc(methodId);
		if (pFuncDesc)
		{
			*pMethodName = pFuncDesc->name.Copy();
		}

		return S_OK;
	}

	return E_FAIL;
}

HRESULT InterfaceMap::GetInterfaceName(REFIID riid, CStringA& name)
{
	Interface* pInterface = GetInterface(riid);
	if (pInterface)
	{
		name = pInterface->m_name;

		return S_OK;
	}

	return E_FAIL;
}

HRESULT InterfaceMap::GetMethodName(REFIID riid, int methodId, CStringA& itfName, CStringA& methodName)
{
	Interface* pInterface = GetInterface(riid);
	if (pInterface)
	{
		itfName = pInterface->m_name;

		FuncDesc* pFuncDesc = pInterface->SafeGetFunc(methodId);
		if (pFuncDesc)
			methodName = pFuncDesc->name;
		else
			methodName.Format("Method%d", methodId);

		return S_OK;
	}

	return E_FAIL;
}

UINT InterfaceMap::Hash(REFGUID guid)
{
	const DWORD* pdwData = reinterpret_cast<const DWORD*>(&guid);

	return (pdwData[0] ^ pdwData[1] ^ pdwData[2] ^ pdwData[3]);
}

HRESULT InterfaceMap::InitBins()
{
	ATLASSERT(!m_ppBins);

	ULONG nBin = 41;

	m_ppBins = new Interface*[nBin];
	if (!m_ppBins)
	{
		SetLastError(ERROR_OUTOFMEMORY);
		return E_OUTOFMEMORY;
	}

	memset(m_ppBins, 0, sizeof(Interface*) * nBin);

	m_binCount = nBin;

	return S_OK;
}

void InterfaceMap::Clear()
{
	m_count = 0;

	if (m_ppBins)
	{
		for (ULONG i = 0; i < m_binCount; ++i)
		{
			Interface *pNode = m_ppBins[i], *pTemp;
			while (pNode)
			{
				pTemp = pNode;
				pNode = pNode->m_pNext;
				delete pTemp;
			}
		}

		delete[] m_ppBins;
		m_ppBins = nullptr;
		m_binCount = 0;
	}
}

InterfaceMap _interfaceMap;