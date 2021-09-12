#include "StdAfx.h"
#include "ProcReplacement.h"

#include "Dbghelp.h"

#pragma comment(lib, "DbgHelp.lib")

ProcEntryList _procList;

ProcEntryList::ProcEntryList()
{
	m_pHead = nullptr;
	m_pTail = nullptr;
}

ProcEntryList::~ProcEntryList()
{
	Clear();
}

HRESULT ProcEntryList::AddProc(HMODULE hModule, LPCSTR name, _ProcType type, FARPROC proc)
{
	if (!name || !proc)
		return E_INVALIDARG;

	_ProcEntry* pEntry = new _ProcEntry;
	if (!pEntry)
		return E_OUTOFMEMORY;

	pEntry->hModule = hModule;
	pEntry->type = type;
	pEntry->proc = proc;

	int n = min(strlen(name), _countof(pEntry->szName) - 1);
	strncpy_s(pEntry->szName, name, n);
	pEntry->szName[n] = '\0';

	pEntry->pNext = nullptr;

	if (m_pTail)
		m_pTail->pNext = pEntry;
	else
		m_pHead = pEntry;

	m_pTail = pEntry;

	return S_OK;
}

BOOL ProcEntryList::RemoveProc(HMODULE hModule, LPCSTR name)
{
	return FALSE;
}

_ProcEntry* ProcEntryList::GetProcEntry(HMODULE hModule, LPCSTR name)
{
	if (!hModule || !name || HIWORD((DWORD)name) == 0)
		return nullptr;

	_ProcEntry* pEntry = m_pHead;
	while (pEntry)
	{
		if (pEntry->hModule == hModule && _stricmp(pEntry->szName, name) == 0)
			return pEntry;

		pEntry = pEntry->pNext;
	}

	return nullptr;
}

FARPROC ProcEntryList::_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	_ProcEntry* pEntry = m_pHead;
	while (pEntry)
	{
		if ((pEntry->hModule == nullptr || pEntry->hModule == hModule) && _stricmp(pEntry->szName, lpProcName) == 0)
		{
			if (pEntry->type == _PROC_DYNAMIC)
			{
				FARPROC proc = ((_GetProcAddressProc)pEntry->proc)(hModule, lpProcName);
				if (proc)
					return proc;
			}
			else
			{
				return pEntry->proc;
			}
		}

		pEntry = pEntry->pNext;
	}

	return nullptr;
}

void ProcEntryList::Clear(HMODULE hModule)
{
	_ProcEntry* pPrev = nullptr;
	_ProcEntry* pEntry = m_pHead;
	while (pEntry)
	{
		_ProcEntry* pTemp = pEntry;
		pEntry = pEntry->pNext;

		if (hModule == nullptr || pTemp->hModule == hModule)
		{
			if (pPrev)
				pPrev->pNext = pTemp->pNext;
			else
				m_pHead = pTemp->pNext;

			if (m_pTail == pTemp)
				m_pTail = pPrev;

			delete pTemp;
		}
		else
			pPrev = pTemp;
	}
}

static LPCSTR _coreDll[] = {
	"API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", // win 8.1
	"API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", // win 8
	"API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", // win 7
	"Kernel32.dll",								// win xp
};

static LPCSTR _comDll[] = {
	"API-MS-WIN-CORE-COM-L1-1-1.DLL", // win 8.1
	"API-MS-WIN-CORE-COM-L1-1-0.DLL", // win 8
	"ole32.dll",
};

static char _getProcAddress[] = "GetProcAddress";

ProcReplacement::ProcReplacement()
{
	m_hModule = nullptr;
}

LPCSTR ProcReplacement::s_coreMod = nullptr;
_GetProcAddressProc ProcReplacement::s_procGetProc = nullptr;

ProcReplacement::~ProcReplacement()
{
	Uninitialize();
}

