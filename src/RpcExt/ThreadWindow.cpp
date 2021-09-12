#include "StdAfx.h"
#include "ThreadWindow.h"

#include "atlwin.h"

RpcThreadWindow::RpcThreadWindow()
{
}

RpcThreadWindow::~RpcThreadWindow()
{
}

ATOM RpcThreadWindow::s_atom = 0;

ATOM RpcThreadWindow::RegisterClass()
{
	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hInstance = _AtlBaseModule.GetModuleInstance();
	wcex.lpszClassName = L"RpcThreadWindow";
	wcex.lpfnWndProc = WndProc;

	return RegisterClassEx(&wcex);
}

HWND RpcThreadWindow::Create()
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadWindow::Create, ThreadId : %d", GetCurrentThreadId());

	if (!s_atom)
	{
		s_atom = RegisterClass();
		if (!s_atom)
			return nullptr;
	}

	HWND hWndParent = GetDesktopWindow();
	HWND hWnd = ::CreateWindowEx(0, MAKEINTATOM(s_atom), nullptr,
		WS_CHILD, 0, 0, 0, 0, hWndParent, nullptr, _AtlBaseModule.GetModuleInstance(), nullptr);

	return hWnd;
}

LRESULT CALLBACK RpcThreadWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_logFile.WriteV(LogCategory::Misc, LogLevel::General, LogOption::Std, "RpcThreadWindow::WndProc, message = #%x, thread id : %d", message, GetCurrentThreadId());

	if (message == RPC_WM_INVOKE)
	{
		_RpcInvokeProc pfnProc = (_RpcInvokeProc)wParam;
		if (pfnProc)
			return pfnProc(lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
