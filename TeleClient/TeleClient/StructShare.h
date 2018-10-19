#pragma once
#ifndef STRUCTSHARE_H_
#define STRUCTSHARE_H_
#include "stdafx.h"
#include "MacroShare.h"
#include "CBuffer.h"

class CClientManager;
class CCommunicationIOCP;
class CTeleClientDlg;

struct tagProfileInfo
{
    CString csServerAddr_;
    DWORD dwPort_;
};

// ͨ�Ű�����
typedef enum tagPacketType
{
    PT_TESE,
    PT_HEATBEAT,
    PT_SCREENPICTURE,
    PT_PROCESS_INFO,
    PT_PROCESSCOMMAND_KILL,
    // Used when get target host file list.
    PT_FILE_LIST,
    // Used when get target host device.
    PT_FILE_DEVICE,
    // Used when need to get or put file.
    PT_FILE_DATA,
    PT_FILECOMMAND_PUTFILE,
    PT_FILECOMMAND_GETFILE,
    PT_FILECOMMAND_PAUSE,
    PT_FILECOMMAND_CONTINUE,
    PT_CMD_ORDER,
    PT_CMD_REPLY,
    PT_CMDCOMMAND_START,
    PT_CMDCOMMAND_END,
} PACKETTYPE;

// IOCP������
typedef enum tagIOCPType
{
    IOCP_RECV,
    IOCP_SEND,
} IOCPTYPE, *PIOCPTYPE;

// ͨ�Ű���ʽ
#pragma pack(push, 1)
typedef struct tagPacketFormat
{
    PACKETTYPE  ePacketType_;
    DWORD       dwSize_;
    char        szContent_[1];
} PACKETFORMAT, *PPACKETFORMAT;
#pragma pack(pop)

// �ص��ṹ���װ
typedef struct tagOverlappedWithData
{
    IOCPTYPE        eIOCPType_;
    OVERLAPPED      stOverlapped_;
    WSABUF          stBuffer_;
    char            szPacket_[PACKET_CONTENT_MAXSIZE];
} OVERLAPPEDWITHDATA, *POVERLAPPEDWITHDATA;


// The parament of thread to connect.
typedef struct tagConnectThreadParam
{
    SOCKET              sctConnectSocket_;
    CCommunicationIOCP  *pIOCP_;
    sockaddr_in         stServerAddrInfo_;
    HANDLE              *phBreakEvent_;
    CTeleClientDlg      *pTeleClientDlg_;
} CONNECTTHREADPARAM, *PCONNECTTHREADPARAM;

// The parament of thread to transport file.
typedef struct tagFileTransportThreadParam
{
    CString             *pcsFileListToGet_;
    CTeleClientDlg      *pTeleClientDlg_;
} FILETRANSPORTTHREADPARAM, *PFILETRANSPORTTHREADPARAM;

// The context of client
typedef struct tagClientInfo
{
    sockaddr_in stClientAddrInfo_;
    // ���һ��ͨ��
    time_t tLastTime_;
    SOCKET   sctClientSocket_;

    // ��д������
    CBuffer RecvBuffer_;
    CBuffer SendBuffer_;
    // ��ʱ������
    char szRecvTmpBuffer_[PACKET_CONTENT_MAXSIZE];
    char szSendTmpBuffer_[PACKET_CONTENT_MAXSIZE];

    // �ٽ���
    CCriticalSection CriticalSection_;

    // CMD���̾��
    HANDLE hCMD_;
    // Cmd��д�ܵ����
    HANDLE hCmdReadPipe_;
    HANDLE hCmdWritePipe_;

    // ServerCmd��д�ܵ����
    HANDLE hServerCmdReadPipe_;
    HANDLE hServerCmdWritePipe_;
    CTeleClientDlg *pTeleClientDlg_ = NULL;
} CLIENTINFO, *PCLIENTINFO;

// IOCP�߳�����Ҫ�ĸ�������
typedef struct tagIOCPThreadAddtionData
{
    CTeleClientDlg *pTeleClientDlg;
} IOCPTHREADADDTIONDATA, *PIOCPTHREADADDTIONDATA;

// IOCP�̲߳���
typedef struct tagIOCPThreadParam
{
    CCommunicationIOCP *pIOCP_;
    PIOCPTHREADADDTIONDATA pThreadAddtionData_;
} IOCPTHREADPARAM, *PIOCPTHREADPARAM;
#endif // !STRUCTSHARE_H_
