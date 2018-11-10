//******************************************************************************
// License:     MIT
// Author:      Hoffman
// Create Time: 2018-11-08
// Description: 
//    achieve of method in CCommunicationIOCP and related function.
//
// Modify Log:
//    2018-10-24    Hoffman
//      Info: Add "SendDataUseIOCP" function achieve.
//******************************************************************************

#include "stdafx.h"
#include "CommunicationIOCP.h"
#include "MacroShare.h"
#include "StructShare.h"
#include "PacketHandle.h"
#include "TeleClientDlg.h"

CCommunicationIOCP::CCommunicationIOCP()
    : m_hIOCP(NULL)
    , m_phthdArray(NULL)
{
}


CCommunicationIOCP::~CCommunicationIOCP()
{
}

BOOL CCommunicationIOCP::Create(PIOCPTHREADADDTIONDATA pAddtionData /*= NULL*/,
                                DWORD dwThreadNum /*= 0*/)
{
    if (m_hIOCP == NULL)
    {
        // Create IOCP.
        m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                                         NULL,
                                         0,
                                         dwThreadNum);

        m_dwMaxThreadNum = dwThreadNum;
        if (m_dwMaxThreadNum == 0)
        {
            // Get cpu cores.
            SYSTEM_INFO stSi;
            ::GetSystemInfo(&stSi);
            
            // Set number of threads.
            m_dwMaxThreadNum = stSi.dwNumberOfProcessors;
        }

        if (NULL != m_hIOCP)
        {
            // Create threads if create iocp successfully.
            m_stIOCPThreadParam.pIOCP_ = this;
            m_stIOCPThreadParam.pThreadAddtionData_ = pAddtionData;

            // This threads will be added into 
            // "Had freed threads queue" of iocp.
            m_phthdArray = new HANDLE[m_dwMaxThreadNum];
            for (size_t cntI = 0; cntI < m_dwMaxThreadNum; ++cntI)
            {
                m_phthdArray[cntI] = 
                    (HANDLE)::_beginthreadex(NULL,
                                             0,
                                             (_beginthreadex_proc_type)
                                             ThreadWork,
                                             &m_stIOCPThreadParam,
                                             0,
                                             NULL);
            }

            return TRUE;
        }
    }

    return FALSE;
}

BOOL CCommunicationIOCP::Associate(HANDLE hFileHandle,
                                   ULONG_PTR pulCompletionKey /*= 0*/)
{
    // The fourth parament will be ignored.
    HANDLE hRet = CreateIoCompletionPort(hFileHandle,
                                         m_hIOCP,
                                         pulCompletionKey,
                                         m_dwMaxThreadNum);
    return hRet == m_hIOCP;
} //! CCommunicationIOCP::Associate END

BOOL CCommunicationIOCP::Destroy()
{

    return 0;
}

