#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CProcessManagerDlg �Ի���
#include "StructShare.h"
#include "CommunicationIOCP.h"

class CProcessManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessManagerDlg)

public:
    // �����б��ȡ����¼�
    HANDLE m_hGetProcessInfoOver = NULL;
protected:
    // IOCP
    CCommunicationIOCP &m_ref_IOCP;
    // IP�Ͷ˿�
    CString &m_ref_csIPAndPort;
    // Client������
    PCLIENTINFO m_pstClientInfo = NULL;
public:
	CProcessManagerDlg(CString &ref_csIPAndPort,
                       PCLIENTINFO pClientInfo,
                       CCommunicationIOCP &ref_IOCP,
                       CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProcessManagerDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESSMANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
