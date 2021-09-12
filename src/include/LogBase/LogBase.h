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
		Severe,	   // ���ش���
		Error,	   // ����
		Warning,   // ����
		Important, // ��Ҫ��Ϣ
		General,   // һ����Ϣ
		Details,   // ��ϸ��Ϣ
	};

	typedef LONG Type;
};

namespace LogCategory
{
	enum
	{
		Issue = 0x01,  // ����
		Misc = 0x02,   // ����
		UI = 0x04,	   // �û�����
		Comm = 0x08,   // ͨ��
		FileIO = 0x10, // �ļ�IO
		System = 0x20, // ϵͳ
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