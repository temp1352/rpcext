#pragma once

#ifdef LOG_BASE_EXPORTS
#define LOG_BASE_API __declspec(dllexport)
#else
#define LOG_BASE_API __declspec(dllimport)
#endif

namespace LogLevel
{
	enum
	{
		Severe,	   // 严重错误
		Error,	   // 错误
		Warning,   // 警告
		Important, // 重要信息
		General,   // 一般信息
		Details,   // 详细信息
	};

	typedef LONG Type;
};

namespace LogCategory
{
	enum
	{
		Issue = 0x01,  // 问题
		Misc = 0x02,   // 杂项
		UI = 0x04,	   // 用户界面
		Comm = 0x08,   // 通信
		FileIO = 0x10, // 文件IO
		System = 0x20, // 系统
		Service = 0x40,
		Business = 0x80,
		Debug = 0x0100,
		Test = 0x0200,

		All = Issue | Misc | UI | Comm | FileIO | System | Service | Business | Debug | Test,
	};

	typedef LONG Type;
};

class LogOption
{
public:
	enum
	{
		Default = 0,

		DateTime = 0x0100,
		ThreadId = 0x0200,
		EndLine = 0x0400,
		NewLine = 0x0800,
		ClearErrorInfo = 0x1000,

		Std = DateTime | ThreadId | EndLine,
	};
};

typedef DWORD LogOptions;