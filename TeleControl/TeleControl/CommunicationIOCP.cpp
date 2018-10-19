#include "stdafx.h"
#include "CommunicationIOCP.h"
#include "MacroShare.h"
#include "StructShare.h"
#include "PacketHandle.h"

CCommunicationIOCP::CCommunicationIOCP()
    :m_hIOCP(NULL)
    ,m_phthdArray(NULL)
{
}


CCommunicationIOCP::~CCommunicationIOCP()
{
}

BOOL CCommunicationIOCP::Create(PIOCPTHREADADDTIONDATA pAddtionData /*= NULL*/,
                                DWORD dwThreadNum /*= 0*/)
{
    // ��û�о��ʱ�ſɴ���
    if (m_hIOCP == NULL)
    {
        m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                                         NULL,
                                         0,
                                         dwThreadNum);

        m_dwMaxThreadNum = dwThreadNum;
        if (m_dwMaxThreadNum == 0)
        {
            // ��ȡϵͳCPU������
            SYSTEM_INFO stSi;
            ::GetSystemInfo(&stSi);
            
            // �����߳���
            m_dwMaxThreadNum = stSi.dwNumberOfProcessors;
        }

        if (m_hIOCP != NULL)
        {
            // �������IOCP�ɹ�����ʼ�����߳�
            m_stIOCPThreadParam.pIOCP_ = this;
            m_stIOCPThreadParam.pThreadAddtionData_ = pAddtionData;
            // *ע��* ��Ϊ������ִ�У���ʱ�������̻߳���뵽IOCP�����ͷ��߳��б�
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
    } //! if m_hIOCPΪ��ʱ���� END

    return FALSE;
}

BOOL CCommunicationIOCP::Associate(HANDLE hFileHandle,
                                   ULONG_PTR pulCompletionKey /*= 0*/)
{
    // *ע��* ���ú��������豸��ʱ����4�������ᱻ����
    HANDLE hRet = CreateIoCompletionPort(hFileHandle,
                                         m_hIOCP,
                                         pulCompletionKey,
                                         m_dwMaxThreadNum);
    return hRet == m_hIOCP;
}

BOOL CCommunicationIOCP::Destroy()
{
    if (m_hIOCP != NULL)
    {
        CloseHandle(m_hIOCP);
        m_hIOCP = NULL;
    }

    return 0;
}

DWORD CCommunicationIOCP::ThreadWork(LPVOID lpParam)
{
    // ��������
    PIOCPTHREADPARAM pstIOCPThreadParam = (PIOCPTHREADPARAM)lpParam;
    CCommunicationIOCP *pIOCP = pstIOCPThreadParam->pIOCP_;
    CClientManager *pClientManager = 
        pstIOCPThreadParam->pThreadAddtionData_->pClientManager_;

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
        CString csErrorMessage;
        GetErrorMessage(dwError, csErrorMessage);
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
#ifdef DEBUG
                    OutputDebugStringWithInfo(csErrorMessage,
                                              __FILET__, 
                                              __LINE__);
#endif // DEBUG
                    // Finish loop and threads exits.
                    break;
                }
            }
#ifdef DEBUG
            OutputDebugStringWithInfo(csErrorMessage,
                                      __FILET__,
                                      __LINE__);
