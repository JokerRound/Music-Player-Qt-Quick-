#pragma once
#ifndef STRUCTSHARE_H_
#define STRUCTSHARE_H_
#include "stdafx.h"
#include "MacroShare.h"
#include "CBuffer.h"

class CClientManager;
class CCommunicationIOCP;
class CHostListView;
class CFileTransferDlg;
class CCmdDlg;
class CProcessManagerDlg;
class CScreenShowerDlg;

namespace std
{
#ifdef _UNICODE
    typedef std::wstring tstring;
    typedef std::wsmatch tsmatch;
    typedef std::wcmatch tcmatch;
#else
    typedef std::string tstring;
    typedef std::smatch tsmatch;
    typedef std::cmatch tcmatch;
#endif
}


// Type of packet
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

// Customize Dialog Type
typedef enum tagDialogType
{
    CDT_FILETRANSFER,
    CDT_CMD,
    CDT_PROCESSMANAGER,
    CDT_SCREENSHOWER,
} DIALOGTYPE;

// ������Ϣˢ������
typedef enum tagInfoFlushType
{
    IFT_ACCEPTCLIENT,
    IFT_SERVERSTART,
    IFT_SERVERCLOSE,
} INFOFLUSHTYPE;

// �����б��������
typedef enum tagHostListColumnType
{
    HLCT_IPADDR,
    HLCT_PORT,
} HOSTLISTCOLUMNTYPE;

// type of log list.
typedef enum tagLogListColumnType
{
    LLCT_TIME,
    LLCT_TYPE,
    LLCT_INFO,
} LOGLISTCOLUMNTYPE;

// type of process list.
typedef enum tagProcessListColumnTYPE
{
    PLCT_IMAGENAME,
    PLCT_ID,
    PLCT_PARENTID,
    PLCT_IMAGEPATH,
} PROCESSLISTCOLUMNTYPE;

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


// �Ӵ��̲߳���
typedef struct tagAcceptThreadParam
{
    SOCKET              sctAcceptSocket_;
    CClientManager      *pClientManager_;
    CCommunicationIOCP  *pIOCP_;
    CHostListView       *pHostListView_;
} ACCEPTTHREADPARAM, *PACCEPTTHREADPARAM;

// �ͻ�����Ϣ
typedef struct tagClientInfo
{
    sockaddr_in         stClientAddrInfo_;
    // ���һ��ͨ��
    time_t              tLastTime_;
    SOCKET              sctClientSocket_;
    // ��д������
    CBuffer             RecvBuffer_;
    CBuffer             SendBuffer_;
    // ���պ����ʱ��������
    char                szRecvTmpBuffer_[PACKET_CONTENT_MAXSIZE];
    // ����ǰ����ʱ����������
    char                szSendTmpBuffer_[PACKET_CONTENT_MAXSIZE];
    // �ٽ���
    CCriticalSection    CriticalSection_;
    // �ļ�����Ի���
    CFileTransferDlg    *pFileTransferDlg_ = NULL;
    // CMD�Ի���
    CCmdDlg             *pCmdDlg_ = NULL;
    // ���̹���Ի���
    CProcessManagerDlg  *pProcessManagerDlg_ = NULL;
    // ��Ļ���
    CScreenShowerDlg    *pScreenShowerDlg_ = NULL;
} CLIENTINFO, *PCLIENTINFO;

// IOCP�߳�����Ҫ�ĸ�������
typedef struct tagIOCPThreadAddtionData
{
    CClientManager *pClientManager_;
} IOCPTHREADADDTIONDATA, *PIOCPTHREADADDTIONDATA;

// IOCP�̲߳���
typedef struct tagIOCPThreadParam
{
    CCommunicationIOCP      *pIOCP_;
    PIOCPTHREADADDTIONDATA  pThreadAddtionData_;
} IOCPTHREADPARAM, *PIOCPTHREADPARAM;
#endif // !STRUCTSHARE_H_
