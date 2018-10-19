#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "CommunicationIOCP.h"
#include "StructShare.h"
#include "FileTransportManager.h"

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
    // The event that had got driver.
    HANDLE m_hGetTargetDeviceEvent = NULL;
    // The event that had got file list.
    HANDLE m_hGetTargetFileListEvent = NULL;
    // The sytle of server's filelist.
    int m_iServerActiveStyleIdx = 0;
    // The sytle of target host's filelist.
    int m_iTargetHostActiveStyleIdx = 0;
    // The driver list of server.
    CComboBox m_cmbServerDevice;
    // The file path of server.
    CEdit m_edtServerFilePath;
    // the file list of target host.
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

    // Ŀ�������̷�
    CComboBox m_cmbTargetHostDevice;
    // ���������б�
    CListCtrl m_lstTransferTask;
    // Ŀ�������ļ�·��
    CEdit m_edtTargetHostFilePath;

    // The manager of task transmission.
    CFileTransportManager m_TransportTaskManager;

    afx_msg void OnNMDblclkLstServerFilelist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCbnSelchangeCmbServerFilelistStyle();
    afx_msg void OnCbnSelchangeCmbServerDevice();

    afx_msg void OnCbnSelchangeCmbTargethostFilelistStyle();
    afx_msg void OnCbnSelchangeCmbTargethostDevice();
    afx_msg void OnBnClickedBtnTargethostSkip();
    afx_msg void OnBnClickedBtnGetfile();
    afx_msg void OnBnClickedBtnPutfile();
    afx_msg void OnNMDblclkLstTargethostFilelist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClose();
};
