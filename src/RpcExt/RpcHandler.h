#pragma once

struct _ExecuteContext
{
	HANDLE hStopeEvent;
};

__interface __declspec(uuid("9F892E04-4C57-4113-A19A-9D6AF57A7618"))
	IRpcHandler : public IUnknown
{
	STDMETHOD(Execute)(_ExecuteContext * pContext) = 0;
};
