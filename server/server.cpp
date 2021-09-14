// server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include "hello_h.h"

#pragma comment(lib, "RpcRT4.lib")

// from https://personal.utdallas.edu/~ivor/cs6386/msrpc.txt
// and https://www.codeproject.com/Articles/4837/Introduction-to-RPC-Part-1?msg=1526644#xx1526644xx

void HelloProc(unsigned char* pszString)
{
    printf("%s\n", pszString);
}

void Shutdown(void)
{
    RPC_STATUS status;
    status = RpcMgmtStopServerListening(NULL);
    if (status) {
        exit(status);
    }

    status = RpcServerUnregisterIf(NULL, NULL, FALSE);
    if (status) {
        exit(status);
    }

    printf("exiting\n");
}

// Naive security callback.
RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE /*hInterface*/, void* /*pBindingHandle*/)
{
    return RPC_S_OK; // Always allow anyone.
}

int main()
{
    RPC_STATUS status;
    RPC_WSTR pszProtocolSequence = (RPC_WSTR) L"ncacn_np";
    unsigned char* pszSecurity = NULL;
    RPC_WSTR pszEndpoint = (RPC_WSTR) L"\\pipe\\hello";
    unsigned int    cMinCalls = 1;
    unsigned int    fDontWait = FALSE;
    //handle_t hello_IfHandle = NULL;

    status = RpcServerUseProtseqEp(pszProtocolSequence,
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        pszEndpoint,
        pszSecurity);

    if (status) exit(status);

    status = RpcServerRegisterIf2(
        hello_v1_0_s_ifspec,              // Interface to register.
        NULL,                                // Use the MIDL generated entry-point vector.
        NULL,                                // Use the MIDL generated entry-point vector.
        RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, // Forces use of security callback.
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,      // Use default number of concurrent calls.
        (unsigned)-1,                        // Infinite max size of incoming data blocks.
        SecurityCallback);                   // Naive security callback.

    if (status) exit(status);


    printf("listening\n");
    status = RpcServerListen(cMinCalls,
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        fDontWait);

    if (status) exit(status);
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