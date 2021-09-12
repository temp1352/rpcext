

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for RpcShell.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __RpcShell_i_h__
#define __RpcShell_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IRpcServer_FWD_DEFINED__
#define __IRpcServer_FWD_DEFINED__
typedef interface IRpcServer IRpcServer;

#endif 	/* __IRpcServer_FWD_DEFINED__ */


#ifndef __IRpcCallContext_FWD_DEFINED__
#define __IRpcCallContext_FWD_DEFINED__
typedef interface IRpcCallContext IRpcCallContext;

#endif 	/* __IRpcCallContext_FWD_DEFINED__ */


#ifndef __IRpcConnectionSink_FWD_DEFINED__
#define __IRpcConnectionSink_FWD_DEFINED__
typedef interface IRpcConnectionSink IRpcConnectionSink;

#endif 	/* __IRpcConnectionSink_FWD_DEFINED__ */


#ifndef __IRpcConnection_FWD_DEFINED__
#define __IRpcConnection_FWD_DEFINED__
typedef interface IRpcConnection IRpcConnection;

#endif 	/* __IRpcConnection_FWD_DEFINED__ */


#ifndef __IRpcClientConnection_FWD_DEFINED__
#define __IRpcClientConnection_FWD_DEFINED__
typedef interface IRpcClientConnection IRpcClientConnection;

#endif 	/* __IRpcClientConnection_FWD_DEFINED__ */


#ifndef __IRpcProxyInfo_FWD_DEFINED__
#define __IRpcProxyInfo_FWD_DEFINED__
typedef interface IRpcProxyInfo IRpcProxyInfo;

#endif 	/* __IRpcProxyInfo_FWD_DEFINED__ */


#ifndef __PsFactoryBuffer_FWD_DEFINED__
#define __PsFactoryBuffer_FWD_DEFINED__

#ifdef __cplusplus
typedef class PsFactoryBuffer PsFactoryBuffer;
#else
typedef struct PsFactoryBuffer PsFactoryBuffer;
#endif /* __cplusplus */

#endif 	/* __PsFactoryBuffer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "dcom.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_RpcShell_0000_0000 */
/* [local] */ 

typedef GUID RPC_SID;


enum __MIDL___MIDL_itf_RpcShell_0000_0000_0001
    {
        E_COMMAND_ERROR	= 0x80040501L,
        E_OID_ERROR	= 0x80040520L,
        E_IID_ERROR	= 0x80040521L,
        E_WAIT_PROCESS	= 0x80040510L,
        E_SERVER_UNAVAILABLE	= 0x80040511L
    } ;
typedef 
enum _RE_SERVICE_CONTROL
    {
        RE_SC_STARTUP	= 0,
        RE_SC_STOP	= 1
    } 	RE_SERVICE_CONTROL;

typedef 
enum _RE_SERVICE_STATE
    {
        RE_SS_STOPED	= 0,
        RE_SS_RUNNING	= 1
    } 	RE_SERVICE_STATE;



extern RPC_IF_HANDLE __MIDL_itf_RpcShell_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_RpcShell_0000_0000_v0_0_s_ifspec;

#ifndef __IRpcServer_INTERFACE_DEFINED__
#define __IRpcServer_INTERFACE_DEFINED__

