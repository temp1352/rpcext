#include "stdafx.h"
#include "Apartment.h"

//#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
//typedef enum
//{
//	APTTYPEQUALIFIER_NONE                 = 0,
//	APTTYPEQUALIFIER_IMPLICIT_MTA         = 1,
//	APTTYPEQUALIFIER_NA_ON_MTA            = 2,
//	APTTYPEQUALIFIER_NA_ON_STA            = 3,
//	APTTYPEQUALIFIER_NA_ON_IMPLICIT_MTA   = 4,
//	APTTYPEQUALIFIER_NA_ON_MAINSTA        = 5
//} APTTYPEQUALIFIER;
//#endif

typedef HRESULT(WINAPI* _GetApartmentTypeProc)(APTTYPE*, APTTYPEQUALIFIER*);
_GetApartmentTypeProc _getAptTypeProc = nullptr;

FARPROC _GetOleProc(LPCSTR lpProcName)
{
	HMODULE hModule = GetModuleHandle(L"ole32.dll");
	if (hModule)
	{
		return GetProcAddress(hModule, lpProcName);
	}

	return nullptr;
}

APTTYPE Apartment::GetType()
{
	APTTYPE type = APTTYPE_STA;

	if (_getAptTypeProc == nullptr)
	{
		_getAptTypeProc = (_GetApartmentTypeProc)_GetOleProc("CoGetApartmentType");
		if (!_getAptTypeProc)
			return type;
	}

	APTTYPEQUALIFIER qu;

	_getAptTypeProc(&type, &qu);

	return type;
}