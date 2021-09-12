#pragma once

#include "ScApi.h"

class SC_API ScGlobal
{
public:
	static HRESULT GetSid(LPCWSTR accountName, REFIID riid, void** ppv);
	static HRESULT CreateAccessToken(HANDLE hToken, REFIID riid, void** ppv);
	static HRESULT CreateAccessChannel(REFGUID appId, REFIID riid, void** ppv);
};