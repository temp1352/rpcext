#pragma once

#ifndef _ATL_COM
#define _ATL_COM
#include <atlbase.h>
#include <atlcom.h>
#endif

namespace XTL
{

#define HR_NOK_RET(x) \
	{ \
		HRESULT hr = x; \
		if (hr != S_OK) \
			return hr; \
	}

#define _HR_NOK_RET(x) \
	hr = x; \
	if (hr != S_OK) \
		return hr;

#define HR_FAILED_RET(x) \
	{ \
		HRESULT hr = x; \
		if (FAILED(x)) \
			return hr; \
	}

#define _HR_FAILED_RET \
	(x) hr = x; \
	if (hr != S_OK) \
		return hr;

#define THROW_HR(x) \
	{ \
		HRESULT hr = x; \
		if (hr != S_OK) \
			AtlThrow(hr); \
	}
#define THROW_HR_F(x) \
	{ \
		HRESULT hr = x; \
		if (FAILED(hr)) \
			AtlThrow(hr); \
	}

	inline HRESULT WINAPI _Chain(void* pv, REFIID iid, void** ppvObject, DWORD_PTR dw)
	{
		_ATL_CHAINDATA* pcd = (_ATL_CHAINDATA*)dw;
		void* p = (void*)((DWORD_PTR)pv + pcd->dwOffset);
		return AtlInternalQueryInterface(p, pcd->pFunc(), iid, ppvObject);
	}

#define BEGIN_INTERFACE_MAP(x) \
	typedef x _ComMapClass; \
	virtual HRESULT _InternalQueryInterface(REFIID iid, void** ppvObject) throw() \
	{ \
		return AtlInternalQueryInterface(this, _GetEntries(), iid, ppvObject); \
	} \
	const static ATL::_ATL_INTMAP_ENTRY* WINAPI _GetEntries() throw() \
	{ \
		static const ATL::_ATL_INTMAP_ENTRY _entries[] = {

#define END_INTERFACE_MAP() \
	{ \
		nullptr, 0, 0 \
	} \
	} \
	; \
	return _entries; \
	}

#define BEGIN_INTERFACE_STRUCT(x) \
	typedef x _ComMapClass; \
	static const ATL::_ATL_INTMAP_ENTRY _entries[] = {

#define END_INTERFACE_STRUCT() \
	{ \
		nullptr, 0, 0 \
	} \
	} \
	;

#define BEGIN_INTERFACE_ENTRIES(x) \
	virtual const ATL::_ATL_INTMAP_ENTRY* WINAPI _GetThisEntries() throw() \
	{ \
		return _GetEntries(); \
	} \
	static const ATL::_ATL_INTMAP_ENTRY* WINAPI _GetEntries() throw() \
	{ \
		typedef x _ComMapClass; \
		static const ATL::_ATL_INTMAP_ENTRY _entries[] = {

#define END_INTERFACE_ENTRIES() \
	{ \
		nullptr, 0, 0 \
	} \
	} \
	; \
	return _entries; \
	}

#define BEGIN_QUERY_INTERFACE(x) \
	STDMETHODIMP x::QueryInterface(REFIID riid, void** ppv) \
	{ \
		typedef x _ComMapClass; \
		static const ATL::_ATL_INTMAP_ENTRY _entries[] = {

#define END_QUERY_INTERFACE() \
	{ \
		nullptr, 0, 0 \
	} \
	} \
	; \
	return AtlInternalQueryInterface(this, _entries, riid, ppv); \
	}

#define BEGIN_COM_MAP_0(x) BEGIN_COM_MAP(x)

#define COM_INTERFACE_ENTRY_AGGREGATE_(iid, obj, base, derived) \
	{ &iid, (DWORD_PTR)offsetof(_ComMapClass, obj) + offsetofclass(base, obj), _Delegate },

#define COM_INTERFACE_ENTRY_AGGREGATE_BLIND_(obj, base, derived) \
	{ NULL, (DWORD_PTR)offsetof(_ComMapClass, obj) + offsetofclass(base, derived), _Delegate },

#ifdef _ATL_DEBUG
#define END_COM_MAP_0() \
	{ \
		nullptr, 0, 0 \
	} \
	} \
	; \
	return &_entries[1]; \
	}

#else
#define END_COM_MAP_0() \
	{ \
		nullptr, 0, 0 \
	} \
	} \
	; \
	return _entries; \
	}

#endif

#define BEGIN_COM_MAP_1(x) BEGIN_COM_MAP(x)

#ifdef _ATL_DEBUG
#define END_COM_MAP_1() \
	{ \
		nullptr, 0, 0 \
	} \
	} \
	; \
	return &_entries[1]; \
	} \
	virtual ULONG STDMETHODCALLTYPE AddRef(void) throw() { return 1; } \
	virtual ULONG STDMETHODCALLTYPE Release(void) throw() { return 1; } \
	STDMETHOD(QueryInterface)(REFIID riid, void** ppv) throw() { return _InternalQueryInterface(riid, ppv); }

