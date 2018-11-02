#include "stdafx.h"
#include "StructShare.h"
#include "FileTransferDlg.h"
#include "ClientManager.h"
#include "FileTransferStruct.h"
#include "PacketHandle.h"
#include "CmdDlg.h"
#include "CommunicationIOCP.h"


BOOL OnHeartBeat(SOCKET sctTargetSocket,
                 PCLIENTINFO pstClientInfo,
                 CCommunicationIOCP &IOCP)
{
    //PPACKETFORMAT pstPacket = (PPACKETFORMAT)pstClientInfo->szSendTmpBuffer_;

    //// *ע��* д������ʱҪ����
    //pstClientInfo->CriticalSection_.Lock();
    //pstPacket->ePacketType_ = PT_HEARTBEAT;
    //pstPacket->dwSize_ = 0;

    //pstClientInfo->SendBuffer_.Write((PBYTE)pstClientInfo->szSendTmpBuffer_,
    //                                 PACKET_HEADER_SIZE + pstPacket->dwSize_);
    //// �����ʱ������
    //memset(pstClientInfo->szSendTmpBuffer_,
    //       0,
    //       PACKET_HEADER_SIZE + pstPacket->dwSize_);

    //// Ͷ�ݷ�������
    //BOOL bRet = IOCP.PostSendRequst(sctTargetSocket,
    //                                pstClientInfo->SendBuffer_);

    //pstClientInfo->SendBuffer_.ClearBuffer();
    //pstClientInfo->CriticalSection_.Unlock();

    CString csTmpData;
    BOOL bRet = SendDataUseIOCP(pstClientInfo, IOCP, csTmpData, PT_HEARTBEAT);

    return bRet;
} 

BOOL OnFileList(SOCKET sctTargetSocket,
                char *szBuffer,
                size_t uiLen,
                PCLIENTINFO pstClientInfo,
                CCommunicationIOCP &IOCP)
{
    pstClientInfo->pFileTransferDlg_->m_csFileList.Empty();

    memmove(pstClientInfo->pFileTransferDlg_->
            m_csFileList.GetBufferSetLength(PACKET_CONTENT_MAXSIZE),
            szBuffer,
            uiLen);
    pstClientInfo->pFileTransferDlg_->m_csFileList.ReleaseBuffer();
    // ��ս�����ʱ������
    memset(szBuffer, 0, uiLen);

    SetEvent(pstClientInfo->pFileTransferDlg_->m_hGetTargetFileListEvent);

    return TRUE;
} //! OnFileList END

BOOL OnFileDevice(SOCKET sctTargetSocket,
                  char *szBuffer,
                  size_t uiLen,
                  PCLIENTINFO pstClientInfo,
                  CCommunicationIOCP &IOCP)
{
    pstClientInfo->pFileTransferDlg_->m_csTargetHostDevice.Empty();
    // �����ݴ����ļ�����Ի���
    pstClientInfo->pFileTransferDlg_->m_uiTargetHostDeviceLen = uiLen;
    memmove(pstClientInfo->pFileTransferDlg_->
            m_csTargetHostDevice.GetBufferSetLength(PACKET_CONTENT_MAXSIZE),
            szBuffer,
            uiLen);

    pstClientInfo->pFileTransferDlg_->
        m_csTargetHostDevice.ReleaseBuffer();

    // Clear temp buffer receive.
    memset(szBuffer, 0, uiLen);

    // Signal the evet get driver successfully.
    SetEvent(pstClientInfo->pFileTransferDlg_->m_hGetTargetDeviceEvent);
    
    return TRUE;
}

