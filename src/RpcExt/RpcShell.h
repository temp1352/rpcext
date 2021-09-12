#pragma once

// RpcExt 实现远过程调用，客户端和服务端使用windows的COM技术，数据传输使用自定义的TCP/IP协议，支持自动重连、自动断开、多种安全认证、 恢复会话、授权码登录、反向调用等功能。

#ifdef RE_EXPORTS
#define RE_API __declspec(dllexport)
#else
#define RE_API __declspec(dllimport)
#endif

#include "ScBase/ScBase.h"
#include "RpcShell_i.h"

// 连接选项
typedef enum _RE_CONNECT_OPTION
{
	RE_CF_ASYNC = 0x01,				  // 异步连接
	RE_CF_AUTO_RECONNECT = 0x02,	  // 自动重连
	RE_CF_AUTO_DISCONNECT = 0x04,	  // 自动断开连接
	RE_CF_ALWAYS_EX = 0x08,			  // 始终使用RpcExt, 即使服务器与客户端位于同一主机
	RE_CF_FIRST_CONNECT_ONE = 0x10,	  // 首次连接只尝试连接一次
	RE_CF_INTEGRATED_SECURITY = 0x20, // 启用集成安全
} RE_CONNECT_OPTION;

typedef struct _RE_CONNECTINFO
{
	DWORD options; // 连接选项
	DWORD timeout; // 连接超时，单位毫秒
	DWORD expireSpan; // 连接过期时间, 单位毫秒
	DWORD port; // 端口
	LPCWSTR serverName; // 服务器名称
	LPCWSTR domain; // 域名
	LPCWSTR referer; // 调用来源
	LPCWSTR params; // 附加参数
	LPCWSTR sessionId; // 会话ID
	LPCWSTR auth_code; // 授权码
	LPCWSTR userName; // 用户名
	LPCWSTR password; // 密码
	LPCWSTR appName; // app名称
} RE_CONNECTINFO;

// 初始化COM环境, 客户端调用
HRESULT RE_API __stdcall RE_Initialize(DWORD coInit);

// 释放COM环境，客户端调用
void RE_API __stdcall RE_Uninitialize();

// 初始化OLE环境， 客户端调用
HRESULT RE_API __stdcall RE_OleInitialize();

// 释放OLE环境， 客户端调用
void RE_API __stdcall RE_OleUninitialize();

//创建COM对象实例，客户端调用， 可以是进程内、进程外、远程服务器
HRESULT RE_API __stdcall RE_CreateInstance(__in REFCLSID rclsid, __in_opt LPUNKNOWN pOUnkOuter, __in DWORD clsContext,
	__in RE_CONNECTINFO* pConnInfo, __in REFIID riid, __out void** ppv);

// 类使用方式
enum RE_CLASS_USAGE
{
	RE_CU_DEFAULT = 0,
	RE_CU_AUTHORIZATION = 0x01, // 启用认证
};

// 注册类工厂，由服务端调用
HRESULT RE_API __stdcall RE_RegisterClassObject(__in REFCLSID rclsid, __in LPUNKNOWN pUnk, __in DWORD clsContext,
	__in DWORD regFlags, __in DWORD usage, __out LPDWORD pRegId);

// 注销类工厂，由服务端调用
HRESULT RE_API __stdcall RE_RevokeClassObject(__in DWORD regId);

// 监听指定端口，服务端调用
HRESULT RE_API __stdcall RE_Listen(__in DWORD port);

// 监听指定应用程序，服务端调用
HRESULT RE_API __stdcall RE_Listen(__in LPCWSTR appName);

// 停止监听，服务端调用
HRESULT RE_API __stdcall RE_StopListening();

// 获取注册路径
HRESULT RE_API __stdcall RE_GetRegKeyName(__in LPCWSTR appName, __out LPWSTR path, __in DWORD size);

// 注册类型库
HRESULT RE_API __stdcall RE_RegisterTypeLib(REFGUID libid);

// 注册安全通道，服务端调用
HRESULT RE_API __stdcall RE_RegisterAccessChannel(__in REFGUID appId, __in REFGUID typeId, __in IScAccessChannel* pChannel);

// 注销安全通道，服务端调用
HRESULT RE_API __stdcall RE_RevokeAccessChannel(__in REFGUID appId, __in REFGUID typeId);

// 获取调用环境，服务端调用
HRESULT RE_API __stdcall RE_GetCallContext(__out IRpcCallContext** ppContext);

// 获取访问令牌
HRESULT RE_API __stdcall RE_GetAccessToken(__out IScAccessToken** ppToken);
HRESULT RE_API __stdcall RE_GetAccessToken(__in REFIID riid, __out void** ppv);

// 注入连接回调接口
HRESULT RE_API __stdcall RE_AdviseConnection(__in IUnknown* pObj, __in IRpcConnectionSink* pSink, __in BOOL addRef);

// 撤出连接回调接口
HRESULT RE_API __stdcall RE_UnadviseConnection(__in IUnknown* pObj, __in IRpcConnectionSink* pSink);

// 连接健康检查， 客户端调用
HRESULT RE_API __stdcall RE_HealthCheck(__in IUnknown* pObj);

// 获取最近一次连接状态， 客户端调用
HRESULT RE_API __stdcall RE_GetLastConnectResult(__in IUnknown* pObj);

#ifndef RE_EXPORTS
#pragma comment(lib, "RpcExt.lib")
#endif
