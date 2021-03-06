//******************************************************************************
// License:     MIT
// Author:      Hoffman
// Create Time: 2018-11-28
// Description: 
//      The achieve of class CFileTransferTaskListCtrl.
//
// Modify Log:
//      2018-11-28    Hoffman
//      Info: 
//******************************************************************************

#include "stdafx.h"
#include "TeleControl.h"
#include "FileTransferTaskListCtrl.h"
#include "FileTransferStruct.h"

// CFileTransforTaskListCtrl

IMPLEMENT_DYNAMIC(CFileTransferTaskListCtrl, CListCtrl)



CFileTransferTaskListCtrl::CFileTransferTaskListCtrl(
    CFileTransportManager &TransportTaskManagerObj,
    const CString acsTaskStatus[],
    CCommunicationIOCP &IOCP,
    PCLIENTINFO pstClientInfo)
    : m_ref_TransportTaskManager(TransportTaskManagerObj)
    , m_acsTaskStatus(acsTaskStatus)
    , m_ref_IOCP(IOCP)
    , m_pstClientInfo(pstClientInfo)
{
}

CFileTransferTaskListCtrl::~CFileTransferTaskListCtrl()
{
}


BEGIN_MESSAGE_MAP(CFileTransferTaskListCtrl, CListCtrl)
    ON_COMMAND(ID_TRANSPORT_MN_PAUSESELECTEDTASK, &CFileTransferTaskListCtrl::OnTransportMnPauseselectedtask)
    ON_COMMAND(ID_TRANSPORT_MN_STARTSELECTEDTASK, &CFileTransferTaskListCtrl::OnTransportMnStartselectedtask)
END_MESSAGE_MAP()



// CFileTransforTaskListCtrl 消息处理程序

void CFileTransferTaskListCtrl::OnTransportMnPauseselectedtask()
{
#ifdef DEBUG
    DWORD dwError = -1;
    CString csErrorMessage;
    DWORD dwLine = 0;
    BOOL bOutputErrMsg = FALSE;
#endif // DEBUG
    PFILETRANSPORTTASK pstTaskInfo = NULL;
    BOOL bRet = FALSE;

    do
    {
        POSITION posI = GetFirstSelectedItemPosition();
        // Has selected items.
        if (NULL != posI)
        {
            do
            {
                int iItemIndex = GetNextSelectedItem(posI);

                // Get task id.
                ULONG ulTaskId = GetItemData(iItemIndex);

                // Get task info by id.
                pstTaskInfo = m_ref_TransportTaskManager.GetTask(ulTaskId);
                if (NULL == pstTaskInfo)
                {
#ifdef DEBUG
                    OutputDebugStringWithInfo(
                        _T("The task info point is NULL.\r\n"),
                        __FILET__,
                        __LINE__);
#endif // DEBUG
                    continue;
                }

                // Change task status.
                pstTaskInfo->syncCriticalSection_.Lock();
                pstTaskInfo->eTaskStatus_ = FTS_PAUSE;
                pstTaskInfo->syncCriticalSection_.Unlock();

                // Change UI.
                bRet = SetItemText(iItemIndex,
                                   FTLC_TASKSTATUS,
                                   m_acsTaskStatus[FTS_PAUSE]);
                if (!bRet)
                {
#ifdef DEBUG
                    dwError = GetLastError();
                    GetErrorMessage(dwError, csErrorMessage);
                    OutputDebugStringWithInfo(csErrorMessage,
                                              __FILET__,
                                              __LINE__);
#endif // DEBUG 
                }

            } while (NULL != posI);
        } //! if "Has selected items" END 
    } while (FALSE);

} //! CFileTransferDlg::OnTransportMnPauseselectedtask() END


void CFileTransferTaskListCtrl::OnTransportMnStartselectedtask()
{
    #ifdef DEBUG
    DWORD dwError = -1;
    CString csErrorMessage;
    DWORD dwLine = 0;
    BOOL bOutputErrMsg = FALSE;
#endif // DEBUG
    PFILETRANSPORTTASK pstTaskInfo = NULL;
    BOOL bRet = FALSE;

    
    do
    {
        POSITION posI = GetFirstSelectedItemPosition();
        // Has selected items.
        if (NULL != posI)
        {
            do
            {
                int iItemIndex = GetNextSelectedItem(posI);

                // Get task id.
                ULONG ulTaskId = GetItemData(iItemIndex);

                // Get task info by id.
                pstTaskInfo = m_ref_TransportTaskManager.GetTask(ulTaskId);
                if (NULL == pstTaskInfo)
                {
#ifdef DEBUG
                    OutputDebugStringWithInfo(
                        _T("The task info point is NULL.\r\n"),
                        __FILET__,
                        __LINE__);
#endif // DEBUG
                    continue;
                }

                // Change task status.
                pstTaskInfo->syncCriticalSection_.Lock();
                pstTaskInfo->eTaskStatus_ = FTS_PENDING;
                pstTaskInfo->syncCriticalSection_.Unlock();

                // Change UI.
                bRet = SetItemText(iItemIndex,
                                   FTLC_TASKSTATUS,
                                   m_acsTaskStatus[FTS_PENDING]);
                if (!bRet)
                {
#ifdef DEBUG
                    dwError = GetLastError();
                    GetErrorMessage(dwError, csErrorMessage);
                    OutputDebugStringWithInfo(csErrorMessage,
                                              __FILET__,
                                              __LINE__);
#endif // DEBUG 
                }

            } while (NULL != posI);
        } //! if "Has selected items" END 

        
    } while (FALSE);
}
