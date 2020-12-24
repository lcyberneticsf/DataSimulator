#pragma once
#include "afxwin.h"


// CCameraSetting �Ի���

class CCameraSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraSetting)

public:
	CCameraSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCameraSetting();
	virtual BOOL OnInitDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_CAMERA_SEETING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
