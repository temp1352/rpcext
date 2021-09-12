#pragma once

#include "Sid.h"
#include <BaseCL/MapStringToUnk.h>

class SidMap
{
public:
	SidMap();
	~SidMap();

	HRESULT GetSid(LPCWSTR accountName, REFIID riid, void** ppv);

protected:
	CComAutoCriticalSection m_cs;

	MapStringToUnk m_map;
};