#pragma once
#include "afxwin.h"


// CCameraSetting 对话框

class CCameraSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraSetting)

public:
	CCameraSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCameraSetting();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_CAMERA_SEETING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString		GetModulePath(void);
	int camera_scan_speed;
	float m_fScanSpeed;
	afx_msg void OnEnChangeEditScanSpeed();
	CEdit m_ChoosSspeed;
	CFont font1;
	CFont font2;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
