

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for ..\idl\ScBase.idl:
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

#ifndef __ScBase_h__
#define __ScBase_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScSid_FWD_DEFINED__
#define __IScSid_FWD_DEFINED__
typedef interface IScSid IScSid;

#endif 	/* __IScSid_FWD_DEFINED__ */


#ifndef __IScSidGroup_FWD_DEFINED__
#define __IScSidGroup_FWD_DEFINED__
typedef interface IScSidGroup IScSidGroup;

#endif 	/* __IScSidGroup_FWD_DEFINED__ */


#ifndef __IScDescriptor_FWD_DEFINED__
#define __IScDescriptor_FWD_DEFINED__
typedef interface IScDescriptor IScDescriptor;

#endif 	/* __IScDescriptor_FWD_DEFINED__ */


#ifndef __IScPackage_FWD_DEFINED__
#define __IScPackage_FWD_DEFINED__
typedef interface IScPackage IScPackage;

#endif 	/* __IScPackage_FWD_DEFINED__ */


#ifndef __IScScope_FWD_DEFINED__
#define __IScScope_FWD_DEFINED__
typedef interface IScScope IScScope;

#endif 	/* __IScScope_FWD_DEFINED__ */


#ifndef __IScAccessToken_FWD_DEFINED__
#define __IScAccessToken_FWD_DEFINED__
typedef interface IScAccessToken IScAccessToken;

#endif 	/* __IScAccessToken_FWD_DEFINED__ */


#ifndef __IScAccessChannel_FWD_DEFINED__
#define __IScAccessChannel_FWD_DEFINED__
typedef interface IScAccessChannel IScAccessChannel;

#endif 	/* __IScAccessChannel_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_ScBase_0000_0000 */
/* [local] */ 

typedef 
enum _SC_ACCOUNT_TYPE
    {
        SC_AT_GENERAL	= 0,
        SC_AT_WINDOWS	= ( SC_AT_GENERAL + 1 ) 
    } 	SC_ACCOUNT_TYPE;

#if 0
typedef DWORD SID_NAME_USE;

#endif


extern RPC_IF_HANDLE __MIDL_itf_ScBase_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ScBase_0000_0000_v0_0_s_ifspec;

#ifndef __IScSid_INTERFACE_DEFINED__
#define __IScSid_INTERFACE_DEFINED__

