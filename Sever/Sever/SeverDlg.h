
// SeverDlg.h : header file
//

#pragma once


// CSeverDlg dialog
class CSeverDlg : public CDialog
{
// Construction
public:
	CSeverDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SEVER_DIALOG };

	void UpdateRcvInfo(CString strInfo); // 更新接收到的信息

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	
private:
	HICON m_hIcon;

public:
	CString m_strRecv;
	CString m_strSend;
	SOCKET  m_ListenSock;
	SOCKET	m_socket;
	afx_msg void OnBnClickedButtonSend();
//	afx_msg void OnEnChangeEditSend();
	afx_msg void OnEnKillfocusEditSend();
};