/* interface IRpcServer */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IRpcServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("206BA116-E51B-49f0-9675-D726F2E6FD51")
    IRpcServer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            LONG port) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ControlService( 
            RE_SERVICE_CONTROL control) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            RE_SERVICE_STATE *pState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Uninitialize( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRpcServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcServer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcServer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcServer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IRpcServer * This,
            LONG port);
        
        HRESULT ( STDMETHODCALLTYPE *ControlService )( 
            IRpcServer * This,
            RE_SERVICE_CONTROL control);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IRpcServer * This,
            RE_SERVICE_STATE *pState);
        
        HRESULT ( STDMETHODCALLTYPE *Uninitialize )( 
            IRpcServer * This);
        
        END_INTERFACE
    } IRpcServerVtbl;

    interface IRpcServer
    {
        CONST_VTBL struct IRpcServerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcServer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcServer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcServer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcServer_Initialize(This,port)	\
    ( (This)->lpVtbl -> Initialize(This,port) ) 

#define IRpcServer_ControlService(This,control)	\
    ( (This)->lpVtbl -> ControlService(This,control) ) 

#define IRpcServer_GetState(This,pState)	\
    ( (This)->lpVtbl -> GetState(This,pState) ) 

#define IRpcServer_Uninitialize(This)	\
    ( (This)->lpVtbl -> Uninitialize(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcServer_INTERFACE_DEFINED__ */


#ifndef __IRpcCallContext_INTERFACE_DEFINED__
#define __IRpcCallContext_INTERFACE_DEFINED__

/* interface IRpcCallContext */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IRpcCallContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("02D02C8F-074D-458C-8118-89FA5CD7CD9D")
    IRpcCallContext : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Authentication( 
            /* [out] */ HANDLE *phToken) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRpcCallContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcCallContext * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcCallContext * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcCallContext * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Authentication )( 
            IRpcCallContext * This,
            /* [out] */ HANDLE *phToken);
        
        END_INTERFACE
    } IRpcCallContextVtbl;

    interface IRpcCallContext
    {
        CONST_VTBL struct IRpcCallContextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcCallContext_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcCallContext_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcCallContext_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcCallContext_Authentication(This,phToken)	\
    ( (This)->lpVtbl -> Authentication(This,phToken) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcCallContext_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_RpcShell_0000_0002 */
/* [local] */ 

typedef struct _RPC_CONNECT_RESP
    {
    GUID cid;
    RPC_SID sid;
    OID oid;
    } 	RPC_CONNECT_RESP;



extern RPC_IF_HANDLE __MIDL_itf_RpcShell_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_RpcShell_0000_0002_v0_0_s_ifspec;

#ifndef __IRpcConnectionSink_INTERFACE_DEFINED__
#define __IRpcConnectionSink_INTERFACE_DEFINED__

/* interface IRpcConnectionSink */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IRpcConnectionSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4558CF4F-2EF8-4648-AD48-730DE0CB2372")
    IRpcConnectionSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnConnected( 
            /* [in] */ RPC_CONNECT_RESP *pResp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnDisconnected( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRpcConnectionSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcConnectionSink * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcConnectionSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcConnectionSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnConnected )( 
            IRpcConnectionSink * This,
            /* [in] */ RPC_CONNECT_RESP *pResp);
        
        HRESULT ( STDMETHODCALLTYPE *OnDisconnected )( 
            IRpcConnectionSink * This);
        
        END_INTERFACE
    } IRpcConnectionSinkVtbl;

    interface IRpcConnectionSink
    {
        CONST_VTBL struct IRpcConnectionSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcConnectionSink_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcConnectionSink_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcConnectionSink_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcConnectionSink_OnConnected(This,pResp)	\
    ( (This)->lpVtbl -> OnConnected(This,pResp) ) 

#define IRpcConnectionSink_OnDisconnected(This)	\
    ( (This)->lpVtbl -> OnDisconnected(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcConnectionSink_INTERFACE_DEFINED__ */


#ifndef __IRpcConnection_INTERFACE_DEFINED__
#define __IRpcConnection_INTERFACE_DEFINED__

/* interface IRpcConnection */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IRpcConnection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BED0B7E0-DDC7-4914-ADF3-0A5A124629CA")
    IRpcConnection : public IUnknown
    {
    public:
        virtual /* [local] */ BOOL STDMETHODCALLTYPE IsConnected( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HealthCheck( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Advise( 
            /* [in] */ IRpcConnectionSink *pSink,
            /* [in] */ BOOL addRef) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Unadvise( 
            /* [in] */ IRpcConnectionSink *pSink) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRpcConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcConnection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcConnection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcConnection * This);
        
        /* [local] */ BOOL ( STDMETHODCALLTYPE *IsConnected )( 
            IRpcConnection * This);
        
        HRESULT ( STDMETHODCALLTYPE *HealthCheck )( 
            IRpcConnection * This);
        
        HRESULT ( STDMETHODCALLTYPE *Advise )( 
            IRpcConnection * This,
            /* [in] */ IRpcConnectionSink *pSink,
            /* [in] */ BOOL addRef);
        
        HRESULT ( STDMETHODCALLTYPE *Unadvise )( 
            IRpcConnection * This,
            /* [in] */ IRpcConnectionSink *pSink);
        
        END_INTERFACE
    } IRpcConnectionVtbl;

    interface IRpcConnection
    {
        CONST_VTBL struct IRpcConnectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcConnection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcConnection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcConnection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcConnection_IsConnected(This)	\
    ( (This)->lpVtbl -> IsConnected(This) ) 

#define IRpcConnection_HealthCheck(This)	\
    ( (This)->lpVtbl -> HealthCheck(This) ) 

#define IRpcConnection_Advise(This,pSink,addRef)	\
    ( (This)->lpVtbl -> Advise(This,pSink,addRef) ) 

#define IRpcConnection_Unadvise(This,pSink)	\
    ( (This)->lpVtbl -> Unadvise(This,pSink) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcConnection_INTERFACE_DEFINED__ */


#ifndef __IRpcClientConnection_INTERFACE_DEFINED__
#define __IRpcClientConnection_INTERFACE_DEFINED__

/* interface IRpcClientConnection */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IRpcClientConnection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4EB89E8D-7885-444C-93C9-E992FA5FFF5E")
    IRpcClientConnection : public IRpcConnection
    {
    public:
        virtual /* [local] */ ULONG STDMETHODCALLTYPE GetConnectCount( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLastConnectResult( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRpcClientConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcClientConnection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcClientConnection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcClientConnection * This);
        
        /* [local] */ BOOL ( STDMETHODCALLTYPE *IsConnected )( 
            IRpcClientConnection * This);
        
        HRESULT ( STDMETHODCALLTYPE *HealthCheck )( 
            IRpcClientConnection * This);
        
        HRESULT ( STDMETHODCALLTYPE *Advise )( 
            IRpcClientConnection * This,
            /* [in] */ IRpcConnectionSink *pSink,
            /* [in] */ BOOL addRef);
        
        HRESULT ( STDMETHODCALLTYPE *Unadvise )( 
            IRpcClientConnection * This,
            /* [in] */ IRpcConnectionSink *pSink);
        
        /* [local] */ ULONG ( STDMETHODCALLTYPE *GetConnectCount )( 
            IRpcClientConnection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastConnectResult )( 
            IRpcClientConnection * This);
        
        END_INTERFACE
    } IRpcClientConnectionVtbl;

    interface IRpcClientConnection
    {
        CONST_VTBL struct IRpcClientConnectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcClientConnection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcClientConnection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcClientConnection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcClientConnection_IsConnected(This)	\
    ( (This)->lpVtbl -> IsConnected(This) ) 

#define IRpcClientConnection_HealthCheck(This)	\
    ( (This)->lpVtbl -> HealthCheck(This) ) 

#define IRpcClientConnection_Advise(This,pSink,addRef)	\
    ( (This)->lpVtbl -> Advise(This,pSink,addRef) ) 

#define IRpcClientConnection_Unadvise(This,pSink)	\
    ( (This)->lpVtbl -> Unadvise(This,pSink) ) 


#define IRpcClientConnection_GetConnectCount(This)	\
    ( (This)->lpVtbl -> GetConnectCount(This) ) 

#define IRpcClientConnection_GetLastConnectResult(This)	\
    ( (This)->lpVtbl -> GetLastConnectResult(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcClientConnection_INTERFACE_DEFINED__ */


#ifndef __IRpcProxyInfo_INTERFACE_DEFINED__
#define __IRpcProxyInfo_INTERFACE_DEFINED__

/* interface IRpcProxyInfo */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IRpcProxyInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7AFD7D4-EFFF-4606-8D49-349F95A742B1")
    IRpcProxyInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSessionId( 
            /* [in] */ LPWSTR buff,
            /* [in] */ ULONG size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConnection( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRpcProxyInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcProxyInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcProxyInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcProxyInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSessionId )( 
            IRpcProxyInfo * This,
            /* [in] */ LPWSTR buff,
            /* [in] */ ULONG size);
        
        HRESULT ( STDMETHODCALLTYPE *GetConnection )( 
            IRpcProxyInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        END_INTERFACE
    } IRpcProxyInfoVtbl;

    interface IRpcProxyInfo
    {
        CONST_VTBL struct IRpcProxyInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcProxyInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcProxyInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcProxyInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcProxyInfo_GetSessionId(This,buff,size)	\
    ( (This)->lpVtbl -> GetSessionId(This,buff,size) ) 

#define IRpcProxyInfo_GetConnection(This,riid,ppv)	\
    ( (This)->lpVtbl -> GetConnection(This,riid,ppv) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcProxyInfo_INTERFACE_DEFINED__ */



#ifndef __RpcExt_LIBRARY_DEFINED__
#define __RpcExt_LIBRARY_DEFINED__

/* library RpcExt */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_RpcExt;

EXTERN_C const CLSID CLSID_PsFactoryBuffer;

#ifdef __cplusplus

class DECLSPEC_UUID("4343405D-69BD-4b60-A3AA-A5F2741F984A")
PsFactoryBuffer;
#endif
#endif /* __RpcExt_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


