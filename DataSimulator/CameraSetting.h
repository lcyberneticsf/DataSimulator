#pragma once


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
};
