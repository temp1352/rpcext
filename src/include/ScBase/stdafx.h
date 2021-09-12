// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN //  从 Windows 头文件中排除极少使用的信息

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // 某些 CString 构造函数将是显式的

#define SC_EXPORTS

#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>

using namespace ATL;

#include <xreg.h>
#include <xstrbuf.h>
#include <xstream.h>

using namespace XTL;
