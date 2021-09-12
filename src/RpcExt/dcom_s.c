

/* this ALWAYS GENERATED file contains the RPC server stubs */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>
#include "dcom.h"

#define TYPE_FORMAT_STRING_SIZE   357                               
#define PROC_FORMAT_STRING_SIZE   397                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _dcom_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } dcom_MIDL_TYPE_FORMAT_STRING;

typedef struct _dcom_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } dcom_MIDL_PROC_FORMAT_STRING;

typedef struct _dcom_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } dcom_MIDL_EXPR_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

extern const dcom_MIDL_TYPE_FORMAT_STRING dcom__MIDL_TypeFormatString;
extern const dcom_MIDL_PROC_FORMAT_STRING dcom__MIDL_ProcFormatString;
extern const dcom_MIDL_EXPR_FORMAT_STRING dcom__MIDL_ExprFormatString;

/* Standard interface: ObjectRpcBaseTypes, ver. 0.0,
   GUID={0x99fcfe60,0x5260,0x101b,{0xbb,0xcb,0x00,0xaa,0x00,0x21,0x34,0x7a}} */


/* Standard interface: IOXIDResolver, ver. 0.0,
   GUID={0x99fcfec4,0x5260,0x101b,{0xbb,0xcb,0x00,0xaa,0x00,0x21,0x34,0x7a}} */


extern const MIDL_SERVER_INFO IOXIDResolver_ServerInfo;

extern RPC_DISPATCH_TABLE IOXIDResolver_v0_0_DispatchTable;

static const RPC_SERVER_INTERFACE IOXIDResolver___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x99fcfec4,0x5260,0x101b,{0xbb,0xcb,0x00,0xaa,0x00,0x21,0x34,0x7a}},{0,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    &IOXIDResolver_v0_0_DispatchTable,
    0,
    0,
    0,
    &IOXIDResolver_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE IOXIDResolver_v0_0_s_ifspec = (RPC_IF_HANDLE)& IOXIDResolver___RpcServerInterface;

extern const MIDL_STUB_DESC IOXIDResolver_StubDesc;


/* Standard interface: IRemoteActivation, ver. 0.0,
   GUID={0x4d9f4ab8,0x7d1c,0x11cf,{0x86,0x1e,0x00,0x20,0xaf,0x6e,0x7c,0x57}} */


extern const MIDL_SERVER_INFO IRemoteActivation_ServerInfo;

extern RPC_DISPATCH_TABLE IRemoteActivation_v0_0_DispatchTable;

static const RPC_SERVER_INTERFACE IRemoteActivation___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x4d9f4ab8,0x7d1c,0x11cf,{0x86,0x1e,0x00,0x20,0xaf,0x6e,0x7c,0x57}},{0,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    &IRemoteActivation_v0_0_DispatchTable,
    0,
    0,
    0,
    &IRemoteActivation_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE IRemoteActivation_v0_0_s_ifspec = (RPC_IF_HANDLE)& IRemoteActivation___RpcServerInterface;

extern const MIDL_STUB_DESC IRemoteActivation_StubDesc;

extern const EXPR_EVAL ExprEvalRoutines[];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const dcom_MIDL_PROC_FORMAT_STRING dcom__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure ResolveOxid */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x1 ),	/* 1 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	NdrFcShort( 0x2a ),	/* 42 */
/* 16 */	NdrFcShort( 0x68 ),	/* 104 */
/* 18 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter hRpc */

/* 20 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 22 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 24 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pOxid */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 30 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter cRequestedProtseqs */

/* 32 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 34 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 36 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter arRequestedProtseqs */

/* 38 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 40 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 42 */	NdrFcShort( 0x10 ),	/* Type Offset=16 */

	/* Parameter ppdsaOxidBindings */

/* 44 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 46 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 48 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter pipidRemUnknown */

/* 50 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 52 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAuthnHint */

