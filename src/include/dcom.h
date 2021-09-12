

/* this ALWAYS GENERATED file contains the definitions for the interfaces */

/* File created by MIDL compiler version 7.00.0500 */
/* at Fri Jun 11 11:52:07 2010
 */
/* Compiler settings for .\dcom.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning(disable : 4049) /* more than 64k source lines */

/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __dcom_h__
#define __dcom_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */

#ifndef __IRemUnknown_FWD_DEFINED__
#define __IRemUnknown_FWD_DEFINED__
typedef interface IRemUnknown IRemUnknown;
#endif /* __IRemUnknown_FWD_DEFINED__ */

#ifndef __IRemUnknown2_FWD_DEFINED__
#define __IRemUnknown2_FWD_DEFINED__
typedef interface IRemUnknown2 IRemUnknown2;
#endif /* __IRemUnknown2_FWD_DEFINED__ */

/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __ObjectRpcBaseTypes_INTERFACE_DEFINED__
#define __ObjectRpcBaseTypes_INTERFACE_DEFINED__

	/* interface ObjectRpcBaseTypes */
	/* [unique][uuid] */

	typedef MIDL_uhyper ID;

	typedef ID MID;

	typedef ID OXID;

	typedef ID OID;

	typedef ID SETID;

	typedef GUID IPID;

	typedef GUID CID;

	typedef REFGUID REFIPID;

#define COM_MINOR_VERSION_1 (1)

#define COM_MINOR_VERSION_2 (2)

#define COM_MAJOR_VERSION (5)

#define COM_MINOR_VERSION (3)

	typedef struct tagCOMVERSION
	{
		USHORT MajorVersion;
		USHORT MinorVersion;
	} COMVERSION;

#define ORPCF_NULL (0)

#define ORPCF_LOCAL (1)

#define ORPCF_RESERVED1 (2)

#define ORPCF_RESERVED2 (4)

#define ORPCF_RESERVED3 (8)

#define ORPCF_RESERVED4 (16)

	typedef struct tagORPC_EXTENT
	{
		GUID id;
		ULONG size;
		byte data[1];
	} ORPC_EXTENT;

	typedef struct tagORPC_EXTENT_ARRAY
	{
		ULONG size;
		ULONG reserved;
		ORPC_EXTENT** extent;
	} ORPC_EXTENT_ARRAY;

	typedef struct tagORPCTHIS
	{
		COMVERSION version;
		ULONG flags;
		ULONG reserved1;
		CID cid;
		ORPC_EXTENT_ARRAY* extensions;
	} ORPCTHIS;

	typedef struct tagORPCTHAT
	{
		ULONG flags;
		ORPC_EXTENT_ARRAY* extensions;
	} ORPCTHAT;

#define NCADG_IP_UDP (0x8)

#define NCACN_IP_TCP (0x7)

#define NCADG_IPX (0xe)

#define NCACN_SPX (0xc)

#define NCACN_NB_NB (0x12)

#define NCACN_NB_IPX (0xd)

#define NCACN_DNET_NSP (0x4)

#define NCACN_HTTP (0x1f)

	typedef struct tagSTRINGBINDING
	{
		USHORT wTowerId;
		WCHAR aNetworkAddr[1];
	} STRINGBINDING;

#define COM_C_AUTHZ_NONE (0xffff)

	typedef struct tagSECURITYBINDING
	{
		USHORT wAuthnSvc;
		USHORT wAuthzSvc;
		WCHAR aPrincName[1];
	} SECURITYBINDING;

	typedef struct tagDUALSTRINGARRAY
	{
		USHORT wNumEntries;
		USHORT wSecurityOffset;
		USHORT aStringArray[1];
	} DUALSTRINGARRAY;

	typedef struct tagOXID_INFO
	{
		DWORD dwTid;
		DWORD dwPid;
		IPID ipidRemUnknown;
		DWORD dwAuthnHint;
		DUALSTRINGARRAY* psa;
	} OXID_INFO;

#define OBJREF_SIGNATURE (0x574f454d)

#define OBJREF_STANDARD (0x1)

#define OBJREF_HANDLER (0x2)

#define OBJREF_CUSTOM (0x4)

#define SORF_OXRES1 (0x1)

#define SORF_OXRES2 (0x20)

#define SORF_OXRES3 (0x40)

#define SORF_OXRES4 (0x80)

#define SORF_OXRES5 (0x100)

#define SORF_OXRES6 (0x200)

#define SORF_OXRES7 (0x400)

#define SORF_OXRES8 (0x800)

#define SORF_NULL (0)

