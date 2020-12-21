
// DataSimulatorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DataSimulator.h"
#include "DataSimulatorDlg.h"
#include "afxdialogex.h"
#include "DialogNetworkSetting.h"
#include "VCameraMessageTrans.h"
//#include "execute.h"
#include "CameraSetting.h"
//#include "classify.h"
#include <thread>
#include	<queue>
//#include "VCameraMessagePrivate.h"
#pragma comment(lib,"Version.lib")
#define ASIO_STANDALONE
//#include "asio.hpp"
//#include "Session.h"

//using asio::ip::tcp;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace aq;
using namespace std;
using namespace cv;
using namespace Vsee;
std::queue<InferFrame>		CDataSimulatorDlg::infer_queue;

HWND CDataSimulatorDlg::m_hWnd;
HWND m_hMainWnd;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
//io_service* CDataSimulatorDlg::ios[8] = {nullptr};

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	virtual BOOL OnInitDialog();
	void  GetVersion(CString &ver);
// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString strVersion;
	GetVersion(strVersion);
	CString strVersionInfo;
	strVersionInfo.Format("版本号:%s", strVersion);
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowTextA(strVersionInfo);
	return true;
}
void CAboutDlg::GetVersion(CString &ver)
{
	DWORD dwInfoSize = 0;
	TCHAR exePath[MAX_PATH];
	memset(exePath, 0, sizeof(exePath));

	ver.Format(_T("V1.00"));

	// 得到程序的自身路径
	GetModuleFileName(NULL, exePath, sizeof(exePath) / sizeof(TCHAR));

	// 判断是否能获取版本号
	dwInfoSize = GetFileVersionInfoSize(exePath, NULL);

	if (dwInfoSize == 0)
	{
		::OutputDebugString("GetFileVersionInfoSize fail\r\n");
	}
	else
	{
		BYTE* pData = new BYTE[dwInfoSize];

		// 获取版本信息
		if (!GetFileVersionInfo(exePath, NULL, dwInfoSize, pData))
		{
			::OutputDebugString("GetFileVersionInfo fail\r\n");
		}
		else
		{
			// 查询版本信息中的具体键值
			LPVOID lpBuffer;
			UINT uLength;
			if (!::VerQueryValue((LPCVOID)pData, _T("\\"), &lpBuffer, &uLength))
			{
				::OutputDebugString("GetFileVersionInfo fail\r\n");
			}
			else
			{
				DWORD dwVerMS;
				DWORD dwVerLS;
				dwVerMS = ((VS_FIXEDFILEINFO*)lpBuffer)->dwProductVersionMS;
				dwVerLS = ((VS_FIXEDFILEINFO*)lpBuffer)->dwProductVersionLS;
				ver.Format(_T("V%d.%d.%d.%d"), (dwVerMS >> 16), (dwVerMS & 0xFFFF), (dwVerLS >> 16), (dwVerLS & 0xFFFF));
			}
		}

		delete pData;
	}
}

// CDataSimulatorDlg 对话框




CDataSimulatorDlg::CDataSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDataSimulatorDlg::IDD, pParent)
{
	//ios[0] = new io_service(1);
	//ios[1] = new io_service(1);
	//ios[2] = new io_service(1);
	//ios[3] = new io_service(1);
	//ios[4] = new io_service(1);
	//ios[5] = new io_service(1);
	//ios[6] = new io_service(1);
	//ios[7] = new io_service(1);

	//sockets[0] = new tcp::socket(*ios[0]);//将socket->io_service变量设置为对应的ios[i]，在VCameraSession<Derived>::sendMessage(VCameraMessage && msg)中要用到io_service来发送信息
	//sockets[1] = new tcp::socket(*ios[1]);
	//sockets[2] = new tcp::socket(*ios[2]);
	//sockets[3] = new tcp::socket(*ios[3]);
	//sockets[4] = new tcp::socket(*ios[4]);
	//sockets[5] = new tcp::socket(*ios[5]);
	//sockets[6] = new tcp::socket(*ios[6]);
	//sockets[7] = new tcp::socket(*ios[7]);

	//sessions[0] = new Vsee::VTx2Session(*sockets[0]);
	//sessions[1] = new Vsee::VTx2Session(*sockets[1]);
	//using MutexLock = std::lock_guard<std::mutex>;

	//eps[0] = tcp::endpoint(address::from_string("127.0.0.1"), PortNumber::PRC);
	//eps[1] = tcp::endpoint(address::from_string("127.0.0.1"), PortNumber::PRC);
	//eps[2] = tcp::endpoint(address::from_string("192.168.1.13"), PortNumber::PRC);
	//eps[3] = tcp::endpoint(address::from_string("192.168.1.14"), PortNumber::PRC);
	//eps[4] = tcp::endpoint(address::from_string("192.168.1.15"), PortNumber::PRC);
	//eps[5] = tcp::endpoint(address::from_string("192.168.1.16"), PortNumber::PRC);
	//eps[6] = tcp::endpoint(address::from_string("192.168.1.17"), PortNumber::PRC);
	//eps[7] = tcp::endpoint(address::from_string("192.168.1.18"), PortNumber::PRC);


	communicator_=new aq::Communicator;
	m_bClientMode = false;
	camera_scan_speed = 1;
	communicator_->camera_scan_speed = camera_scan_speed;
	m_bNetParametersSetting = false;


	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDataSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_MODE_SELECT, m_checkClientMode);
	DDX_Control(pDX, IDC_CHECK_BATCH_LOAD, m_checkBatchLoad);
	DDX_Control(pDX, IDC_CHECK_SINGLE_LOAD, m_checkSingleLoad);
	DDX_Control(pDX, IDC_CHECK_SERVER_SEND, m_checkServerSend);
	DDX_Control(pDX, IDC_CHECK_SERVER_MODE_SELECT, m_checkServerModeSelect);
	DDX_Control(pDX, IDC_CHECK_CLIENT_SEND, m_checkClientSend);
	DDX_Control(pDX, IDC_CHECK_PictureSend, m_Checksendimage);
}

BEGIN_MESSAGE_MAP(CDataSimulatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_NETWORK_SETTING, &CDataSimulatorDlg::OnBnClickedNetworkSetting)
	ON_BN_CLICKED(IDC_IMAGE_SENDING_START, &CDataSimulatorDlg::OnBnClickedImageSendingStart)
	ON_BN_CLICKED(IDC_START_NETSERVER, &CDataSimulatorDlg::OnBnClickedStartNetserver)
	ON_BN_CLICKED(IDC_FILELIST_PATH, &CDataSimulatorDlg::OnBnClickedFilelistPath)
	ON_BN_CLICKED(IDC_CHECK_BATCH_LOAD, &CDataSimulatorDlg::OnBnClickedCheckBatchLoad)
	ON_BN_CLICKED(IDC_CHECK_SINGLE_LOAD, &CDataSimulatorDlg::OnBnClickedCheckSingleLoad)
	ON_BN_CLICKED(IDC_IMAGE_NEXT, &CDataSimulatorDlg::OnBnClickedImageNext)
	ON_BN_CLICKED(IDC_CHECK_SERVER_SEND, &CDataSimulatorDlg::OnBnClickedCheckServerSend)
	ON_BN_CLICKED(IDC_CHECK_SERVER_MODE_SELECT, &CDataSimulatorDlg::OnBnClickedCheckServerModeSelect)
	ON_BN_CLICKED(IDC_CHECK_MODE_SELECT, &CDataSimulatorDlg::OnBnClickedCheckModeSelect)
	ON_BN_CLICKED(IDC_CHECK_CLIENT_SEND, &CDataSimulatorDlg::OnBnClickedCheckClientSend)
	ON_BN_CLICKED(IDC_SCAN_SPEED_SETTING, &CDataSimulatorDlg::OnBnClickedScanSpeedSetting)
	ON_MESSAGE(ID_SHOWCTRLMESSAGE, ShowCtrlMessage)
	ON_MESSAGE(ID_SENDMESSAGE, &CDataSimulatorDlg::sendmessage)
	ON_MESSAGE(ID_STOPSEND, &CDataSimulatorDlg::StopSend)
	ON_BN_CLICKED(IDC_IMAGE_PREVIOUS, &CDataSimulatorDlg::OnBnClickedImagePrevious)
	ON_BN_CLICKED(IDC_CHECK_PictureSend, &CDataSimulatorDlg::OnBnClickedCheckPicturesend)
	ON_STN_CLICKED(IDC_STATIC_ShowPicture, &CDataSimulatorDlg::OnStnClickedStaticShowpicture)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDataSimulatorDlg 消息处理程序

