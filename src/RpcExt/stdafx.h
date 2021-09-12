// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include <SDKDDKVer.h>

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // ĳЩ CString ���캯��������ʽ��

#define RE_EXPORTS
#define SECURITY_WIN32

#include "resource.h"

#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlstr.h>
#include <atlcomtime.h>

using namespace ATL;

#include <xreg.h>
#include <xcom.h>
#include <xerror.h>

using namespace XTL;

#include "dcom.h"

#include "RpcShell.h"

#include "LogBase/LogFile.h"

extern __declspec(selectany) LogFile _logFile(L"rpc-ext", nullptr, LOG_AUTO_CLEAN, 0, 0xffff, 5);

