
// DataSimulatorDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
//io_service* CDataSimulatorDlg::ios[8] = {nullptr};

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	virtual BOOL OnInitDialog();
	void  GetVersion(CString &ver);

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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
	strVersionInfo.Format("�汾��:%s", strVersion);
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowTextA(strVersionInfo);
	return true;
}
void CAboutDlg::GetVersion(CString &ver)
{
	DWORD dwInfoSize = 0;
	TCHAR exePath[MAX_PATH];
	memset(exePath, 0, sizeof(exePath));

	ver.Format(_T("V1.00"));

	// �õ����������·��
	GetModuleFileName(NULL, exePath, sizeof(exePath) / sizeof(TCHAR));

	// �ж��Ƿ��ܻ�ȡ�汾��
	dwInfoSize = GetFileVersionInfoSize(exePath, NULL);

	if (dwInfoSize == 0)
	{
		::OutputDebugString("GetFileVersionInfoSize fail\r\n");
	}
	else
	{
		BYTE* pData = new BYTE[dwInfoSize];

		// ��ȡ�汾��Ϣ
		if (!GetFileVersionInfo(exePath, NULL, dwInfoSize, pData))
		{
			::OutputDebugString("GetFileVersionInfo fail\r\n");
		}
		else
		{
			// ��ѯ�汾��Ϣ�еľ����ֵ
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

// CDataSimulatorDlg �Ի���



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

	//sockets[0] = new tcp::socket(*ios[0]);//��socket->io_service��������Ϊ��Ӧ��ios[i]����VCameraSession<Derived>::sendMessage(VCameraMessage && msg)��Ҫ�õ�io_service��������Ϣ
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
END_MESSAGE_MAP()


// CDataSimulatorDlg ��Ϣ�������

BOOL CDataSimulatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_hWnd = AfxGetMainWnd()->m_hWnd;            //To get the main Window Handle;
	m_hMainWnd = m_hWnd;
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

	SetClinetMode();
	OnBnClickedCheckBatchLoad();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDataSimulatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
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
		CString strText = "��γ���˼�����ݷ������";
		PushTailShowText(strText);
		CRect DrawRect(0, 0, 300, 50);
		ShowText(m_pwnd, m_strShowText, DrawRect);

		strText.Format("���ɨ���ٶ�: %dK", camera_scan_speed);
		PushTailShowText(strText);
		ShowText(m_pwnd, m_strShowText, nullptr);

		strText.Format("��������ַ:%s,�˿ں�:%d", parameters_setting_trans.local_ip, parameters_setting_trans.local_port);
		PushTailShowText(strText);
		ShowText(m_pwnd, m_strShowText, nullptr);

		//OnBnClickedCheckBatchLoad();
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDataSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDataSimulatorDlg::OnBnClickedNetworkSetting()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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

	switch (camera_serial_number)
	{
	case 1:
		SetWindowTextA("��γ����ģ���������A");
		break;
	case 2:
		SetWindowTextA("��γ����ģ���������B");
		break;
	case 3:
		SetWindowTextA("��γ����ģ���������C");
		break;
	case 4:
		SetWindowTextA("��γ����ģ���������D");
		break;
	default:
		break;
	}

}
void  CDataSimulatorDlg::io_thread_fun_run(int beg, int end)
{
	io_service* io = ios[beg];
	while (1)
	{
		connect_and_reconnect(beg, end, *io);
		io->run();			//ֻ����IO.run()���к�socket�ſ�ʼ������
	}
	//return true;

}

bool  CDataSimulatorDlg::io_thread_fun(int beg, int end, io_service& io)
{
	while (1)
	{
		connect_and_reconnect(beg, end, io);
		io.run();			//ֻ����IO.run()���к�socket�ſ�ʼ������
	}
	return true;

}


void CDataSimulatorDlg::OnBnClickedImageSendingStart()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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

		if (!sessions[i])		//�����һ��ִ��sessions[i]��û�г�ʼ������������µĳ�ʼ������
		{						//������ǵ�һ��ִ�У����������벻ִ��
			sessions[i] = new Vsee::VTx2Session(*socket);

			sessions[i]->setProcessFrameFunc([&, i](VCameraFrame&& frame) //��sessions[i]->_process_frame_func����Ϊ����{ ...}�е�ִ�д���,�������Ƕ���_process_frame_func��
			{								 //�ڴ�ʱ����ִ��_process_frame_func()����������VCameraSession::asyncRecv()->VTx2Session::ProcessMessage()����������ʱ�Ż�ִ��

				//MutexLock lock(infer_mutex);
				infer_queue.push(InferFrame(sessions[i], std::move(frame)));//����һ��InferFrame,InferFrame._session=sessions[i],InferFrame._frame=frame,����InferFrame;
				//push��infer_queue��
				VCameraFrame _frame;
			});

			sessions[i]->setSessionAbortedFunc([&, i, beg, end]()
			{

				connect_and_reconnect(beg, end, io);		//������Ƕ����ƣ���connect_and_reconnect����ִ�������Ĵ��뻹��Ҫ����ִ�е�
			});
		}

		if (socket->is_open())//���socket�Ѿ��򿪲����ӣ�������Ժ���룬��������ѭ��
			continue;

		asio::error_code ec;
		socket->cancel(ec);

		socket->async_connect(eps[i], [&, i, beg, end](asio::error_code ec)//��һ��ִ�У�����socket�����Ӳ���
		{
			if (ec)//ִ��socket->cancel(ec)����socket��û��ʼ����
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

		if (!sessions[i])		//�����һ��ִ��sessions[i]��û�г�ʼ������������µĳ�ʼ������
		{						//������ǵ�һ��ִ�У����������벻ִ��
			sessions[i] = new Vsee::VTx2Session(*socket);

			sessions[i]->setProcessFrameFunc([&, i](VCameraFrame&& frame) //��sessions[i]->_process_frame_func����Ϊ����{ ...}�е�ִ�д���,�������Ƕ���_process_frame_func��
			{								 //�ڴ�ʱ����ִ��_process_frame_func()����������VCameraSession::asyncRecv()->VTx2Session::ProcessMessage()����������ʱ�Ż�ִ��
				
				MessageBox("setProcessFrameFunc");
				//MutexLock lock(infer_mutex);
				infer_queue.push(InferFrame(sessions[i], std::move(frame)));//����һ��InferFrame,InferFrame._session=sessions[i],InferFrame._frame=frame,����InferFrame;
				//push��infer_queue��
				VCameraFrame _frame;
			});

			sessions[i]->setSessionAbortedFunc([&, i, beg, end]()
			{

				connect_and_reconnect(beg, end, io);		//������Ƕ����ƣ���connect_and_reconnect����ִ�������Ĵ��뻹��Ҫ����ִ�е�
			});
		}

		if (socket->is_open())//���socket�Ѿ��򿪲����ӣ�������Ժ���룬��������ѭ��
			continue;

		asio::error_code ec;
		socket->cancel(ec);

		socket->async_connect(eps, [&, i, beg, end](asio::error_code ec)//��һ��ִ�У�����socket�����Ӳ���
		{
			if (ec)//ִ��socket->cancel(ec)����socket��û��ʼ����
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
		io->run();			//ֻ����IO.run()���к�socket�ſ�ʼ������
	//	sessions[0]->startSession();

	}
}
/*************************************************************
���ܣ���õ�ǰ���������Ŀ¼(.exe)
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	if (!m_bClientMode)
	{
		CString strText;
		strText.Format("���ɨ���ٶ�: %dK", camera_scan_speed);
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL isOpen = TRUE;		        //�Ƿ��(����Ϊ����)
	CString defaultDir = "F:\\";	//Ĭ�ϴ򿪵��ļ�·��
	CString fileName = "";			//Ĭ�ϴ򿪵��ļ���
	CString filter = "�ļ� (*.bmp; *.jpg; *.png)|*.bmp;*.jpg;*.png||";	//�ļ����ǵ�����
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = "";
	if (!(result == IDOK))
	{
		MessageBox("���ļ�ʧ�ܣ�");
		return;
	}
	filePath = openFileDlg.GetPathName();
	file_load_path = filePath.GetBuffer();
	communicator_->get_file_list(file_load_path);
}


void CDataSimulatorDlg::OnBnClickedCheckBatchLoad()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int n = m_checkBatchLoad.GetCheck();
	if (m_checkBatchLoad.GetCheck() == 1)
	{
		m_checkSingleLoad.SetCheck(false);
		aq::Communicator::batch_image_load = true;
		aq::Communicator::single_image_load = false;
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
	}		
	else
	{
		m_checkSingleLoad.SetCheck(true);
		aq::Communicator::batch_image_load = false;
		aq::Communicator::single_image_load = true;
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
		SetWindowTextA("��γ����ģ���������A");
		break;
	case 2:
		SetWindowTextA("��γ����ģ���������B");
		break;
	case 3:
		SetWindowTextA("��γ����ģ���������C");
		break;
	case 4:
		SetWindowTextA("��γ����ģ���������D");
		break;
	default:
		break;
	}
		
}


void CDataSimulatorDlg::OnBnClickedCheckSingleLoad()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int n = m_checkSingleLoad.GetCheck();
	if (m_checkSingleLoad.GetCheck() == 1)
	{
		m_checkBatchLoad.SetCheck(false);
		aq::Communicator::batch_image_load = false;
		aq::Communicator::single_image_load = true;
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(true);
	}	
	else
	{
		m_checkBatchLoad.SetCheck(true);
		aq::Communicator::batch_image_load = true;
		aq::Communicator::single_image_load = false;
		GetDlgItem(IDC_IMAGE_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_IMAGE_PREVIOUS)->EnableWindow(false);
	}
		
}


void CDataSimulatorDlg::OnBnClickedImageNext()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	Communicator::single_image_load = true;
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CCameraSetting dlg;
	if (dlg.DoModal() != IDOK)
		return;

	camera_scan_speed = dlg.camera_scan_speed;
	m_fScanSpeed = dlg.m_fScanSpeed;
	communicator_->camera_scan_speed = m_fScanSpeed;
}


void    CDataSimulatorDlg::PushTailShowText(CString strText)
{
	int i = 0;
	bool bPush = false;
	for (i = 0; i < SHOWTEXTLENGTH; i++)
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
		for (i = 0; i < SHOWTEXTLENGTH - 1; i++)
		{
			strcpy(m_strShowText[i], m_strShowText[i + 1]);
		}
		strcpy(m_strShowText[i], strText);
		return;
	}
}

void CDataSimulatorDlg::RefreshWnd(CWnd *pwnd)
{

	//CWnd *pwnd = GetDlgItem(IDC_ShowImage);
	CRect PictureRect;
	pwnd->GetClientRect(&PictureRect);
	int nWidth = PictureRect.Width();
	int nHeight = PictureRect.Height();
	CClientDC pDc(pwnd);
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));//����һ�����ʹ���
	CBrush brush;
	brush.CreateSolidBrush(RGB(BACK_COLOR_GRAY, BACK_COLOR_GRAY, BACK_COLOR_GRAY));   //BACK_COLOR_GRAY: Window default color;

	//	pDc.DrawText(strText, DrawRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //��ʾ�ı�

	pDc.FillRect(PictureRect, &brush);

	POINT pt1;
	pt1.x = 0; pt1.y = 0;
	POINT pt2;
	pt2.x = 100;// (100, 100);
	pt2.y = 100;
	pDc.SelectObject(&pen);//ѡ�񻭱�
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

LRESULT CDataSimulatorDlg::ShowCtrlMessage(WPARAM wParam, LPARAM lParam)
{
	RefreshWnd(m_pwnd);   //clear the print words from the show window;
	CString strMsg;
	int nCtrlSignalling = (int)wParam;
	Vsee::VCameraMessageTrans *MagneticVolveMsg = nullptr;

	//AfxMessageBox(strMsg);

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
	CPen pen(PS_SOLID, 5, RGB(255, 0, 0));//����һ�����ʹ���	
	m_nRectWidth = m_ImageRect.Width();
	m_nRectHeight = m_ImageRect.Height();

	pDc.SetTextColor(RGB(0, 255, 0));
	//	pDc.DrawText(strText, DrawRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //��ʾ�ı�

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
	//pDc.TextOut(0, 0, "д��");
	char str[5][255];
	memset(str[0], 0, 255);
	strcpy(str[0], strText);

	pDc.DrawText(str[0], DrawRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //��ʾ�ı�

	POINT pt1;
	pt1.x = 0; pt1.y = 0;
	POINT pt2;
	pt2.x = 100;// (100, 100);
	pt2.y = 100;
	//pDc.SelectObject(&pen);//ѡ�񻭱�
	//pDc.MoveTo(pt1);
	//pDc.LineTo(pt2);	
}

void CDataSimulatorDlg::ShowText(CWnd *pwnd, char strText[SHOWTEXTLENGTH][255], CRect DrawRect)
{

	//CWnd *pwnd = GetDlgItem(IDC_ShowImage);


	CClientDC pDc(pwnd);
	CPen pen(PS_SOLID, 5, RGB(255, 0, 0));//����һ�����ʹ���	
	m_nRectWidth = m_ImageRect.Width();
	m_nRectHeight = m_ImageRect.Height();

	pDc.SetTextColor(RGB(0, 255, 0));
	//	pDc.DrawText(strText, DrawRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //��ʾ�ı�

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
	//pDc.TextOut(0, 0, "д��");
	int i = 0;
	for (i = 0; i < SHOWTEXTLENGTH; i++)
	{
		if (m_strShowText[i][0] != '\0')
		{
			CRect TextRect(10, i * 50, 600, (i + 1) * 50);
			pDc.DrawText(m_strShowText[i], TextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); //��ʾ�ı�
		}
	}
}