BOOL CDataSimulatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	m_checkClientMode.SetCheck(false);

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	int m_nWindowWidth = 900;
	int m_nWindowHeight = 500;
	MoveWindow(0, 0, m_nWindowWidth, m_nWindowHeight, 1);
	m_hWnd = AfxGetMainWnd()->m_hWnd;            //To get the main Window Handle;
	m_hMainWnd = m_hWnd;
	DrawWindows();
	m_pwnd = GetDlgItem(IDC_STATIC_IMAGE_SHOW);//IDC_SHOWIMAGE
	//m_pwnd = GetDlgItem(IDC_SHOWIMAGE);//IDC_SHOWIMAGE
	//pwnd->MoveWindow(35,30,352,288);
	m_pDC = m_pwnd->GetDC();
	m_hDC = m_pDC->GetSafeHdc();
	m_pwnd->GetClientRect(&m_ImageRect);

	m_checkBatchLoad.SetCheck(true);
	m_checkSingleLoad.SetCheck(false);
	m_checkServerSend.SetCheck(false);
	m_bServerSendMode = false;

	GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
	GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
	GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
	GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
	GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
	GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(false);
	GetDlgItem(IDC_START_NETSERVER)->EnableWindow(false);
	GetDlgItem(IDC_STATIC_ShowPicture)->ShowWindow(false);
	char IniRead[255];
	memset(IniRead, 0, 255);
	char IniPath[255];
	memset(IniPath, 0, 255);
	//camera_ip_info ci_info;
	CString strPath = GetModulePath();
	strcpy_s(IniPath, 255, strPath.GetBuffer());
	strcat_s(IniPath, "/param.ini");
	
	GetPrivateProfileStringA("System", "scan_speed", "", IniRead, 24, IniPath);
	m_fScanSpeed = atof(IniRead);
	camera_scan_speed = m_fScanSpeed;

	GetPrivateProfileStringA("System", "box1_ip", "", IniRead, 24, IniPath);
	nx_box1_ip = IniRead;
	strcpy(parameters_setting_trans.local_ip, IniRead);

	GetPrivateProfileStringA("System", "box1_port", "", IniRead, 24, IniPath);
	nx_box1_port = atoi(IniRead);
	parameters_setting_trans.local_port = nx_box1_port;

	GetPrivateProfileStringA("System", "camera_serial_number", "", IniRead, 24, IniPath);
	camera_serial_number = atoi(IniRead);

	SetClinetMode();
	OnBnClickedCheckBatchLoad();

	//CRect rect;
	//::GetWindowRect(m_hWnd, rect);
	//ScreenToClient(rect);
	//m_nDlgWidth = rect.right - rect.left;
	//m_nDlgHeight = rect.bottom - rect.top;

	////计算分辨率
	//m_nWidth = GetSystemMetrics(SM_CXSCREEN);
	//m_nHeight = GetSystemMetrics(SM_CYSCREEN);

	////计算放大倍数
	//m_Multiple_width = float(m_nWidth) / float(m_nDlgWidth);
	//m_Mutiple_heith = float(m_nHeight) / float(m_nDlgHeight);
	//change_flag = TRUE;//判断onsize执行时oninitdlg是否已经执行了

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDataSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDataSimulatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		RefreshWnd(m_pwnd);   //clear the print words from the show window;
		CString cs_str;
		string str_str;
		str_str = cs_str;
		Point P1, P2, P0, P3;
		Point pRect1, pRect2;
		POINT  pTopLeft, pRightBottom;
		pTopLeft.x = 5000; pTopLeft.y = 5000; pRightBottom.x = 0; pRightBottom.y = 0;
		CString strText = "经纬异纤检测数据仿真软件";
		PushTailShowText(strText);
		CRect DrawRect(0, 0, 300, 50);
		ShowText(m_pwnd, m_strShowText, DrawRect);

		strText.Format("相机扫描速度: %2.2f", camera_scan_speed);
		PushTailShowText(strText);
		ShowText(m_pwnd, m_strShowText, nullptr);

		strText.Format("服务器地址:%s,端口号:%d", parameters_setting_trans.local_ip, parameters_setting_trans.local_port);
		PushTailShowText(strText);
		ShowText(m_pwnd, m_strShowText, nullptr);

		//OnBnClickedCheckBatchLoad();
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDataSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CDataSimulatorDlg::RefreshWindow()
{
	RefreshWnd(m_pwnd);
	CString strText = "经纬异纤检测数据仿真软件";
	CRect DrawRect(0, 0, 300, 50);
	ShowText(m_pwnd, strText, DrawRect);

	strText.Format("相机扫描速度:  %2.2f", camera_scan_speed);
	CRect DrawRect1(0, 100, 300, 50);
	ShowText(m_pwnd, strText, DrawRect1);

	strText.Format("服务器地址:%s,端口号:%d", parameters_setting_trans.local_ip, parameters_setting_trans.local_port);
	CRect DrawRect2(0, 200, 300, 50);
	ShowText(m_pwnd, strText, DrawRect2);

}

void CDataSimulatorDlg::OnBnClickedNetworkSetting()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogNetworkSetting dlg;
	if (dlg.DoModal() != IDOK)
		return;

	nx_box1_ip = dlg.nx_box1_ip;
	nx_box2_ip = dlg.nx_box2_ip;
	nx_box1_port = dlg.nx_box1_port;
	nx_box2_port = dlg.nx_box2_port;
	local_ip = dlg.local_ip;
	local_ip_port = dlg.local_ip_port;
	simulate_camera=dlg.simulate_camera;    //Simulate camera serial number: A---1,B---2,C---3, D---4
	camera_serial_number=dlg.camera_serial_number;	//Simulate camera serial number: A---1,B---2,C---3, D---4
	m_bNetParametersSetting = true;

	char IniRead[255];
	memset(IniRead, 0, 255);
	char IniPath[255];
	memset(IniPath, 0, 255);
	//camera_ip_info ci_info;
	CString strPath = GetModulePath();
	strcpy_s(IniPath, 255, strPath.GetBuffer());
	strcat_s(IniPath, "/param.ini");

	GetPrivateProfileStringA("System", "scan_speed", "", IniRead, 24, IniPath);
	m_fScanSpeed = atof(IniRead);
	camera_scan_speed = m_fScanSpeed;

	GetPrivateProfileStringA("System", "box1_ip", "", IniRead, 24, IniPath);
	nx_box1_ip = IniRead;
	strcpy(parameters_setting_trans.local_ip, IniRead);

	GetPrivateProfileStringA("System", "box1_port", "", IniRead, 24, IniPath);
	nx_box1_port = atoi(IniRead);
	parameters_setting_trans.local_port = nx_box1_port;

	GetPrivateProfileStringA("System", "camera_serial_number", "", IniRead, 24, IniPath);
	camera_serial_number = atoi(IniRead);
	
	switch (camera_serial_number)
	{
	case 1:
		SetWindowTextA("经纬数据模拟器：相机A");
		break;
	case 2:
		SetWindowTextA("经纬数据模拟器：相机B");
		break;
	case 3:
		SetWindowTextA("经纬数据模拟器：相机C");
		break;
	case 4:
		SetWindowTextA("经纬数据模拟器：相机D");
		break;
	default:
		break;
	}
	//RefreshWnd(m_pwnd);   //clear the print words from the show window;
	CString cs_str;
	string str_str;
	str_str = cs_str;
	Point P1, P2, P0, P3;
	Point pRect1, pRect2;
	POINT  pTopLeft, pRightBottom;
	pTopLeft.x = 5000; pTopLeft.y = 5000; pRightBottom.x = 0; pRightBottom.y = 0;
	RefreshWindow();
}
void  CDataSimulatorDlg::io_thread_fun_run(int beg, int end)
{
	io_service* io = ios[beg];
	while (1)
	{
		connect_and_reconnect(beg, end, *io);
		io->run();			//只有在IO.run()运行后，socket才开始起启动
	}
	//return true;

}

bool  CDataSimulatorDlg::io_thread_fun(int beg, int end, io_service& io)
{
	while (1)
	{
		connect_and_reconnect(beg, end, io);
		io.run();			//只有在IO.run()运行后，socket才开始起启动
	}
	return true;

}


void CDataSimulatorDlg::OnBnClickedImageSendingStart()
{
	// TODO:  在此添加控件通知处理程序代码
	Communicator::batch_image_load = true;
	Communicator::single_image_load = false;
	m_checkBatchLoad.SetCheck(true);
	m_checkSingleLoad.SetCheck(false);
}