BOOL OnFileData(SOCKET sctTargetSocket,
                char *szBuffer,
                PACKETFORMAT &ref_stHeader,
                PCLIENTINFO pstClientInfo,
                CCommunicationIOCP &IOCP)
{
    CString csFileData;
    FILEDATAINQUEUE stFileData = { 0 };

    // Get file name and postion.
    stFileData.csFileFullName_ = ref_stHeader.szFileFullName_;
    stFileData.ullFilePointPos_ = ref_stHeader.ullFilePointPos_;

    // Wirte file data to buffer object.
    stFileData.FileDataBuffer_.Write((PBYTE)szBuffer, ref_stHeader.dwSize_);

    // Clear temp buffer receive.
    memset(szBuffer, 0, ref_stHeader.dwSize_);

    // Put file data into queue that FileTransfer do it.
    BOOL bRet = FALSE;
    bRet = pstClientInfo->pFileTransferDlg_->SendMessage(WM_HASFILEDATA, 
                                                         (WPARAM)&stFileData, 
                                                         0);

    return bRet;
} //! OnFileData END

// Deal with the reply from target host.
BOOL OnCMDReply(SOCKET sctTargetSocket,
                char *szBuffer,
                size_t uiLen,
                PCLIENTINFO pstClintInfo)
{
    CString csCmdReply;
    szBuffer[uiLen] = _T('\0');
    csCmdReply = szBuffer;

    memset(szBuffer, 0, uiLen);

    BOOL bRet = 
        pstClintInfo->pCmdDlg_->SendMessage(WM_HASCMDREPLY,
                                            (WPARAM)&csCmdReply,
                                            0);

    return bRet;
}

// Deal with the info from target host.
BOOL OnProcessInfo(SOCKET sctTargetSocket,
                   char *pszBuffer,
                   size_t uiLen,
                   PCLIENTINFO pstClintInfo)
{
    CString csProcessInfoList;
    char *pszTmpBuffer = new char[uiLen + 1];
    memset(pszTmpBuffer, 0, uiLen + 1);
    memmove(pszTmpBuffer, pszBuffer, uiLen + 1);

    csProcessInfoList = pszTmpBuffer;

    if (pszTmpBuffer != NULL)
    {
        delete[] pszTmpBuffer;
        pszTmpBuffer = NULL;
    }

    // �����ͻ��˷��͹����Ľ�����Ϣ�б�
    while (!csProcessInfoList.IsEmpty())
    {
        CString csProcessInfo;
        int iProcessInfoPos = csProcessInfoList.Find(_T("|"));
        csProcessInfo = csProcessInfoList.Left(iProcessInfoPos);
        csProcessInfoList =
            csProcessInfoList.Right(
                csProcessInfoList.GetLength() - iProcessInfoPos - 1);

        // EXE�ļ���
        int iExeFilePos = csProcessInfo.Find(_T("?"));
        CString csExeFile = csProcessInfo.Left(iExeFilePos);
        csProcessInfo =
            csProcessInfo.Right(
                csProcessInfo.GetLength() - iExeFilePos - 1);

        // ����ID
        int iProcessIdPos = csProcessInfo.Find(_T("?"));
        CString csProcessId = csProcessInfo.Left(iProcessIdPos);
        csProcessInfo =
            csProcessInfo.Right(
                csProcessInfo.GetLength() - iExeFilePos - 1);

        // ������ID
        int iParentIdPos = csProcessInfo.Find(_T("?"));
        CString csParentid = csProcessInfo.Left(iParentIdPos);
        csProcessInfo =
            csProcessInfo.Right(
                csProcessInfo.GetLength() - iParentIdPos - 1);

        // ��ȡȫ·��
        iParentIdPos = csProcessInfo.Find(_T("?"));
        CString csFullPath = csProcessInfo.Left(iParentIdPos);
        csProcessInfo =
            csProcessInfo.Right(
                csProcessInfo.GetLength() - iParentIdPos - 1);
    } //! while ���������б� END

    return TRUE;
}

BOOL OnScreenPicture(SOCKET sctTargetSocket,
                     char *szBuffer,
                     size_t uiLen,
                     PCLIENTINFO pstClintInfo)
{

    return TRUE;
}


