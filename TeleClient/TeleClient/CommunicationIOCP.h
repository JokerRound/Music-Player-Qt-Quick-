#pragma once
#ifndef COMMUNICATIONIOCP_H_
#define COMMUNICATIONIOCP_H_
#include "stdafx.h"
#include "StructShare.h"
#include "CBuffer.h"

class CCommunicationIOCP
{
private:
    HANDLE m_hIOCP = NULL;
    HANDLE *m_pahThreadArray = NULL;
    // �̳߳�����߳�����
    DWORD m_dwMaxThreadNum = 0;
    // ��ǰ�̳߳��е�����
    DWORD m_dwCurrentThreadNum = 0;
    // ��ǰ�����е��߳�����
    DWORD m_dwBusyThreadNum = 0;
    // �ٽ���
    CCriticalSection m_CriticalSection;
    // �̲߳���
    IOCPTHREADPARAM m_stIOCPThreadParam = { 0 };
public:
    CCommunicationIOCP();
    ~CCommunicationIOCP();

    // ����
    BOOL Create(_In_ PIOCPTHREADADDTIONDATA pAddtionData = NULL,
                _In_ DWORD dwThreadNum = 0);
    // ��
    BOOL Associate(_In_ HANDLE hFileHandle,
                   _In_ ULONG_PTR pulCompletionKey = 0);
    // Free resource.
    BOOL Destroy();

    // �̹߳����ص�
    static DWORD ThreadWork(LPVOID lpParam);

    // Ͷ�ݽ�������
    BOOL PostSendRequst(const SOCKET sctTarget, CBuffer &SendBuffer);
    // Ͷ�ݷ�������
    BOOL PostRecvRequst(const SOCKET sctTarget);

};

BOOL SendDataUseIOCP(_In_ CLIENTINFO *&ref_pstClientInfo,
                     _In_ CCommunicationIOCP &ref_IOCP,
                     _In_ const CString &ref_csData,
                     _In_ PACKETTYPE ePacketType);


BOOL SendDataUseIOCP(_In_ CLIENTINFO *&ref_pstClientInfo,
                     _In_ CCommunicationIOCP &ref_IOCP,
                     _In_ const CString &ref_csData,
                     _In_ const DWORD &ref_dwSize,
                     _In_ CString &ref_csFileFullName,
                     _In_ const ULONGLONG &ref_ullFilePointPos,
                     _In_ const ULONGLONG &ref_ullTaskId);
#endif // !COMMUNICATIONIOCP_H_
