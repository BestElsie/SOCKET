
// SeverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sever.h"
#include "SeverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString g_strIP;
void CString2Char(CString strSrc, char chDst[]);
UINT  serverThread(LPVOID lpParam);

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


// CSeverDlg dialog




CSeverDlg::CSeverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSeverDlg::IDD, pParent)
	, m_strRecv(_T(""))
	, m_strSend(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSeverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RECEIVE, m_strRecv);
	DDX_Text(pDX, IDC_EDIT_SEND, m_strSend);
}

BEGIN_MESSAGE_MAP(CSeverDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSeverDlg::OnBnClickedButtonSend)
//	ON_EN_CHANGE(IDC_EDIT_SEND, &CSeverDlg::OnEnChangeEditSend)
ON_EN_KILLFOCUS(IDC_EDIT_SEND, &CSeverDlg::OnEnKillfocusEditSend)
END_MESSAGE_MAP()


// CSeverDlg message handlers

BOOL CSeverDlg::OnInitDialog()
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

	// TODO: Add extra initialization here'
	char	chHostName[128];
	HOSTENT *pHost;
	// 获得主机名
	gethostname(chHostName, 128);
	// 获得主机结构
	pHost = gethostbyname(chHostName);

	// 将一个十进制网络字节序转换为点分十进制IP格式的字符串
	g_strIP = inet_ntoa(*(in_addr*)pHost->h_addr);

	UpdateRcvInfo(_T("本服务器IP地址：\t") + g_strIP);

	//AfxBeginThread(serverThread, this);
	//CreateThread(NULL, 0, serverThread, this, 0, NULL);
//	AfxBeginThread(Test, NULL);
	serverThread(this);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSeverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSeverDlg::OnPaint()
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
HCURSOR CSeverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSeverDlg::UpdateRcvInfo(CString strInfo)
{
	m_strRecv += strInfo;
	UpdateData(FALSE);
}

UINT  serverThread(LPVOID lpParam)
{

	WSADATA		wsaData;
	WORD		wVersion;
	SOCKADDR_IN localAddr;
	SOCKADDR_IN	clientAddr;
	char		chMsg[1024];
	char		chIP[20];

	CSeverDlg *ServerDlg = (CSeverDlg*)lpParam;
	wVersion = MAKEWORD(2, 2);
	WSAStartup(wVersion, &wsaData);

	CString2Char(g_strIP, chIP);

	// 将一个点分十进制的IP转换成一个长整数型数
	localAddr.sin_addr.S_un.S_addr = inet_addr(chIP);
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(8888);

	// 创建套接字
	if ((ServerDlg->m_ListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		ServerDlg->UpdateRcvInfo(_T("\r\n创建监听失败"));
	}

	// 绑定套接字
	if (bind(ServerDlg->m_ListenSock, (struct sockaddr*)& localAddr, sizeof(SOCKADDR_IN)))
	{
		ServerDlg->UpdateRcvInfo(_T("\r\n绑定错误"));
	}

	listen(ServerDlg->m_ListenSock, 1);

	// 接收套接字
	int addrSize;
	if ((ServerDlg->m_socket = accept(ServerDlg->m_ListenSock, (struct sockaddr*)&clientAddr, &addrSize)) == INVALID_SOCKET)
	{
		ServerDlg->UpdateRcvInfo(_T("\r\naccept失败"));
	}
	else
	{
		CString strPort;
		strPort.Format(_T("%d"), int(ntohs(clientAddr.sin_port)));
		ServerDlg->UpdateRcvInfo(_T("\r\n已连接客户端：" + g_strIP+"端口："+strPort));
	}

	// 接收数据
	int res;
	while (1)
	{
		if (res = recv(ServerDlg->m_socket, chMsg, 1024, 0) == -1)
		{
			ServerDlg->UpdateRcvInfo(_T("\r\n失去客户端的连接"));
			break;
		}
		else
		{
			chMsg[res] = '\0';
			ServerDlg->UpdateRcvInfo(_T("\r\nclient:\t")+CString(chMsg));
		}
	}

	return 0;
}

void CString2Char(CString strSrc, char chDst[])
{
	int wLen;
	wLen = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, chDst, wLen, NULL, NULL);
}

// 发送消息
void CSeverDlg::OnBnClickedButtonSend()
{
	char chMsg[1024];
	CString2Char(m_strSend, chMsg);

	if (send(m_socket, chMsg, strlen(chMsg), 0) == SOCKET_ERROR)
	{
		this->UpdateRcvInfo(_T("\r\n发送失败"));
	}
	else if (m_strSend == "")
	{
		MessageBox(_T("请输入信息"));
	}
	else
	{
		this->UpdateRcvInfo(_T("\r\nserver:\t")+m_strSend);
	}
}

void CSeverDlg::OnEnKillfocusEditSend()
{
	UpdateData(FALSE);
}
