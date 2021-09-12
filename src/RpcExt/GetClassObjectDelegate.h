#pragma once

class GetClassObjectDelegate
{
public:
	static HRESULT Initialize();

	static HRESULT WINAPI GetClassObject(int index, REFCLSID rclsid, REFIID riid, LPVOID* ppv);

	static FARPROC WINAPI GetClassObjectProc(HMODULE hModule, LPCSTR lpProcName);

protected:
};
