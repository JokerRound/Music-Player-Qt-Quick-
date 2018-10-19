// CmdDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TeleControl.h"
#include "CmdDlg.h"
#include "afxdialogex.h"


// CCmdDlg �Ի���

IMPLEMENT_DYNAMIC(CCmdDlg, CDialogEx)

CCmdDlg::CCmdDlg(CString &ref_csIPAndPort,
                 PCLIENTINFO pClientInfo,
                 CCommunicationIOCP &ref_IOCP,
                 CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CMD, pParent)
    , m_ref_csIPAndPort(ref_csIPAndPort)
    , m_ref_IOCP(ref_IOCP)
    , m_pstClientInfo(pClientInfo)
{

}

CCmdDlg::~CCmdDlg()
{
}

void CCmdDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDT_CMD, m_edtCmd);
}


BEGIN_MESSAGE_MAP(CCmdDlg, CDialogEx)
    ON_MESSAGE(WM_HASCMDREPLY, &CCmdDlg::OnHascmdreply)
    ON_MESSAGE(WM_HASORDERTOSEND, &CCmdDlg::OnHasordertosend)
END_MESSAGE_MAP()


// CCmdDlg ��Ϣ�������


BOOL CCmdDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    // Ͷ��Send���󣬿����Զ˵�CMD
    PPACKETFORMAT pstPacket =
        (PPACKETFORMAT)m_pstClientInfo->szSendTmpBuffer_;

    // *ע��* д������ʱҪ����
    m_pstClientInfo->CriticalSection_.Lock();
    pstPacket->ePacketType_ = PT_CMDCOMMAND_START;
    pstPacket->dwSize_ = 0;

    m_pstClientInfo->SendBuffer_.Write(
        (PBYTE)m_pstClientInfo->szSendTmpBuffer_,
        PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // ��������ʱ������
    memset(m_pstClientInfo->szSendTmpBuffer_,
           0,
           PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // Ͷ�ݷ�������
    BOOL bRet =
        m_ref_IOCP.PostSendRequst(m_pstClientInfo->sctClientSocket_,
                                  m_pstClientInfo->SendBuffer_);

    m_pstClientInfo->SendBuffer_.ClearBuffer();
    m_pstClientInfo->CriticalSection_.Unlock();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // �쳣: OCX ����ҳӦ���� FALSE
}


afx_msg LRESULT CCmdDlg::OnHascmdreply(WPARAM wParam, LPARAM lParam)
{
    CString *pcsCmdReply = (CString *)wParam;
    m_csScreen += *pcsCmdReply;

    m_edtCmd.SetWindowText(m_csScreen);

    // Focus auto move down.
    int nLenth = m_edtCmd.GetWindowTextLength();
    m_edtCmd.SetSel(nLenth, nLenth, FALSE);
    m_edtCmd.SetFocus();
    UpdateData();

    return TRUE;
}


afx_msg LRESULT CCmdDlg::OnHasordertosend(WPARAM wParam, LPARAM lParam)
{
    CString *csOreder = (CString *)wParam;

    // ���������Ŀ�������
    // Ͷ��Send���󣬿����Զ˵�CMD
    PPACKETFORMAT pstPacket =
        (PPACKETFORMAT)m_pstClientInfo->szSendTmpBuffer_;

    // *ע��* д������ʱҪ����
    m_pstClientInfo->CriticalSection_.Lock();
    pstPacket->ePacketType_ = PT_CMD_ORDER;

    pstPacket->dwSize_ = (csOreder->GetLength() + 1) * sizeof(TCHAR);
    memmove(pstPacket->szContent_,
            csOreder->GetBuffer(),
            pstPacket->dwSize_);


    m_pstClientInfo->SendBuffer_.Write(
        (PBYTE)m_pstClientInfo->szSendTmpBuffer_,
        PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // ��������ʱ������
    memset(m_pstClientInfo->szSendTmpBuffer_,
           0,
           PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // Ͷ�ݷ�������
    BOOL bRet =
        m_ref_IOCP.PostSendRequst(m_pstClientInfo->sctClientSocket_,
                                  m_pstClientInfo->SendBuffer_);

    m_pstClientInfo->SendBuffer_.ClearBuffer();
    m_pstClientInfo->CriticalSection_.Unlock();
    
    return 0;
}
