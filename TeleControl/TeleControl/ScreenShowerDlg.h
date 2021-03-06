#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "StructShare.h"
#include "CommunicationIOCP.h"
// CScreenShowerDlg 对话框

class CScreenShowerDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CScreenShowerDlg)
protected:
    // IOCP
    CCommunicationIOCP &m_ref_IOCP;
    // IP和端口
    CString &m_ref_csIPAndPort;
    // Client上下文
    PCLIENTINFO m_pstClientInfo = NULL;
public:
    CScreenShowerDlg(CString &ref_csIPAndPort,
                     PCLIENTINFO pClientInfo,
                     CCommunicationIOCP &ref_IOCP, 
                     CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CScreenShowerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SCREENSHOWER };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
