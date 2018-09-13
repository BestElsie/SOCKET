
// ClientDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CClientDlg dialog
class CClientDlg : public CDialog
{
// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor
	void UpdateRecvInfo(CString strInfo);

// Dialog Data
	enum { IDD = IDD_CLIENT_DIALOG };

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strSend;
	CString m_strRcv;
	afx_msg void OnBnClickedButtonConnect();
	SOCKET m_socket;
	CIPAddressCtrl m_IP;
	CButton m_btnConnect;
	afx_msg void OnBnClickedButtonSend();
//	afx_msg void OnEnKillfocusEditSend();
	afx_msg void OnEnKillfocusEditSend();
};
