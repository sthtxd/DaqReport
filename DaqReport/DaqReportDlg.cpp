
// DaqReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DaqReport.h"
#include "DaqReportDlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

httplib::Client m_Client("s24209562d.51vip.biz",52818);
static char szCmd[] = { 0x03, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDaqReportDlg dialog



CDaqReportDlg::CDaqReportDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDaqReportDlg::IDD, pParent)
	, m_nPort(0)
	, m_sIp(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDaqReportDlg::~CDaqReportDlg()
{
	m_bExitThread = true;
	

	int nErr = WSACleanup();
	Sleep(1000);
	//mysql_free_result(m_result);
	//mysql_close(&m_Mysql);
	m_Log->Print("<---app close---\n");
	delete m_Log;
	m_Log = NULL;
}

void CDaqReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_IP_ADDR, m_IpAddr);
	//  DDX_Text(pDX, IDC_EDIT_PORT, m_sPort);
	//  DDX_IPAddress(pDX, IDC_IP_ADDR, m_IpAddr);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_IP, m_sIp);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_COMBO1, m_ComBox);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	//  DDX_Text(pDX, IDC_EDIT_UP, m_edtup);
	//  DDX_Text(pDX, IDC_EDIT_LOW, m_edtlow);
	//  DDX_Control(pDX, IDC_EDIT_LOW, m_edtHigh);
	DDX_Control(pDX, IDC_EDIT_UP, m_edtHigh);
	DDX_Control(pDX, IDC_EDIT_LOW, m_edtLow);
}

BEGIN_MESSAGE_MAP(CDaqReportDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LINK, &CDaqReportDlg::OnBtnLink)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CDaqReportDlg::OnBtnCancel)
	ON_BN_CLICKED(IDC_BTN_SEND, &CDaqReportDlg::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_INSERT, &CDaqReportDlg::OnBtnInsert)
	ON_BN_CLICKED(IDC_BTN_ERR, &CDaqReportDlg::OnBnClickedBtnErr)
	ON_BN_CLICKED(IDC_BTN_STATUE, &CDaqReportDlg::OnBnClickedBtnStatue)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CDaqReportDlg::OnNMDblclkList)
//	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST, &CDaqReportDlg::OnNMKillfocusList)
ON_CBN_KILLFOCUS(IDC_COMBO1, &CDaqReportDlg::OnCbnKillfocusCombo1)
ON_EN_KILLFOCUS(IDC_EDIT1, &CDaqReportDlg::OnEnKillfocusEdit1)
ON_CBN_SELCHANGE(IDC_COMBO1, &CDaqReportDlg::OnCbnSelchangeCombo1)
ON_BN_CLICKED(IDC_BTN_SAVE, &CDaqReportDlg::OnBnClickedBtnSave)
ON_WM_TIMER()
END_MESSAGE_MAP()


// CDaqReportDlg message handlers

BOOL CDaqReportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	//CMenu menu;
	//menu.LoadMenuW(IDR_MENU);
	//SetMenu(&menu);

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
	    return FALSE;
	}

	m_Log = new TEcLog;
	m_Log->Print("---app open--->\n");
	m_bExitThread = false;
	m_bSend = false;
	memset(m_bSignal, 0x00, sizeof(int)*INPUT_COUNT);

//	m_sIp = _T("192.168.10.250");
//	GetPrivateProfileString(_T("Internet"), _T("station01_IP"), _T("0.0.0.0"), m_sIp.GetBuffer(15), 15, _T("D:\\Project\\DaqReport\\DaqReport\\MySQL.ini"));
//	m_sIp.ReleaseBuffer();
//	m_nPort = 4196;
//	m_nPort = GetPrivateProfileInt(_T("Internet"), _T("station01_Port"),0, _T("D:\\Project\\DaqReport\\DaqReport\\MySQL.ini"));
	m_nTest = 1;
	m_nStatus = 0;
	m_fTemp = 40.0;
	UpdateData(FALSE);
	
	Init();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDaqReportDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDaqReportDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDaqReportDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDaqReportDlg::OnBtnLink()
{
	// TODO: Add your control notification handler code here
	//m_bStartSend = true;
	UpdateData(TRUE);
	//if (InitSocket())
	//{
	//	GetDlgItem(IDC_BTN_LINK)->EnableWindow(false);
	//	WritePrivateProfileString(_T("Internet"), _T("station01_IP"), m_sIp.GetBuffer(15), _T("D:\\Project\\DaqReport\\DaqReport\\MySQL.ini"));
	//	m_sIp.ReleaseBuffer();
	//	CString sPort;
	//	sPort.Format(_T("%d"),m_nPort);
	//	WritePrivateProfileString(_T("Internet"), _T("station01_Port"), sPort.GetBuffer(0), _T("D:\\Project\\DaqReport\\DaqReport\\MySQL.ini"));
	//}

}


