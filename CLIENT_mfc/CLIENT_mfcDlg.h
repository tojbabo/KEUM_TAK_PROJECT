
// CLIENT_mfcDlg.h: 헤더 파일
//

#pragma once


// CCLIENTmfcDlg 대화 상자
class CCLIENTmfcDlg : public CDialogEx
{
// 생성입니다.
public:
	CCLIENTmfcDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_picture;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnConnect();

	void BitInfo_init(Mat mat, RECT rect, BITMAPINFO * bitInfo);
	void OJJJ_Adr_Memset(SOCKADDR_IN * adr, char * ip, int port);
	static UINT OJJJ_Thread_msgRecv(LPVOID arg);
	static UINT OJJJ_Thread_imgSend(LPVOID arg);


	VideoCapture *capture;
	Mat mat_frame;
	CImage cimage_mfc;
	bool isConnected;
	WSADATA wsaData;
	SOCKADDR_IN main_sever_adr;
	SOCKET main_server_sock;
	int my_id;
	int my_port;

	afx_msg void OnBnClickedBtnSending();
};
