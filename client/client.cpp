// rpc_hello.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "hello_h.h"

#pragma comment(lib, "RpcRT4.lib")

int main()
{
    RPC_STATUS status;
    RPC_WSTR pszUuid = NULL;
    RPC_WSTR pszProtoSeq = (RPC_WSTR) L"ncacn_np";
    RPC_WSTR pszNetkAddrs = NULL;
    RPC_WSTR pszEndpt = (RPC_WSTR) L"\\pipe\\hello";
    RPC_WSTR pszOptions = NULL;
    RPC_WSTR pszStringBinding = NULL;
    unsigned char* pszString = (unsigned char*) "hello world";
    unsigned long ulCode;
    /* Use MS function to merge elements of the string binding into
           proper sequence. */
    status = RpcStringBindingCompose(
        pszUuid,		/* use uuid in hello.idl */
        pszProtoSeq,      /* ncacn_np = named pipe */
        pszNetkAddrs,    /* name of server machine */
        pszEndpt,         /* endpoint address of server */
        pszOptions,       /* read manual */
        &pszStringBinding); /* where result goes */
    if (status) {
        exit(status);
    }

    /* Set the binding handle that will be used to bind to the server. */
    status = RpcBindingFromStringBinding(
        pszStringBinding, /* string made above */
        &hello_IfHandle); /* where result goes */
    if (status) {
        exit(status);
    }

    RpcTryExcept{		/* MS macro */
       HelloProc(pszString);  	/* RPC call */
    printf("helloed\n");
       Shutdown();  	       	/* RPC call */
       printf("exiting server\n");
    }

        RpcExcept(1) {			/* MS macro */
        ulCode = RpcExceptionCode();
        printf("Error 0x%lx=%ld\n", ulCode, ulCode);
    }
    RpcEndExcept				/* MS macro */

 /* any error in either RPC call causes the RpcExcept block to be executed */

 /*  Free string and binding handle */
        status = RpcStringFree(&pszStringBinding);
    if (status) {
        exit(status);
    }
    status = RpcBindingFree(&hello_IfHandle);
    if (status) {
        exit(status);
    }
    exit(0);
}


// Implemented as per here https://docs.microsoft.com/en-us/windows/win32/rpc/the-client-application
// although there's no notion of these in the hello_h.h file
void __RPC_FAR* __RPC_USER midl_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR* ptr)
{
    free(ptr);
}