/* 56 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 58 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 60 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure SimplePing */


	/* Return value */

/* 62 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 64 */	NdrFcLong( 0x1 ),	/* 1 */
/* 68 */	NdrFcShort( 0x1 ),	/* 1 */
/* 70 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 72 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 76 */	NdrFcShort( 0x24 ),	/* 36 */
/* 78 */	NdrFcShort( 0x8 ),	/* 8 */
/* 80 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter hRpc */

/* 82 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 84 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 86 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pSetId */

/* 88 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 90 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 92 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure ComplexPing */


	/* Return value */

/* 94 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 96 */	NdrFcLong( 0x1 ),	/* 1 */
/* 100 */	NdrFcShort( 0x2 ),	/* 2 */
/* 102 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 104 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 106 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 108 */	NdrFcShort( 0x36 ),	/* 54 */
/* 110 */	NdrFcShort( 0x46 ),	/* 70 */
/* 112 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x8,		/* 8 */

	/* Parameter hRpc */

/* 114 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 116 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 118 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pSetId */

/* 120 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 122 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 124 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter SequenceNum */

/* 126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 128 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 130 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter cAddToSet */

/* 132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 134 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 136 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter cDelFromSet */

/* 138 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 140 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 142 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter AddToSet */

/* 144 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 146 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 148 */	NdrFcShort( 0x54 ),	/* Type Offset=84 */

	/* Parameter DelFromSet */

/* 150 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 152 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 154 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pPingBackoffFactor */

/* 156 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 158 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 160 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure ServerAlive */


	/* Return value */

/* 162 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 164 */	NdrFcLong( 0x1 ),	/* 1 */
/* 168 */	NdrFcShort( 0x3 ),	/* 3 */
/* 170 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 172 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x8 ),	/* 8 */
/* 180 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Parameter hRpc */

/* 182 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 184 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 186 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure ResolveOxid2 */


	/* Return value */

/* 188 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 190 */	NdrFcLong( 0x1 ),	/* 1 */
/* 194 */	NdrFcShort( 0x4 ),	/* 4 */
/* 196 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 198 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 200 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 202 */	NdrFcShort( 0x2a ),	/* 42 */
/* 204 */	NdrFcShort( 0x90 ),	/* 144 */
/* 206 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x8,		/* 8 */

	/* Parameter hRpc */

/* 208 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 210 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 212 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pOxid */

/* 214 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 216 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 218 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter cRequestedProtseqs */

/* 220 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 222 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 224 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter arRequestedProtseqs */

/* 226 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 228 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 230 */	NdrFcShort( 0x10 ),	/* Type Offset=16 */

	/* Parameter ppdsaOxidBindings */

/* 232 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 234 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 236 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter pipidRemUnknown */

/* 238 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 240 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 242 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAuthnHint */

/* 244 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 246 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 248 */	NdrFcShort( 0x6a ),	/* Type Offset=106 */

	/* Parameter pComVersion */

/* 250 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 252 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 254 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure RemoteActivation */


	/* Return value */

/* 256 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 258 */	NdrFcLong( 0x0 ),	/* 0 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x54 ),	/* x86 Stack size/offset = 84 */
/* 266 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 268 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 270 */	NdrFcShort( 0x62 ),	/* 98 */
/* 272 */	NdrFcShort( 0xd0 ),	/* 208 */
/* 274 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x14,		/* 20 */

	/* Parameter hRpc */

/* 276 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 278 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 280 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Parameter ORPCthis */

/* 282 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 284 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 286 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */

	/* Parameter ORPCthat */

/* 288 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 290 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 292 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter Clsid */

/* 294 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 296 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 298 */	NdrFcShort( 0xfa ),	/* Type Offset=250 */

	/* Parameter pwszObjectName */

/* 300 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 302 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 304 */	NdrFcShort( 0xfe ),	/* Type Offset=254 */

	/* Parameter pObjectStorage */

