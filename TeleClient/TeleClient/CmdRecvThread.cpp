#include "stdafx.h"
#include "TeleClientDlg.h"
#include "CmdRecvThread.h"


CCmdRecvThread::CCmdRecvThread()
{
}


CCmdRecvThread::~CCmdRecvThread()
{
}

bool CCmdRecvThread::OnThreadEventRun(LPVOID lpParam)
{
    CTeleClientDlg *pTeleClientDlg = (CTeleClientDlg *)lpParam;
    BOOL bRet = FALSE;

    while (TRUE)
    {
        // Get data of cmd from pipe.
        bRet = pTeleClientDlg->ReadDataFromCmd();
        if (!bRet)
        {
#ifdef DEBUG
            OutputDebugString(_T("��CMD�ж�ȡ����ʧ��\r\n"));
#endif // DEBUG
            // Connect failed.
            if (pTeleClientDlg->m_bCmdQuit)
            {
                // Close those pipe.


                break;
            }
        }
    }

    return true;
}
