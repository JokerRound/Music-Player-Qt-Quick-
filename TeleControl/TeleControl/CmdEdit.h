#pragma once


// CCmdEdit

class CCmdEdit : public CEdit
{
    DECLARE_DYNAMIC(CCmdEdit)
private:
    // 缓冲区
    CString m_csScreen;
    // cmd句柄
    HANDLE m_hCmd = INVALID_HANDLE_VALUE;

    // WSADATA;
    WSADATA m_stWsaData = { 0 };

public:
    CCmdEdit();
    virtual ~CCmdEdit();

protected:
    DECLARE_MESSAGE_MAP()
public:
    // 退出标识
    BOOL m_bIsQuit = FALSE;
    // 写入标识
    BOOL m_bHasDataToWrite = FALSE;
    // 用户输入
    CString m_csUserInput;
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