#else
#define END_COM_MAP_1() \
	{ \
		nullptr, 0, 0 \
	} \
	} \
	; \
	return _entries; \
	} \
	virtual ULONG STDMETHODCALLTYPE AddRef(void) throw() { return 1; } \
	virtual ULONG STDMETHODCALLTYPE Release(void) throw() { return 1; } \
	STDMETHOD(QueryInterface)(REFIID riid, void** ppv) throw() { return _InternalQueryInterface(riid, ppv); }

#endif // _ATL_DEBUG

	namespace ComApi
	{
		template <class T>
		T* Assign(T** ppT, T* pT)
		{
			if (ppT == nullptr)
			{
				return nullptr;
			}

			if (*ppT != pT)
			{
				if (pT != nullptr)
				{
					pT->AddRef();
				}

				if (*ppT)
				{
					(*ppT)->Release();
				}

				*ppT = pT;
			}

			return pT;
		}

		template <class T>
		HRESULT QIAssign(T** ppT, IUnknown* pUnk, HRESULT nullResult = S_OK)
		{
			if (ppT == nullptr)
				return E_POINTER;

			if (*ppT)
			{
				(*ppT)->Release();
				*ppT = nullptr;
			}

			if (pUnk)
			{
				return pUnk->QueryInterface(__uuidof(T), (void**)ppT);
			}

			return nullResult;
		}

		template <class T>
		HRESULT CompareAssign(T** ppT, T* pT)
		{
			if (ppT == nullptr)
				return E_POINTER;

			if (*ppT != pT)
			{
				if (*ppT)
				{
					(*ppT)->Release();
				}

				*ppT = pT;

				return S_OK;
			}

			return S_FALSE;
		}

		template <class T>
		HRESULT CompareQIAssign(T** ppT, IUnknown* pUnk)
		{
			if (ppT == nullptr)
				return E_POINTER;

			T* pT = nullptr;
			if (pUnk)
			{
				pUnk->QueryInterface(__uuidof(T), (void**)&pT);
			}

			if (*ppT != pT)
			{
				if (*ppT)
				{
					(*ppT)->Release();
				}

				*ppT = pT;

				return S_OK;
			}

			return S_FALSE;
		}

		template <class T>
		T* Attach(T** ppT, T* pT)
		{
			if (ppT == nullptr)
			{
				return nullptr;
			}

			if (*ppT != pT)
			{
				if (*ppT)
				{
					(*ppT)->Release();
				}

				*ppT = pT;
			}

			return pT;
		}

		template <class T>
		T* Detach(T** ppT)
		{
			auto pT = *ppT;
			*ppT = nullptr;

			return pT;
		}

		template <class T>
		HRESULT QueryInterface(IUnknown* pUnk, T** ppT)
		{
			return pUnk ? pUnk->QueryInterface(__uuidof(T), (void**)ppT) : E_NOINTERFACE;
		}

		template <class T>
		T* QueryInterface(IUnknown* pUnk)
		{
			if (!pUnk)
				return nullptr;

			T* pT = nullptr;
			HRESULT hr = pUnk->QueryInterface(__uuidof(T), (void**)&pT);

			return pT;
		}

		template <class T>
		T* AddRef(T* pT)
		{
			if (pT)
			{
				pT->AddRef();
			}

			return pT;
		}

		template <class T>
		void Release(T** ppT)
		{
			ATLASSERT(ppT);

			if (*ppT)
			{
				(*ppT)->Release();
				*ppT = nullptr;
			}
		}

		template <class T, typename Func>
		void Release(T** ppT, Func func)
		{
			ATLASSERT(ppT);

			if (*ppT)
			{
				func(*ppT);
				(*ppT)->Release();
				*ppT = nullptr;
			}
		}

		template <class T>
		HRESULT CopyTo(T* pT, T** ppT, HRESULT nullResult = S_FALSE)
		{
			if (!ppT)
				return E_POINTER;

			if (*ppT)
			{
				(*ppT)->Release();
			}

			*ppT = pT;

			if (pT)
			{
				pT->AddRef();

				return S_OK;
			}

			return nullResult;
		}

		template <class T>
		HRESULT CreateInstance(T** ppT, REFCLSID clsid, IUnknown* pOuter = nullptr, DWORD context = CLSCTX_ALL)
		{
			T* pT = nullptr;
			HRESULT hr = CoCreateInstance(clsid, pOuter, context, __uuidof(T), (void**)&pT);
			if (SUCCEEDED(hr))
			{
				if (*ppT)
				{
					(*ppT)->Release();
				}

				*ppT = pT;
			}

			return hr;
		}
	}

	template <class T>
	class CComClassFactorySingletonEx : public CComClassFactorySingleton<T>
	{
	public:
		HRESULT FinalConstruct()
		{
			CComObjectCached<T>* p;
			m_hrCreate = CComObjectCached<T>::CreateInstance(&p);
			if (SUCCEEDED(m_hrCreate))
			{
				m_hrCreate = p->QueryInterface(IID_IUnknown, (void**)&m_spObj);
				if (FAILED(m_hrCreate))
				{
					delete p;
				}
			}
			return m_hrCreate;
		}
	};

