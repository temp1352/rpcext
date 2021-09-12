#pragma once

#include "ScApi.h"

class SC_API CScGlobal
{
public:
	static HRESULT GetSid(LPCWSTR accountName, REFIID riid, void** ppv);
};