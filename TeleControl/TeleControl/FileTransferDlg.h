#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "CommunicationIOCP.h"
#include "StructShare.h"

// CFileTransferDlg �Ի���

class CFileTransferDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileTransferDlg)

public:
	CFileTransferDlg(CString &ref_csIPAndPort,
                     PCLIENTINFO pstClientInfo,
                     CCommunicationIOCP &ref_IOCP,
                     CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFileTransferDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILETRANSFER };
#endif
private:
    // �̷�
    CString m_csDevice;
    CImageList *m_pSysSmallIconImageList = NULL;
    CImageList *m_pSysBigIconImageList = NULL;


    // IOCP
    CCommunicationIOCP &m_ref_IOCP;
    // IP�Ͷ˿�
    CString &m_ref_csIPAndPort;
    // Client������
    PCLIENTINFO m_pstClientInfo = NULL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    // �̷���ȡ����¼�
    HANDLE m_hGetTargetDeviceEvent = NULL;
    // �ļ��б��ȡ����¼�
    HANDLE m_hGetTargetFileListEvent = NULL;
    // Server�˵�ǰ�ļ��б���
    int m_iServerActiveStyleIdx = 0;
    // Ŀ�굱ǰ�ļ��б���
    int m_iTargetHostActiveStyleIdx = 0;
    // �������̷��б�
    CComboBox m_cmbServerDevice;
    // ���������ļ�·��
    CEdit m_edtServerFilePath;
    // Ŀ��˵��ļ��б�
    CString m_csFileList;
    // Ŀ��˵��̷�
    CString m_csTargetHostDevice;
    // Ŀ��˵��̷��ֽ���
    size_t m_uiTargetHostDeviceLen = 0;

    afx_msg void OnBnClickedBtnServerSkip();
    virtual BOOL OnInitDialog();
    void ShowFileList(CListCtrl &lstTarget,
                      CComboBox &cmbDevice, CEdit & edtFilePath, const int & iActiveStyleIdx);

    void ChangeListStyle(CListCtrl &lstTarget,
                         int &iActiveStyleIdx,
                         int iOldIndex,
                         int iNewIndex);

    CString GetSubName(CComboBox &ref_cmbDevice,
                       CEdit &ref_edtFilePath, 
                       CString &ref_csFilename);

    void BackParentDirctory(CComboBox &ref_cmbDevice,
                            CEdit &ref_edtFilePath);

    BOOL IsDirectory(CComboBox &ref_cmbDevice,
                     CEdit &ref_edtFilePath, 
                     const CString *TargetFile = NULL);

    // Server���ļ��б���
    CComboBox m_cmbServerFileListStyle;
    CComboBox m_cmbTargetHostFileListStyle;
    CListCtrl m_lstServerFileList;
    CListCtrl m_lstTargetHostFileList;
    afx_msg void OnNMDblclkLstServerFilelist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCbnSelchangeCmbServerFilelistStyle();
    afx_msg void OnCbnSelchangeCmbServerDevice();
    // Ŀ�������̷�
    CComboBox m_cmbTargetHostDevice;
    // ���������б�
    CListCtrl m_lstTransferTask;
    // Ŀ�������ļ�·��
    CEdit m_edtTargetHostFilePath;
    afx_msg void OnCbnSelchangeCmbTargethostFilelistStyle();
    afx_msg void OnCbnSelchangeCmbTargethostDevice();
    afx_msg void OnBnClickedBtnTargethostSkip();
    afx_msg void OnBnClickedBtnGetfile();
    afx_msg void OnBnClickedBtnPutfile();
    afx_msg void OnClose();
    afx_msg void OnNMDblclkLstTargethostFilelist(NMHDR *pNMHDR, LRESULT *pResult);
};
