// DialogNetworkSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "DataSimulator.h"
#include "DialogNetworkSetting.h"
#include "DataSimulatorDlg.h"
#include "afxdialogex.h"

// CDialogNetworkSetting 对话框

IMPLEMENT_DYNAMIC(CDialogNetworkSetting, CDialogEx)

CDialogNetworkSetting::CDialogNetworkSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogNetworkSetting::IDD, pParent)
{

}

CDialogNetworkSetting::~CDialogNetworkSetting()
{
}

void CDialogNetworkSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogNetworkSetting, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDialogNetworkSetting::OnBnClickedOk)
	ON_EN_CHANGE(IDC_SIMULATE_CAMERA, &CDialogNetworkSetting::OnEnChangeSimulateCamera)
END_MESSAGE_MAP()


// CDialogNetworkSetting 消息处理程序

BOOL CDialogNetworkSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_EDIT_ADRESS1)->SetWindowTextA("192.168.1.100");
	GetDlgItem(IDC_EDIT_ADRESS2)->SetWindowTextA("192.168.1.100");
	GetDlgItem(IDC_EDIT_ADRESS_LOCAL)->SetWindowTextA("192.168.1.100");
	GetDlgItem(IDC_EDIT_PORT_NUNBER1)->SetWindowTextA("9712");
	GetDlgItem(IDC_EDIT_PORT_NUNBER2)->SetWindowTextA("9712");
	GetDlgItem(IDC_EDIT_PORT_NUNBER_LOCAL)->SetWindowTextA("9712");

	GetDlgItem(IDC_SIMULATE_CAMERA)->SetWindowTextA("A");
	GetDlgItem(IDC_CAMERA_NUMBER)->SetWindowTextA("1");

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

	GetDlgItem(IDC_EDIT_ADRESS1)->SetWindowTextA(nx_box1_ip.c_str());

	GetDlgItem(IDC_EDIT_PORT_NUNBER1)->SetWindowTextA(std::to_string(nx_box1_port).c_str());

	GetDlgItem(IDC_CAMERA_NUMBER)->SetWindowTextA(std::to_string(camera_serial_number).c_str());  ////IDC_SIMULATE_CAMERA
	if (camera_serial_number == 1)
		GetDlgItem(IDC_SIMULATE_CAMERA)->SetWindowTextA("A");
	else if (camera_serial_number == 2)
		GetDlgItem(IDC_SIMULATE_CAMERA)->SetWindowTextA("B");
	else if (camera_serial_number == 3)
		GetDlgItem(IDC_SIMULATE_CAMERA)->SetWindowTextA("C");
	else if (camera_serial_number == 4)
		GetDlgItem(IDC_SIMULATE_CAMERA)->SetWindowTextA("D");

	return true;
}

/*************************************************************
功能：获得当前程序的运行目录(.exe)
**************************************************************/
CString CDialogNetworkSetting::GetModulePath(void)
{
	CString    sPath;
	GetModuleFileNameA(NULL, sPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	sPath.ReleaseBuffer();
	int    nPos;
	nPos = sPath.ReverseFind('\\');
	sPath = sPath.Left(nPos);
	return   sPath;
}

void CDialogNetworkSetting::OnBnClickedOk()
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

	CString str;
	GetDlgItem(IDC_EDIT_ADRESS1)->GetWindowTextA(str);
	nx_box1_ip = str.GetBuffer();
	GetDlgItem(IDC_EDIT_PORT_NUNBER1)->GetWindowTextA(str);
	nx_box1_port = atoi(str);

	GetDlgItem(IDC_EDIT_ADRESS2)->GetWindowTextA(str);
	nx_box2_ip = str.GetBuffer();
	GetDlgItem(IDC_EDIT_PORT_NUNBER2)->GetWindowTextA(str);
	nx_box2_port = atoi(str);

	GetDlgItem(IDC_EDIT_ADRESS_LOCAL)->GetWindowTextA(str);
	local_ip = str.GetBuffer();
	GetDlgItem(IDC_EDIT_PORT_NUNBER_LOCAL)->GetWindowTextA(str);
	local_ip_port = atoi(str);

	GetDlgItem(IDC_SIMULATE_CAMERA)->GetWindowTextA(str); 
	simulate_camera = str.GetBuffer();

	GetDlgItem(IDC_CAMERA_NUMBER)->GetWindowTextA(str);
	camera_serial_number = atoi(str);

	CString IniWrite;
	IniWrite.Format("%s", nx_box1_ip.c_str());
	WritePrivateProfileString("System", "box1_ip", IniWrite, IniPath);

	IniWrite.Format("%d", nx_box1_port);
	WritePrivateProfileString("System", "box1_port", IniWrite, IniPath);

	IniWrite.Format("%d", camera_serial_number);
	WritePrivateProfileString("System", "camera_serial_number", IniWrite, IniPath);

	CDialogEx::OnOK();
}


void CDialogNetworkSetting::OnEnChangeSimulateCamera()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	GetDlgItem(IDC_SIMULATE_CAMERA)->GetWindowTextA(str);
	if (str == "A" || str =="a")
		GetDlgItem(IDC_CAMERA_NUMBER)->SetWindowTextA("1");
	else if (str == "B" || str == "b")
		GetDlgItem(IDC_CAMERA_NUMBER)->SetWindowTextA("2");
	else if (str == "C" || str == "c")
		GetDlgItem(IDC_CAMERA_NUMBER)->SetWindowTextA("3");
	else if (str == "D" || str == "d")
		GetDlgItem(IDC_CAMERA_NUMBER)->SetWindowTextA("4");
	else 
		GetDlgItem(IDC_CAMERA_NUMBER)->SetWindowTextA("0");
}
