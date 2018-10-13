// ScreenShowerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TeleControl.h"
#include "ScreenShowerDlg.h"
#include "afxdialogex.h"


// CScreenShowerDlg �Ի���

IMPLEMENT_DYNAMIC(CScreenShowerDlg, CDialogEx)

CScreenShowerDlg::CScreenShowerDlg(CString &ref_csIPAndPort,
                                   PCLIENTINFO pstClientInfo,
                                   CCommunicationIOCP &ref_IOCP,
                                   CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SCREENSHOWER, pParent)
    , m_ref_IOCP(ref_IOCP)
    , m_ref_csIPAndPort(ref_csIPAndPort)
{
        m_pstClientInfo = pstClientInfo;
}

CScreenShowerDlg::~CScreenShowerDlg()
{
}

void CScreenShowerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenShowerDlg, CDialogEx)
END_MESSAGE_MAP()


// CScreenShowerDlg ��Ϣ�������


BOOL CScreenShowerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    PPACKETFORMAT pstPacket =
         (PPACKETFORMAT)m_pstClientInfo->szSendTmpBuffer_;

    // *ע��* д������ʱҪ����
    m_pstClientInfo->CriticalSection_.Lock();
    pstPacket->ePacketType_ = PT_SCREENPICTURE;
    pstPacket->dwSize_ = 0;

    m_pstClientInfo->SendBuffer_.Write(
        (PBYTE)m_pstClientInfo->szSendTmpBuffer_,
        PACKET_HEADER_SIZE + pstPacket->dwSize_);
    // �����ʱ������
    memset(m_pstClientInfo->szSendTmpBuffer_,
           0,
           PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // Ͷ�ݷ�������
    BOOL bRet = m_ref_IOCP.PostSendRequst(m_pstClientInfo->sctClientSocket_,
                                          m_pstClientInfo->SendBuffer_);
    m_pstClientInfo->CriticalSection_.Unlock();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // �쳣: OCX ����ҳӦ���� FALSE
}
