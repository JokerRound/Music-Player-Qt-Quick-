// HostLisView.cpp: 实现文件
//

#include "stdafx.h"
#include "TeleControl.h"
#include "HostListView.h"
#include "TeleControlDoc.h"
#include "LogListView.h"
#include "ScreenShowerDlg.h"
#include "ProcessManagerDlg.h"


// CHostLisView

IMPLEMENT_DYNCREATE(CHostListView, CListView)

CHostListView::CHostListView()
    : m_sctAccept(INVALID_SOCKET)
    , m_pstAcceptThreadParam(NULL)
    , m_pthdAcceptThread(NULL)
{

}

CHostListView::~CHostListView()
{
}

BEGIN_MESSAGE_MAP(CHostListView, CListView)
    ON_COMMAND(ID_MN_START, &CHostListView::OnMnStart)
    ON_MESSAGE(WM_HASINFOTOFLUSH, &CHostListView::OnHasinfotoflush)
    ON_COMMAND(ID_MN_CLOSE, &CHostListView::OnMnClose)
    ON_NOTIFY_REFLECT(NM_RCLICK, &CHostListView::OnNMRClick)
    ON_COMMAND(ID_MN_OPERATION_FILETRANSFER, &CHostListView::OnMnOperationFiletransfer)
    ON_COMMAND(ID_MN_CMD, &CHostListView::OnMnCmd)
    ON_COMMAND(ID_MN_PROCESSMANAGER, &CHostListView::OnMnProcessmanager)
    ON_COMMAND(ID_MN_SCREENSHOWER, &CHostListView::OnMnScreenshower)
END_MESSAGE_MAP()


// CHostLisView 诊断

#ifdef _DEBUG
void CHostListView::AssertValid() const
{
    CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CHostListView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CHostLisView 消息处理程序


void CHostListView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();

    CListCtrl &lstHostList = GetListCtrl();

    // To change the style of list.
    DWORD dwListStyle = lstHostList.GetStyle();
    dwListStyle |= LVS_REPORT | LVS_SHOWSELALWAYS;
    lstHostList.ModifyStyle(0, dwListStyle);

    // Init list control.
    int iColumnIndex = 0;
    int iTargetColumn = 0;
    // Address.
    iTargetColumn = lstHostList.InsertColumn(iColumnIndex++, _T("地址"));
    lstHostList.SetColumnWidth(iTargetColumn, 100);
    // Port.
    iTargetColumn = lstHostList.InsertColumn(iColumnIndex++, _T("端口"));
    lstHostList.SetColumnWidth(iTargetColumn, 50);
    // System Version.
    iTargetColumn = lstHostList.InsertColumn(iColumnIndex++, _T("系统版本"));
    lstHostList.SetColumnWidth(iTargetColumn, 100);
}


void CHostListView::OnMnStart()
{
#ifdef DEBUG
    DWORD dwError = -1;
    CString csErrorMessage;
#endif // DEBUG

    // Get LogListView.
    CLogListView *pLogListView =
        (CLogListView *)GetTargetView(RUNTIME_CLASS(CLogListView));

    do
    {
        // Create socket.
        m_sctAccept = socket(AF_INET,
                             SOCK_STREAM,
                             IPPROTO_TCP);

        if (m_sctAccept == INVALID_SOCKET)
        {
            OutputDebugString(_T("创建Socket失败"));
            break;
        }

        // 获取端口配置
        DWORD dwPort = GetPrivateProfileInt(_T("Base"),
                                            _T("Port"),
                                            NULL,
                                            m_csInitFile);
        sockaddr_in stServerAddrInfo = { 0 };
        stServerAddrInfo.sin_family = AF_INET;
        stServerAddrInfo.sin_port = htons((short)dwPort);
        stServerAddrInfo.sin_addr.S_un.S_addr = INADDR_ANY;

        // 为Server Socket命名
        int iRet = bind(m_sctAccept,
            (sockaddr *)&stServerAddrInfo,
                        sizeof(stServerAddrInfo));
        if (iRet == SOCKET_ERROR)
        {
            OutputDebugString(_T("服务Socket命名失败"));
            break;
        }

        // 开启Server Socket监听
        iRet = listen(m_sctAccept, SOMAXCONN);
        if (iRet == SOCKET_ERROR)
        {
            OutputDebugString(_T("服务Socket监听失败"));
            break;
        }

        // 创建IOCP
        m_stIOCPThreadAddtionData.pClientManager_ = &m_ClientManager;
        BOOL bRet = m_IOCP.Create(&m_stIOCPThreadAddtionData);
        if (!bRet)
        {
            OutputDebugString(_T("IOCP创建失败"));
            break;
        }

        //**********************************************************
        //* ALARM * Those memory will free when this view destroy.
        //**********************************************************
        // Start thread and wait for accept.
        m_pstAcceptThreadParam = new ACCEPTTHREADPARAM;
        m_pstAcceptThreadParam->pClientManager_ = &m_ClientManager;
        m_pstAcceptThreadParam->pIOCP_ = &m_IOCP;
        m_pstAcceptThreadParam->sctAcceptSocket_ = m_sctAccept;
        m_pstAcceptThreadParam->pHostListView_ = this;

        m_pthdAcceptThread = new CAcceptThread();
        m_pthdAcceptThread->StartThread(m_pstAcceptThreadParam);

        // Flush log.
        if (NULL != pLogListView)
        {
            pLogListView->InsertLogInfo(0, _T("监控服务成功启动."));
        }

        // TODO: 开启心跳检测定时器

        // End
        return;
    } while (FALSE);

    // 异常处理
    if (m_sctAccept != INVALID_SOCKET)
    {
        closesocket(m_sctAccept);
        m_sctAccept = INVALID_SOCKET;
    }
}


