
#pragma once
#ifndef CLIENTMANAGER_H_
#define CLIENTMANAGER_H_
#include "stdafx.h"
#include "StructShare.h"

class CClientManager
{
public:
    // Binding client infomation with SOCKET.
    CMap<SOCKET, SOCKET, tagClientInfo *, tagClientInfo *> m_mapClient;
    // Binding SOCKET with IP and port. 
    CMap<CString, LPCTSTR, SOCKET, SOCKET> m_mapIPPortWithSocket;
private:
    // 
    CCriticalSection m_CriticalSection;
public:
    CClientManager();
    virtual ~CClientManager();

    //**********************************************************************
    // FUNTION:     Insert client infomation to m_mapClient
    // OUPUT:       None
    // RETURN:      Successful (TRUE) or failure (FALSE)
    // PARAMETER:
    //      sctTargetSocket:    A socket which take to bind, it's Key in map
    //      pClientInfo:        A ClientInfo point which bind with socket,
    //                          it's value in map
    // NOTE:        
    //      1. Point cann't be NULL.
    //**********************************************************************
    BOOL InsertClient(_In_ const SOCKET &sctTargetSocket,
                      _In_ tagClientInfo *pClientInfo);


    // Insert SOECKET to m_mapIPPortWithSocket
    BOOL InsertSocket(_In_ const CString &ref_csIPAndPort, 
                      _In_ const SOCKET &ref_sctTargetSocket);

    SOCKET GetSocket(const CString &csIPAndPort);

    tagClientInfo *GetClient(const SOCKET &sctTargetSocket);

    BOOL RemoveClient(const SOCKET &sctTargetSocket);

    // Clean and free resource
    void Destory();
};

#endif // !SOCKETMANAGER_H_
