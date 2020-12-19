#pragma once
#include <string> 
using namespace std;

// CDialogNetworkSetting 对话框

class CDialogNetworkSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogNetworkSetting)

public:
	CDialogNetworkSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogNetworkSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_NETWORK_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	std::string nx_box1_ip;//computer 1 network address
	std::string nx_box2_ip;//computer 2 network address
	std::string local_ip;  //local computer ip adress
	int nx_box1_port;		//NX computer 1 port number
	int nx_box2_port;		//NX computer 2 port number 
	int local_ip_port;		//local computer port number 
	std::string simulate_camera;    //Simulate camera serial number: A---1,B---2,C---3, D---4
	int		camera_serial_number;	//Simulate camera serial number: A---1,B---2,C---3, D---4
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeSimulateCamera();
	CString		GetModulePath(void);
};
