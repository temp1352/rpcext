#pragma once

typedef LRESULT (*_RpcInvokeProc)(LPARAM);

_declspec(selectany) UINT RPC_WM_INVOKE = RegisterWindowMessage(L"RPC_WM_INVOKE");

class RpcThreadWindow
{
public:
	RpcThreadWindow();
	~RpcThreadWindow();

	static ATOM RegisterClass();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static HWND Create();

public:
	static ATOM s_atom;
};
