#pragma once

class RpcBuffer;

class ErrorInfo : public CComObjectRootEx<CComMultiThreadModel>,
				  public IErrorInfo
{
public:
	ErrorInfo();
	~ErrorInfo();

	static HRESULT _CreateInstance(RpcBuffer* pBuffer, REFIID riid, void** ppv);

	BEGIN_COM_MAP(ErrorInfo)
		COM_INTERFACE_ENTRY(IErrorInfo)
	END_COM_MAP()

	HRESULT Load(RpcBuffer* pBuffer);

	STDMETHOD(GetGUID)(GUID* pGUID);

	STDMETHOD(GetSource)(BSTR* pBstrSource);

	STDMETHOD(GetDescription)(BSTR* pBstrDescription);

	STDMETHOD(GetHelpFile)(BSTR* pBstrHelpFile);

	STDMETHOD(GetHelpContext)(DWORD* pdwHelpContext);

protected:
	GUID m_guid;
	DWORD m_context;

	CComBSTR m_source;
	CComBSTR m_description;
	CComBSTR m_helpFile;
};

class ErrorInfoBuffer
{
public:
	ErrorInfoBuffer();
	~ErrorInfoBuffer();

	void Set(IErrorInfo* pInfo);
	void Report();

protected:
	CComAutoCriticalSection m_cs;

	IErrorInfo* m_pInfo;
};