/* interface IScSid */
/* [object][local][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IScSid;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4D27C09F-03A4-4204-8987-13C087A73F01")
    IScSid : public IUnknown
    {
    public:
        virtual PSID STDMETHODCALLTYPE GetPSID( void) = 0;
        
        virtual ULONG STDMETHODCALLTYPE GetLength( void) = 0;
        
        virtual BOOL STDMETHODCALLTYPE Equal( 
            /* [in] */ const IScSid *pSid) = 0;
        
        virtual int STDMETHODCALLTYPE Compare( 
            /* [in] */ const IScSid *pSid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAccountName( 
            /* [out] */ BSTR *pName,
            /* [defaultvalue][out] */ SID_NAME_USE *pNameUse = 0) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScSidVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScSid * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScSid * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScSid * This);
        
        PSID ( STDMETHODCALLTYPE *GetPSID )( 
            IScSid * This);
        
        ULONG ( STDMETHODCALLTYPE *GetLength )( 
            IScSid * This);
        
        BOOL ( STDMETHODCALLTYPE *Equal )( 
            IScSid * This,
            /* [in] */ const IScSid *pSid);
        
        int ( STDMETHODCALLTYPE *Compare )( 
            IScSid * This,
            /* [in] */ const IScSid *pSid);
        
        HRESULT ( STDMETHODCALLTYPE *GetAccountName )( 
            IScSid * This,
            /* [out] */ BSTR *pName,
            /* [defaultvalue][out] */ SID_NAME_USE *pNameUse);
        
        END_INTERFACE
    } IScSidVtbl;

    interface IScSid
    {
        CONST_VTBL struct IScSidVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScSid_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScSid_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScSid_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScSid_GetPSID(This)	\
    ( (This)->lpVtbl -> GetPSID(This) ) 

#define IScSid_GetLength(This)	\
    ( (This)->lpVtbl -> GetLength(This) ) 

#define IScSid_Equal(This,pSid)	\
    ( (This)->lpVtbl -> Equal(This,pSid) ) 

#define IScSid_Compare(This,pSid)	\
    ( (This)->lpVtbl -> Compare(This,pSid) ) 

#define IScSid_GetAccountName(This,pName,pNameUse)	\
    ( (This)->lpVtbl -> GetAccountName(This,pName,pNameUse) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScSid_INTERFACE_DEFINED__ */


#ifndef __IScSidGroup_INTERFACE_DEFINED__
#define __IScSidGroup_INTERFACE_DEFINED__

/* interface IScSidGroup */
/* [object][local][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IScSidGroup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F2D68D8B-9F8A-43B0-93E2-ACC0B474FE41")
    IScSidGroup : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Insert( 
            /* [in] */ IScSid *pSid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsMember( 
            /* [in] */ IScSid *pSid) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScSidGroupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScSidGroup * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScSidGroup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScSidGroup * This);
        
        HRESULT ( STDMETHODCALLTYPE *Insert )( 
            IScSidGroup * This,
            /* [in] */ IScSid *pSid);
        
        HRESULT ( STDMETHODCALLTYPE *IsMember )( 
            IScSidGroup * This,
            /* [in] */ IScSid *pSid);
        
        END_INTERFACE
    } IScSidGroupVtbl;

    interface IScSidGroup
    {
        CONST_VTBL struct IScSidGroupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScSidGroup_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScSidGroup_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScSidGroup_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScSidGroup_Insert(This,pSid)	\
    ( (This)->lpVtbl -> Insert(This,pSid) ) 

#define IScSidGroup_IsMember(This,pSid)	\
    ( (This)->lpVtbl -> IsMember(This,pSid) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScSidGroup_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ScBase_0000_0002 */
/* [local] */ 

typedef struct _SC_ACCESS_ENTRY
    {
    SC_ACCOUNT_TYPE accountType;
    LPWSTR accountName;
    LPWSTR accountInfo;
    DWORD allowed;
    DWORD denied;
    } 	SC_ACCESS_ENTRY;


enum SC_ACL_FLAG
    {
        SC_ACL_DEFAULT	= 0,
        SC_ACL_CLASS_VALID	= 0x1
    } ;
typedef DWORD SC_ACL_FLAGS;



extern RPC_IF_HANDLE __MIDL_itf_ScBase_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ScBase_0000_0002_v0_0_s_ifspec;

#ifndef __IScDescriptor_INTERFACE_DEFINED__
#define __IScDescriptor_INTERFACE_DEFINED__

/* interface IScDescriptor */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IScDescriptor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AE539391-528D-4441-9653-35CB0E8AFDA3")
    IScDescriptor : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetGuid( 
            /* [out] */ GUID *pGuid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParent( 
            /* [out] */ IScDescriptor **ppParent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetParent( 
            /* [in] */ IScDescriptor *pParent) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetAcl( 
            /* [in] */ SC_ACL_FLAGS flags,
            /* [out] */ ULONG *pCount,
            /* [size_is][size_is][out] */ SC_ACCESS_ENTRY **prgEntries,
            /* [out] */ LPWSTR *ppStringsBuffer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAcl( 
            /* [in] */ SC_ACL_FLAGS flags,
            /* [in] */ ULONG count,
            /* [size_is][in] */ SC_ACCESS_ENTRY *rgEntries) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddAce( 
            /* [in] */ REFGUID guid,
            /* [in] */ DWORD allowed,
            /* [in] */ DWORD denied) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddAce2( 
            /* [in] */ IScSid *pSid,
            /* [in] */ DWORD allowed,
            /* [in] */ DWORD denied) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddAce3( 
            /* [in] */ SC_ACCOUNT_TYPE type,
            /* [in] */ LPCWSTR name,
            /* [in] */ DWORD allowed,
            /* [in] */ DWORD denied) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsValid( 
            /* [in] */ SC_ACCOUNT_TYPE type,
            /* [in] */ BOOL bInherit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AccessCheck( 
            /* [in] */ IScSidGroup *pGroup,
            /* [in] */ DWORD desiredAccess,
            /* [out] */ DWORD *pAllowed,
            /* [out] */ DWORD *pDenied) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScDescriptorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScDescriptor * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScDescriptor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScDescriptor * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetGuid )( 
            IScDescriptor * This,
            /* [out] */ GUID *pGuid);
        
        HRESULT ( STDMETHODCALLTYPE *GetParent )( 
            IScDescriptor * This,
            /* [out] */ IScDescriptor **ppParent);
        
        HRESULT ( STDMETHODCALLTYPE *SetParent )( 
            IScDescriptor * This,
            /* [in] */ IScDescriptor *pParent);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetAcl )( 
            IScDescriptor * This,
            /* [in] */ SC_ACL_FLAGS flags,
            /* [out] */ ULONG *pCount,
            /* [size_is][size_is][out] */ SC_ACCESS_ENTRY **prgEntries,
            /* [out] */ LPWSTR *ppStringsBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *SetAcl )( 
            IScDescriptor * This,
            /* [in] */ SC_ACL_FLAGS flags,
            /* [in] */ ULONG count,
            /* [size_is][in] */ SC_ACCESS_ENTRY *rgEntries);
        
        HRESULT ( STDMETHODCALLTYPE *AddAce )( 
            IScDescriptor * This,
            /* [in] */ REFGUID guid,
            /* [in] */ DWORD allowed,
            /* [in] */ DWORD denied);
        
        HRESULT ( STDMETHODCALLTYPE *AddAce2 )( 
            IScDescriptor * This,
            /* [in] */ IScSid *pSid,
            /* [in] */ DWORD allowed,
            /* [in] */ DWORD denied);
        
        HRESULT ( STDMETHODCALLTYPE *AddAce3 )( 
            IScDescriptor * This,
            /* [in] */ SC_ACCOUNT_TYPE type,
            /* [in] */ LPCWSTR name,
            /* [in] */ DWORD allowed,
            /* [in] */ DWORD denied);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IScDescriptor * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsValid )( 
            IScDescriptor * This,
            /* [in] */ SC_ACCOUNT_TYPE type,
            /* [in] */ BOOL bInherit);
        
        HRESULT ( STDMETHODCALLTYPE *AccessCheck )( 
            IScDescriptor * This,
            /* [in] */ IScSidGroup *pGroup,
            /* [in] */ DWORD desiredAccess,
            /* [out] */ DWORD *pAllowed,
            /* [out] */ DWORD *pDenied);
        
        END_INTERFACE
    } IScDescriptorVtbl;

    interface IScDescriptor
    {
        CONST_VTBL struct IScDescriptorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScDescriptor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScDescriptor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScDescriptor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScDescriptor_GetGuid(This,pGuid)	\
    ( (This)->lpVtbl -> GetGuid(This,pGuid) ) 

#define IScDescriptor_GetParent(This,ppParent)	\
    ( (This)->lpVtbl -> GetParent(This,ppParent) ) 

#define IScDescriptor_SetParent(This,pParent)	\
    ( (This)->lpVtbl -> SetParent(This,pParent) ) 

#define IScDescriptor_GetAcl(This,flags,pCount,prgEntries,ppStringsBuffer)	\
    ( (This)->lpVtbl -> GetAcl(This,flags,pCount,prgEntries,ppStringsBuffer) ) 

#define IScDescriptor_SetAcl(This,flags,count,rgEntries)	\
    ( (This)->lpVtbl -> SetAcl(This,flags,count,rgEntries) ) 

#define IScDescriptor_AddAce(This,guid,allowed,denied)	\
    ( (This)->lpVtbl -> AddAce(This,guid,allowed,denied) ) 

#define IScDescriptor_AddAce2(This,pSid,allowed,denied)	\
    ( (This)->lpVtbl -> AddAce2(This,pSid,allowed,denied) ) 

#define IScDescriptor_AddAce3(This,type,name,allowed,denied)	\
    ( (This)->lpVtbl -> AddAce3(This,type,name,allowed,denied) ) 

#define IScDescriptor_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IScDescriptor_IsValid(This,type,bInherit)	\
    ( (This)->lpVtbl -> IsValid(This,type,bInherit) ) 

#define IScDescriptor_AccessCheck(This,pGroup,desiredAccess,pAllowed,pDenied)	\
    ( (This)->lpVtbl -> AccessCheck(This,pGroup,desiredAccess,pAllowed,pDenied) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IScDescriptor_Remote_GetAcl_Proxy( 
    IScDescriptor * This,
    /* [in] */ SC_ACL_FLAGS flags,
    /* [out] */ ULONG *pCount,
    /* [size_is][size_is][out] */ SC_ACCESS_ENTRY **prgEntries,
    /* [size_is][size_is][out] */ ULONG **prgNameOffsets,
    /* [size_is][size_is][out] */ ULONG **prgInfoffsets,
    /* [out][in] */ ULONG *pcbStringsBuffer,
    /* [size_is][size_is][unique][out][in] */ OLECHAR **ppStringsBuffer,
    /* [out] */ IErrorInfo **ppErrorInfoRem);


void __RPC_STUB IScDescriptor_Remote_GetAcl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScDescriptor_INTERFACE_DEFINED__ */


#ifndef __IScPackage_INTERFACE_DEFINED__
#define __IScPackage_INTERFACE_DEFINED__

/* interface IScPackage */
/* [object][unique][helpstring][nonextensible][uuid] */ 


EXTERN_C const IID IID_IScPackage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AC7EDF16-9FA2-4101-A067-90EAB081501B")
    IScPackage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetGuid( 
            /* [out] */ GUID *pGuid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LookupDescriptor( 
            /* [in] */ REFGUID guid,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateDescriptor( 
            /* [in] */ REFGUID idObject,
            /* [in] */ REFGUID idParent,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppDesc) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScPackageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScPackage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScPackage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScPackage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetGuid )( 
            IScPackage * This,
            /* [out] */ GUID *pGuid);
        
        HRESULT ( STDMETHODCALLTYPE *LookupDescriptor )( 
            IScPackage * This,
            /* [in] */ REFGUID guid,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDescriptor )( 
            IScPackage * This,
            /* [in] */ REFGUID idObject,
            /* [in] */ REFGUID idParent,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppDesc);
        
        END_INTERFACE
    } IScPackageVtbl;

    interface IScPackage
    {
        CONST_VTBL struct IScPackageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScPackage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScPackage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScPackage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScPackage_GetGuid(This,pGuid)	\
    ( (This)->lpVtbl -> GetGuid(This,pGuid) ) 

#define IScPackage_LookupDescriptor(This,guid,riid,ppv)	\
    ( (This)->lpVtbl -> LookupDescriptor(This,guid,riid,ppv) ) 

#define IScPackage_CreateDescriptor(This,idObject,idParent,riid,ppDesc)	\
    ( (This)->lpVtbl -> CreateDescriptor(This,idObject,idParent,riid,ppDesc) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScPackage_INTERFACE_DEFINED__ */


#ifndef __IScScope_INTERFACE_DEFINED__
#define __IScScope_INTERFACE_DEFINED__

/* interface IScScope */
/* [object][unique][helpstring][nonextensible][uuid] */ 


EXTERN_C const IID IID_IScScope;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3AC90C8A-27C0-41E3-97A8-0E72309C5D7D")
    IScScope : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPackage( 
            /* [out] */ IScPackage **ppPackage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPackage( 
            /* [in] */ IScPackage *pPackage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AccessCheck( 
            /* [in] */ REFGUID guid,
            /* [in] */ DWORD desiredAccess,
            /* [out] */ DWORD *pGrantedAccess) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScScopeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScScope * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScScope * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScScope * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPackage )( 
            IScScope * This,
            /* [out] */ IScPackage **ppPackage);
        
        HRESULT ( STDMETHODCALLTYPE *SetPackage )( 
            IScScope * This,
            /* [in] */ IScPackage *pPackage);
        
        HRESULT ( STDMETHODCALLTYPE *AccessCheck )( 
            IScScope * This,
            /* [in] */ REFGUID guid,
            /* [in] */ DWORD desiredAccess,
            /* [out] */ DWORD *pGrantedAccess);
        
        END_INTERFACE
    } IScScopeVtbl;

    interface IScScope
    {
        CONST_VTBL struct IScScopeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScScope_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScScope_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScScope_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScScope_GetPackage(This,ppPackage)	\
    ( (This)->lpVtbl -> GetPackage(This,ppPackage) ) 

#define IScScope_SetPackage(This,pPackage)	\
    ( (This)->lpVtbl -> SetPackage(This,pPackage) ) 

#define IScScope_AccessCheck(This,guid,desiredAccess,pGrantedAccess)	\
    ( (This)->lpVtbl -> AccessCheck(This,guid,desiredAccess,pGrantedAccess) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScScope_INTERFACE_DEFINED__ */


#ifndef __IScAccessToken_INTERFACE_DEFINED__
#define __IScAccessToken_INTERFACE_DEFINED__

/* interface IScAccessToken */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IScAccessToken;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("24118923-79ED-478E-A1DB-6EB9BC664343")
    IScAccessToken : public IUnknown
    {
    public:
        virtual /* [local] */ SC_ACCOUNT_TYPE STDMETHODCALLTYPE GetAccountType( void) = 0;
        
        virtual /* [local] */ LONG STDMETHODCALLTYPE GetUserName( 
            /* [out] */ WCHAR *pBuffer,
            /* [in] */ DWORD dwSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Impersonate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Revert( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AccessCheck( 
            /* [in] */ IScDescriptor *pDescriptor,
            /* [in] */ DWORD desiredAccess,
            /* [out] */ DWORD *pAllowed,
            /* [out] */ DWORD *pDenied) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScAccessTokenVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScAccessToken * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScAccessToken * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScAccessToken * This);
        
        /* [local] */ SC_ACCOUNT_TYPE ( STDMETHODCALLTYPE *GetAccountType )( 
            IScAccessToken * This);
        
        /* [local] */ LONG ( STDMETHODCALLTYPE *GetUserName )( 
            IScAccessToken * This,
            /* [out] */ WCHAR *pBuffer,
            /* [in] */ DWORD dwSize);
        
        HRESULT ( STDMETHODCALLTYPE *Impersonate )( 
            IScAccessToken * This);
        
        HRESULT ( STDMETHODCALLTYPE *Revert )( 
            IScAccessToken * This);
        
        HRESULT ( STDMETHODCALLTYPE *AccessCheck )( 
            IScAccessToken * This,
            /* [in] */ IScDescriptor *pDescriptor,
            /* [in] */ DWORD desiredAccess,
            /* [out] */ DWORD *pAllowed,
            /* [out] */ DWORD *pDenied);
        
        END_INTERFACE
    } IScAccessTokenVtbl;

    interface IScAccessToken
    {
        CONST_VTBL struct IScAccessTokenVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScAccessToken_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScAccessToken_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScAccessToken_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScAccessToken_GetAccountType(This)	\
    ( (This)->lpVtbl -> GetAccountType(This) ) 

#define IScAccessToken_GetUserName(This,pBuffer,dwSize)	\
    ( (This)->lpVtbl -> GetUserName(This,pBuffer,dwSize) ) 

#define IScAccessToken_Impersonate(This)	\
    ( (This)->lpVtbl -> Impersonate(This) ) 

#define IScAccessToken_Revert(This)	\
    ( (This)->lpVtbl -> Revert(This) ) 

#define IScAccessToken_AccessCheck(This,pDescriptor,desiredAccess,pAllowed,pDenied)	\
    ( (This)->lpVtbl -> AccessCheck(This,pDescriptor,desiredAccess,pAllowed,pDenied) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScAccessToken_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ScBase_0000_0006 */
/* [local] */ 

typedef struct _SC_AUTH_INFO
    {
    GUID sid;
    CLSID clsid;
    LPCWSTR domain;
    LPCWSTR referer;
    LPCWSTR authCode;
    LPCWSTR refreshToken;
    LPCWSTR userName;
    LPCWSTR password;
    LPCWSTR params;
    } 	SC_AUTH_INFO;

typedef struct _SC_AUTH_RESP
    {
    IScAccessToken *pToken;
    BSTR refreshToken;
    } 	SC_AUTH_RESP;



extern RPC_IF_HANDLE __MIDL_itf_ScBase_0000_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ScBase_0000_0006_v0_0_s_ifspec;

#ifndef __IScAccessChannel_INTERFACE_DEFINED__
#define __IScAccessChannel_INTERFACE_DEFINED__

/* interface IScAccessChannel */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IScAccessChannel;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB076102-8895-400D-8C59-4B87D41EBC61")
    IScAccessChannel : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Logon( 
            /* [in] */ SC_AUTH_INFO *pInfo,
            /* [out] */ SC_AUTH_RESP *pResp) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE AccessCheck( 
            /* [in] */ HANDLE hToken,
            /* [out] */ IScAccessToken **ppToken) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScAccessChannelVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScAccessChannel * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScAccessChannel * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScAccessChannel * This);
        
        HRESULT ( STDMETHODCALLTYPE *Logon )( 
            IScAccessChannel * This,
            /* [in] */ SC_AUTH_INFO *pInfo,
            /* [out] */ SC_AUTH_RESP *pResp);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *AccessCheck )( 
            IScAccessChannel * This,
            /* [in] */ HANDLE hToken,
            /* [out] */ IScAccessToken **ppToken);
        
        END_INTERFACE
    } IScAccessChannelVtbl;

    interface IScAccessChannel
    {
        CONST_VTBL struct IScAccessChannelVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScAccessChannel_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScAccessChannel_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScAccessChannel_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScAccessChannel_Logon(This,pInfo,pResp)	\
    ( (This)->lpVtbl -> Logon(This,pInfo,pResp) ) 

#define IScAccessChannel_AccessCheck(This,hToken,ppToken)	\
    ( (This)->lpVtbl -> AccessCheck(This,hToken,ppToken) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScAccessChannel_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

/* [local] */ HRESULT STDMETHODCALLTYPE IScDescriptor_GetAcl_Proxy( 
    IScDescriptor * This,
    /* [in] */ SC_ACL_FLAGS flags,
    /* [out] */ ULONG *pCount,
    /* [size_is][size_is][out] */ SC_ACCESS_ENTRY **prgEntries,
    /* [out] */ LPWSTR *ppStringsBuffer);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IScDescriptor_GetAcl_Stub( 
    IScDescriptor * This,
    /* [in] */ SC_ACL_FLAGS flags,
    /* [out] */ ULONG *pCount,
    /* [size_is][size_is][out] */ SC_ACCESS_ENTRY **prgEntries,
    /* [size_is][size_is][out] */ ULONG **prgNameOffsets,
    /* [size_is][size_is][out] */ ULONG **prgInfoffsets,
    /* [out][in] */ ULONG *pcbStringsBuffer,
    /* [size_is][size_is][unique][out][in] */ OLECHAR **ppStringsBuffer,
    /* [out] */ IErrorInfo **ppErrorInfoRem);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