HRESULT ProcReplacement::Initialize()
{
	if (s_procGetProc == nullptr)
	{
		for (int i = 0; i < _countof(_coreDll); ++i)
		{
			HMODULE hModCore = GetModuleHandleA(_coreDll[i]);
			if (hModCore)
			{
				s_procGetProc = (_GetProcAddressProc)::GetProcAddress(hModCore, _getProcAddress);
				if (s_procGetProc)
				{
					s_coreMod = _coreDll[i];
					break;
				}
			}
		}
	}

	if (!s_procGetProc)
		return E_FAIL;

	for (int i = 0; i < _countof(_comDll); ++i)
	{
		m_hModule = GetModuleHandleA(_comDll[i]);
		if (m_hModule)
		{
			if (ReplaceIATEntry(m_hModule, s_coreMod, (PROC)s_procGetProc, (PROC)_GetProcAddress))
				break;
		}
	}

	return S_OK;
}

void ProcReplacement::Uninitialize()
{
	_procList.Clear(m_hModule);
}

typedef PVOID(WINAPI* _ImageDirectoryEntryToDataFunc)(PVOID, BOOLEAN, USHORT, PULONG);

_ImageDirectoryEntryToDataFunc _imageDETD = []() -> _ImageDirectoryEntryToDataFunc {
	HINSTANCE hInst = LoadLibrary(L"DbgHelp.dll");
	if (hInst)
		return (_ImageDirectoryEntryToDataFunc)::GetProcAddress(hInst, "ImageDirectoryEntryToData");

	return nullptr;
}();

PVOID _ImageDirectoryEntryToData(PVOID Base, BOOLEAN MappedAsImage, USHORT DirectoryEntry, PULONG Size)
{
	return _imageDETD ? _imageDETD(Base, MappedAsImage, DirectoryEntry, Size) : nullptr;
}

BOOL ProcReplacement::ReplaceIATEntry(HMODULE hModCaller, LPCSTR modCallee, PROC pfnCurrent, PROC pfnNew)
{
	ULONG ulSize = 0;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)_ImageDirectoryEntryToData(
		hModCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

	if (pImportDesc == nullptr)
		return FALSE;

	for (; pImportDesc->Name; ++pImportDesc)
	{
		LPCSTR pszModName = (LPCSTR)((LPBYTE)hModCaller + pImportDesc->Name);
		if (lstrcmpiA(pszModName, modCallee) == 0)
			break;
	}

	if (pImportDesc->Name == 0)
		return FALSE;

	HANDLE hProcess = GetCurrentProcess();

	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((LPBYTE)hModCaller + pImportDesc->FirstThunk);
	for (; pThunk->u1.Function; ++pThunk)
	{
		PROC* ppfn = (PROC*)&pThunk->u1.Function;
		if ((*ppfn == pfnCurrent))
		{
			DWORD dwOld = 0;
			BOOL bRet = VirtualProtectEx(hProcess, ppfn, sizeof(PROC), PAGE_EXECUTE_READWRITE, &dwOld);
			if (bRet)
			{
				SIZE_T nBytes = 0;
				bRet = WriteProcessMemory(hProcess, ppfn, &pfnNew, sizeof(pfnNew), &nBytes);
			}

			return bRet;
		}
	}

	return FALSE;
}

HRESULT ProcReplacement::AddProc(LPCSTR name, _ProcType type, FARPROC proc)
{
	return _procList.AddProc(m_hModule, name, type, proc);
}

HRESULT ProcReplacement::AddProc(LPCSTR name, _ProcType type, FARPROC proc, FARPROC* pOldProc)
{
	if (pOldProc)
		*pOldProc = ::GetProcAddress(m_hModule, name);

	return _procList.AddProc(m_hModule, name, type, proc);
}

BOOL ProcReplacement::RemoveProc(LPCSTR name)
{
	return _procList.RemoveProc(m_hModule, name);
}

FARPROC WINAPI ProcReplacement::_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	AtlTrace("ProcReplacement::_GetProcAddress, procName : %s\n", lpProcName);

	if (!hModule || !lpProcName || HIWORD((DWORD)lpProcName) == 0)
		return s_procGetProc(hModule, lpProcName);

	FARPROC proc = _procList._GetProcAddress(hModule, lpProcName);
	if (proc)
		return proc;

	return s_procGetProc(hModule, lpProcName);
}