bool  CDataSimulatorDlg::connect_and_reconnect(int beg, int end, asio::io_service& io)
{
	for (int i = beg; i < end; ++i)
	{
		auto socket = sockets[i];

		if (!sessions[i])		//如果第一次执行sessions[i]还没有初始化，则进行如下的初始化操作
		{						//如果不是第一次执行，则下述代码不执行
			sessions[i] = new Vsee::VTx2Session(*socket);

			sessions[i]->setProcessFrameFunc([&, i](VCameraFrame&& frame) //将sessions[i]->_process_frame_func设置为以下{ ...}中的执行代码,但仅仅是定义_process_frame_func，
			{								 //在此时并不执行_process_frame_func()函数，需在VCameraSession::asyncRecv()->VTx2Session::ProcessMessage()函数被调用时才会执行

				//MutexLock lock(infer_mutex);
				infer_queue.push(InferFrame(sessions[i], std::move(frame)));//定义一个InferFrame,InferFrame._session=sessions[i],InferFrame._frame=frame,并将InferFrame;
				//push进infer_queue中
				VCameraFrame _frame;
			});

			sessions[i]->setSessionAbortedFunc([&, i, beg, end]()
			{

				connect_and_reconnect(beg, end, io);		//这里用嵌套设计，在connect_and_reconnect（）执行完后面的代码还是要继续执行的
			});
		}

		if (socket->is_open())//如果socket已经打开并连接，则忽略以后代码，进入无限循环
			continue;

		asio::error_code ec;
		socket->cancel(ec);

		socket->async_connect(eps[i], [&, i, beg, end](asio::error_code ec)//第一次执行，进行socket的连接操作
		{
			if (ec)//执行socket->cancel(ec)出错，socket还没初始化好
			{
				connect_and_reconnect(beg, end, io);
				return;
			}

			sessions[i]->startSession();
		});
	}
	return true;
}

bool  CDataSimulatorDlg::connect_and_reconnect(int beg, int end, asio::io_service& io, tcp::socket* _socket, tcp::endpoint eps)
{
	for (int i = beg; i < end; ++i)
	{
		auto socket = _socket;

		if (!sessions[i])		//如果第一次执行sessions[i]还没有初始化，则进行如下的初始化操作
		{						//如果不是第一次执行，则下述代码不执行
			sessions[i] = new Vsee::VTx2Session(*socket);

			sessions[i]->setProcessFrameFunc([&, i](VCameraFrame&& frame) //将sessions[i]->_process_frame_func设置为以下{ ...}中的执行代码,但仅仅是定义_process_frame_func，
			{								 //在此时并不执行_process_frame_func()函数，需在VCameraSession::asyncRecv()->VTx2Session::ProcessMessage()函数被调用时才会执行
				
				MessageBox("setProcessFrameFunc");
				//MutexLock lock(infer_mutex);
				infer_queue.push(InferFrame(sessions[i], std::move(frame)));//定义一个InferFrame,InferFrame._session=sessions[i],InferFrame._frame=frame,并将InferFrame;
				//push进infer_queue中
				VCameraFrame _frame;
			});

			sessions[i]->setSessionAbortedFunc([&, i, beg, end]()
			{

				connect_and_reconnect(beg, end, io);		//这里用嵌套设计，在connect_and_reconnect（）执行完后面的代码还是要继续执行的
			});
		}

		if (socket->is_open())//如果socket已经打开并连接，则忽略以后代码，进入无限循环
			continue;

		asio::error_code ec;
		socket->cancel(ec);

		socket->async_connect(eps, [&, i, beg, end](asio::error_code ec)//第一次执行，进行socket的连接操作
		{
			if (ec)//执行socket->cancel(ec)出错，socket还没初始化好
			{
				connect_and_reconnect(beg, end, io);
				return;
			}

			sessions[i]->startSession();
		});
		sessions[i]->startSession();
	}
	return true;
}

void CDataSimulatorDlg::run_tcp_server(std::string _ip, int _port)
{
	std::thread t(&CDataSimulatorDlg::tcp_server, this, _ip, _port);
	t.detach();
}


void CDataSimulatorDlg::tcp_server(std::string _ip, int _port)
{
	//CWnd::MessageBox("to tcp_connetct!");
	if (0)
		sessions[0]->startSession();
	std::string error_str;
	if (!communicator_->server(_ip, _port, error_str))	//Start a Server state machine,to server for all the net request from client,
	{													//here the the function "server()" actuall is an endless loop.
		std::cerr << error_str << std::endl;
		return;
	}
}


/*
Autoor: LF
Date :   2020-9-1
Function:
      1)Start a thread to connnetc the remote server;
	  2)Send images to remote server;
	  3)This is the mode to connect TX2/NX box,and this mode is the DataSimulator work mode;
*/
void CDataSimulatorDlg::run_tcp_client(std::string _ip, int _port)
{
	int nsize = sizeof(std::size_t);
	nsize = sizeof(int);
	nsize = sizeof(std::string);

	communicator_->run_tcp_connect(_ip, _port);
}

/*
Autoor: LF
Date :   2020-10-29
Function:
1)Start a thread to connnetc the remote server;
2)Send images to remote server;
3)This is the mode to connect TX2/NX box,and this mode is the DataSimulator work mode;
*/
void CDataSimulatorDlg::run_tcp_client(std::string _ip, int _port, int camera_number)
{
	 GetDlgItem(IDC_START_NETSERVER)->EnableWindow(FALSE);
	int nsize = sizeof(std::size_t);
	nsize = sizeof(int);
	nsize = sizeof(std::string);

	communicator_->run_tcp_connect(_ip, _port, camera_number);
}
void CDataSimulatorDlg::tcp_client(std::string _ip, int port, io_service* io)
{
	while (true)
	{
		//connect_and_reconnect(_ip, _port, io);
		connect_and_reconnect(0, 1, *io);
		io->run();			//只有在IO.run()运行后，socket才开始起启动
	//	sessions[0]->startSession();

	}
}
/*************************************************************
功能：获得当前程序的运行目录(.exe)
**************************************************************/
CString CDataSimulatorDlg::GetModulePath(void)
{
	CString    sPath;
	GetModuleFileNameA(NULL, sPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	sPath.ReleaseBuffer();
	int    nPos;
	nPos = sPath.ReverseFind('\\');
	sPath = sPath.Left(nPos);
	return   sPath;
}


void CDataSimulatorDlg::OnBnClickedStartNetserver()
{
	// TODO:  在此添加控件通知处理程序代码
	char IniRead[255];
	memset(IniRead, 0, 255);
	char IniPath[255];
	memset(IniPath, 0, 255);
	//camera_ip_info ci_info;
	CString strPath = GetModulePath();
	strcpy_s(IniPath, 255, strPath.GetBuffer());
	strcat_s(IniPath, "/param.ini");
	GetPrivateProfileStringA("System", "box1_ip", "", IniRead, 24, IniPath);
	//m_dlgNetworkParameters.GetDlgItem(IDC_EDIT_IP_ADRESS)->SetWindowTextA(IniRead);
	nx_box1_ip = IniRead;
	GetPrivateProfileStringA("System", "box1_port", "", IniRead, 24, IniPath);
	nx_box1_port = atoi(IniRead);
	GetPrivateProfileStringA("System", "camera_serial_number", "", IniRead, 24, IniPath);
	camera_serial_number = atoi(IniRead);
	//if (!m_bNetParametersSetting)
	//{
	//	CDialogNetworkSetting dlg;
	//	if (dlg.DoModal() != IDOK)
	//		return;

	//	nx_box1_ip = dlg.nx_box1_ip;
	//	nx_box2_ip = dlg.nx_box2_ip;
	//	nx_box1_port = dlg.nx_box1_port;
	//	nx_box2_port = dlg.nx_box2_port;
	//	local_ip = dlg.local_ip;
	//	local_ip_port = dlg.local_ip_port;
	//	simulate_camera = dlg.simulate_camera;    //Simulate camera serial number: A---1,B---2,C---3, D---4
	//	camera_serial_number = dlg.camera_serial_number;	//Simulate camera serial number: A---1,B---2,C---3, D---4
	//	m_bNetParametersSetting = true;
	//}
	m_bClientMode = m_checkClientMode.GetCheck();
	if (m_Checksendimage.GetCheck()==1)
	{
		aq::Communicator::image_show = true;
	}
	else if(m_Checksendimage.GetCheck()==0)
	{
		aq::Communicator::image_show = false;
	}
	
	if (!m_bClientMode)
	{
		CString strText;
		strText.Format("相机扫描速度:  %2.2f", camera_scan_speed);
		PushTailShowText(strText);
		ShowText(m_pwnd, m_strShowText, nullptr);

		run_tcp_server(nx_box1_ip, nx_box1_port);   //To start a server mode;
	}
	//This is  connecting to  TX2 box mode,and this is the current mode;	
	else
		//run_tcp_client(local_ip, local_ip_port);
		run_tcp_client(nx_box1_ip, nx_box1_port, camera_serial_number);   //To start a server mode;
}


void CDataSimulatorDlg::OnBnClickedFilelistPath()
{
	// TODO:  在此添加控件通知处理程序代码
	BOOL isOpen = TRUE;		        //是否打开(否则为保存)
	CString defaultDir = "F:\\";	//默认打开的文件路径
	CString fileName = "";			//默认打开的文件名
	CString filter = "文件 (*.bmp; *.jpg; *.png)|*.bmp;*.jpg;*.png||";	//文件过虑的类型
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = "";
	if (!(result == IDOK))
	{
		MessageBox("打开文件失败！");
		return;
	}
	filePath = openFileDlg.GetPathName();
	file_load_path = filePath.GetBuffer();
	communicator_->get_file_list(file_load_path);
}


void CDataSimulatorDlg::OnBnClickedCheckBatchLoad()
{
	// TODO:  在此添加控件通知处理程序代码
	int n = m_checkBatchLoad.GetCheck();
	if (m_checkBatchLoad.GetCheck() == 1)
	{
		m_checkSingleLoad.SetCheck(false);
		aq::Communicator::batch_image_load = true;
		aq::Communicator::single_image_load = false;
		aq::Communicator::last_image_load = false;
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
	}		
	else
	{
		m_checkSingleLoad.SetCheck(true);
		aq::Communicator::batch_image_load = false;
		aq::Communicator::single_image_load = true;
		aq::Communicator::last_image_load = false;
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);

	}
	char IniRead[255];
	memset(IniRead, 0, 255);
	char IniPath[255];
	memset(IniPath, 0, 255);
	//camera_ip_info ci_info;
	CString strPath = GetModulePath();
	strcpy_s(IniPath, 255, strPath.GetBuffer());
	strcat_s(IniPath, "/param.ini");
	GetPrivateProfileStringA("System", "box1_ip", "", IniRead, 24, IniPath);
	//m_dlgNetworkParameters.GetDlgItem(IDC_EDIT_IP_ADRESS)->SetWindowTextA(IniRead);
	nx_box1_ip = IniRead;
	GetPrivateProfileStringA("System", "box1_port", "", IniRead, 24, IniPath);
	nx_box1_port = atoi(IniRead);
	GetPrivateProfileStringA("System", "camera_serial_number", "", IniRead, 24, IniPath);
	camera_serial_number = atoi(IniRead);
	switch (camera_serial_number)
	{
	case 1:
		SetWindowTextA("经纬数据模拟器：相机A");
		break;
	case 2:
		SetWindowTextA("经纬数据模拟器：相机B");
		break;
	case 3:
		SetWindowTextA("经纬数据模拟器：相机C");
		break;
	case 4:
		SetWindowTextA("经纬数据模拟器：相机D");
		break;
	default:
		break;
	}
		
}


