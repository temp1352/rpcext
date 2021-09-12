#pragma once

// RpcExt ʵ��Զ���̵��ã��ͻ��˺ͷ����ʹ��windows��COM���������ݴ���ʹ���Զ����TCP/IPЭ�飬֧���Զ��������Զ��Ͽ������ְ�ȫ��֤�� �ָ��Ự����Ȩ���¼��������õȹ��ܡ�

#ifdef RE_EXPORTS
#define RE_API __declspec(dllexport)
#else
#define RE_API __declspec(dllimport)
#endif

#include "ScBase/ScBase.h"
#include "RpcShell_i.h"

// ����ѡ��
typedef enum _RE_CONNECT_OPTION
{
	RE_CF_ASYNC = 0x01,				  // �첽����
	RE_CF_AUTO_RECONNECT = 0x02,	  // �Զ�����
	RE_CF_AUTO_DISCONNECT = 0x04,	  // �Զ��Ͽ�����
	RE_CF_ALWAYS_EX = 0x08,			  // ʼ��ʹ��RpcExt, ��ʹ��������ͻ���λ��ͬһ����
	RE_CF_FIRST_CONNECT_ONE = 0x10,	  // �״�����ֻ��������һ��
	RE_CF_INTEGRATED_SECURITY = 0x20, // ���ü��ɰ�ȫ
} RE_CONNECT_OPTION;

typedef struct _RE_CONNECTINFO
{
	DWORD options; // ����ѡ��
	DWORD timeout; // ���ӳ�ʱ����λ����
	DWORD expireSpan; // ���ӹ���ʱ��, ��λ����
	DWORD port; // �˿�
	LPCWSTR serverName; // ����������
	LPCWSTR domain; // ����
	LPCWSTR referer; // ������Դ
	LPCWSTR params; // ���Ӳ���
	LPCWSTR sessionId; // �ỰID
	LPCWSTR auth_code; // ��Ȩ��
	LPCWSTR userName; // �û���
	LPCWSTR password; // ����
	LPCWSTR appName; // app����
} RE_CONNECTINFO;

// ��ʼ��COM����, �ͻ��˵���
HRESULT RE_API __stdcall RE_Initialize(DWORD coInit);

// �ͷ�COM�������ͻ��˵���
void RE_API __stdcall RE_Uninitialize();

// ��ʼ��OLE������ �ͻ��˵���
HRESULT RE_API __stdcall RE_OleInitialize();

// �ͷ�OLE������ �ͻ��˵���
void RE_API __stdcall RE_OleUninitialize();

//����COM����ʵ�����ͻ��˵��ã� �����ǽ����ڡ������⡢Զ�̷�����
HRESULT RE_API __stdcall RE_CreateInstance(__in REFCLSID rclsid, __in_opt LPUNKNOWN pOUnkOuter, __in DWORD clsContext,
	__in RE_CONNECTINFO* pConnInfo, __in REFIID riid, __out void** ppv);

// ��ʹ�÷�ʽ
enum RE_CLASS_USAGE
{
	RE_CU_DEFAULT = 0,
	RE_CU_AUTHORIZATION = 0x01, // ������֤
};

// ע���๤�����ɷ���˵���
HRESULT RE_API __stdcall RE_RegisterClassObject(__in REFCLSID rclsid, __in LPUNKNOWN pUnk, __in DWORD clsContext,
	__in DWORD regFlags, __in DWORD usage, __out LPDWORD pRegId);

// ע���๤�����ɷ���˵���
HRESULT RE_API __stdcall RE_RevokeClassObject(__in DWORD regId);

// ����ָ���˿ڣ�����˵���
HRESULT RE_API __stdcall RE_Listen(__in DWORD port);

// ����ָ��Ӧ�ó��򣬷���˵���
HRESULT RE_API __stdcall RE_Listen(__in LPCWSTR appName);

// ֹͣ����������˵���
HRESULT RE_API __stdcall RE_StopListening();

// ��ȡע��·��
HRESULT RE_API __stdcall RE_GetRegKeyName(__in LPCWSTR appName, __out LPWSTR path, __in DWORD size);

// ע�����Ϳ�
HRESULT RE_API __stdcall RE_RegisterTypeLib(REFGUID libid);

// ע�ᰲȫͨ��������˵���
HRESULT RE_API __stdcall RE_RegisterAccessChannel(__in REFGUID appId, __in REFGUID typeId, __in IScAccessChannel* pChannel);

// ע����ȫͨ��������˵���
HRESULT RE_API __stdcall RE_RevokeAccessChannel(__in REFGUID appId, __in REFGUID typeId);

// ��ȡ���û���������˵���
HRESULT RE_API __stdcall RE_GetCallContext(__out IRpcCallContext** ppContext);

// ��ȡ��������
HRESULT RE_API __stdcall RE_GetAccessToken(__out IScAccessToken** ppToken);
HRESULT RE_API __stdcall RE_GetAccessToken(__in REFIID riid, __out void** ppv);

// ע�����ӻص��ӿ�
HRESULT RE_API __stdcall RE_AdviseConnection(__in IUnknown* pObj, __in IRpcConnectionSink* pSink, __in BOOL addRef);

// �������ӻص��ӿ�
HRESULT RE_API __stdcall RE_UnadviseConnection(__in IUnknown* pObj, __in IRpcConnectionSink* pSink);

// ���ӽ�����飬 �ͻ��˵���
HRESULT RE_API __stdcall RE_HealthCheck(__in IUnknown* pObj);

// ��ȡ���һ������״̬�� �ͻ��˵���
HRESULT RE_API __stdcall RE_GetLastConnectResult(__in IUnknown* pObj);

#ifndef RE_EXPORTS
#pragma comment(lib, "RpcExt.lib")
#endif