BOOL OnHandlePacket(PACKETTYPE ePacketType,
                    SOCKET sctTargetSocket,
                    char *szBuffer,
                    PACKETFORMAT &ref_stHeader,
                    PCLIENTINFO pstClientInfo,
                    CCommunicationIOCP &IOCP)
{
    BOOL bRet = FALSE;
    
    // Packet Identification.
    switch (ePacketType)
    {
        case PT_HEARTBEAT:
        {
            OutputDebugString(_T("�յ�������\r\n"));
            // Deel with heartbeat.
            bRet = OnHeartBeat(sctTargetSocket,
                               pstClientInfo,
                               IOCP);
            if (!bRet)
            {
                OutputDebugString(_T("����������ʧ��\r\n"));
            }

            break;
        }
        // The data of target host's list.
        case PT_FILE_LIST:
        {
            OutputDebugString(_T("�յ��ļ��б���Ϣ\r\n"));
            bRet = OnFileList(sctTargetSocket,
                              szBuffer,
                              ref_stHeader.dwSize_,
                              pstClientInfo,
                              IOCP);
            if (!bRet)
            {
                OutputDebugString(_T("�ļ��б���Ϣ����ʧ��"));
            }
            break;
        }
        // The device of target host.
        case PT_FILE_DEVICE:
        {
#ifdef DEBUG
            OutputDebugString(_T("�յ������̷���Ϣ\r\n"));
#endif // DEBUG
            bRet = OnFileDevice(sctTargetSocket,
                                szBuffer,
                                ref_stHeader.dwSize_,
                                pstClientInfo,
                                IOCP);
            if (!bRet)
            {
                OutputDebugString(_T("�����̷���Ϣ����ʧ��"));
            }
            break;
        }
        case PT_FILE_DATA:
        {
            bRet = OnFileData(sctTargetSocket,
                              szBuffer,
                              ref_stHeader,
                              pstClientInfo,
                              IOCP);
            if (!bRet)
            {
#ifdef DEBUG
                OutputDebugStringWithInfo(_T("Recive the GETFILE data "
                                             "from target host"),
                                          __FILET__,
                                          __LINE__);
#endif // DEBUG
            }
            break;
        }
        case PT_PROCESS_INFO:
        {
#ifdef DEBUG
            OutputDebugString(_T("�յ�Ŀ�귢���Ľ�����Ϣ\r\n"));
#endif // DEBUG
            bRet = OnProcessInfo(sctTargetSocket,
                                 szBuffer,
                                 ref_stHeader.dwSize_,
                                 pstClientInfo);
            if (!bRet)
            {
#ifdef DEBUG
            OutputDebugString(_T("�յ�Ŀ�귢���Ľ�����Ϣ\r\n"));
#endif // DEBUG
            }
            break;
        }
        case PT_PROCESSCOMMAND_KILL:
        {
            OutputDebugString(_T("����KILL�ɹ�\r\n"));
            break;
        }
        case PT_SCREENPICTURE:
        {
            bRet = OnScreenPicture(sctTargetSocket,
                                   szBuffer,
                                   ref_stHeader.dwSize_,
                                   pstClientInfo);
            break;
        }
        case PT_CMD_REPLY:
        {
#ifdef DEBUG
            OutputDebugString(_T("�յ�CMD�Ļظ���Ϣ\r\n"));
#endif // DEBUG
            bRet = OnCMDReply(sctTargetSocket,
                              szBuffer,
                              ref_stHeader.dwSize_,
                              pstClientInfo);
            if (!bRet)
            {
#ifdef DEBUG
                OutputDebugString(_T("CMD�Ļظ���Ϣ\r\n"));
#endif // DEBUG
            }
            break;
        }
        default:
        {
            OutputDebugString(_T("���ݰ����Ͳ��ڴ���Χ��\r\n"));
            break;
        }
    } //! switch "Packet Identification" END

    return bRet;
} //! OnHandlePacket END