void CDataSimulatorDlg::OnBnClickedCheckSingleLoad()
{
	// TODO:  在此添加控件通知处理程序代码
	int n = m_checkSingleLoad.GetCheck();
	if (m_checkSingleLoad.GetCheck() == 1)
	{
		m_checkBatchLoad.SetCheck(false);
		aq::Communicator::batch_image_load = false;
		aq::Communicator::single_image_load = true;
		aq::Communicator::last_image_load = false;
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
	}	
	else
	{
		m_checkBatchLoad.SetCheck(true);
		aq::Communicator::batch_image_load = true;
		aq::Communicator::single_image_load = false;
		aq::Communicator::last_image_load = false;
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
	}
		
}


void CDataSimulatorDlg::OnBnClickedImageNext()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_Checksendimage.GetCheck() == 1)
	{
		aq::Communicator::image_show = true;
	}
	else if (!m_Checksendimage.GetCheck() == 0)
	{
		aq::Communicator::image_show = false;
	}
	Communicator::single_image_load = true;
	Communicator::last_image_load = false;
	Communicator::batch_image_load = false;
	m_checkBatchLoad.SetCheck(false);
	m_checkSingleLoad.SetCheck(true);
}


void CDataSimulatorDlg::SetClinetMode()
{
	m_checkServerModeSelect.SetCheck(false);
	if (m_checkServerModeSelect.GetCheck() == 1)
	{
		m_bServerMode = true;
		m_bClientMode = false;
		m_checkServerModeSelect.SetCheck(m_bServerMode);
		m_checkClientMode.SetCheck(m_bClientMode);
	}
	else
	{
		m_bServerMode = false;
		m_bClientMode = true;
		m_checkServerModeSelect.SetCheck(m_bServerMode);
		m_checkClientMode.SetCheck(m_bClientMode);
	}
	m_checkClientSend.SetCheck(true);
	if (m_checkClientSend.GetCheck() == 1)
	{
		m_bClientSendMode = true;
		m_bServerSendMode = false;
		communicator_->m_bServerSendMode = m_bServerSendMode;
		communicator_->m_bClientSendMode = m_bClientSendMode;
		m_checkClientSend.SetCheck(m_bClientSendMode);
		m_checkServerSend.SetCheck(m_bServerSendMode);
	}
	else
	{
		m_bClientSendMode = false;
		m_bServerSendMode = true;
		communicator_->m_bServerSendMode = m_bServerSendMode;
		communicator_->m_bClientSendMode = m_bClientSendMode;
		m_checkClientSend.SetCheck(m_bClientSendMode);
		m_checkServerSend.SetCheck(m_bServerSendMode);
	}
	if (m_bServerMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
	else if (m_bServerMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);

	}
	else if (m_bClientMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
	else if (m_bClientMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
	return;
}


void CDataSimulatorDlg::OnBnClickedCheckServerModeSelect()
{
	// TODO:  在此添加控件通知处理程序代码
	int n = m_checkServerModeSelect.GetCheck();
	if (m_checkServerModeSelect.GetCheck() == 1)
	{
		m_bServerMode = true;
		m_bClientMode = false;
		m_checkServerModeSelect.SetCheck(m_bServerMode);
		m_checkClientMode.SetCheck(m_bClientMode);
	}
	else
	{
		m_bServerMode = false;
		m_bClientMode = true;
		m_checkServerModeSelect.SetCheck(m_bServerMode);
		m_checkClientMode.SetCheck(m_bClientMode);
	}
	if (m_bServerMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
	else if (m_bServerMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);

	}
	else if (m_bClientMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
	else if (m_bClientMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
}


void CDataSimulatorDlg::OnBnClickedCheckModeSelect()    //Client mode set
{
	// TODO:  在此添加控件通知处理程序代码
	int n = m_checkClientMode.GetCheck();
	//int m_checkServerModeSelect.GetCheck();

	if (m_checkClientMode.GetCheck() == 1)
	{
		m_bServerMode = false;
		m_bClientMode = true;
		m_checkServerModeSelect.SetCheck(m_bServerMode);
		m_checkClientMode.SetCheck(m_bClientMode);
	}
	else
	{
		m_bServerMode = true;
		m_bClientMode = false;
		m_checkServerModeSelect.SetCheck(m_bServerMode);
		m_checkClientMode.SetCheck(m_bClientMode);
	}
	if (m_bServerMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);

	}
	else if (m_bServerMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);

	}
	else if (m_bClientMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);

	}
	else if (m_bClientMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
}


void CDataSimulatorDlg::OnBnClickedCheckServerSend()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_checkServerSend.GetCheck() == 1)
	{
		m_bServerSendMode = true;
		m_bClientSendMode = false;
		communicator_->m_bServerSendMode = m_bServerSendMode;
		communicator_->m_bClientSendMode = m_bClientSendMode;
		m_checkServerSend.SetCheck(m_bServerSendMode);
		m_checkClientSend.SetCheck(m_bClientSendMode);
	}
	else
	{
		m_bServerSendMode = false;
		m_bClientSendMode = true;
		communicator_->m_bServerSendMode = m_bServerSendMode;
		communicator_->m_bClientSendMode = m_bClientSendMode;
		m_checkServerSend.SetCheck(m_bServerSendMode);
		m_checkClientSend.SetCheck(m_bClientSendMode);
	}
	if (m_bServerMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
	else if (m_bServerMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);

	}
	else if (m_bClientMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);

	}
	else if (m_bClientMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
}
void CDataSimulatorDlg::OnBnClickedCheckClientSend()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_checkClientSend.GetCheck() == 1)
	{
		m_bClientSendMode = true;
		m_bServerSendMode = false;
		communicator_->m_bServerSendMode = m_bServerSendMode;
		communicator_->m_bClientSendMode = m_bClientSendMode;
		m_checkClientSend.SetCheck(m_bClientSendMode);
		m_checkServerSend.SetCheck(m_bServerSendMode);
	}
	else
	{
		m_bClientSendMode = false;
		m_bServerSendMode = true;
		communicator_->m_bServerSendMode = m_bServerSendMode;
		communicator_->m_bClientSendMode = m_bClientSendMode;
		m_checkClientSend.SetCheck(m_bClientSendMode);
		m_checkServerSend.SetCheck(m_bServerSendMode);
	}
	if (m_bServerMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
	else if (m_bServerMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);

	}
	else if (m_bClientMode && m_bServerSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(false);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);

	}
	else if (m_bClientMode && m_bClientSendMode)
	{
		GetDlgItem(IDC_SCAN_SPEED_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_FILELIST_PATH)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_BATCH_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_SINGLE_LOAD)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_SENDING_START)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_NETWORK_SETTING)->EnableWindow(true);
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(true);
	}
}