void CDaqReportDlg::OnBtnCancel()
{
	// TODO: Add your control notification handler code here

	m_bExitThread = true;
	GetDlgItem(IDC_BTN_LINK)->EnableWindow(true);
}

bool CDaqReportDlg::InitSocket(DEVICE_IP* pDev)
{
	
	pDev->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pDev->socket == INVALID_SOCKET)
	{
		return FALSE;
	}
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(pDev->nPort);
	sockaddr.sin_addr.s_addr = inet_addr(pDev->szIpAddr);


	//m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		//tcp
	//if (m_Socket == INVALID_SOCKET)
	//{
	//	return FALSE;
	//}
	//sockaddr_in sockaddr;
	//sockaddr.sin_family = AF_INET;
	//sockaddr.sin_port = htons(m_nPort);
	//
	//int len = WideCharToMultiByte(CP_ACP, 0, m_sIp, m_sIp.GetLength(), NULL, 0, NULL, NULL);
	//char  pIpAddr[256];
	//memset(pIpAddr, 0x00, sizeof(pIpAddr));
	//WideCharToMultiByte(CP_ACP, 0, m_sIp, m_sIp.GetLength(), pIpAddr, len, NULL, NULL);
	//pIpAddr[len + 1] = '\0';
	//sockaddr.sin_addr.s_addr = inet_addr(pIpAddr);


	//sockaddr.sin_addr.s_addr = inet_addr(m_sIp.GetBuffer());
	//m_sIp.ReleaseBuffer();
	int nError;
	if (nError = connect(pDev->socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		nError = GetLastError();
		return false;
	}

	int nNetTimeout = 5000; //5秒
	//发送时限
	setsockopt(pDev->socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(int));
	//接收时限
	setsockopt(pDev->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));

	UINT nThreadID;
	HANDLE pThread;
	if (pDev->nDevNo<9)
		pThread = (HANDLE)_beginthreadex(NULL, 0, SendRecvThread, pDev, 0, &nThreadID);
	else
		pThread = (HANDLE)_beginthreadex(NULL, 0, MrZhouThread, pDev, 0, &nThreadID);

	if (pThread == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	if (!SetThreadPriority(pThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	CloseHandle(pThread);
//	m_bExitThread = false;
	return true;
}

UINT WINAPI CDaqReportDlg::SendRecvThread(void* pParam)
{
//	CDaqReportDlg* pMain = reinterpret_cast<CDaqReportDlg*>(pParam);
//	TEcLog *Log = new TEcLog(true, 2048, "D:\\Project\\ZLanLog.txt");
	DEVICE_IP* pDev = (DEVICE_IP*)pParam;
	char bufRecv[32];
	char bufSend[32];
	memset(bufSend, 0x00, sizeof(bufSend));
	//读取采集模块的DI输入信号的命令
	//指令返回：	01 01 01 0f 11 8c 返回的第4个字节，
	//这里是0x0f就表示8路DI的输入情况。
	//从右至左，分别表示DI1~DI8的状态。1表示有输入，0表示无输入。
	bufSend[0] = 0x01;		
	bufSend[1] = 0x01;		
	bufSend[2] = 0x00;		
	bufSend[3] = 0x00;		
	bufSend[4] = 0x00;		
	bufSend[5] = 0x08;		
	bufSend[6] = 0x3d;		
	bufSend[7] = 0xcc;		 
	
	//读取采集模块的AI信号的命令
	//发送-> 01 04 00 00 00 08 f1 cc
	//返回-> 01 04 10 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 b4 55 ab
	char bufTempSend[32];
	memset(bufTempSend, 0x00, sizeof(bufTempSend));
	bufTempSend[0] = 0x01;
	bufTempSend[1] = 0x04;
	bufTempSend[2] = 0x00;
	bufTempSend[3] = 0x00;
	bufTempSend[4] = 0x00;
	bufTempSend[5] = 0x08;
	bufTempSend[6] = 0xf1;
	bufTempSend[7] = 0xcc;

	int nNo=0;

	//httplib::Client Client("zcmesdata.com");
	httplib::Client Client("s24209562d.51vip.biz",52818);
	
	Client.set_timeout_sec(5);
	Client.set_read_timeout(5);
	Client.set_write_timeout(5);
	
	char cFormat[32];
	string str; 
	
	int m_bSignal[8];
	int nCircleTime = 0;
	int nAiData = 0;
	
	time_t curr_t;
	struct tm tm;
	char s[100];
	int temp=600;
	while (1)
	{
		nNo = send(pDev->socket, bufSend, 32, 0);
		
		if (nNo <= 0)
		{
				
			break;
		}
		memset(bufRecv, 0x00, sizeof(bufRecv));
		nNo = recv(pDev->socket, bufRecv, 32, 0);
		if (nNo <= 0)
		{
			break;
		}
			
		
		int bStatus = 0;
		unsigned char nData = static_cast<unsigned int>(bufRecv[3]);
		for (int i = 0; i < 8; i++)
		{
			bStatus = (nData >> i) & 0x1;
			if (bStatus != m_bSignal[i])
			{
				m_bSignal[i] = bStatus;
				switch (i)
				{
				//DI1：自动,DI2：产量,DI3~DI8备用
				case 0:		//device status  0：close ;1: working
				{
					memset(cFormat, 0x00, sizeof(cFormat));
					sprintf_s(cFormat, "{\"holdingFurnaceId\": %d,\"status\": %d,\"currentTemperature\":%d}", pDev->nDevNo, bStatus, temp);
					str = cFormat;
					auto res = Client.Post("/api/collectHoldingFurnace", str, "application/json");
					
					if (res && res->status == 200)
					{
						str = res->body;
					}
					break;
				}
				
				case 1:		//property KEEP WARM	(3)
					if (bStatus&&m_bSignal[0])
					{
						memset(cFormat, 0x00, sizeof(cFormat));
						sprintf_s(cFormat, "{\"device_no\": %d,\"quality\": %d}", pDev->nDevNo,  1);
						str = cFormat;
						auto res = Client.Post("/api/home/device/product", str, "application/json");
						if (res && res->status == 200)
						{
							str = res->body;
						}

					}
					//time(&curr_t);
					//tm = *localtime(&curr_t);
					//strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
					//Log->Print("%s in1=%02x in2=%02x out1=%02x out2=%02x",
					//	s, nData, 0, 0, 0);
					//break;
				
				case 4:		//device status 1: repairing
					//if (bStatus)
					//{
					//	memset(cFormat, 0x00, sizeof(cFormat));
					//	sprintf_s(cFormat, "{\"device_no\": %d,\"status\": %d}", pDev->nDevNo, 2);
					//	str = cFormat;
					//	auto res = Client.Post("/api/home/device/change_status", str, "application/json");
					//	if (res && res->status == 200)
					//	{
					//		str = res->body;
					//	}
					//	break;
					//}
				case 5:		//error code
					//{
					//	memset(cFormat, 0x00, sizeof(cFormat));
					//	sprintf_s(cFormat, "{\"device_no\": %d,\"code\": %d}", pDev->nDevNo, 2);
					//	str = cFormat;
					//	auto res = m_Client.Post("/api/home/device/error_code", str, "application/json");
					//	if (res && res->status == 200)
					//	{
					//		str = res->body;
					//	}
					//}
				case 6:
					break;
				}
			}
		}

		//nNo = send(pDev->socket, bufTempSend, 32, 0);

		//if (nNo <= 0)
		//{
		//	break;
		//}
		//memset(bufRecv, 0x00, sizeof(bufRecv));
		//nNo = recv(pDev->socket, bufRecv, 32, 0);
		//if (nNo <= 0)
		//{
		//	break;
		//}

		//auto nTempData = (bufTempSend[17] & 0xf0) * 4096 + (bufTempSend[17]&0x0f) * 256 + 
		//		(bufTempSend[18] & 0xf0) * 16 + bufTempSend[18]&0x0f;

		//if (abs(nAiData - nTempData)<1)
		//{
		//	nAiData = nTempData;
		//	memset(cFormat, 0x00, sizeof(cFormat));
		//	sprintf_s(cFormat, "{\"device_no\": %d,\"temperature\": %d}", pDev->nDevNo, nAiData/1000);
		//	str = cFormat;
		//	auto res = m_Client.Post("/api/home/device/change_temperature", str, "application/json");
		//	if (res && res->status == 200)
		//	{
		//		str = res->body; 
		//	}

		//}
		Sleep(100);

	}
	closesocket(pDev->socket);
	pDev->bIsConnected = false;
	
	return 0;
}

UINT WINAPI CDaqReportDlg::MrZhouThread(void* pParam)
{
	DEVICE_IP* pDev = (DEVICE_IP*)pParam;
//	TEcLog* Log = new TEcLog(true, 2048, "D:\\Project\\TemperLog.txt");
	httplib::Client Client("zcmesdata.com");
	Client.set_timeout_sec(5);
	Client.set_read_timeout(5);
	Client.set_write_timeout(5);

	uint64_t cur_timestamp;
	uint64_t last_timestamp;
	time_t curr_t;
	time_t daqtime;
	struct tm tm;
	char s[100];
	char szRecv[2048];
	unsigned char s2us[4];
	memset(szRecv, 0x0, sizeof(szRecv));
	char cFormat[64];
	int nNo;
	char sCmd[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x01, 0x00, 0x00, 0x02 };
	int nLastTemp=0;
	string str;
	bool bTemphigh,bTemplow,bTempOK = false;
	while (1)
	{

		nNo = send(pDev->socket, sCmd, sizeof(sCmd), 0);
		if (nNo <= 0)
		{
			break;
		}
		memset(szRecv, 0x00, sizeof(szRecv));
		nNo = recv(pDev->socket, szRecv, 256, 0);
		if (nNo <= 0)
		{
			break;
		}
		auto nCurrTemp = (static_cast<unsigned char>(szRecv[9])*256+static_cast<unsigned char>(szRecv[10]));
		if (nCurrTemp == 3276)
			nCurrTemp = 0;
		if (nCurrTemp > HIGH_TEMP&& !bTemphigh)
		{
			bTemphigh = true;
			bTempOK = false;
			memset(cFormat, 0x00, sizeof(cFormat));
			sprintf_s(cFormat, "{\"device_no\": %d,\"code\": %d}", pDev->nDevNo-8, 1);
			str = cFormat;
			auto res = Client.Post("/api/home/device/error_code", str, "application/json");
			if (res && res->status == 200)
			{
				str = res->body;
			}
		}
		else if (nCurrTemp < LOW_TEMP&&!bTemplow)
		{
			bTemplow = true;
			bTempOK = false;
			memset(cFormat, 0x00, sizeof(cFormat));
			sprintf_s(cFormat, "{\"device_no\": %d,\"code\": %d}", pDev->nDevNo-8, 2);
			str = cFormat;
			auto res = Client.Post("/api/home/device/error_code", str, "application/json");
			if (res && res->status == 200)
			{
				str = res->body;
			}
					
		}
		 if (nCurrTemp<HIGH_TEMP && nCurrTemp>LOW_TEMP&& !bTempOK)
		{
			bTemphigh = false;
			bTemplow = false;
			bTempOK = true;
			memset(cFormat, 0x00, sizeof(cFormat));
			sprintf_s(cFormat, "{\"device_no\": %d,\"code\": %d}", pDev->nDevNo-8, 0);
			str = cFormat;
			auto res = Client.Post("/api/home/device/error_code", str, "application/json");
			if (res && res->status == 200)
			{
				str = res->body;
			}
		}
		if (abs(nLastTemp - nCurrTemp) > 1)
		{
			nLastTemp = nCurrTemp;
			memset(cFormat, 0x00, sizeof(cFormat));
			sprintf_s(cFormat, "{\"device_no\": %d,\"temperature\": %d}", pDev->nDevNo-8, nCurrTemp);
			str = cFormat;
			auto res = Client.Post("/api/home/device/change_temperature", str, "application/json");
			if (res && res->status == 200)
			{
				str = res->body;
			}
		}


		//慧泉采集盒
		//nNo = send(pDev->socket, szCmd, 9, 0);
		//if (nNo <= 0)
		//{
		//	break;
		//}
		//memset(szRecv, 0x00, sizeof(szRecv));
		//nNo = recv(pDev->socket, szRecv, 2048, 0);
		//if (nNo <= 0)
		//{
		//	break;
		//}
		//int nCount = nNo / 16;

		////for (int n = 0; n < nCount; n++)
		////{
		////	Log->Print("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
		////		static_cast<unsigned char>(szRecv[n * 16]), static_cast<unsigned char>(szRecv[n * 16 + 1])
		////		, static_cast<unsigned char>(szRecv[n * 16 + 2]), static_cast<unsigned char>(szRecv[n * 16 + 3])
		////		, static_cast<unsigned char>(szRecv[n * 16 + 4]), static_cast<unsigned char>(szRecv[n * 16 + 5])
		////		, static_cast<unsigned char>(szRecv[n * 16 + 6]), static_cast<unsigned char>(szRecv[n * 16 + 7])
		////		, static_cast<unsigned char>(szRecv[n * 16 + 8]), static_cast<unsigned char>(szRecv[n * 16 + 9])
		////		, static_cast<unsigned char>(szRecv[n * 16 + 10]), static_cast<unsigned char>(szRecv[n * 16 + 11])
		////		, static_cast<unsigned char>(szRecv[n * 16 + 12]), static_cast<unsigned char>(szRecv[n * 16 + 13])
		////		, static_cast<unsigned char>(szRecv[n * 16 + 14]), static_cast<unsigned char>(szRecv[n * 16 + 15]));

		////}

		////数据有变化
		//if (nCount > 1)
		//{
		//	Log->Print("%02x %02x %02x %02x %02x %02x %02x %02x %02x",
		//		static_cast<unsigned char>(szCmd[0]), static_cast<unsigned char>(szCmd[1])
		//		, static_cast<unsigned char>(szCmd[2]), static_cast<unsigned char>(szCmd[3])
		//		, static_cast<unsigned char>(szCmd[4]), static_cast<unsigned char>(szCmd[5])
		//		, static_cast<unsigned char>(szCmd[6]), static_cast<unsigned char>(szCmd[7])
		//		, static_cast<unsigned char>(szCmd[8]));
		//	time(&curr_t);
		//	memcpy(&cur_timestamp, &szRecv[0], 8);
		//	
		//	for (int i = 0; i < nCount; i++)
		//	{
		//		memcpy(&last_timestamp, &szRecv[16*i], 8);
		//		daqtime = curr_t + (time_t)(last_timestamp - cur_timestamp) / 1000000;
		//		tm = *localtime(&daqtime);
		//		strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
		//		//s2us[0] = static_cast<unsigned char>(szRecv[16 * i + 8]);
		//		//s2us[1] = static_cast<unsigned char>(szRecv[16 * i + 9]);
		//		//s2us[2] = static_cast<unsigned char>(szRecv[16 * i + 10]);
		//		//s2us[3] = static_cast<unsigned char>(szRecv[16 * i + 11]);
		//		//Log->Print("%s in1=%02x in2=%02x out1=%02x out2=%02x",
		//		//	s,s2us[0], s2us[1], s2us[2], s2us[3]);
		//		Log->Print("%s %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
		//			s,static_cast<unsigned char>(szRecv[i * 16]), static_cast<unsigned char>(szRecv[i * 16 + 1])
		//			, static_cast<unsigned char>(szRecv[i * 16 + 2]), static_cast<unsigned char>(szRecv[i * 16 + 3])
		//			, static_cast<unsigned char>(szRecv[i * 16 + 4]), static_cast<unsigned char>(szRecv[i * 16 + 5])
		//			, static_cast<unsigned char>(szRecv[i * 16 + 6]), static_cast<unsigned char>(szRecv[i * 16 + 7])
		//			, static_cast<unsigned char>(szRecv[i * 16 + 8]), static_cast<unsigned char>(szRecv[i * 16 + 9])
		//			, static_cast<unsigned char>(szRecv[i * 16 + 10]), static_cast<unsigned char>(szRecv[i * 16 + 11])
		//			, static_cast<unsigned char>(szRecv[i * 16 + 12]), static_cast<unsigned char>(szRecv[i * 16 + 13])
		//			, static_cast<unsigned char>(szRecv[i * 16 + 14]), static_cast<unsigned char>(szRecv[i * 16 + 15]));

		//	}
		//	memcpy(&szCmd[1], &szRecv[16 * (nCount-1)], 8);
		//}
		////数据无变化
		//else
		//{
		//	memcpy(&szCmd[1], &szRecv[8], 8);
		//}
			
		
		Sleep(100);
	}
	pDev->bIsConnected = false;
	closesocket(pDev->socket);
//	delete Log;
	return 0;
}

bool CDaqReportDlg::Init()
{
	//mysql_init(&m_Mysql);

	////localhost:服务器地址，可以直接填入IP;root:账号;  
	////123:密码;test:数据库名;3306:网络端口   

	////if (!mysql_real_connect(&m_Mysql, "127.0.0.1", "root",
	////	"zhongchuang2018", "io_data", 3306, NULL, 0))
	//for (int i = 0; i < 3; i++)
	//{
	//	if (!mysql_real_connect(&m_Mysql, "211.149.230.180", "jlw_test",
	//		"U^Q8R9xDz-", "jlw_test", 3306, NULL, 0))
	//	{
	//		m_Log->Print("Connect to SQL failed (%s) times:=%d\n", mysql_error(&m_Mysql),i+1);
	//		Sleep(500);
	//		continue;
	//	}
	//	else
	//	{
	//		m_Log->Print("Connected ... \n");
	//		return true;
	//	}
	//}
	m_List.InsertColumn(0, TEXT("站点"));
	m_List.InsertColumn(1, TEXT("IP地址"));
	m_List.InsertColumn(2, TEXT("端口号"));
	m_List.InsertColumn(3, TEXT("当前设定"));
	m_List.InsertColumn(4, TEXT("连接状态"));
	m_List.SetColumnWidth(0, 100);
	m_List.SetColumnWidth(1, 200);
	m_List.SetColumnWidth(2, 100);
	m_List.SetColumnWidth(3, 100);
	m_List.SetColumnWidth(4, 100);
	
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EDITLABELS | LVS_EX_FULLROWSELECT);
	CString sName;
	for (int i = 0; i < 16; i++)
	{
		sName.Format(TEXT("%d号站点"),i+1);
		m_List.InsertItem(i, sName);
	}
	CImageList imageList;
	imageList.Create(1, 28, TRUE | ILC_COLOR32, 1, 0);
	m_List.SetImageList(&imageList, LVSIL_SMALL);
	CFont cFont;
	cFont.CreatePointFont(50, TEXT("新宋体"));
	m_List.SetFont(&cFont);
	m_ComBox.ResetContent();
	m_ComBox.InsertString(0,_T("停止"));
	m_ComBox.InsertString(1,_T("启用"));

	ReadPara();


	//memset(dev_IP,0x00, sizeof(DEVICE_IP)*MAX_CONNECT_COUNT);
	//dev_IP[0].bIsConnected = false;
	//dev_IP[0].nDevNo = 1;
	//dev_IP[0].bIsEnable = false;
	//dev_IP[0].nPort = 4196;
	//memcpy(dev_IP[0].szIpAddr,"192.168.10.200",sizeof(char)*16);
	//dev_IP[1].bIsConnected = false;
	//dev_IP[1].nDevNo = 2;
	//dev_IP[1].bIsEnable = true;
	//dev_IP[1].nPort = 4196;
	//memcpy(dev_IP[1].szIpAddr, "192.168.0.250", sizeof(char) * 16);
	//dev_IP[2].bIsConnected = false;
	//dev_IP[2].nDevNo = 3;
	//dev_IP[2].bIsEnable = false;
	//dev_IP[2].nPort = 4196;
	//memcpy(dev_IP[2].szIpAddr, "192.168.10.201", sizeof(char) * 16);

	//dev_IP[3].bIsConnected = false;
	//dev_IP[3].nDevNo = 20;
	//dev_IP[3].bIsEnable = true;
	//dev_IP[3].nPort = 3333;
	//memcpy(dev_IP[3].szIpAddr, "192.168.0.115", sizeof(char) * 16);
	UINT nThreadID;
	HANDLE pThread = (HANDLE)_beginthreadex(NULL, 0, MutiConnect, this, 0, &nThreadID);
	if (pThread == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	if (!SetThreadPriority(pThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}
	CloseHandle(pThread);

	SetTimer(1,2000,NULL);
	return true;
}

void CDaqReportDlg::InsertProductTable(int a, int nDevcNo)
{
	TCHAR sOrder[16];
	CString csOrder;
	csOrder.Format(_T("Order%d"), nDevcNo);
	GetPrivateProfileString(_T("MySql"), csOrder, _T("00000000"), sOrder, sizeof(sOrder), _T("D:\\Project\\DaqReport\\DaqReport\\MySQL.ini"));

	char mySql[] = "insert into pro_data(time,device_no,order_no,quality) value('%S',%d,'%S',%d)";
	char cSqlBuff[1024];
	sprintf_s(cSqlBuff, mySql, GetCurTime(), nDevcNo, sOrder, a);
	
	//if (mysql_query(&m_Mysql, cSqlBuff))
	//{
	//	m_Log->Print("InsertProductTable failed (%s)\n", mysql_error(&m_Mysql));
	//}
}

void CDaqReportDlg::InsertTemperTable(float fTemper, int nDevcNo)
{
	char mySql[] = "insert into temperature(time,device_no,temp_value) value('%S',%d,%3.1f)";
	char cSqlBuff[1024];
	sprintf_s(cSqlBuff, mySql, GetCurTime(), nDevcNo, fTemper);
	//if (mysql_query(&m_Mysql, cSqlBuff))
	//{
	//	m_Log->Print("InsertTemperTable failed (%s)\n", mysql_error(&m_Mysql));
	//}
}

void CDaqReportDlg::InsertStatusTable(int nType, int nDevcNo)
{
	char mySql[] = "insert into device_status(time,device_no,device_status) value('%S',%d,%d)";
	char cSqlBuff[1024];
	sprintf_s(cSqlBuff, mySql, GetCurTime(), nDevcNo, nType);
	//if (mysql_query(&m_Mysql, cSqlBuff))
	//{
	//	m_Log->Print("InsertStatusTable failed (%s)\n", mysql_error(&m_Mysql));
	//}
}

void CDaqReportDlg::InsertErrCodeTable(int nCode, int nDevcNo)
{
	char mySql[] = "insert into error_code(time,device_no,error_code) value('%S',%d,%d)";
	char cSqlBuff[1024];
	sprintf_s(cSqlBuff, mySql, GetCurTime(), nDevcNo, nCode);
	//if (mysql_query(&m_Mysql, cSqlBuff))
	//{
	//	m_Log->Print("InsertStatusTable failed (%s)\n", mysql_error(&m_Mysql));
	//}
}

CString CDaqReportDlg::GetCurTime()
{
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	CString sMyTime;
	sMyTime.Format(_T("%S-%02S-%02S %02S:%02S:%02S"), to_string(sTime.wYear), to_string(sTime.wMonth), to_string(sTime.wDay)
		, to_string(sTime.wHour), to_string(sTime.wMinute), to_string(sTime.wSecond));
	return sMyTime;
}

void CDaqReportDlg::OnBtnInsert()
{
	// TODO: Add your control notification handler code here

	string sTmp = "{\"device_no\": 1,\"temperature\": 500}";
	auto res = m_Client.Post("/api/home/device/change_temperature", sTmp, "application/json");
	if (res && res->status == 200)
	{
		sTmp = res->body;
//		m_Log->Print("%s", res->body);

	}
}


void CDaqReportDlg::OnBnClickedBtnErr()
{
	// TODO: Add your control notification handler code here

	string sTmp = "{\"holdingFurnaceId\": \"ssss\",\"status\": 1,\"currentTemperature\":555}";
	auto res = m_Client.Post("/api/collectHoldingFurnace", sTmp, "application/json");

	if (res && res->status == 200)
	{
		sTmp = res->body;
//		m_Log->Print("%s", res->body);
	}
}


void CDaqReportDlg::OnBnClickedBtnStatue()
{
	// TODO: Add your control notification handler code here
	//m_nStatus = (m_nStatus + 1) % 2;
	//InsertStatusTable(m_nStatus, 1);
	string sTmp;
	httplib::ContentReceiver httpRecieve;
	auto res = m_Client.Get("/api/collectHoldingFurnace", httpRecieve);
	//sTmp = "{\"device_no\": 1,\"status\": 1}";
	//auto res = m_Client.Post("/api/home/device/change_status", sTmp, "application/json");
	if (res && res->status == 200)
	{
		sTmp= res->body;
//		m_Log->Print("%s", res->body); 

	}
}

void CDaqReportDlg::OnBnClickedBtnSend()
{
	// TODO: Add your control notification handler code here

	string sTmp = "{\"device_no\": 2,\"quality\": 1}";
	auto res = m_Client.Post("/api/home/device/product", sTmp, "application/json");
	if (res && res->status == 200)
	{
		sTmp = res->body;
//		m_Log->Print("%s", res->body);

	}

}

UINT	WINAPI CDaqReportDlg::MutiConnect(void* pParam)
{
	CDaqReportDlg* pPara = (CDaqReportDlg*)pParam;
	int nIdx=0;
	bool bConnectedFlag[MAX_CONNECT_COUNT] ;
	memset(bConnectedFlag, 0x0, sizeof(bConnectedFlag));
	while (1)
	{
		if (pPara->dev_IP[nIdx].bIsEnable ) 
		{
			if (!pPara->dev_IP[nIdx].bIsConnected)
			{
				if (pPara->InitSocket(&pPara->dev_IP[nIdx]))
				{
					pPara->dev_IP[nIdx].bIsConnected = true;
					pPara->m_Log->Print("与%d号站点链接成功。。。", nIdx+1);
					bConnectedFlag[nIdx] = true;
				}
				else
				{
					if (bConnectedFlag[nIdx])
					{
					
					pPara->m_Log->Print("与%d号站点链接失败！", nIdx + 1);
					}
				}
			}
		}
		nIdx++;
		if (nIdx == MAX_CONNECT_COUNT)
			nIdx = 0;
		Sleep(500);
	}
	return 0;
}

void CDaqReportDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pListView = (NM_LISTVIEW*)pNMHDR;
	//m_nRow = pListView->iItem;
	m_nCol = pListView->iSubItem;

	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	m_List.ScreenToClient(&point);
	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	lvinfo.flags = LVHT_ABOVE;

	m_nRow = m_List.SubItemHitTest(&lvinfo);

	CRect rect;
	if ((m_nCol == 1 || m_nCol==2) && m_nRow != -1)
	{
		m_List.GetSubItemRect(m_nRow, m_nCol, LVIR_LABEL, rect);
		m_Edit.SetParent(&m_List);
		m_Edit.MoveWindow(rect);
		m_Edit.SetWindowTextW(m_List.GetItemText(m_nRow, m_nCol));
		m_Edit.ShowWindow(SW_SHOW);
		m_Edit.SetFocus();
		m_Edit.ShowCaret();
		m_Edit.SetSel(-1);
	}
	else if (m_nCol == 3 && m_nRow != -1)
	{
		m_List.GetSubItemRect(m_nRow, m_nCol, LVIR_LABEL, rect);
		m_ComBox.SetParent(&m_List);
		m_ComBox.MoveWindow(rect);
		m_ComBox.SetWindowTextW(m_List.GetItemText(m_nRow, m_nCol));
		m_ComBox.ShowWindow(SW_SHOW);
		m_ComBox.SetFocus();
		m_ComBox.ShowCaret();
		//m_ComBox.SetSel(-1);
	}
	else if (m_nCol == 4)
	{
		*pResult = 0;
		return ;
	}

	*pResult = 0;
}


void CDaqReportDlg::OnCbnKillfocusCombo1()
{
	// TODO: Add your control notification handler code here
	CString sTmp;
	m_ComBox.GetWindowTextW(sTmp);
	m_List.SetItemText(m_nRow, m_nCol, sTmp);
	m_ComBox.ShowWindow(SW_HIDE);
}


void CDaqReportDlg::OnEnKillfocusEdit1()
{
	// TODO: Add your control notification handler code here
	CString sTmp;
	m_Edit.GetWindowTextW(sTmp);
	m_List.SetItemText(m_nRow, m_nCol, sTmp);
	m_Edit.ShowWindow(SW_HIDE);
}


void CDaqReportDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	dev_IP[m_nRow].bIsEnable = m_ComBox.GetCurSel();
	
}

bool CDaqReportDlg::ReadPara()
{
	memset(&dev_IP, 0x0, sizeof(DEVICE_IP)*MAX_CONNECT_COUNT);
	FILE* fp;
	errno_t err;
	CString sValue;
	if ((err = fopen_s(&fp, "D:\\Project\\DaqReport\\Param.dat", "rb")) == 0)
	{
		fread(&dev_IP, sizeof(DEVICE_IP)*MAX_CONNECT_COUNT, 1, fp);
		for (int i = 0; i < MAX_CONNECT_COUNT; i++)
		{
			sValue.Format(_T("%S"), dev_IP[i].szIpAddr);
			m_List.SetItemText(i, 1, sValue);
			sValue.Format(_T("%d"), dev_IP[i].nPort);
			m_List.SetItemText(i, 2, sValue);
			m_ComBox.GetLBText(dev_IP[i].bIsEnable, sValue.GetBuffer(m_ComBox.GetLBTextLen(dev_IP[i].bIsEnable)));
			sValue.ReleaseBuffer();
			m_List.SetItemText(i, 3, sValue);
			//dev_IP[i].nHighlimit = HIGH_TEMP;
			//dev_IP[i].nLowlimit = LOW_TEMP;
			dev_IP[i].bIsConnected = false;
		}
		
		fclose(fp);
		return true;
	}
	UpdateData();
	return false;
}

bool CDaqReportDlg::WritePara()
{
	FILE* fp;
	errno_t err;
	CString sValue;
	if ((err = fopen_s(&fp, "D:\\Project\\DaqReport\\Param.dat", "wb")) == 0)
	{

		for (int i = 0; i < MAX_CONNECT_COUNT; i++)
		{
			sValue = m_List.GetItemText(i, 1);
			int nLen = WideCharToMultiByte(CP_ACP, 0, sValue, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, sValue, -1, dev_IP[i].szIpAddr, nLen, NULL, NULL);
			
			sValue = m_List.GetItemText(i, 2);
			dev_IP[i].nPort = _ttoi(sValue);

			sValue = m_List.GetItemText(i, 3);
			dev_IP[i].bIsEnable = m_ComBox.FindStringExact(0, sValue);

			dev_IP[i].nDevNo = i+1;
			
		}
		fwrite(dev_IP, sizeof(DEVICE_IP)*MAX_CONNECT_COUNT, 1, fp);
		fclose(fp);
//		MessageBox(_T("OK"));
		return true;
	}
	return false;
}

void CDaqReportDlg::SetStatus(int nDevcNo, bool bStatus)
{
	CString sValue;
	if (bStatus)
	{
		sValue = _T("已连接");
	}
	else
	{
		sValue = _T("断开");
	}
	m_List.SetItemText(nDevcNo-1, 4, sValue);
}

void CDaqReportDlg::OnBnClickedBtnSave()
{
	// TODO: Add your control notification handler code here
	if (WritePara())
	{
		MessageBox(_T("保存成功"));
	}
	else
	{
		MessageBox(_T("保存失败"));
	}
}


void CDaqReportDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	for (int i = 0; i < MAX_CONNECT_COUNT; i++)
	{
		SetStatus(i+1, dev_IP[i].bIsConnected);
	}

	CDialogEx::OnTimer(nIDEvent);
}
