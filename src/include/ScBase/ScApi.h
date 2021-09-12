#pragma once

#include "ScBase.h"

#ifdef SC_EXPORTS
#define SC_API __declspec(dllexport)
#else
#define SC_API __declspec(dllimport)
#pragma comment(lib, "ScBase.lib")
#endif