#define SORF_NOPING (0x1000)

	typedef struct tagSTDOBJREF
	{
		ULONG flags;
		ULONG cPublicRefs;
		OXID oxid;
		OID oid;
		IPID ipid;
	} STDOBJREF;

	typedef struct tagOBJREF
	{
		ULONG signature;
		ULONG flags;
		GUID iid;
		/* [switch_type] */ union
		{
			struct OR_STANDARD
			{
				STDOBJREF std;
				DUALSTRINGARRAY saResAddr;
			} u_standard;
			struct OR_HANDLER
			{
				STDOBJREF std;
				CLSID clsid;
				DUALSTRINGARRAY saResAddr;
			} u_handler;
			struct OR_CUSTOM
			{
				CLSID clsid;
				ULONG cbExtension;
				ULONG size;
				byte* pData;
			} u_custom;
		} u_objref;
	} OBJREF;

	typedef struct tagMInterfacePointer
	{
		ULONG ulCntData;
		BYTE abData[1];
	} MInterfacePointer;

	typedef /* [unique] */ MInterfacePointer* PMInterfacePointer;

	extern RPC_IF_HANDLE ObjectRpcBaseTypes_v0_0_c_ifspec;
	extern RPC_IF_HANDLE ObjectRpcBaseTypes_v0_0_s_ifspec;
#endif /* __ObjectRpcBaseTypes_INTERFACE_DEFINED__ */

#ifndef __IRemUnknown_INTERFACE_DEFINED__
#define __IRemUnknown_INTERFACE_DEFINED__

	/* interface IRemUnknown */
	/* [uuid][object] */

	typedef /* [unique] */ IRemUnknown* LPREMUNKNOWN;

	typedef struct tagREMQIRESULT
	{
		HRESULT hResult;
		STDOBJREF std;
	} REMQIRESULT;

	typedef struct tagREMINTERFACEREF
	{
		IPID ipid;
		ULONG cPublicRefs;
		ULONG cPrivateRefs;
	} REMINTERFACEREF;

	EXTERN_C const IID IID_IRemUnknown;

#if defined(__cplusplus) && !defined(CINTERFACE)

	MIDL_INTERFACE("00000131-0000-0000-C000-000000000046")
	IRemUnknown : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE RemQueryInterface(
			/* [in] */ REFIPID ripid,
			/* [in] */ ULONG cRefs,
			/* [in] */ USHORT cIids,
			/* [size_is][in] */ IID * iids,
			/* [size_is][size_is][out] */ REMQIRESULT * *ppQIResults) = 0;

		virtual HRESULT STDMETHODCALLTYPE RemAddRef(
			/* [in] */ USHORT cInterfaceRefs,
			/* [size_is][in] */ REMINTERFACEREF * InterfaceRefs,
			/* [size_is][out] */ HRESULT * pResults) = 0;

		virtual HRESULT STDMETHODCALLTYPE RemRelease(
			/* [in] */ USHORT cInterfaceRefs,
			/* [size_is][in] */ REMINTERFACEREF * InterfaceRefs) = 0;
	};

#else /* C style interface */

	typedef struct IRemUnknownVtbl
	{
		BEGIN_INTERFACE

			HRESULT(STDMETHODCALLTYPE * QueryInterface)
			(
				IRemUnknown * This,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */
				__RPC__deref_out void** ppvObject);

			ULONG(STDMETHODCALLTYPE * AddRef)
			(
				IRemUnknown * This);

			ULONG(STDMETHODCALLTYPE * Release)
			(
				IRemUnknown * This);

			HRESULT(STDMETHODCALLTYPE * RemQueryInterface)
			(
				IRemUnknown * This,
				/* [in] */ REFIPID ripid,
				/* [in] */ ULONG cRefs,
				/* [in] */ USHORT cIids,
				/* [size_is][in] */ IID * iids,
				/* [size_is][size_is][out] */ REMQIRESULT * *ppQIResults);

			HRESULT(STDMETHODCALLTYPE * RemAddRef)
			(
				IRemUnknown * This,
				/* [in] */ USHORT cInterfaceRefs,
				/* [size_is][in] */ REMINTERFACEREF * InterfaceRefs,
				/* [size_is][out] */ HRESULT * pResults);

			HRESULT(STDMETHODCALLTYPE * RemRelease)
			(
				IRemUnknown * This,
				/* [in] */ USHORT cInterfaceRefs,
				/* [size_is][in] */ REMINTERFACEREF * InterfaceRefs);

		END_INTERFACE
	} IRemUnknownVtbl;

	interface IRemUnknown
	{
		CONST_VTBL struct IRemUnknownVtbl* lpVtbl;
	};

#ifdef COBJMACROS

#define IRemUnknown_QueryInterface(This, riid, ppvObject) \
	((This)->lpVtbl->QueryInterface(This, riid, ppvObject))

