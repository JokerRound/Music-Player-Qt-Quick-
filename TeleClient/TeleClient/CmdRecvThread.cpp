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
        // �ӹܵ��н�������
        bRet = pTeleClientDlg->ReadDataFromCmd();
        if (!bRet)
        {
#ifdef DEBUG
            OutputDebugString(_T("��CMD�ж�ȡ����ʧ��\r\n"));
#endif // DEBUG
            if (pTeleClientDlg->m_bCmdQuit)
            {
                break;
            }
        }
    }

    return true;
}
