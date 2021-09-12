#pragma once

class RpcHashMap
{
public:

	static UINT Hash(REFGUID guid);

	static ULONG BinSize(ULONG maxCount);
	static BOOL Rehash(ULONG bicount, ULONG elements);
};