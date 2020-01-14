
// CLIENT_mfcDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "CLIENT_mfc.h"
#include "CLIENT_mfcDlg.h"
#include "afxdialogex.h"

#include "OJJJ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCLIENTmfcDlg 대화 상자



CCLIENTmfcDlg::CCLIENTmfcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_MFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCLIENTmfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_picture);
}

BEGIN_MESSAGE_MAP(CCLIENTmfcDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_BTN_CONNECT, &CCLIENTmfcDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(ID_BTN_SENDING, &CCLIENTmfcDlg::OnBnClickedBtnSending)
END_MESSAGE_MAP()


// CCLIENTmfcDlg 메시지 처리기

BOOL CCLIENTmfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.


	capture = new VideoCapture(0);
	if (!capture->isOpened()) 
		MessageBox(_T("캠을 열 수 없습니다. \n"));
	
	capture->set(CAP_PROP_FRAME_WIDTH, WIDTH);	
	capture->set(CAP_PROP_FRAME_HEIGHT, HEIGHT);

	SetTimer(1000, 30, NULL);
	
	isConnected = FALSE;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CCLIENTmfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCLIENTmfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCLIENTmfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCLIENTmfcDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CCLIENTmfcDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	capture->read(mat_frame);																			// 캠으로부터 이미지 읽어서 mat에 저장
																						        		//mat = cam.stream_capture();

	CImage frame2;

	RECT rect;
	m_picture.GetClientRect(&rect);

	cv::Size winSize(rect.right, rect.bottom);
	frame2.Create(winSize.width, winSize.height, 24);


	BITMAPINFO *bitInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	BitInfo_init(mat_frame, rect, bitInfo);

	SetDIBitsToDevice(frame2.GetDC(), 0, 0, mat_frame.cols, mat_frame.rows, 0, 0, 0, mat_frame.rows, mat_frame.data, bitInfo, DIB_RGB_COLORS);


	m_picture.MoveWindow(0, 0, mat_frame.cols, mat_frame.rows);  ///////////////////////////////////////// 이미지 표시될 위치
	CString get;
	
	//get.Format(_T("%d - %d"), mat.cols, mat.rows); // 320 - 240

	//static_debug.SetWindowTextW(get); // 320 240

	HDC dc = ::GetDC(m_picture.m_hWnd);
	//Send_Picture(dc);
	frame2.BitBlt(dc, 0, 0); // 실제로 그리는 출력을 하는 코드
	::ReleaseDC(m_picture.m_hWnd, dc);

	frame2.ReleaseDC();
	frame2.Destroy();
	CDialogEx::OnTimer(nIDEvent);
}

void CCLIENTmfcDlg::BitInfo_init(Mat mat, RECT rect, BITMAPINFO* bitInfo)
{
	// TODO: 여기에 구현 코드 추가.
	int bpp = 8 * mat.elemSize();
	assert((bpp == 8 || bpp == 24 || bpp == 32)); // bpp 가 8 or 24 or 32가  아니라면 종료

	bitInfo->bmiHeader.biBitCount = bpp;
	bitInfo->bmiHeader.biWidth = mat.cols;
	bitInfo->bmiHeader.biHeight = -mat.rows;
	bitInfo->bmiHeader.biPlanes = 1;
	bitInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo->bmiHeader.biCompression = BI_RGB;
	bitInfo->bmiHeader.biClrImportant = 0;
	bitInfo->bmiHeader.biClrUsed = 0;
	bitInfo->bmiHeader.biSizeImage = 0;
	bitInfo->bmiHeader.biXPelsPerMeter = 0;
	bitInfo->bmiHeader.biYPelsPerMeter = 0;
}


void CCLIENTmfcDlg::OnBnClickedBtnConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (isConnected==FALSE) {
		char msg[BUF_SIZE];
		int str_len;
		// 연결 시도 및 진행
		if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {}

		OJJJ_Adr_Memset(&main_sever_adr, IP, PORT);

		main_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (main_server_sock == INVALID_SOCKET) {}
		connect(main_server_sock, (struct sockaddr*)&main_sever_adr, sizeof(main_sever_adr));											// 서버 연결 시도

		str_len = recv(main_server_sock, msg, BUF_SIZE, 0);
		msg[str_len] = 0;

		printf("%s", msg);
		sscanf(msg, "#ID:%d,PORT:%d", &my_id, &my_port);

		
		CWinThread *Thread_msgRecv = NULL;
		Thread_msgRecv = AfxBeginThread(OJJJ_Thread_msgRecv, NULL);														// 메인 메시지 받는 쓰레드 생성
		if (Thread_msgRecv == NULL) {
			AfxMessageBox(L"Error");
		}
		CloseHandle(Thread_msgRecv);
		
		isConnected = TRUE;
	}
	else																							// 연결이 되어있는 경우
		isConnected = TRUE;																			// 연결 해제
}

void CCLIENTmfcDlg::OJJJ_Adr_Memset(SOCKADDR_IN *adr, char* ip, int port)
{
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}
UINT CCLIENTmfcDlg::OJJJ_Thread_msgRecv(LPVOID arg) {
	CCLIENTmfcDlg* PTR = (CCLIENTmfcDlg*)AfxGetApp()->m_pMainWnd;
	char msg[BUF_SIZE];
	int msg_len;
	CString message;

	int target_id, target_port;

	DWORD recvTimeout = 1000;
	setsockopt(PTR->main_server_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeout, sizeof(recvTimeout));

	while (1) {
		msg_len = recv(PTR->main_server_sock, msg, BUF_SIZE, 0);
		if (msg_len == -1) {
			continue;
		}

		msg[msg_len] = 0;
		printf("%s", msg);
		message = msg;

		if (msg[0] == '$') {																		// 다른 유저가 접속했을 때
			sscanf(msg, "$NEW:%d,PORT:%d", &target_id, &target_port);
			//
		}
		else if (msg[0] == '@') {
			sscanf(msg, "@%d", &target_port);
		}
		else {
			printf("just message\n");
		}
	}
	return 0;
}

UINT CCLIENTmfcDlg::OJJJ_Thread_imgSend(LPVOID arg) {
	CCLIENTmfcDlg* PTR = (CCLIENTmfcDlg*)AfxGetApp()->m_pMainWnd;
	SOCKADDR_IN TO;
	SOCKET sock;
	
	PTR->OJJJ_Adr_Memset(&TO, IP, PTR->my_port);
	printf("%d\n", PTR->my_port);
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	connect(sock, (struct sockaddr*)&TO, sizeof(TO));

	Mat temp;
	int jpegqual = ENCODE_QUALITY;

	vector<uchar> encoded;
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_JPEG_QUALITY);
	compression_params.push_back(jpegqual);

	int total_pack; 
	int ibuf[1];

	while (1) {
		temp = PTR->mat_frame;
		imencode(".jpg", temp, encoded, compression_params);
		total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;
		ibuf[0] = total_pack;
		send(sock, (char*)ibuf, sizeof(int), 0);
		for (int i = 0; i < total_pack; i++)
			send(sock, (char*)&encoded[i*PACK_SIZE], PACK_SIZE, 0);
		//printf("send img\n");
	}
	return 0;

}

void CCLIENTmfcDlg::OnBnClickedBtnSending()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWinThread *Thread_imgSend = NULL;
	Thread_imgSend = AfxBeginThread(OJJJ_Thread_imgSend, NULL);
	if (Thread_imgSend == NULL) {
		AfxMessageBox(L"Error");
	}
	CloseHandle(Thread_imgSend);
}
