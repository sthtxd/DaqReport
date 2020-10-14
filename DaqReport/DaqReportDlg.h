
// DaqReportDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "winsock2.h"
#include <string>
#include <string.h>
#include "time.h"
//#include "mysql.h" 
#include "Eclog.h"
#include "httplib.h"
#include <thread>
#include "afxwin.h"
#include <future>
#include <thread>
#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib,"libmysql.lib")
#pragma comment(lib,"elmeclog.lib")
using namespace std;

#define INPUT_COUNT		8
#define MAX_CONNECT_COUNT	16
#define HIGH_TEMP		850
#define LOW_TEMP		750
typedef struct
{
	bool bIsEnable;
	bool bIsConnected;
	char szIpAddr[16];
	int nPort;
	SOCKET socket;
	int	nDevNo;
	int nLowlimit;
	int nHighlimit;
}DEVICE_IP;

struct save_data
{
	uint64_t cur_timestamp;
	uint16_t digital_in;
	uint16_t digital_out;
};

struct send_data_info
{
	uint64_t cur_timestamp;
	uint64_t last_timestamp;
	struct save_data data[100];
	unsigned int count;
};


// CDaqReportDlg dialog
class CDaqReportDlg : public CDialogEx
{
// Construction
public:
	CDaqReportDlg(CWnd* pParent = NULL);	// standard constructor
	~CDaqReportDlg();
// Dialog Data
	enum { IDD = IDD_DAQREPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnLink();
	afx_msg void OnBtnCancel();
//	CIPAddressCtrl m_IpAddr;
//	CString m_sPort;
//	DWORD m_IpAddr;

	TEcLog *m_Log;
	SOCKET m_Socket;
	int m_nPort;

	bool InitSocket(DEVICE_IP*);
	CString m_sIp;

	DEVICE_IP dev_IP[MAX_CONNECT_COUNT];
	void DataAnalysis(char nData);
	int m_bSignal[INPUT_COUNT];
	static UINT WINAPI SendRecvThread(void* pParam);
	static UINT	WINAPI MutiConnect(void* pParam);
	static UINT WINAPI MrZhouThread(void* pParam);
	HANDLE m_pThread;
	bool	m_bExitThread;
	bool	m_bSend;
	
	int m_nRow;
	int m_nCol;

	bool Init();
	bool ReadPara();
	bool WritePara();
	void InsertTemperTable(float fTemper, int nDevcNo);
	void InsertProductTable(int n,int nDevcNo);
	void InsertStatusTable(int nType, int nDevcNo); 
	void InsertErrCodeTable(int nCode,int nDevcNo);
	CString GetCurTime();
	void SetStatus(int nDevcNo,bool bStatus);
//	MYSQL	m_Mysql;
//	MYSQL_RES* m_result; //保存结果集的
//	MYSQL_ROW m_row; //代表的是结果集中的一行

	int m_nTest;
	int m_nStatus;
	float m_fTemp;

	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBtnInsert();
	afx_msg void OnBnClickedBtnErr();
	afx_msg void OnBnClickedBtnStatue();
	CListCtrl m_List;
	CComboBox m_ComBox;
	CEdit m_Edit;
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMKillfocusList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnKillfocusCombo1();
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	CString m_edtup;
//	CString m_edtlow;
//	CEdit m_edtHigh;
	CEdit m_edtHigh;
	CEdit m_edtLow;
};