void CHostListView::PostNcDestroy()
{
    // TODO: 在此添加专用代码和/或调用基类
    OnMnClose();

    // 释放Accept线程参数
    if (m_pstAcceptThreadParam != NULL)
    {
        delete m_pstAcceptThreadParam;
        m_pstAcceptThreadParam = NULL;
    }

    // 释放Accept线程
    if (m_pthdAcceptThread != NULL)
    {
        delete m_pthdAcceptThread;
        m_pthdAcceptThread = NULL;
    }

    
    CListView::PostNcDestroy();
}


CView *CHostListView::GetTargetView(const CRuntimeClass *pClass)
{
    CTeleControlDoc *pDoc = (CTeleControlDoc *)GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
    {
        return NULL;
    }

    POSITION Pos = pDoc->GetFirstViewPosition();
    CView *pView = NULL;
    while (Pos != NULL)
    {
        pView = pDoc->GetNextView(Pos);
        if (pView->IsKindOf(pClass))
        {
            break;
        }
    }

    return pView;
}

// Deal with WM_HASINFOTOFLUSH information, the interface should to update.
afx_msg LRESULT CHostListView::OnHasinfotoflush(WPARAM wParam, LPARAM lParam)
{
    INFOFLUSHTYPE eInfoFlushType = (INFOFLUSHTYPE)wParam;
    
    switch (eInfoFlushType)
    {
        case IFT_ACCEPTCLIENT:
        {
            PCLIENTINFO pClientInfo = (PCLIENTINFO)lParam;
            CListCtrl &lstHostList = GetListCtrl();

            CString csClientIP;
            CString csClientPort;
            csClientPort.Format(
                _T("%d"),
                ntohs(pClientInfo->stClientAddrInfo_.sin_port));

            USES_CONVERSION;
            csClientIP = A2T(
                inet_ntoa(pClientInfo->stClientAddrInfo_.sin_addr));

            lstHostList.InsertItem(0, csClientIP);
            BOOL bRet = lstHostList.SetItemText(0, HLCT_PORT, csClientPort);
            if (!bRet)
            {
                // TODO: deal with SetItemText.
            }

            break;
        }
        case IFT_SERVERCLOSE:
        {
            // TODO: Deal with server stop.
            break;
        }
        case IFT_SERVERSTART:
        {
            // TODO: Deal with server start.
            break;
        }
        default:
        {
            break;
        }
    }
    return 0;
} //! CHostListView::OnHasinfotoflush END


void CHostListView::OnMnClose()
{
    //shutdown(m_sctAccept, SD_BOTH);
    if (shutdown(m_sctAccept, SD_SEND))
    {
        int iError = WSAGetLastError();
    }
    closesocket(m_sctAccept);

    m_IOCP.Destroy();
}


void CHostListView::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    CListCtrl &lstHostList = GetListCtrl();

    CMenu aMenu[2];
    aMenu[0].LoadMenu(IDR_MENU_HOSTLIST_RBTNDOWN_SELECTED);

    CPoint Point;
    GetCursorPos(&Point);

    // 转换坐标
    lstHostList.ScreenToClient(&Point);
    UINT uiFlags = 0;
    CMenu *pSubMenu = NULL;
    int iIndex = lstHostList.HitTest(Point, &uiFlags);
    if (iIndex != -1 &&
        uiFlags & LVHT_ONITEMLABEL)
    {
        pSubMenu = aMenu[0].GetSubMenu(0);
    }
    else
    {
        return;
    }

    // 将坐标从客户区转到屏幕
    lstHostList.ClientToScreen(&Point);
    // 弹出菜单
    pSubMenu->TrackPopupMenu(TPM_RIGHTBUTTON, Point.x, Point.y, this);

    *pResult = 0;
}


