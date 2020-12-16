
// DataSimulatorDlg.h : 头文件
//

#pragma once
#include <condition_variable>
#include <iostream>
#include <string> 
#include "VCameraFrame.h"
#include <Windows.h>
#include "execute.h"
#include "rw_ini.h"
#include <opencv2\opencv.hpp>
#include "VTx2Session.h"
#include "communicator.h"
#include "afxwin.h"
#define  SHOWTEXTLENGTH 6
using namespace std;
using namespace aq;
using namespace Vsee;



// CDataSimulatorDlg 对话框
class CDataSimulatorDlg : public CDialogEx
{
// 构造
public:
	CDataSimulatorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DATASIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedStartNetserver();
	afx_msg void OnBnClickedNetworkSetting();
	afx_msg void OnBnClickedImageSendingStart();
	DECLARE_MESSAGE_MAP()
public:
	std::string nx_box1_ip;//computer 1 network address
	std::string nx_box2_ip;//computer 2 network address
	int nx_box1_port;		//NX computer 1 port number
	int nx_box2_port;		//NX computer 2 port number 
	std::string local_ip;   //local computer ip adress
	int local_ip_port;      //local computer ip port;
	std::string simulate_camera;    //Simulate camera serial number: A---1,B---2,C---3, D---4
	int		camera_serial_number;	//Simulate camera serial number: A---1,B---2,C---3, D---4
	//aq::Execute execute1;
	//aq::Execute execute2;
	aq:: Communicator*			communicator_;

	Vsee::VTx2Session*			sessions[8];
	tcp::socket*				sockets[8];
	io_service*					ios[8];
	static std::queue<InferFrame>		infer_queue;
	std::mutex					infer_mutex;
	InferFrame					infer_frame;
	tcp::endpoint				eps[8];
	
	std::string					ip_adress;
	int							ip_port;
	std::vector<std::string> m_FileList;
	std::string file_load_path;

	bool	connect_and_reconnect(int beg, int end, asio::io_service& io);
	bool	connect_and_reconnect(int beg, int end, asio::io_service& io, tcp::socket* _socket, tcp::endpoint eps);
	bool	io_thread_fun(int beg, int end, io_service& io);
	void	io_thread_fun_run(int beg, int end);
	void	tcp_server(std::string _ip, int _port);
	void	run_tcp_server(std::string _ip, int _port);
	void	run_tcp_client(std::string _ip, int _port);
	void	run_tcp_client(std::string _ip, int _port, int camera_number);
	void	tcp_client(std::string _ip, int port, io_service* io);	
	void    SetClinetMode();
	void	RefreshWnd(CWnd *pwnd);
	void    PushTailShowText(CString strText);
	CString		GetModulePath(void);


	bool	m_bClientMode;
	bool	m_bServerMode;
	bool    m_bServerSendMode;
	bool    m_bClientSendMode;
	CButton m_checkBatchLoad;
	CButton m_checkSingleLoad;

	CButton m_checkServerSend;
	CButton m_checkClientSend;
	CButton m_checkServerModeSelect;
	CButton	m_checkClientMode;
	bool    m_bNetParametersSetting;
	int     camera_scan_speed;    //Camera scan speed,the unit is K.

	int		m_nRectWidth;
	int		m_nRectHeight;
	CRect	m_ImageRect;			//The area th show image
	CDC		*m_pDC;
	HDC		m_hDC;
	CWnd	*m_pwnd;
	char    m_strShowText[SHOWTEXTLENGTH][255];
	static HWND m_hWnd;				//=AfxGetMainWnd()->m_hWnd;
	CParameterSetting			parameters_setting_trans;
	float   m_fScanSpeed;
	
	afx_msg void OnBnClickedFilelistPath();
	
	afx_msg void OnBnClickedCheckBatchLoad();
	afx_msg void OnBnClickedCheckSingleLoad();
	afx_msg void OnBnClickedImageNext();
	
	afx_msg void OnBnClickedCheckServerSend();
	
	afx_msg void OnBnClickedCheckServerModeSelect();
	afx_msg void OnBnClickedCheckModeSelect();	
	afx_msg void OnBnClickedCheckClientSend();
	afx_msg void OnBnClickedScanSpeedSetting();
	afx_msg LRESULT ShowCtrlMessage(WPARAM wParam, LPARAM lParam);
	void	ShowText(CWnd *pwnd, CString strText, CRect DrawRect);
	void	ShowText(CWnd *pwnd, char strText[SHOWTEXTLENGTH][255], CRect DrawRect);
};
