// ProcessManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TeleControl.h"
#include "ProcessManagerDlg.h"
#include "afxdialogex.h"


// CProcessManagerDlg �Ի���

IMPLEMENT_DYNAMIC(CProcessManagerDlg, CDialogEx)

CProcessManagerDlg::CProcessManagerDlg(CString &ref_csIPAndPort,
                                       PCLIENTINFO pstClientInfo,
                                       CCommunicationIOCP &ref_IOCP,
                                       CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PROCESSMANAGER, pParent)
    , m_ref_IOCP(ref_IOCP)
    , m_ref_csIPAndPort(ref_csIPAndPort)
{
    m_pstClientInfo = pstClientInfo;
}

CProcessManagerDlg::~CProcessManagerDlg()
{
}

void CProcessManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CProcessManagerDlg, CDialogEx)
END_MESSAGE_MAP()


// CProcessManagerDlg ��Ϣ�������


BOOL CProcessManagerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    do
    {
        PPACKETFORMAT pstPacket =
            (PPACKETFORMAT)m_pstClientInfo->szSendTmpBuffer_;

        // *ע��* д������ʱҪ����
        m_pstClientInfo->CriticalSection_.Lock();
        pstPacket->ePacketType_ = PT_PROCESS_INFO;
        pstPacket->dwSize_ = 0;

        m_pstClientInfo->SendBuffer_.Write(
            (PBYTE)m_pstClientInfo->szSendTmpBuffer_,
            PACKET_HEADER_SIZE + pstPacket->dwSize_);
        // �����ʱ������
        memset(m_pstClientInfo->szSendTmpBuffer_,
               0,
               PACKET_HEADER_SIZE + pstPacket->dwSize_);

        // Ͷ�ݷ�������
        BOOL bRet = 
            m_ref_IOCP.PostSendRequst(m_pstClientInfo->sctClientSocket_,
                                      m_pstClientInfo->SendBuffer_);

        m_pstClientInfo->CriticalSection_.Unlock();

        DWORD dwRet = WaitForSingleObject(m_hGetProcessInfoOver, INFINITE);
        if (dwRet != WAIT_OBJECT_0)
        {
#ifdef DEBUG
            OutputDebugString(_T("�ȴ���ȡĿ�������Ϣʧ��"));
#endif // DEBUG
            break;
        }

        // ������ȡ����Ϣ�������б�



    } while (FALSE);


    return TRUE;  // return TRUE unless you set the focus to a control
                  // �쳣: OCX ����ҳӦ���� FALSE
}