#define DECLARE_CLASSFACTORY_SINGLETON_EX(obj) DECLARE_CLASSFACTORY_EX(CComClassFactorySingletonEx<obj>)

	template <class Base, typename Arg1>
	class CComObject1 : public Base
	{
	public:
		typedef Base _BaseClass;

		CComObject1(Arg1 arg1)
			: Base(arg1)
		{
			_pAtlModule->Lock();
		}

		// Set refcount to -(LONG_MAX/2) to protect destruction and
		// also catch mismatched Release in debug builds
		virtual ~CComObject1()
		{
			m_dwRef = -(LONG_MAX / 2);
			FinalRelease();
#ifdef _ATL_DEBUG_INTERFACES
			_AtlDebugInterfacesModule.DeleteNonAddRefThunk(_GetRawUnknown());
#endif
			_pAtlModule->Unlock();
		}

		//If InternalAddRef or InternalRelease is undefined then your class
		//doesn't derive from CComObjectRoot
		STDMETHOD_(ULONG, AddRef)()
		{
			return InternalAddRef();
		}

		STDMETHOD_(ULONG, Release)()
		{
			ULONG l = InternalRelease();
			if (l == 0)
			{
				delete this;
			}

			return l;
		}

		//if _InternalQueryInterface is undefined then you forgot BEGIN_COM_MAP
		STDMETHOD(QueryInterface)(REFIID iid, _COM_Outptr_ void** ppvObject) throw()
		{
			return _InternalQueryInterface(iid, ppvObject);
		}

		template <class Q>
		HRESULT STDMETHODCALLTYPE QueryInterface(_COM_Outptr_ Q** pp) throw()
		{
			return QueryInterface(__uuidof(Q), (void**)pp);
		}

		static HRESULT WINAPI CreateInstance(Arg1 arg1, _COM_Outptr_ CComObject1<Base, Arg1>** pp) throw();
	};

	template <class Base, typename Arg1>
	HRESULT WINAPI CComObject1<Base, Arg1>::CreateInstance(Arg1 arg1, _COM_Outptr_ CComObject1<Base, Arg1>** pp) throw()
	{
		ATLASSERT(pp != nullptr);
		if (pp == nullptr)
			return E_POINTER;
		*pp = nullptr;

		CComObject1<Base, Arg1>* p = new CComObject1<Base, Arg1>(arg1);
		if (p == nullptr)
			return E_OUTOFMEMORY;

		p->SetVoid(nullptr);
		p->InternalFinalConstructAddRef();

		HRESULT hRes = p->_AtlInitialConstruct();
		if (SUCCEEDED(hRes))
		{
			hRes = p->FinalConstruct();
		}

		if (SUCCEEDED(hRes))
		{
			hRes = p->_AtlFinalConstruct();
		}

		p->InternalFinalConstructRelease();
		if (hRes != S_OK)
		{
			delete p;
			p = nullptr;
		}

		*pp = p;

		return hRes;
	}

	class ApartmentInterface
	{
	public:
		ApartmentInterface()
		{
			m_hGlobal = nullptr;
		}

		~ApartmentInterface()
		{
			Clear();
		}

		ApartmentInterface& operator=(ApartmentInterface& source)
		{
			if (this != &source)
			{
				Clear();

				m_hGlobal = source.m_hGlobal;
				source.m_hGlobal = nullptr;
			}

			return *this;
		}

		HRESULT Marshal(REFIID riid, IUnknown* pUnk)
		{
			Clear();

			CComPtr<IStream> spStream;
			HRESULT hr = CreateStreamOnHGlobal(0, FALSE, &spStream);
			if (hr == S_OK)
			{
				hr = CoMarshalInterface(spStream, riid, pUnk, MSHCTX_INPROC, 0, MSHLFLAGS_NORMAL);
				if (SUCCEEDED(hr))
				{
					hr = GetHGlobalFromStream(spStream, &m_hGlobal);
				}
			}

			return hr;
		}

		HRESULT Unmarshal(REFIID riid, void** ppv) const
		{
			if (!m_hGlobal)
			{
				return E_NOINTERFACE;
			}

			CComPtr<IStream> spStream;
			HRESULT hr = CreateStreamOnHGlobal(m_hGlobal, FALSE, &spStream);
			if (hr == S_OK)
			{
				hr = CoUnmarshalInterface(spStream, riid, ppv);
			}

			return hr;
		}

		void Clear()
		{
			if (m_hGlobal)
			{
				GlobalUnlock(m_hGlobal);
				GlobalFree(m_hGlobal);
				m_hGlobal = nullptr;
			}
		}

	protected:
		HGLOBAL m_hGlobal;
	};
}