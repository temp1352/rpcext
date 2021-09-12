#include "stdafx.h"
#include "HashMap.h"

UINT RpcHashMap::Hash(REFGUID guid)
{
	const DWORD* pData = reinterpret_cast<const DWORD*>(&guid);

	return (pData[0] ^ pData[1] ^ pData[2] ^ pData[3]);
}

float fOptimalLoad = 0.75f;
float fLoThreshold = 0.25f;
float fHiThreshold = 2.25f;

ULONG RpcHashMap::BinSize(ULONG maxCount)
{
	static const ULONG _primes[] = {
		17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821,
		1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331,
		13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033,
		131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563,
		832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319,
		5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
	};

	ULONG bicount = (ULONG)(maxCount / fOptimalLoad);
	ULONG bicountEstimate = UINT_MAX < bicount ? UINT_MAX : bicount;

	// Find the smallest prime greater than our estimate
	int prime = 0;
	while (bicountEstimate > _primes[prime])
	{
		prime++;
	}

	if (_primes[prime] == UINT_MAX)
	{
		return bicountEstimate;
	}
	else
	{
		return _primes[prime];
	}
}

BOOL RpcHashMap::Rehash(ULONG bicount, ULONG elements)
{
	ULONG hiRehashThreshold = ULONG(fHiThreshold * bicount);
	ULONG loRehashThreshold = ULONG(fLoThreshold * bicount);

	if (loRehashThreshold < 17)
	{
		loRehashThreshold = 0;
	}

	return (elements > hiRehashThreshold) || (elements < loRehashThreshold);
}