/* 306 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 308 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ClientImpLevel */

/* 312 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 314 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Mode */

/* 318 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 320 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Interfaces */

/* 324 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 326 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 328 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter pIIDs */

/* 330 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 332 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 334 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter cRequestedProtseqs */

/* 336 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 338 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 340 */	NdrFcShort( 0x126 ),	/* Type Offset=294 */

	/* Parameter RequestedProtseqs */

/* 342 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 344 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 346 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pOxid */

/* 348 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 350 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 352 */	NdrFcShort( 0x10 ),	/* Type Offset=16 */

	/* Parameter ppdsaOxidBindings */

/* 354 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 356 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 358 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter pipidRemUnknown */

/* 360 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 362 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAuthnHint */

/* 366 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 368 */	NdrFcShort( 0x40 ),	/* x86 Stack size/offset = 64 */
/* 370 */	NdrFcShort( 0x6a ),	/* Type Offset=106 */

	/* Parameter pServerVersion */

/* 372 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 374 */	NdrFcShort( 0x44 ),	/* x86 Stack size/offset = 68 */
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter phr */

/* 378 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 380 */	NdrFcShort( 0x48 ),	/* x86 Stack size/offset = 72 */
/* 382 */	NdrFcShort( 0x138 ),	/* Type Offset=312 */

	/* Parameter ppInterfaceData */

/* 384 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 386 */	NdrFcShort( 0x4c ),	/* x86 Stack size/offset = 76 */
/* 388 */	NdrFcShort( 0x15a ),	/* Type Offset=346 */

	/* Parameter pResults */

/* 390 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 392 */	NdrFcShort( 0x50 ),	/* x86 Stack size/offset = 80 */
/* 394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const dcom_MIDL_TYPE_FORMAT_STRING dcom__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  4 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/*  8 */	NdrFcShort( 0x2 ),	/* 2 */
/* 10 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x0,		/*  */
/* 12 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 14 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 16 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 18 */	NdrFcShort( 0x2 ),	/* Offset= 2 (20) */
/* 20 */	
			0x12, 0x0,	/* FC_UP */
/* 22 */	NdrFcShort( 0xc ),	/* Offset= 12 (34) */
/* 24 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 26 */	NdrFcShort( 0x2 ),	/* 2 */
/* 28 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 30 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 32 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 34 */	
			0x17,		/* FC_CSTRUCT */
			0x1,		/* 1 */
/* 36 */	NdrFcShort( 0x4 ),	/* 4 */
/* 38 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (24) */
/* 40 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 42 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 44 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 46 */	NdrFcShort( 0x8 ),	/* Offset= 8 (54) */
/* 48 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 50 */	NdrFcShort( 0x8 ),	/* 8 */
/* 52 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 54 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 56 */	NdrFcShort( 0x10 ),	/* 16 */
/* 58 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 60 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 62 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (48) */
			0x5b,		/* FC_END */
/* 66 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 68 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 70 */	
			0x12,		/* FC_UP */
			0x0,		/* 0 */
/* 72 */	NdrFcShort( 0x2 ),	/* Offset= 2 (74) */
/* 74 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 76 */	NdrFcShort( 0x8 ),	/* 8 */
/* 78 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x0,		/*  */
/* 80 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 82 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 84 */	
			0x12,		/* FC_UP */
			0x0,		/* 0 */
/* 86 */	NdrFcShort( 0x2 ),	/* Offset= 2 (88) */
/* 88 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 90 */	NdrFcShort( 0x8 ),	/* 8 */
/* 92 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x0,		/*  */
/* 94 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 96 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 98 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 100 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 102 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 104 */	NdrFcShort( 0x2 ),	/* Offset= 2 (106) */
/* 106 */	
			0x15,		/* FC_STRUCT */
			0x1,		/* 1 */
/* 108 */	NdrFcShort( 0x4 ),	/* 4 */
/* 110 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 112 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 114 */	
			0x11, 0x0,	/* FC_RP */