#endif // DEBUG
            continue;
        }

        POVERLAPPEDWITHDATA pstData = CONTAINING_RECORD(pstOverlapped,
                                                        OVERLAPPEDWITHDATA,
                                                        stOverlapped_);
        PCLIENTINFO pstClientInfo = (PCLIENTINFO)ulCompletionKey;

        switch (pstData->eIOCPType_)
        {
            case IOCP_RECV:
            {
                // Get packet of data successful.
                // Update the time.
                pstClientInfo->tLastTime_ = time(NULL);
                // Write data to recive buffer of client.
                pstClientInfo->RecvBuffer_.Write((PBYTE)pstData->szPacket_,
                                                 dwTransferNumBytes);

                // Use loop to receive.
                while (TRUE)
                {
                    // �ж��Ƿ����յ�������ͷ, ���������˳�
                    if (pstClientInfo->RecvBuffer_.GetBufferLen() < 
                        PACKET_HEADER_SIZE)
                    {
                        break;
                    }
                    
                    PPACKETFORMAT pstPacket = 
                        (PPACKETFORMAT)pstClientInfo->RecvBuffer_.GetBuffer();

                    // To exit if data which in package had missed.
                    DWORD dwCurrentPacketSize =
                        (pstClientInfo->RecvBuffer_.GetBufferLen() -
                         PACKET_HEADER_SIZE);
                    if (dwCurrentPacketSize < pstPacket->dwSize_)
                    {
                        break;
                    }
                    
                    // ��ʼ�������ݰ�
                    PACKETFORMAT stTmpHeader;

                    //*****************************
                    // *ALARM* It should remove the one byte in flexible array.
                    //*****************************
                    pstClientInfo->RecvBuffer_.Read((PBYTE)&stTmpHeader,
                                                    PACKET_HEADER_SIZE);
                    if (stTmpHeader.dwSize_ > 0 &&
                        stTmpHeader.dwSize_ < PACKET_CONTENT_MAXSIZE)
                    {
                        pstClientInfo->CriticalSection_.Lock();
                        // Swap data to buffer tmporary, 
                        // RecvBuffer had been cleaned.
                        pstClientInfo->RecvBuffer_.Read(
                            (PBYTE)pstClientInfo->szRecvTmpBuffer_,
                            stTmpHeader.dwSize_);
                        // Free lock.
                        pstClientInfo->CriticalSection_.Unlock();
                    }

                    OnHandlePacket(stTmpHeader.ePacketType_,
                                   pstClientInfo->sctClientSocket_,
                                   pstClientInfo->szRecvTmpBuffer_,
                                   stTmpHeader.dwSize_,
                                   pstClientInfo,
                                   *pIOCP);

                } //! while ѭ������ END

                // �ٴ�Ͷ��һ��Recv����
                bRet = pIOCP->PostRecvRequst(pstClientInfo->sctClientSocket_);
                if (!bRet)
                {
                    OutputDebugString(_T("Recv����Ͷ��ʧ��\r\n"));
                }
                break;
            } //! case IOCP_RECV END
            case IOCP_SEND:
            {
                // �ɹ����������ݰ�
                // �����˶������ݣ���ӷ��ͻ������������������
                //pstClientInfo->CriticalSection_.Lock();
                //pstClientInfo->SendBuffer_.Delete(dwTransferNumBytes);
                //pstClientInfo->CriticalSection_.Unlock();

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
    POVERLAPPEDWITHDATA pstOverlappedWithData = NULL;

    do
    {
        DWORD dwSendedBytes = 0;
        pstOverlappedWithData = new OVERLAPPEDWITHDATA();

        if (pstOverlappedWithData == NULL)
        {
            OutputDebugString(_T("PostRecvRequst�����ڴ�ʧ��\r\n"));
            break;
        }

        // buffer�ͳ��ȸ�ֵ
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
        if (iRet == SOCKET_ERROR &&
            WSAGetLastError() != ERROR_IO_PENDING)
        {
            OutputDebugString(_T("WSASendʧ��\r\n"));
            break;
        }


        // End
        return TRUE;
    } while (FALSE);
    
    // Free resource when had errored.
    if (pstOverlappedWithData == NULL)
    {
        delete pstOverlappedWithData;
        pstOverlappedWithData = NULL;
    } 

    return FALSE;
} //! CCommunicationIOCP::PostSendRequst END

BOOL CCommunicationIOCP::PostRecvRequst(const SOCKET sctTarget)
{
    POVERLAPPEDWITHDATA pstOverlappedWithData = NULL;

    do
    {
        DWORD dwRecvedBytes = 0;
        //****************************************
        //*ALARM* This memory will free when IOCP deal with it finished.
        //****************************************
        pstOverlappedWithData = new OVERLAPPEDWITHDATA();

        if (pstOverlappedWithData == NULL)
        {
            OutputDebugString(_T("PostRecvRequst�����ڴ�ʧ��\r\n"));
            break;
        }

        // Give buffer and length.
        pstOverlappedWithData->eIOCPType_ = IOCP_RECV;
        pstOverlappedWithData->stBuffer_.buf = 
            pstOverlappedWithData->szPacket_;
        pstOverlappedWithData->stBuffer_.len =
            PACKET_CONTENT_MAXSIZE;

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
            OutputDebugString(_T("WSARecvʧ��"));
            break;
        }

        // End
        return TRUE;
    } while (FALSE);
    
    // Free resource when had errored.
    if (pstOverlappedWithData == NULL)
    {
        delete pstOverlappedWithData;
        pstOverlappedWithData = NULL;
    }

    return FALSE;
} //! CCommunicationIOCP::PostRecvRequst END
