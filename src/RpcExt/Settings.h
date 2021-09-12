#pragma once

class ThreadPoolSetting
{
public:
	HRESULT Query(HKEY hContextKey);

	DWORD threadTimeout = 60 * 1000;
	ULONG maxThreadCount = 1000;
	ULONG maxTaskCount = 10000;
};

class ConnectionSetting
{
public:
	HRESULT Query(HKEY hContextKey);

	int idle = 500;

	int connectInterval = 10 * 1000;
	int connectTimeout = 30 * 1000;

	int testInterval = 60 * 1000;
	int testCount = 3;

	int expireSpan = 60 * 1000;
};

class AuthSetting
{
public:
	HRESULT Query(HKEY hContextKey);
};

class RpcSettings
{
public:
	HRESULT Query();

	ThreadPoolSetting threadPool;
	ConnectionSetting connection;
};