#define IRemUnknown_AddRef(This) \
	((This)->lpVtbl->AddRef(This))

#define IRemUnknown_Release(This) \
	((This)->lpVtbl->Release(This))

#define IRemUnknown_RemQueryInterface(This, ripid, cRefs, cIids, iids, ppQIResults) \
	((This)->lpVtbl->RemQueryInterface(This, ripid, cRefs, cIids, iids, ppQIResults))

#define IRemUnknown_RemAddRef(This, cInterfaceRefs, InterfaceRefs, pResults) \
	((This)->lpVtbl->RemAddRef(This, cInterfaceRefs, InterfaceRefs, pResults))

#define IRemUnknown_RemRelease(This, cInterfaceRefs, InterfaceRefs) \
	((This)->lpVtbl->RemRelease(This, cInterfaceRefs, InterfaceRefs))

#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IRemUnknown_INTERFACE_DEFINED__ */

#ifndef __IRemUnknown2_INTERFACE_DEFINED__
#define __IRemUnknown2_INTERFACE_DEFINED__

	/* interface IRemUnknown2 */
	/* [uuid][object] */

	typedef /* [unique] */ IRemUnknown2* LPREMUNKNOWN2;

	EXTERN_C const IID IID_IRemUnknown2;

#if defined(__cplusplus) && !defined(CINTERFACE)

	MIDL_INTERFACE("00000142-0000-0000-C000-000000000046")
	IRemUnknown2 : public IRemUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE RemQueryInterface2(
			/* [in] */ REFIPID ripid,
			/* [in] */ USHORT cIids,
			/* [size_is][in] */ IID * iids,
			/* [size_is][out] */ HRESULT * phr,
			/* [size_is][out] */ MInterfacePointer * *ppMIF) = 0;
	};

#else /* C style interface */

	typedef struct IRemUnknown2Vtbl
	{
		BEGIN_INTERFACE

			HRESULT(STDMETHODCALLTYPE * QueryInterface)
			(
				IRemUnknown2 * This,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */
				__RPC__deref_out void** ppvObject);

			ULONG(STDMETHODCALLTYPE * AddRef)
			(
				IRemUnknown2 * This);

			ULONG(STDMETHODCALLTYPE * Release)
			(
				IRemUnknown2 * This);

			HRESULT(STDMETHODCALLTYPE * RemQueryInterface)
			(
				IRemUnknown2 * This,
				/* [in] */ REFIPID ripid,
				/* [in] */ ULONG cRefs,
				/* [in] */ USHORT cIids,
				/* [size_is][in] */ IID * iids,
				/* [size_is][size_is][out] */ REMQIRESULT * *ppQIResults);

			HRESULT(STDMETHODCALLTYPE * RemAddRef)
			(
				IRemUnknown2 * This,
				/* [in] */ USHORT cInterfaceRefs,
				/* [size_is][in] */ REMINTERFACEREF * InterfaceRefs,
				/* [size_is][out] */ HRESULT * pResults);

			HRESULT(STDMETHODCALLTYPE * RemRelease)
			(
				IRemUnknown2 * This,
				/* [in] */ USHORT cInterfaceRefs,
				/* [size_is][in] */ REMINTERFACEREF * InterfaceRefs);

			HRESULT(STDMETHODCALLTYPE * RemQueryInterface2)
			(
				IRemUnknown2 * This,
				/* [in] */ REFIPID ripid,
				/* [in] */ USHORT cIids,
				/* [size_is][in] */ IID * iids,
				/* [size_is][out] */ HRESULT * phr,
				/* [size_is][out] */ MInterfacePointer * *ppMIF);

		END_INTERFACE
	} IRemUnknown2Vtbl;

	interface IRemUnknown2
	{
		CONST_VTBL struct IRemUnknown2Vtbl* lpVtbl;
	};

#ifdef COBJMACROS

#define IRemUnknown2_QueryInterface(This, riid, ppvObject) \
	((This)->lpVtbl->QueryInterface(This, riid, ppvObject))

#define IRemUnknown2_AddRef(This) \
	((This)->lpVtbl->AddRef(This))

#define IRemUnknown2_Release(This) \
	((This)->lpVtbl->Release(This))

#define IRemUnknown2_RemQueryInterface(This, ripid, cRefs, cIids, iids, ppQIResults) \
	((This)->lpVtbl->RemQueryInterface(This, ripid, cRefs, cIids, iids, ppQIResults))

#define IRemUnknown2_RemAddRef(This, cInterfaceRefs, InterfaceRefs, pResults) \
	((This)->lpVtbl->RemAddRef(This, cInterfaceRefs, InterfaceRefs, pResults))

