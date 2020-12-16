// CameraSetting.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DataSimulator.h"
#include "CameraSetting.h"
#include "afxdialogex.h"
#include <string>
using namespace  std;


// CCameraSetting �Ի���

IMPLEMENT_DYNAMIC(CCameraSetting, CDialogEx)

CCameraSetting::CCameraSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCameraSetting::IDD, pParent)
{
	camera_scan_speed = 10;

}

CCameraSetting::~CCameraSetting()
{
}

void CCameraSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCameraSetting, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCameraSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// CCameraSetting ��Ϣ�������

BOOL CCameraSetting::OnInitDialog()
{

	CDialogEx::OnInitDialog();

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
	CString str;
	str.Format("%.2f", m_fScanSpeed);

	//GetDlgItem(IDC_EDIT_SCAN_SPEED)->SetWindowTextA(std::to_string(m_fScanSpeed).c_str());
	GetDlgItem(IDC_EDIT_SCAN_SPEED)->SetWindowTextA(str);

	return true;
}


void CCameraSetting::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_EDIT_SCAN_SPEED)->GetWindowTextA(str);
	camera_scan_speed = atoi(str);
	m_fScanSpeed = atof(str);

	char IniRead[255];
	memset(IniRead, 0, 255);
	char IniPath[255];
	memset(IniPath, 0, 255);
	//camera_ip_info ci_info;
	CString strPath = GetModulePath();
	strcpy_s(IniPath, 255, strPath.GetBuffer());
	strcat_s(IniPath, "/param.ini");
	CString IniWrite;

	IniWrite.Format("%.2f", m_fScanSpeed);
	WritePrivateProfileString("System", "scan_speed", IniWrite, IniPath);


	CDialogEx::OnOK();
}
/*************************************************************
���ܣ���õ�ǰ���������Ŀ¼(.exe)
**************************************************************/
CString CCameraSetting::GetModulePath(void)
{
	CString    sPath;
	GetModuleFileNameA(NULL, sPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	sPath.ReleaseBuffer();
	int    nPos;
	nPos = sPath.ReverseFind('\\');
	sPath = sPath.Left(nPos);
	return   sPath;
}