/* 116 */	NdrFcShort( 0x4c ),	/* Offset= 76 (192) */
/* 118 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 120 */	NdrFcShort( 0x1 ),	/* 1 */
/* 122 */	0x0,		/* Corr desc:  field,  */
			0x59,		/* FC_CALLBACK */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 128 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 130 */	NdrFcShort( 0x14 ),	/* 20 */
/* 132 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (118) */
/* 134 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 136 */	NdrFcShort( 0xffae ),	/* Offset= -82 (54) */
/* 138 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 140 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 142 */	NdrFcShort( 0x4 ),	/* 4 */
/* 144 */	0x10,		/* Corr desc:  field pointer,  */
			0x59,		/* FC_CALLBACK */
/* 146 */	NdrFcShort( 0x1 ),	/* 1 */
/* 148 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 150 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 152 */	NdrFcShort( 0x4 ),	/* 4 */
/* 154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x1 ),	/* 1 */
/* 158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 162 */	0x12, 0x0,	/* FC_UP */
/* 164 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (128) */
/* 166 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 168 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 170 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 172 */	NdrFcShort( 0xc ),	/* 12 */
/* 174 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 176 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 178 */	NdrFcShort( 0x8 ),	/* 8 */
/* 180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 182 */	0x12, 0x0,	/* FC_UP */
/* 184 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (140) */
/* 186 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 188 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 190 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 192 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 194 */	NdrFcShort( 0x20 ),	/* 32 */
/* 196 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 198 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 200 */	NdrFcShort( 0x1c ),	/* 28 */
/* 202 */	NdrFcShort( 0x1c ),	/* 28 */
/* 204 */	0x12, 0x0,	/* FC_UP */
/* 206 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (170) */
/* 208 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 210 */	0x0,		/* 0 */
			NdrFcShort( 0xff97 ),	/* Offset= -105 (106) */
			0x8,		/* FC_LONG */
/* 214 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 216 */	0x0,		/* 0 */
			NdrFcShort( 0xff5d ),	/* Offset= -163 (54) */
			0x8,		/* FC_LONG */
/* 220 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 222 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 224 */	NdrFcShort( 0x2 ),	/* Offset= 2 (226) */
/* 226 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 228 */	NdrFcShort( 0x8 ),	/* 8 */
/* 230 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 232 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 234 */	NdrFcShort( 0x4 ),	/* 4 */
/* 236 */	NdrFcShort( 0x4 ),	/* 4 */
/* 238 */	0x12, 0x0,	/* FC_UP */
/* 240 */	NdrFcShort( 0xffba ),	/* Offset= -70 (170) */
/* 242 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 244 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 246 */	
			0x11, 0x0,	/* FC_RP */
/* 248 */	NdrFcShort( 0xff3e ),	/* Offset= -194 (54) */
/* 250 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 252 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 254 */	
			0x12, 0x0,	/* FC_UP */
/* 256 */	NdrFcShort( 0xc ),	/* Offset= 12 (268) */
/* 258 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 260 */	NdrFcShort( 0x1 ),	/* 1 */
/* 262 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 264 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 266 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 268 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 270 */	NdrFcShort( 0x4 ),	/* 4 */
/* 272 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (258) */
/* 274 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 276 */	
			0x12, 0x0,	/* FC_UP */
/* 278 */	NdrFcShort( 0x2 ),	/* Offset= 2 (280) */
/* 280 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 282 */	NdrFcShort( 0x10 ),	/* 16 */
/* 284 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 286 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 288 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 290 */	NdrFcShort( 0xff14 ),	/* Offset= -236 (54) */
/* 292 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 294 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 296 */	NdrFcShort( 0x2 ),	/* 2 */
/* 298 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x0,		/*  */
/* 300 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 302 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 304 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 306 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 308 */	
			0x11, 0x0,	/* FC_RP */