void CHostListView::OnMnOperationFiletransfer()
{
    // TODO: 在此添加命令处理程序代码
    BOOL bHasError = FALSE;

    CListCtrl &lstHostList = GetListCtrl();
    // 获取选中的的主机的IP和端口
    CString csIPAndPort;
    int iIndex = lstHostList.GetSelectionMark();
    csIPAndPort = lstHostList.GetItemText(iIndex, HLCT_IPADDR);
    csIPAndPort += _T(':');
    csIPAndPort += lstHostList.GetItemText(iIndex, HLCT_PORT);

    PCLIENTINFO pClientInfo = NULL;

    do
    {
        // 在ClientManager中查找，找到对应Socket和上下文信息
        SOCKET sctTargetClient = m_ClientManager.GetSocket(csIPAndPort);
        if (sctTargetClient == INVALID_SOCKET)
        {
            OutputDebugString(_T("未找到对应socket\r\n"));
            break;
        }

        pClientInfo = m_ClientManager.GetClient(sctTargetClient);
        if (pClientInfo == NULL)
        {
            OutputDebugString(_T("根据SOCKET未找到ClientInfo\r\n"));
            break;
        }

        if (pClientInfo->pFileTransferDlg_ == NULL)
        {
            // This memory will free when dailog is close.
            pClientInfo->pFileTransferDlg_ =
                new CFileTransferDlg(csIPAndPort,
                                     pClientInfo,
                                     m_IOCP,
                                     this);

            if (pClientInfo->pFileTransferDlg_ == NULL)
            {
                OutputDebugString(_T("FileTransferDlg 申请空间失败\r\n"));
                bHasError = TRUE;
                break;
            }

            BOOL bRet = 
                pClientInfo->pFileTransferDlg_->Create(IDD_FILETRANSFER,
                                                       this);
            if (!bRet)
            {
                OutputDebugString(_T("文件传输对话框创建失败\r\n"));
                bHasError = TRUE;
                break;
            }
        }

        pClientInfo->pFileTransferDlg_->ShowWindow(SW_SHOW);

    } while (FALSE);

    if (bHasError)
    {
        if (pClientInfo->pFileTransferDlg_ != NULL)
        {
            delete pClientInfo->pFileTransferDlg_;
            pClientInfo->pFileTransferDlg_ = NULL;
        }
    }
} //! CHostListView::OnMnOperationFiletransfer END


void CHostListView::OnMnCmd()
{
    CListCtrl &lstHostList = GetListCtrl();

    BOOL bHasError = FALSE;

    // Get IP and port of host had selected.
    CString csIPAndPort;
    int iIndex = lstHostList.GetSelectionMark();
    csIPAndPort = lstHostList.GetItemText(iIndex, HLCT_IPADDR);
    csIPAndPort += _T(':');
    csIPAndPort += lstHostList.GetItemText(iIndex, HLCT_PORT);

    PCLIENTINFO pClientInfo = NULL;

    do
    {
        // 在ClientManager中查找，找到对应Socket和上下文信息
        SOCKET sctTargetClient = m_ClientManager.GetSocket(csIPAndPort);
        if (sctTargetClient == INVALID_SOCKET)
        {
            OutputDebugString(_T("未找到对应socket\r\n"));
            break;
        }

        pClientInfo = m_ClientManager.GetClient(sctTargetClient);
        if (pClientInfo == NULL)
        {
            OutputDebugString(_T("根据SOCKET未找到ClientInfo\r\n"));
            break;
        }

        if (pClientInfo->pCmdDlg_ == NULL)
        {
            pClientInfo->pCmdDlg_ = new CCmdDlg(csIPAndPort,
                                                pClientInfo,
                                                m_IOCP);

            if (pClientInfo->pCmdDlg_ == NULL)
            {
                OutputDebugString(_T("CMDDlg 申请空间失败\r\n"));
                bHasError = TRUE;
                break;
            }

            BOOL bRet = 
                pClientInfo->pCmdDlg_->Create(IDD_CMD, this);
            if (!bRet)
            {
                OutputDebugString(_T("CMD对话框创建失败\r\n"));
                bHasError = TRUE;
                break;
            }
        }

        pClientInfo->pCmdDlg_->ShowWindow(SW_SHOW);

    } while (FALSE);

    if (bHasError)
    {
        if (pClientInfo->pCmdDlg_ != NULL)
        {
            delete pClientInfo->pCmdDlg_;
            pClientInfo->pCmdDlg_ = NULL;
        }
    }
}


