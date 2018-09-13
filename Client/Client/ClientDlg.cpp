
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CString2Char(CString strSrc, char chDst[]);
UINT recvThread(LPVOID p);
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CClientDlg dialog




CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent)
	, m_strSend(_T(""))
	, m_strRcv(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SEND, m_strSend);
	DDX_Text(pDX, IDC_EDIT_RCV, m_strRcv);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IP);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_btnConnect);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CClientDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CClientDlg::OnBnClickedButtonSend)
//	ON_EN_KILLFOCUS(IDC_EDIT_SEND, &CClientDlg::OnEnKillfocusEditSend)
ON_EN_KILLFOCUS(IDC_EDIT_SEND, &CClientDlg::OnEnKillfocusEditSend)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::UpdateRecvInfo(CString strInfo)
{
	m_strRcv += strInfo;
	UpdateData(FALSE);
}

void CClientDlg::OnBnClickedButtonConnect()
{
	WSADATA		wsaData;
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));

	WSAStartup(0x0202, &wsaData);

	if ((m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		UpdateRecvInfo(_T("\r\n创建SOCKET失败"));
		return;
	}

	BYTE nf1, nf2, nf3, nf4;
	m_IP.GetAddress(nf1, nf2, nf3, nf4);
	CString strIP;
	strIP.Format(_T("%d.%d.%d.%d"), nf1, nf2, nf3, nf4);

	char chIP[20];
	CString2Char(strIP, chIP);
	serverAddr.sin_addr.s_addr = inet_addr(chIP);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8888);

	bind(m_socket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR));
	if (connect(m_socket, (struct sockaddr*)& serverAddr, sizeof(SOCKADDR_IN))==SOCKET_ERROR)
	{
		UpdateRecvInfo(_T("\r\n连接失败\r\n"));
	}
	else
	{
		UpdateRecvInfo(_T("\r\n连接成功"));
		m_btnConnect.EnableWindow(FALSE);
		AfxBeginThread(recvThread, NULL);		
	}
}

UINT recvThread(LPVOID p)
{
	int res;
	char chMsg[1024];

	CClientDlg * dlg = (CClientDlg*)AfxGetApp()->GetMainWnd();

	// 接收数据
	while (1)
	{
		if ((res = recv(dlg->m_socket, chMsg, 1024, 0)) == -1)
		{
			dlg->UpdateRecvInfo(_T("\r\n失去连接"));
			closesocket(dlg->m_socket);
			return -1;
		}
		else
		{
			chMsg[res] = '\0';
			dlg->UpdateRecvInfo(_T("\r\nserver:\t")+CString(chMsg));
		}
	}

	return 0;
}

void CString2Char(CString strSrc, char chDst[])
{
	int wLength;
	wLength = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, chDst, wLength, NULL, NULL);
}
void CClientDlg::OnBnClickedButtonSend()
{
	char	chMsg[1024];


	CString2Char(m_strSend, chMsg);
	if ((send(m_socket, chMsg, strlen(chMsg), 0)) == SOCKET_ERROR)
	{
		UpdateRecvInfo(_T("\r\n发送失败"));
		closesocket(m_socket);
		return;
	}
	else if (m_strSend == "")
	{
		MessageBox(_T("请输入信息"));
	}
	else
	{
		UpdateRecvInfo(_T("\r\nclient:\t" + m_strSend));
		m_strSend = "";
		GetDlgItem(IDC_EDIT_SEND)->SetFocus();
	}
}

void CClientDlg::OnEnKillfocusEditSend()
{
	UpdateData(TRUE);
}
