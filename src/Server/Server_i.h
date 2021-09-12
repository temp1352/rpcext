

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for Server.idl:
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

#ifndef __Server_i_h__
#define __Server_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUserService_FWD_DEFINED__
#define __IUserService_FWD_DEFINED__
typedef interface IUserService IUserService;

#endif 	/* __IUserService_FWD_DEFINED__ */


#ifndef __UserService_FWD_DEFINED__
#define __UserService_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserService UserService;
#else
typedef struct UserService UserService;
#endif /* __cplusplus */

#endif 	/* __UserService_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IUserService_INTERFACE_DEFINED__
#define __IUserService_INTERFACE_DEFINED__

/* interface IUserService */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IUserService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("81331af6-e61d-405b-bc7b-81c03843b49b")
    IUserService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Hello( 
            /* [in] */ LPCWSTR name,
            /* [out] */ BSTR *pResult) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUserServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUserService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUserService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUserService * This);
        
        HRESULT ( STDMETHODCALLTYPE *Hello )( 
            IUserService * This,
            /* [in] */ LPCWSTR name,
            /* [out] */ BSTR *pResult);
        
        END_INTERFACE
    } IUserServiceVtbl;

    interface IUserService
    {
        CONST_VTBL struct IUserServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUserService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUserService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUserService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUserService_Hello(This,name,pResult)	\
    ( (This)->lpVtbl -> Hello(This,name,pResult) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUserService_INTERFACE_DEFINED__ */



#ifndef __Server_LIBRARY_DEFINED__
#define __Server_LIBRARY_DEFINED__

/* library Server */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_Server;

EXTERN_C const CLSID CLSID_UserService;

#ifdef __cplusplus

class DECLSPEC_UUID("538426ce-d0c0-4082-baca-75e24f781840")
UserService;
#endif
#endif /* __Server_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