#define IRemUnknown2_RemRelease(This, cInterfaceRefs, InterfaceRefs) \
	((This)->lpVtbl->RemRelease(This, cInterfaceRefs, InterfaceRefs))

#define IRemUnknown2_RemQueryInterface2(This, ripid, cIids, iids, phr, ppMIF) \
	((This)->lpVtbl->RemQueryInterface2(This, ripid, cIids, iids, phr, ppMIF))

#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IRemUnknown2_INTERFACE_DEFINED__ */

#ifndef __IOXIDResolver_INTERFACE_DEFINED__
#define __IOXIDResolver_INTERFACE_DEFINED__

	/* interface IOXIDResolver */
	/* [unique][uuid] */

	/* [idempotent] */ error_status_t ResolveOxid(
		/* [in] */ handle_t hRpc,
		/* [in] */ OXID* pOxid,
		/* [in] */ USHORT cRequestedProtseqs,
		/* [size_is][ref][in] */ USHORT arRequestedProtseqs[],
		/* [ref][out] */ DUALSTRINGARRAY** ppdsaOxidBindings,
		/* [ref][out] */ IPID* pipidRemUnknown,
		/* [ref][out] */ DWORD* pAuthnHint);

	/* [idempotent] */ error_status_t SimplePing(
		/* [in] */ handle_t hRpc,
		/* [in] */ SETID* pSetId);

	/* [idempotent] */ error_status_t ComplexPing(
		/* [in] */ handle_t hRpc,
		/* [out][in] */ SETID* pSetId,
		/* [in] */ USHORT SequenceNum,
		/* [in] */ USHORT cAddToSet,
		/* [in] */ USHORT cDelFromSet,
		/* [size_is][unique][in] */ OID AddToSet[],
		/* [size_is][unique][in] */ OID DelFromSet[],
		/* [out] */ USHORT* pPingBackoffFactor);

	/* [idempotent] */ error_status_t ServerAlive(
		/* [in] */ handle_t hRpc);

	/* [idempotent] */ error_status_t ResolveOxid2(
		/* [in] */ handle_t hRpc,
		/* [in] */ OXID* pOxid,
		/* [in] */ USHORT cRequestedProtseqs,
		/* [size_is][ref][in] */ USHORT arRequestedProtseqs[],
		/* [ref][out] */ DUALSTRINGARRAY** ppdsaOxidBindings,
		/* [ref][out] */ IPID* pipidRemUnknown,
		/* [ref][out] */ DWORD* pAuthnHint,
		/* [ref][out] */ COMVERSION* pComVersion);

	extern RPC_IF_HANDLE IOXIDResolver_v0_0_c_ifspec;
	extern RPC_IF_HANDLE IOXIDResolver_v0_0_s_ifspec;
#endif /* __IOXIDResolver_INTERFACE_DEFINED__ */

#ifndef __IRemoteActivation_INTERFACE_DEFINED__
#define __IRemoteActivation_INTERFACE_DEFINED__

	/* interface IRemoteActivation */
	/* [unique][uuid] */

#define MODE_GET_CLASS_OBJECT (0xffffffff)

	HRESULT RemoteActivation(
		/* [in] */ handle_t hRpc,
		/* [in] */ ORPCTHIS* ORPCthis,
		/* [out] */ ORPCTHAT* ORPCthat,
		/* [in] */ GUID* Clsid,
		/* [unique][string][in] */ WCHAR* pwszObjectName,
		/* [unique][in] */ MInterfacePointer* pObjectStorage,
		/* [in] */ DWORD ClientImpLevel,
		/* [in] */ DWORD Mode,
		/* [in] */ DWORD Interfaces,
		/* [size_is][unique][in] */ IID* pIIDs,
		/* [in] */ USHORT cRequestedProtseqs,
		/* [size_is][in] */ USHORT RequestedProtseqs[],
		/* [out] */ OXID* pOxid,
		/* [out] */ DUALSTRINGARRAY** ppdsaOxidBindings,
		/* [out] */ IPID* pipidRemUnknown,
		/* [out] */ DWORD* pAuthnHint,
		/* [out] */ COMVERSION* pServerVersion,
		/* [out] */ HRESULT* phr,
		/* [size_is][out] */ MInterfacePointer** ppInterfaceData,
		/* [size_is][out] */ HRESULT* pResults);

	extern RPC_IF_HANDLE IRemoteActivation_v0_0_c_ifspec;
	extern RPC_IF_HANDLE IRemoteActivation_v0_0_s_ifspec;
#endif /* __IRemoteActivation_INTERFACE_DEFINED__ */

	/* Additional Prototypes for ALL interfaces */

	/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