DWORD CCommunicationIOCP::ThreadWork(LPVOID lpParam)
{
    // ��������
    PIOCPTHREADPARAM pstIOCPThreadParam = (PIOCPTHREADPARAM)lpParam;
    CCommunicationIOCP *pIOCP = pstIOCPThreadParam->pIOCP_;
    CTeleClientDlg *pTeleClientDlg = 
        pstIOCPThreadParam->pThreadAddtionData_->pTeleClientDlg;

    // �߳�ѭ����ȡ��ɰ�
    while (TRUE)
    {
        // ��ȡ�������
        DWORD dwTransferNumBytes = 0;
        ULONG_PTR ulCompletionKey = 0;
        OVERLAPPED *pstOverlapped = NULL;

        // *ע��* ��ʱ���̴߳����ͷ��̶߳����н��뵽�ȴ��̶߳���(�����ȳ�)
        BOOL bRet = GetQueuedCompletionStatus(pIOCP->m_hIOCP,
                                              &dwTransferNumBytes,
                                              &ulCompletionKey,
                                              &pstOverlapped,
                                              IOCP_WAIT_TIME);
        // *ע��* ��ʱ���߳��Ѵӵȴ��̶߳����н��뵽���ͷ��̶߳���
        DWORD dwError = GetLastError();
        if (!bRet)
        {
            CString csFailedInfo;
            if (pstOverlapped != NULL)
            {
                csFailedInfo.Format(_T("I/O������ʧ��: %u\r\n"),
                                    dwError);
            }
            else
            {
                if (dwError == WAIT_TIMEOUT)
                {
                    csFailedInfo = _T("�ȴ���ʱ\r\n");
                }
                else
                {
                    // ����ʧ����ֱ���˳�
                    csFailedInfo.Format(_T("Get I/O����������ʧ��: %u\r\n"),
                                        dwError);
                    OutputDebugString(csFailedInfo.GetString());
                    break;
                }
            }
            OutputDebugString(csFailedInfo.GetString());
            continue;
        }

        // ��ȡ�ص��ṹ����
        POVERLAPPEDWITHDATA pstData = CONTAINING_RECORD(pstOverlapped,
                                                        OVERLAPPEDWITHDATA,
                                                        stOverlapped_);
        PCLIENTINFO pstClientInfo = (PCLIENTINFO)ulCompletionKey;
        switch (pstData->eIOCPType_)
        {
            case IOCP_RECV:
            {
                // �ɹ��յ������ݰ�
                // �����ݰ�д��Client�Ľ��ջ�����
                pstClientInfo->RecvBuffer_.Write((PBYTE)pstData->szPacket_,
                                               dwTransferNumBytes);

                // ѭ������ 
                while (TRUE)
                {
                    // �ж��Ƿ����յ�������ͷ, ���������˳�
                    if (pstClientInfo->RecvBuffer_.GetBufferLen() < 
                        PACKET_HEADER_SIZE)
                    {
                        break;
                    }
                    
                    // ������ͷ
                    PPACKETFORMAT pstPacket = 
                        (PPACKETFORMAT)pstClientInfo->RecvBuffer_.GetBuffer();

                    // �жϰ������Ƿ�����, ���������˳�
                    DWORD dwCurrentPacketSize =
                        (pstClientInfo->RecvBuffer_.GetBufferLen() -
                         PACKET_HEADER_SIZE);
                    if (dwCurrentPacketSize < pstPacket->dwSize_)
                    {
                        break;
                    }
                    
                    // ��ʼ�������ݰ�
                    pstClientInfo->CriticalSection_.Lock();
                    PACKETFORMAT stTmpHeader;
                    pstClientInfo->RecvBuffer_.Read((PBYTE)&stTmpHeader,
                                                    PACKET_HEADER_SIZE);

                    if (stTmpHeader.dwSize_ > 0 &&
                        stTmpHeader.dwSize_ < PACKET_CONTENT_MAXSIZE)
                    {
                        // ��������ʱ����Buffer��ȥ��RecvBuffer���
                        pstClientInfo->RecvBuffer_.Read(
                            (PBYTE)pstClientInfo->szRecvTmpBuffer_,
                            stTmpHeader.dwSize_);
                    }

                    OnHandlePacket(stTmpHeader.ePacketType_,
                                   pstClientInfo->sctClientSocket_,
                                   pstClientInfo->szRecvTmpBuffer_,
                                   stTmpHeader,
                                   pstClientInfo,
                                   *pIOCP);
                    pstClientInfo->CriticalSection_.Unlock();

                } //! while ѭ������ END

                // �ٴ�Ͷ��һ��Recv����
                pIOCP->PostRecvRequst(pstClientInfo->sctClientSocket_);
                if (!bRet)
                {
                    OutputDebugString(_T("Recv����Ͷ��ʧ��\r\n"));
                }

                break;
            } //! case IOCP_RECV END
            case IOCP_SEND:
            {
                // ���ͻ���������������
                if (pstClientInfo->SendBuffer_.GetBufferLen() > 0)
                {
                    pIOCP->PostSendRequst(pstClientInfo->sctClientSocket_,
                                          pstClientInfo->SendBuffer_);
                }

                break;
            }
        } //! switch END

        // �ͷ��ص��ṹ��
        if (pstData != NULL)
        {
            delete pstData;
            pstData = NULL;
        }
    } //! while �߳�ѭ����ȡ��ɰ� END

    return 0;
} //! CCommunicationIOCP::ThreadWork END

BOOL CCommunicationIOCP::PostSendRequst(const SOCKET sctTarget,
                                        CBuffer &SendBuffer)
{
#ifdef DEBUG
    DWORD dwError = 0;
    CString csErrorMessage;
    DWORD dwFileLine = 0;
#endif // DEBUG
    POVERLAPPEDWITHDATA pstOverlappedWithData = NULL;
    BOOL bNoError = FALSE;

    do
    {
        DWORD dwSendedBytes = 0;
        pstOverlappedWithData = new OVERLAPPEDWITHDATA();

        if (NULL == pstOverlappedWithData)
        {
#ifdef DEBUG
            dwFileLine = __LINE__;
#endif // DEBU
            break;
        }

        // Set buffer and length.
        pstOverlappedWithData->eIOCPType_ = IOCP_SEND;
        pstOverlappedWithData->stBuffer_.buf =
            (char *)SendBuffer.GetBuffer();
        pstOverlappedWithData->stBuffer_.len =
            SendBuffer.GetBufferLen();

        int iRet = 
            WSASend(sctTarget,
                    &pstOverlappedWithData->stBuffer_,
                    1,
                    &dwSendedBytes,
                    0,
                    (WSAOVERLAPPED *)&pstOverlappedWithData->stOverlapped_,
                    NULL);
        if (SOCKET_ERROR == iRet  &&
            WSAGetLastError() != ERROR_IO_PENDING)
        {
#ifdef DEBUG
            dwFileLine = __LINE__;
#endif // DEBU
            break;
        }

        bNoError = TRUE;
    } while (FALSE);
    
    if (!bNoError)
    {
#ifdef DEBUG
        dwError = GetLastError();
        GetErrorMessage(dwError, csErrorMessage);
        OutputDebugStringWithInfo(csErrorMessage, __FILET__, dwFileLine);
#endif // DEBUG

        if (NULL != pstOverlappedWithData)
        {
            delete pstOverlappedWithData;
            pstOverlappedWithData = NULL;
        }
    }

    return bNoError;
} //! CCommunicationIOCP::PostSendRequst END