void CDataSimulatorDlg::OnBnClickedScanSpeedSetting()
{
	// TODO:  在此添加控件通知处理程序代码
	CCameraSetting dlg;
	if (dlg.DoModal() != IDOK)
		return;

	camera_scan_speed = dlg.camera_scan_speed;
	m_fScanSpeed = dlg.m_fScanSpeed;
	communicator_->camera_scan_speed = m_fScanSpeed;

	char IniRead[255];
	memset(IniRead, 0, 255);
	char IniPath[255];
	memset(IniPath, 0, 255);
	//camera_ip_info ci_info;
	CString strPath = GetModulePath();
	strcpy_s(IniPath, 255, strPath.GetBuffer());
	strcat_s(IniPath, "/param.ini");

	GetPrivateProfileStringA("System", "scan_speed", "", IniRead, 24, IniPath);
	m_fScanSpeed = atof(IniRead);
	camera_scan_speed = m_fScanSpeed;

	GetPrivateProfileStringA("System", "box1_ip", "", IniRead, 24, IniPath);
	nx_box1_ip = IniRead;
	strcpy(parameters_setting_trans.local_ip, IniRead);

	GetPrivateProfileStringA("System", "box1_port", "", IniRead, 24, IniPath);
	nx_box1_port = atoi(IniRead);
	parameters_setting_trans.local_port = nx_box1_port;

	GetPrivateProfileStringA("System", "camera_serial_number", "", IniRead, 24, IniPath);
	camera_serial_number = atoi(IniRead);

	RefreshWnd(m_pwnd);   //clear the print words from the show window;
	CString cs_str;
	string str_str;
	str_str = cs_str;
	Point P1, P2, P0, P3;
	Point pRect1, pRect2;
	POINT  pTopLeft, pRightBottom;
	pTopLeft.x = 5000; pTopLeft.y = 5000; pRightBottom.x = 0; pRightBottom.y = 0;
	CString strText = "经纬异纤检测数据仿真软件";
	CRect DrawRect(0, 0, 300, 50);
	ShowText(m_pwnd, strText, DrawRect);

	strText.Format("相机扫描速度:  %2.2f", camera_scan_speed);
	CRect DrawRect1(0, 100, 300, 50);
	ShowText(m_pwnd, strText, DrawRect1);

	strText.Format("服务器地址:%s,端口号:%d", parameters_setting_trans.local_ip, parameters_setting_trans.local_port);
	CRect DrawRect2(0, 200, 300, 50);
	ShowText(m_pwnd, strText, DrawRect2);
}


void    CDataSimulatorDlg::PushTailShowText(CString strText)
{
	int i = 0;
	bool bPush = false;

	CRect	WindowRect;
	GetClientRect(&WindowRect);
	float m_nHeight = WindowRect.Height();
	float m_nWidth = WindowRect.Width();

	int magnification = (float)(m_nWidth) / (float)(m_nWindowWidth);
	for (i = 0; i < SHOWTEXTLENGTH*magnification+4; i++)
	{
		if (m_strShowText[i][0] == '\0')
		{
			strcpy(m_strShowText[i], strText);
			bPush = true;
			break;
		}
	}
	if (bPush)
		return;
	else
	{
		for (i = 0; i < SHOWTEXTLENGTH*magnification+4 - 1; i++)
		{
			strcpy(m_strShowText[i], m_strShowText[i + 1]);
		}
		strcpy(m_strShowText[i], strText);
		return;
	}
}

void CDataSimulatorDlg::RefreshWnd(CWnd *pwnd)
{
	pwnd = GetDlgItem(IDC_STATIC_IMAGE_SHOW);
	//CWnd *pwnd = GetDlgItem(IDC_ShowImage);
	CRect PictureRect;
	pwnd->GetClientRect(&PictureRect);
	int nWidth = PictureRect.Width();
	int nHeight = PictureRect.Height();
	CClientDC pDc(pwnd);
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));//创建一个画笔工具
	CBrush brush;
	brush.CreateSolidBrush(RGB(BACK_COLOR_GRAY, BACK_COLOR_GRAY, BACK_COLOR_GRAY));   //BACK_COLOR_GRAY: Window default color;

	//	pDc.DrawText(strText, DrawRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //显示文本

	pDc.FillRect(PictureRect, &brush);

	POINT pt1;
	pt1.x = 0; pt1.y = 0;
	POINT pt2;
	pt2.x = 100;// (100, 100);
	pt2.y = 100;
	pDc.SelectObject(&pen);//选择画笔
	POINT point;
	point.x = 0;
	point.y = 0;
	pDc.MoveTo(point);

	point.x = nWidth;
	point.y = 0;
	pDc.LineTo(point);

	point.x = nWidth;
	point.y = nHeight;
	pDc.LineTo(point);

	point.x = 0;
	point.y = nHeight;
	pDc.LineTo(point);

	point.x = 0;
	point.y = 0;
	pDc.LineTo(point);

}
LRESULT CDataSimulatorDlg::sendmessage(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(FALSE);
	int check_send =(int)wParam;
	if (check_send)
	{
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(TRUE);
	}
	else if (!check_send)
	{
		GetDlgItem(IDC_START_NETSERVER)->EnableWindow(FALSE);
	}
	UpdateData(true);
	return 1;
}

LRESULT CDataSimulatorDlg::StopSend(WPARAM wParam, LPARAM lParam)
{
	int check_send = (int)wParam;
	if (check_send>=1)
	{
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(TRUE);
	}
	else 
	{
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(FALSE);
	}
	UpdateData(true);
	return 1;
}

LRESULT CDataSimulatorDlg::ShowCtrlMessage(WPARAM wParam, LPARAM lParam)
{
	RefreshWnd(m_pwnd);   //clear the print words from the show window;
	CString strMsg;
	int nCtrlSignalling = (int)wParam;
	Vsee::VCameraMessageTrans *MagneticVolveMsg = nullptr;
	struct information
	{
		std::uint32_t height;
		std::uint32_t width;
		std::uint32_t channels;
		std::uint32_t data_legnth;
		char* m_picture_data;
	}*Info;

	//AfxMessageBox(strMsg);

	if (nCtrlSignalling == 5)
	{
		Info = (information*)lParam;
		cv::Mat show_img(Info->height, Info->width, CV_8UC3, Info->m_picture_data);
		//GetDlgItem(IDC_STATIC_IMAGE_SHOW)->ShowWindow(false);
		//GetDlgItem(IDC_STATIC_ShowPicture)->ShowWindow(TRUE);
		std::thread draw_mat(&CDataSimulatorDlg::DrawMatp, this,show_img);
		draw_mat.detach();
	}
	if (nCtrlSignalling == 10)
	{
		RefreshWnd(m_pwnd);   //clear the print words from the show window;
		CString str;
		char * strMessage = (char*)lParam;
		str.Format("%s", strMessage);
		PushTailShowText(str);
		ShowText(m_pwnd, m_strShowText, NULL);
		MagneticVolveMsg = (Vsee::VCameraMessageTrans*)lParam;
	}
	else if (nCtrlSignalling == 3)      //To test the magnetic valve hardware;
	{
		RefreshWnd(m_pwnd);   //clear the print words from the show window;
		MagneticVolveMsg = (Vsee::VCameraMessageTrans*)lParam;
		int nMagneticNum = MagneticVolveMsg->magnetic_valve_num;
		strMsg.Format("wParam=%d,MagneticNum=%d", wParam, MagneticVolveMsg->magnetic_valve_num);
		memcpy(&aq::Communicator::box_ctrl_msg, MagneticVolveMsg, sizeof(Vsee::VCameraMessageTrans));	

	}
	else if (nCtrlSignalling == 4)     //To set the camera detect parameters;
	{
		MagneticVolveMsg = (Vsee::VCameraMessageTrans*)lParam;
		int nMagneticNum = MagneticVolveMsg->magnetic_valve_num;
		strMsg.Format("wParam=%d,MagneticNum=%d", wParam, MagneticVolveMsg->magnetic_valve_num);
		memcpy(&aq::Communicator::box_ctrl_msg, MagneticVolveMsg, sizeof(Vsee::VCameraMessageTrans));
		RefreshWnd(m_pwnd);   //clear the print words from the show window;
		CString str;
		str.Format("To set camera detect parameters.");
		PushTailShowText(str);
		ShowText(m_pwnd, m_strShowText, NULL);

	}
	if (MagneticVolveMsg != nullptr)    //Here need to delete the data pointer that transmit from ::PostMessage() function;
	{
		delete MagneticVolveMsg;
		MagneticVolveMsg = nullptr;
	}

	UpdateData(true);
	//Invalidate(TRUE);   //If use function "Invalidate(TRUE)" will induce window flashing;
	return 1;
}

