#pragma once

typedef FARPROC(WINAPI* _GetProcAddressProc)(HMODULE hModule, LPCSTR lpProcName);

enum _ProcType
{
	_PROC_STATIC,
	_PROC_DYNAMIC,
};

struct _ProcEntry
{
	HMODULE hModule;
	char szName[64];
	_ProcType type;
	FARPROC proc;

	_ProcEntry* pNext;
};

class ProcEntryList
{
public:
	ProcEntryList();
	~ProcEntryList();

	HRESULT AddProc(HMODULE hModule, LPCSTR name, _ProcType type, FARPROC proc);
	BOOL RemoveProc(HMODULE hModule, LPCSTR name);
	_ProcEntry* GetProcEntry(HMODULE hModule, LPCSTR name);

	FARPROC _GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

	void Clear(HMODULE hModule = nullptr);

protected:
	_ProcEntry* m_pHead;
	_ProcEntry* m_pTail;
};

extern ProcEntryList _procList;

class ProcReplacement
{
public:
	ProcReplacement();
	~ProcReplacement();

	HRESULT Initialize();
	void Uninitialize();

	BOOL ReplaceIATEntry(HMODULE hModCaller, LPCSTR modCallee, PROC pfnCurrent, PROC pfnNew);

	HRESULT AddProc(LPCSTR name, _ProcType type, FARPROC proc);
	HRESULT AddProc(LPCSTR name, _ProcType type, FARPROC proc, FARPROC* pOldProc);
	BOOL RemoveProc(LPCSTR name);

	static FARPROC WINAPI _GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

protected:
	HMODULE m_hModule;

	static LPCSTR s_coreMod;
	static _GetProcAddressProc s_procGetProc;
};