/* 310 */	NdrFcShort( 0x2 ),	/* Offset= 2 (312) */
/* 312 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 314 */	NdrFcShort( 0x4 ),	/* 4 */
/* 316 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 318 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 320 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 322 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 324 */	NdrFcShort( 0x4 ),	/* 4 */
/* 326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0x1 ),	/* 1 */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 334 */	0x12, 0x0,	/* FC_UP */
/* 336 */	NdrFcShort( 0xffbc ),	/* Offset= -68 (268) */
/* 338 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 340 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 342 */	
			0x11, 0x0,	/* FC_RP */
/* 344 */	NdrFcShort( 0x2 ),	/* Offset= 2 (346) */
/* 346 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 348 */	NdrFcShort( 0x4 ),	/* 4 */
/* 350 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 352 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 354 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */

			0x0
        }
    };

static void __RPC_USER IRemoteActivation_ORPC_EXTENTExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    ORPC_EXTENT *pS	=	( ORPC_EXTENT * )(( pStubMsg->StackTop - 20 ) );
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( ( ( pS->size + 7 )  & ~7 )  );
}

static void __RPC_USER IRemoteActivation_ORPC_EXTENT_ARRAYExprEval_0001( PMIDL_STUB_MESSAGE pStubMsg )
{
    ORPC_EXTENT_ARRAY *pS	=	( ORPC_EXTENT_ARRAY * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( ( ( pS->size + 1 )  & ~1 )  );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    IRemoteActivation_ORPC_EXTENTExprEval_0000
    ,IRemoteActivation_ORPC_EXTENT_ARRAYExprEval_0001
    };


static const unsigned short IOXIDResolver_FormatStringOffsetTable[] =
    {
    0,
    62,
    94,
    162,
    188
    };


static const MIDL_STUB_DESC IOXIDResolver_StubDesc = 
    {
    (void *)& IOXIDResolver___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    0,
    0,
    ExprEvalRoutines,
    0,
    dcom__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x70001f4, /* MIDL Version 7.0.500 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

static RPC_DISPATCH_FUNCTION IOXIDResolver_table[] =
    {
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    0
    };
RPC_DISPATCH_TABLE IOXIDResolver_v0_0_DispatchTable = 
    {
    5,
    IOXIDResolver_table
    };

static const SERVER_ROUTINE IOXIDResolver_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)ResolveOxid,
    (SERVER_ROUTINE)SimplePing,
    (SERVER_ROUTINE)ComplexPing,
    (SERVER_ROUTINE)ServerAlive,
    (SERVER_ROUTINE)ResolveOxid2
    };

static const MIDL_SERVER_INFO IOXIDResolver_ServerInfo = 
    {
    &IOXIDResolver_StubDesc,
    IOXIDResolver_ServerRoutineTable,
    dcom__MIDL_ProcFormatString.Format,
    IOXIDResolver_FormatStringOffsetTable,
    0,
    0,
    0,
    0};

static const unsigned short IRemoteActivation_FormatStringOffsetTable[] =
    {
    256
    };


static const MIDL_STUB_DESC IRemoteActivation_StubDesc = 
    {
    (void *)& IRemoteActivation___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    0,
    0,
    ExprEvalRoutines,
    0,
    dcom__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x70001f4, /* MIDL Version 7.0.500 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

static RPC_DISPATCH_FUNCTION IRemoteActivation_table[] =
    {
    NdrServerCall2,
    0
    };
RPC_DISPATCH_TABLE IRemoteActivation_v0_0_DispatchTable = 
    {
    1,
    IRemoteActivation_table
    };

static const SERVER_ROUTINE IRemoteActivation_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)RemoteActivation
    };

static const MIDL_SERVER_INFO IRemoteActivation_ServerInfo = 
    {
    &IRemoteActivation_StubDesc,
    IRemoteActivation_ServerRoutineTable,
    dcom__MIDL_ProcFormatString.Format,
    IRemoteActivation_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