void CDataSimulatorDlg::ShowText(CWnd *pwnd, CString strText, CRect DrawRect)
{

	//CWnd *pwnd = GetDlgItem(IDC_ShowImage);
	CClientDC pDc(pwnd);
	CPen pen(PS_SOLID, 5, RGB(255, 0, 0));//创建一个画笔工具	
	m_nRectWidth = m_ImageRect.Width();
	m_nRectHeight = m_ImageRect.Height();

	pDc.SetTextColor(RGB(0, 255, 0));
	//	pDc.DrawText(strText, DrawRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //显示文本

	CFont font;

	font.CreateFont(
		20, // nHeight
		0, // nWidth
		0, // nEscapement
		0, // nOrientation
		FW_NORMAL, // nWeight
		FALSE, // bItalic
		FALSE, // bUnderline
		0, // cStrikeOut
		ANSI_CHARSET, // nCharSet
		OUT_DEFAULT_PRECIS, // nOutPrecision
		CLIP_DEFAULT_PRECIS, // nClipPrecision
		DEFAULT_QUALITY, // nQuality
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial") // nPitchAndFamily Arial
		);
	pDc.SelectObject(&font);
	pDc.SetBkMode(TRANSPARENT);


	pDc.SetTextColor(RGB(0, 0, 0));

	pDc.SetBkColor(RGB(255, 255, 0));
	//pDc.TextOut(0, 0, "写字");
	char str[5][255];
	memset(str[0], 0, 255);
	strcpy(str[0], strText);

	pDc.DrawText(str[0], DrawRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //显示文本

	POINT pt1;
	pt1.x = 0; pt1.y = 0;
	POINT pt2;
	pt2.x = 100;// (100, 100);
	pt2.y = 100;
	//pDc.SelectObject(&pen);//选择画笔
	//pDc.MoveTo(pt1);
	//pDc.LineTo(pt2);	
}

void CDataSimulatorDlg::ShowText(CWnd *pwnd, char strText[SHOWTEXTLENGTH][255], CRect DrawRect)
{

	CRect	WindowRect;
	GetClientRect(&WindowRect);
	float m_nHeight = WindowRect.Height();
	float m_nWidth = WindowRect.Width();

	int magnification = (float)(m_nWidth) / (float)(m_nWindowWidth);

	CClientDC pDc(pwnd);
	CPen pen(PS_SOLID, 5, RGB(255, 0, 0));//创建一个画笔工具	
	m_nRectWidth = m_ImageRect.Width();
	m_nRectHeight = m_ImageRect.Height();

	pDc.SetTextColor(RGB(0, 255, 0));
	//	pDc.DrawText(strText, DrawRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //显示文本

	CFont font;

	font.CreateFont(
		20, // nHeight
		0, // nWidth
		0, // nEscapement
		0, // nOrientation
		FW_NORMAL, // nWeight
		FALSE, // bItalic
		FALSE, // bUnderline
		0, // cStrikeOut
		ANSI_CHARSET, // nCharSet
		OUT_DEFAULT_PRECIS, // nOutPrecision
		CLIP_DEFAULT_PRECIS, // nClipPrecision
		DEFAULT_QUALITY, // nQuality
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial") // nPitchAndFamily Arial
		);
	pDc.SelectObject(&font);
	pDc.SetBkMode(TRANSPARENT);

	pDc.SetTextColor(RGB(0, 0, 0));

	pDc.SetBkColor(RGB(255, 255, 0));
	//pDc.TextOut(0, 0, "写字");
	int i = 0;
	if(magnification>1)
		for (i = 0; i < SHOWTEXTLENGTH*magnification+4; i++)
		{
		if (m_strShowText[i][0] != '\0')
		{
			CRect TextRect(10, i * 50, 600, (i + 1) * 50);
			pDc.DrawText(m_strShowText[i], TextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //显示文本
		}
		}
	else
	for (i = 0; i < SHOWTEXTLENGTH*magnification; i++)
	{
		if (m_strShowText[i][0] != '\0')
		{
			CRect TextRect(10, i * 50, 600, (i + 1) * 50);
			pDc.DrawText(m_strShowText[i], TextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //显示文本
		}
	}
}




void CDataSimulatorDlg::OnBnClickedImagePrevious()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_Checksendimage.GetCheck() == 1)
	{
		aq::Communicator::image_show = true;
	}
	else if (!m_Checksendimage.GetCheck() == 0)
	{
		aq::Communicator::image_show = false;
	}
	Communicator::single_image_load = false;
	Communicator::last_image_load = true;
	Communicator::batch_image_load = false;
	m_checkBatchLoad.SetCheck(false);
	m_checkSingleLoad.SetCheck(true);
}

void CDataSimulatorDlg::DrawMat(cv::Mat& img, UINT nID)
{
	cv::Mat imgTmp = img;
	CRect rect;
	GetDlgItem(nID)->GetClientRect(&rect); // 获取控件大小  
	//cv::resize(img, imgTmp, cv::Size(rect.Width(), rect.Height()));// 缩放Mat并备份 // 转一下格式 ,这段可以放外面,
	//switch (imgTmp.channels())
	//{
	//case 1:
	//	cv::cvtColor(imgTmp, imgTmp, CV_GRAY2BGRA); // GRAY单通道
	//	break;
	//case 3:
	//	cv::cvtColor(imgTmp, imgTmp, CV_BGR2BGRA); // BGR三通道 
	//	break;
	//default:
	//	break;
	//}
	float magnification = (float)(rect.Width()) / (float)(imgTmp.cols);
	cv::Mat image_max(imgTmp.cols * magnification, imgTmp.rows * magnification, imgTmp.type());
	cv::resize(imgTmp, image_max, Size(imgTmp.cols * magnification, imgTmp.rows * magnification));
	//int pixelBytes = imgTmp.channels() * (imgTmp.depth() + 1); // 计算一个像素多少个字节 // 制作bitmapinfo(数据头) 
	//BITMAPINFO bitInfo;
	//bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	//bitInfo.bmiHeader.biWidth = imgTmp.cols;
	//bitInfo.bmiHeader.biHeight = -imgTmp.rows;
	//bitInfo.bmiHeader.biPlanes = 1;
	//bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bitInfo.bmiHeader.biCompression = BI_RGB;
	//bitInfo.bmiHeader.biClrImportant = 0;
	//bitInfo.bmiHeader.biClrUsed = 0;
	//bitInfo.bmiHeader.biSizeImage = 0; 
	//bitInfo.bmiHeader.biXPelsPerMeter = 0;
	//bitInfo.bmiHeader.biYPelsPerMeter = 0; // Mat.data + bitmap数据头 -> MFC 
	int pixelBytes = image_max.channels() * (image_max.depth() + 1); // 计算一个像素多少个字节 // 制作bitmapinfo(数据头) 
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = image_max.cols;
	bitInfo.bmiHeader.biHeight = -image_max.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0; // Mat.data + bitmap数据头 -> MFC 
	CDC* pDC = GetDlgItem(nID)->GetDC();
	int nYbegin = 0;
	//int nHeight = int(100.00*(float)(rect.Width()) / (float)(imgTmp.cols));
	//nYbegin = (rect.Height() - nHeight) / 2;
	nYbegin = (rect.Height() - image_max.rows) / 2;

	CDC memDC;
	CDC * dc = GetDC();
	CBitmap *oldBitmap;
	memDC.CreateCompatibleDC(pDC);
	//CRect rect;
	//GetDlgItem(IDC_STATIC_IMAGE_SHOW)->GetClientRect(&rect); // 获取控件大小  
	//ScreenToClient(&rect);
	CBitmap bufferMemBitmap;
	GetClientRect(&rect);
	//CRect rect1;
	//GetDlgItem(IDC_BUTTON_HARDWARE_TEST)->GetWindowRect(&rect1);//获取控件的屏幕坐标
	//GetDlgItem(IDC_BUTTON_HARDWARE_TEST)->GetWindowRect(&rect1);//获取控件的屏幕坐标
	//ScreenToClient(&rect1);//转换为对话框上的客户坐标
	CRect rect2;
	GetDlgItem(IDC_STATIC_ShowPicture)->GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	bufferMemBitmap.CreateCompatibleBitmap(pDC, rect2.Width(), rect2.Height());
	oldBitmap = memDC.SelectObject(&bufferMemBitmap);
	CBrush brush = GetSysColor(COLOR_WINDOW);
	//memDC.FillRect(CRect(0, 0, rect.Width(), rect.Height()), &brush);
	memDC.FillRect(CRect(rect.left, rect.top, rect2.Width(), rect2.Height()), &brush);
	// 使用memDC做绘图操作

	// 将内存缓冲DC拷贝至当前DC
	pDC->BitBlt(rect.left, rect.top, rect2.Width(), rect2.Height(), &memDC, 0, 0, SRCCOPY);
	bufferMemBitmap.DeleteObject();
	brush.DeleteObject();
	memDC.DeleteDC();

	//::StretchDIBits(pDC->GetSafeHdc(), 0, nYbegin, rect.Width(), nHeight, 0, 0, imgTmp.cols, imgTmp.rows, imgTmp.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
	::StretchDIBits(pDC->GetSafeHdc(), 0, nYbegin, image_max.cols, image_max.rows, 0, 0, image_max.cols, image_max.rows, image_max.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(pDC);
	aq::Communicator::m_show_ok = true;
}

void CDataSimulatorDlg::DrawMatp(cv::Mat& img)
{
	cv::Mat imgTmp = img;
	CRect rect;
	GetDlgItem(IDC_STATIC_ShowPicture)->GetClientRect(&rect); // 获取控件大小  
	//cv::resize(img, imgTmp, cv::Size(rect.Width(), rect.Height()));// 缩放Mat并备份 // 转一下格式 ,这段可以放外面,
	//switch (imgTmp.channels())
	//{
	//case 1:
	//	cv::cvtColor(imgTmp, imgTmp, CV_GRAY2BGRA); // GRAY单通道
	//	break;
	//case 3:
	//	cv::cvtColor(imgTmp, imgTmp, CV_BGR2BGRA); // BGR三通道 
	//	break;
	//default:
	//	break;
	//}
	float magnification = (float)(rect.Width()) / (float)(imgTmp.cols);
	cv::Mat image_max = imgTmp.clone();
	cv::resize(image_max, image_max, Size(imgTmp.cols * magnification, imgTmp.rows * magnification));
	//int pixelBytes = imgTmp.channels() * (imgTmp.depth() + 1); // 计算一个像素多少个字节 // 制作bitmapinfo(数据头) 
	//BITMAPINFO bitInfo;
	//bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	//bitInfo.bmiHeader.biWidth = imgTmp.cols;
	//bitInfo.bmiHeader.biHeight = -imgTmp.rows;
	//bitInfo.bmiHeader.biPlanes = 1;
	//bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bitInfo.bmiHeader.biCompression = BI_RGB;
	//bitInfo.bmiHeader.biClrImportant = 0;
	//bitInfo.bmiHeader.biClrUsed = 0;
	//bitInfo.bmiHeader.biSizeImage = 0; 
	//bitInfo.bmiHeader.biXPelsPerMeter = 0;
	//bitInfo.bmiHeader.biYPelsPerMeter = 0; // Mat.data + bitmap数据头 -> MFC 
	int pixelBytes = image_max.channels() * (image_max.depth() + 1); // 计算一个像素多少个字节 // 制作bitmapinfo(数据头) 
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = image_max.cols;
	bitInfo.bmiHeader.biHeight = -image_max.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0; // Mat.data + bitmap数据头 -> MFC 
	CDC* pDC = GetDlgItem(IDC_STATIC_ShowPicture)->GetDC();
	int nYbegin = 0;
	//int nHeight = int(100.00*(float)(rect.Width()) / (float)(imgTmp.cols));
	//nYbegin = (rect.Height() - nHeight) / 2;
	nYbegin = (rect.Height() - image_max.rows) / 2;

	CDC memDC;
	CDC * dc = GetDC();
	CBitmap *oldBitmap;
	memDC.CreateCompatibleDC(pDC);
	//CRect rect;
	//GetDlgItem(IDC_STATIC_IMAGE_SHOW)->GetClientRect(&rect); // 获取控件大小  
	//ScreenToClient(&rect);
	CBitmap bufferMemBitmap;
	GetClientRect(&rect);
	//CRect rect1;
	//GetDlgItem(IDC_BUTTON_HARDWARE_TEST)->GetWindowRect(&rect1);//获取控件的屏幕坐标
	//GetDlgItem(IDC_BUTTON_HARDWARE_TEST)->GetWindowRect(&rect1);//获取控件的屏幕坐标
	//ScreenToClient(&rect1);//转换为对话框上的客户坐标
	CRect rect2;
	GetDlgItem(IDC_STATIC_ShowPicture)->GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	bufferMemBitmap.CreateCompatibleBitmap(pDC, rect2.Width(), rect2.Height());
	oldBitmap = memDC.SelectObject(&bufferMemBitmap);
	CBrush brush = GetSysColor(COLOR_WINDOW);
	//memDC.FillRect(CRect(0, 0, rect.Width(), rect.Height()), &brush);
	memDC.FillRect(CRect(rect.left, rect.top, rect2.Width(), rect2.Height()), &brush);
	// 使用memDC做绘图操作

	// 将内存缓冲DC拷贝至当前DC
	pDC->BitBlt(rect.left, rect.top, rect2.Width(), rect2.Height(), &memDC, 0, 0, SRCCOPY);
	bufferMemBitmap.DeleteObject();
	brush.DeleteObject();
	memDC.DeleteDC();

	//::StretchDIBits(pDC->GetSafeHdc(), 0, nYbegin, rect.Width(), nHeight, 0, 0, imgTmp.cols, imgTmp.rows, imgTmp.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
	::StretchDIBits(pDC->GetSafeHdc(), 0, nYbegin, image_max.cols, image_max.rows, 0, 0, image_max.cols, image_max.rows, image_max.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(pDC);
	aq::Communicator::m_show_ok = true;
}

void CDataSimulatorDlg::OnBnClickedCheckPicturesend()
{
	// TODO:  在此添加控件通知处理程序代码
	//CWnd *pWnd;
	//pWnd = GetDlgItem(IDC_SCAN_SPEED_SETTING);    //获取控件指针，IDC_BUTTON1为控件ID号
	//CRect rect;    // 获取控件变化前大小
	//pWnd->GetWindowRect(&rect);
	//ScreenToClient(&rect); // 将控件大小转换为在对话框中的区域坐标
	//m_nButtonHeight = rect.Height();
	//m_nButtonWidth = rect.Width();
	//m_nButtonUpleftX = rect.TopLeft().x;
	//m_nButtonUpleftY = rect.TopLeft().y;
	if (m_Checksendimage.GetCheck()==1)
	{
		aq::Communicator::image_show = TRUE;
		GetDlgItem(IDC_STATIC_ShowPicture)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_IMAGE_SHOW)->ShowWindow(FALSE);
		
	}
	else if (m_Checksendimage.GetCheck() == 0)
	{
		aq::Communicator::image_show = FALSE;
		GetDlgItem(IDC_STATIC_IMAGE_SHOW)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_ShowPicture)->ShowWindow(FALSE);
		UpdateData(true);
	}
}


void CDataSimulatorDlg::OnStnClickedStaticShowpicture()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CDataSimulatorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (m_bResizeEnable)
		DrawWindows();
	// TODO:  在此处添加消息处理程序代码
	//if (change_flag)//如果确定oninitdlg已经调用完毕.
	//{
	//	ReSize(IDC_SCAN_SPEED_SETTING);
	//	ReSize(IDC_FILELIST_PATH);
	//	ReSize(IDC_CHECK_BATCH_LOAD);
	//	ReSize(IDC_CHECK_SINGLE_LOAD);
	//	ReSize(IDC_CHECK_PictureSend);
	//	ReSize(IDC_IMAGE_NEXT);
	//	ReSize(IDC_IMAGE_PREVIOUS);
	//	ReSize(IDC_NETWORK_SETTING);
	//	ReSize(IDC_START_NETSERVER);

	//	ReSize(IDOK);
	//	ReSize(IDCANCEL);
	//	ReSize(IDC_STATIC_IMAGE_SHOW);
	//	ReSize(IDC_STATIC_ShowPicture);
	//	//恢复放大倍数，并保存 (确保还原时候能够还原到原来的大小)

	//	m_Multiple_width = float(1) / m_Multiple_width;
	//	m_Mutiple_heith = float(1) / m_Mutiple_heith;
	//}

}
void CDataSimulatorDlg::DrawWindows()
{
	m_bResizeEnable = true;
	CRect	WindowRect;
	GetClientRect(&WindowRect);
	int m_nWindowWidth_new = WindowRect.Width();
	int m_nWindowHeight_new = WindowRect.Height();

	CWnd *pWnd;
	//pWnd = GetDlgItem(IDC_SCAN_SPEED_SETTING);    //获取控件指针，IDC_BUTTON1为控件ID号
	//CRect rect;    // 获取控件变化前大小
	//pWnd->GetWindowRect(&rect);
	////ScreenToClient(&rect); // 将控件大小转换为在对话框中的区域坐标
	//m_nButtonHeight = rect.Height();
	//m_nButtonWidth = rect.Width();
	//m_nButtonUpleftX = rect.TopLeft().x;
	//m_nButtonUpleftY = rect.TopLeft().y;

	int nButtonNum = 9;
	int nButtonGap = (m_nWindowHeight_new - nButtonNum * m_nButtonHeight - m_nButtonUpleftX) / (nButtonNum - 1);

	//pWnd = GetDlgItem(IDC_SCAN_SPEED_SETTING);    //获取控件指针，IDC_BUTTON1为控件ID号
	//pWnd->SetWindowPos(NULL, m_nButtonUpleftX, m_nButtonUpleftY + 0 * (m_nButtonHeight + nButtonGap), 0, 0, SWP_NOZORDER | SWP_NOSIZE);    
	//CWnd *pWnd1 = GetDlgItem(IDC_FILELIST_PATH);
	//pWnd1->SetWindowPos(NULL, m_nButtonUpleftX, m_nButtonUpleftY + 1 * (m_nButtonHeight + nButtonGap), 0, 0, SWP_NOZORDER | SWP_NOSIZE);    
	//CWnd *pWnd2 = GetDlgItem(IDC_CHECK_BATCH_LOAD);
	//pWnd2->SetWindowPos(NULL, m_nButtonUpleftX, m_nButtonUpleftY + 2 * (m_nButtonHeight + nButtonGap), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	//CWnd *pWnd3 = GetDlgItem(IDC_CHECK_SINGLE_LOAD);
	//pWnd3->SetWindowPos(NULL, m_nButtonUpleftX, m_nButtonUpleftY + 3 * (m_nButtonHeight + nButtonGap), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	//CWnd *pWnd4 = GetDlgItem(IDC_CHECK_PictureSend);
	//pWnd4->SetWindowPos(NULL, m_nButtonUpleftX, m_nButtonUpleftY + 4 * (m_nButtonHeight + nButtonGap), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	//CWnd *pWnd5 = GetDlgItem(IDC_IMAGE_NEXT);
	//pWnd5->SetWindowPos(NULL, m_nButtonUpleftX, m_nButtonUpleftY + 5 * (m_nButtonHeight + nButtonGap), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	//CWnd *pWnd6 = GetDlgItem(IDC_IMAGE_PREVIOUS);
	//pWnd6->SetWindowPos(NULL, m_nButtonUpleftX, m_nButtonUpleftY + 6 * (m_nButtonHeight + nButtonGap), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	//CWnd *pWnd7 = GetDlgItem(IDC_NETWORK_SETTING);
	//pWnd7->SetWindowPos(NULL, m_nButtonUpleftX, m_nButtonUpleftY + 7 * (m_nButtonHeight + nButtonGap), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	//CWnd *pWnd8 = GetDlgItem(IDC_START_NETSERVER);
	//pWnd8->SetWindowPos(NULL, m_nButtonUpleftX, m_nButtonUpleftY + 8 * (m_nButtonHeight + nButtonGap), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	GetDlgItem(IDC_SCAN_SPEED_SETTING)->MoveWindow(m_nButtonUpleftX, m_nButtonUpleftY + 0 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDC_FILELIST_PATH)->MoveWindow(m_nButtonUpleftX, m_nButtonUpleftY + 1 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDC_CHECK_BATCH_LOAD)->MoveWindow(m_nButtonUpleftX, m_nButtonUpleftY + 2 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDC_CHECK_SINGLE_LOAD)->MoveWindow(m_nButtonUpleftX, m_nButtonUpleftY + 3 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDC_CHECK_PictureSend)->MoveWindow(m_nButtonUpleftX, m_nButtonUpleftY + 4 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDC_IMAGE_NEXT)->MoveWindow(m_nButtonUpleftX, m_nButtonUpleftY + 5 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDC_IMAGE_PREVIOUS)->MoveWindow(m_nButtonUpleftX, m_nButtonUpleftY + 6 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDC_NETWORK_SETTING)->MoveWindow(m_nButtonUpleftX, m_nButtonUpleftY + 7 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDC_START_NETSERVER)->MoveWindow(m_nButtonUpleftX, m_nButtonUpleftY + 8 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	
	GetDlgItem(IDOK)->MoveWindow(m_nButtonUpleftX + m_nButtonWidth + (m_nWindowWidth_new - m_nButtonUpleftX - 3*m_nButtonWidth)/3, m_nButtonUpleftY + 8 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDCANCEL)->MoveWindow(m_nWindowWidth_new - m_nButtonUpleftX - m_nButtonWidth - (m_nWindowWidth_new - m_nButtonUpleftX - 3 * m_nButtonWidth) / 3, m_nButtonUpleftY + 8 * (m_nButtonHeight + nButtonGap), m_nButtonWidth, m_nButtonHeight);
	GetDlgItem(IDC_STATIC_IMAGE_SHOW)->MoveWindow(m_nButtonUpleftX + m_nButtonWidth + 20, m_nButtonUpleftY, m_nWindowWidth_new - (m_nButtonUpleftX + m_nButtonWidth + 20)-10, m_nButtonUpleftY + 8 * (m_nButtonHeight + nButtonGap)-20);
	GetDlgItem(IDC_STATIC_ShowPicture)->MoveWindow(m_nButtonUpleftX + m_nButtonWidth + 20, m_nButtonUpleftY, m_nWindowWidth_new - (m_nButtonUpleftX + m_nButtonWidth + 20)-10, m_nButtonUpleftY + 8 * (m_nButtonHeight + nButtonGap) - 20);
	if (m_Checksendimage.GetCheck() == 1)
	{
		GetDlgItem(IDC_STATIC_ShowPicture)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_IMAGE_SHOW)->ShowWindow(FALSE);
	}
	if (m_Checksendimage.GetCheck() == 0)
	{
		GetDlgItem(IDC_STATIC_ShowPicture)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_IMAGE_SHOW)->ShowWindow(TRUE);
	}
	RedrawWindow();
}
void CDataSimulatorDlg::ReSize(int nID)
{
	CRect Rect;
	GetDlgItem(nID)->GetWindowRect(Rect);
	ScreenToClient(Rect);
	//计算控件左上角点 
	CPoint OldTLPoint, TLPoint;
	OldTLPoint = Rect.TopLeft();
	TLPoint.x = long(OldTLPoint.x *m_Multiple_width);
	TLPoint.y = long(OldTLPoint.y * m_Mutiple_heith);

	//计算控件右下角点
	CPoint OldBRPoint, BRPoint;
	OldBRPoint = Rect.BottomRight();
	BRPoint.x = long(OldBRPoint.x *m_Multiple_width);
	BRPoint.y = long(OldBRPoint.y * m_Mutiple_heith);
	//移动控件到新矩形

	Rect.SetRect(TLPoint, BRPoint);
	GetDlgItem(nID)->MoveWindow(Rect, TRUE);
}