void CHostListView::OnMnProcessmanager()
{
    // TODO: 在此添加命令处理程序代码
     BOOL bHasError = FALSE;

    CListCtrl &lstHostList = GetListCtrl();
    // 获取选中的的主机的IP和端口
    CString csIPAndPort;
    int iIndex = lstHostList.GetSelectionMark();
    csIPAndPort = lstHostList.GetItemText(iIndex, HLCT_IPADDR);
    csIPAndPort += _T(':');
    csIPAndPort += lstHostList.GetItemText(iIndex, HLCT_PORT);

    PCLIENTINFO pClientInfo = NULL;

    do
    {
        // 在ClientManager中查找，找到对应Socket和上下文信息
        SOCKET sctTargetClient = m_ClientManager.GetSocket(csIPAndPort);
        if (sctTargetClient == INVALID_SOCKET)
        {
            OutputDebugString(_T("未找到对应socket\r\n"));
            break;
        }

        pClientInfo = m_ClientManager.GetClient(sctTargetClient);
        if (pClientInfo == NULL)
        {
            OutputDebugString(_T("根据SOCKET未找到ClientInfo\r\n"));
            break;
        }

        if (pClientInfo->pProcessManagerDlg_ == NULL)
        {
            pClientInfo->pProcessManagerDlg_ =
                new CProcessManagerDlg(csIPAndPort,
                                       pClientInfo,
                                       m_IOCP);

            if (pClientInfo->pProcessManagerDlg_ == NULL)
            {
                OutputDebugString(_T("CMDDlg 申请空间失败\r\n"));
                bHasError = TRUE;
                break;
            }

            BOOL bRet = 
                pClientInfo->pProcessManagerDlg_->Create(IDD_CMD, this);
            if (!bRet)
            {
                OutputDebugString(_T("CMD对话框创建失败\r\n"));
                bHasError = TRUE;
                break;
            }
        }

        pClientInfo->pProcessManagerDlg_->ShowWindow(SW_SHOW);

    } while (FALSE);

    if (bHasError)
    {
        if (pClientInfo->pProcessManagerDlg_ != NULL)
        {
            delete pClientInfo->pProcessManagerDlg_;
            pClientInfo->pProcessManagerDlg_ = NULL;
        }
    }
}


void CHostListView::OnMnScreenshower()
{
    // TODO: 在此添加命令处理程序代码
     BOOL bHasError = FALSE;

    CListCtrl &lstHostList = GetListCtrl();
    // 获取选中的的主机的IP和端口
    CString csIPAndPort;
    int iIndex = lstHostList.GetSelectionMark();
    csIPAndPort = lstHostList.GetItemText(iIndex, HLCT_IPADDR);
    csIPAndPort += _T(':');
    csIPAndPort += lstHostList.GetItemText(iIndex, HLCT_PORT);

    PCLIENTINFO pClientInfo = NULL;

    do
    {
        // 在ClientManager中查找，找到对应Socket和上下文信息
        SOCKET sctTargetClient = m_ClientManager.GetSocket(csIPAndPort);
        if (sctTargetClient == INVALID_SOCKET)
        {
            OutputDebugString(_T("未找到对应socket\r\n"));
            break;
        }

        pClientInfo = m_ClientManager.GetClient(sctTargetClient);
        if (pClientInfo == NULL)
        {
            OutputDebugString(_T("根据SOCKET未找到ClientInfo\r\n"));
            break;
        }

        if (pClientInfo->pScreenShowerDlg_ == NULL)
        {
            pClientInfo->pScreenShowerDlg_ = 
                new CScreenShowerDlg(csIPAndPort,
                                     pClientInfo,
                                     m_IOCP);

            if (pClientInfo->pScreenShowerDlg_ == NULL)
            {
                OutputDebugString(_T("CMDDlg 申请空间失败\r\n"));
                bHasError = TRUE;
                break;
            }

            BOOL bRet = 
                pClientInfo->pScreenShowerDlg_->Create(IDD_CMD, this);
            if (!bRet)
            {
                OutputDebugString(_T("CMD对话框创建失败\r\n"));
                bHasError = TRUE;
                break;
            }
        }

        pClientInfo->pScreenShowerDlg_->ShowWindow(SW_SHOW);

    } while (FALSE);

    if (bHasError)
    {
        if (pClientInfo->pScreenShowerDlg_ != NULL)
        {
            delete pClientInfo->pScreenShowerDlg_;
            pClientInfo->pScreenShowerDlg_ = NULL;
        }
    }
} //! CHostListView::OnMnScreenshower END
