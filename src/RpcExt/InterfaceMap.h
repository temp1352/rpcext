#pragma once

class InterfaceMap;

class FuncDesc
{
public:
	FuncDesc()
	{
		memid = 0;
		invkind = INVOKE_FUNC;
		index = 0;
	}

	MEMBERID memid;
	INVOKEKIND invkind;
	UINT index;
	CComBSTR name;
};

// Interface

class Interface
{
public:
	friend InterfaceMap;

	Interface();
	~Interface();

	HRESULT Init(REFIID riid);

	HRESULT Load(ITypeInfo* pTypeInfo, TYPEATTR* pTypeAttr);
	HRESULT LoadFuncs(ITypeInfo* pTypeInfo, LONG count);

	HRESULT AddFunc(FUNCDESC* pFuncDesc, ITypeInfo* pTypeInfo);

	ULONG GetFuncCount() { return m_aFunc.GetCount(); }
	FuncDesc* GetFunc(ULONG index) { return m_aFunc[index]; }
	FuncDesc* SafeGetFunc(ULONG index);

public:
	UINT m_hash;

	IID m_iid;

	CComBSTR m_name;

	CAutoPtrArray<FuncDesc> m_aFunc;

protected:
	Interface* m_pNext;
};

// InterfaceMap

class InterfaceMap
{
public:
	InterfaceMap();
	~InterfaceMap();

	HRESULT LoadInterfaces(LPCWSTR fileName);
	HRESULT LoadInterfaces(REFGUID libid);
	HRESULT LoadInterfaces(ITypeLib* pTypeLib);

	HRESULT Insert(Interface* pInterface);

	Interface* GetInterface(REFIID riid);
	Interface* Lookup(REFIID riid);

	HRESULT GetInterfaceName(REFIID riid, BSTR* pName);
	HRESULT GetMethodName(REFIID riid, int methodId, BSTR* pItfName, BSTR* pMethodName);

	HRESULT GetInterfaceName(REFIID riid, CStringA& name);
	HRESULT GetMethodName(REFIID riid, int methodId, CStringA& itfName, CStringA& methodName);

	UINT Hash(REFGUID guid);

	HRESULT InitBins();

	void Clear();

protected:
	ULONG m_count;
	ULONG m_binCount;
	Interface** m_ppBins;
};

extern InterfaceMap _interfaceMap;