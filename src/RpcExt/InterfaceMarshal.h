#pragma once

typedef HRESULT(WINAPI* _MarshalInterfaceProc)(LPSTREAM pStm, REFIID riid, LPUNKNOWN pUnk,
	DWORD dwDestContext, LPVOID pvDestContext, DWORD mshlflags);

typedef HRESULT(WINAPI* _UnmarshalInterfaceProc)(LPSTREAM pStm, REFIID riid, LPVOID* ppv);

class InterfaceMarshal
{
public:
	static HRESULT Initialize();

	static HRESULT WINAPI MarshalInterface(LPSTREAM pStm, REFIID riid, LPUNKNOWN pUnk,
		DWORD dwDestContext, LPVOID pvDestContext, DWORD mshlflags);

	static HRESULT WINAPI MarshalEx(LPSTREAM pStm, REFIID riid, LPUNKNOWN pUnk,
		DWORD dwDestContext, LPVOID pvDestContext, DWORD mshlflags);

	static HRESULT WINAPI UnmarshalInterface(LPSTREAM pStm, REFIID riid, LPVOID* ppv);

	static HRESULT WINAPI UnmarshalEx(LPSTREAM pStm, REFIID riid, LPVOID* ppv);
};