BOOL CCommunicationIOCP::PostRecvRequst(const SOCKET sctTarget)
{
    POVERLAPPEDWITHDATA pstOverlappedWithData = NULL;

    do
    {
        DWORD dwRecvedBytes = 0;
        pstOverlappedWithData = new OVERLAPPEDWITHDATA();

        if (pstOverlappedWithData == NULL)
        {
            OutputDebugString(_T("PostRecvRequst�����ڴ�ʧ��\r\n"));
            break;
        }

        // buffer�ͳ��ȸ�ֵ
        pstOverlappedWithData->eIOCPType_ = IOCP_RECV;
        pstOverlappedWithData->stBuffer_.buf = pstOverlappedWithData->szPacket_;
        pstOverlappedWithData->stBuffer_.len = PACKET_CONTENT_MAXSIZE;

        DWORD dwFlags = 0;
        int iRet = 
            WSARecv(sctTarget,
                    &pstOverlappedWithData->stBuffer_,
                    1,
                    &dwRecvedBytes,
                    &dwFlags,
                    (WSAOVERLAPPED *)&pstOverlappedWithData->stOverlapped_,
                    NULL);
        if (iRet == SOCKET_ERROR &&
            WSAGetLastError() != ERROR_IO_PENDING)
        {
            OutputDebugString(_T("WSARecvʧ��\r\n"));
            break;
        }

        // ����
        return TRUE;
    } while (FALSE);
    
    // �쳣������Դ
    if (NULL != pstOverlappedWithData)
    {
        delete pstOverlappedWithData;
        pstOverlappedWithData = NULL;
    }

    return FALSE;
} //! CCommunicationIOCP::PostRecvRequst END

BOOL SendDataUseIOCP(CLIENTINFO *&ref_pstClientInfo,
                     CCommunicationIOCP &ref_IOCP,
                     CString &ref_csData,
                     PACKETTYPE ePacketType)
{
    PPACKETFORMAT pstPacket = 
        (PPACKETFORMAT)ref_pstClientInfo->szSendTmpBuffer_;

    //***********************************************
    //* ALARM * It should complete thread synchronize
    //***********************************************
    ref_pstClientInfo->CriticalSection_.Lock();
    pstPacket->ePacketType_ = ePacketType;
    pstPacket->dwSize_ =
        (ref_csData.GetLength() + 1) * sizeof(TCHAR);

    // Copy
    memmove(pstPacket->szContent_,
            ref_csData.GetBuffer(),
            pstPacket->dwSize_);

    // Write data of need to send.
    ref_pstClientInfo->SendBuffer_.Write(
        (PBYTE)ref_pstClientInfo->szSendTmpBuffer_,
        PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // Make SendTmpBuffer be zero.
    memset(ref_pstClientInfo->szSendTmpBuffer_,
           0,
           PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // Post the send requst to iocp.
    BOOL bRet =
        ref_IOCP.PostSendRequst(ref_pstClientInfo->sctClientSocket_,
                                ref_pstClientInfo->SendBuffer_);

    ref_pstClientInfo->SendBuffer_.ClearBuffer();
    ref_pstClientInfo->CriticalSection_.Unlock();

    return bRet;
} //! SendDataUseIOCP END

BOOL SendDataUseIOCP(CLIENTINFO *&ref_pstClientInfo,
                     CCommunicationIOCP &ref_IOCP,
                     CString &ref_csData,
                     const DWORD &ref_dwSize,
                     CString &ref_csFileFullName,
                     const ULONGLONG ullFilePointPos)
{
    PPACKETFORMAT pstPacket = 
        (PPACKETFORMAT)ref_pstClientInfo->szSendTmpBuffer_;

    //***********************************************
    //* ALARM * It should complete thread synchronize
    //***********************************************
    ref_pstClientInfo->CriticalSection_.Lock();
    pstPacket->ePacketType_ = PT_FILE_DATA;
    pstPacket->dwSize_ = ref_dwSize;
    // Copy file name.
    memmove(pstPacket->szFileFullName_,
            ref_csFileFullName.GetBuffer(),
            MAX_PATH); 
    pstPacket->ullFilePointPos_ = ullFilePointPos;

    // Copy
    memmove(pstPacket->szContent_,
            ref_csData.GetBuffer(),
            pstPacket->dwSize_);

    // Write data of need to send.
    ref_pstClientInfo->SendBuffer_.Write(
        (PBYTE)ref_pstClientInfo->szSendTmpBuffer_,
        PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // Make SendTmpBuffer be zero.
    memset(ref_pstClientInfo->szSendTmpBuffer_,
           0,
           PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // Post the send requst to iocp.
    BOOL bRet =
        ref_IOCP.PostSendRequst(ref_pstClientInfo->sctClientSocket_,
                                ref_pstClientInfo->SendBuffer_);

    ref_pstClientInfo->SendBuffer_.ClearBuffer();
    ref_pstClientInfo->CriticalSection_.Unlock();

    return bRet;
} //! SendDataUseIOCP END