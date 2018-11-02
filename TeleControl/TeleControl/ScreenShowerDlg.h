#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "StructShare.h"
#include "CommunicationIOCP.h"
// CScreenShowerDlg �Ի���

class CScreenShowerDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CScreenShowerDlg)
protected:
    // IOCP
    CCommunicationIOCP &m_ref_IOCP;
    // IP�Ͷ˿�
    CString &m_ref_csIPAndPort;
    // Client������
    PCLIENTINFO m_pstClientInfo = NULL;
public:
    CScreenShowerDlg(CString &ref_csIPAndPort,
                     PCLIENTINFO pClientInfo,
                     CCommunicationIOCP &ref_IOCP, 
                     CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CScreenShowerDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SCREENSHOWER };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
