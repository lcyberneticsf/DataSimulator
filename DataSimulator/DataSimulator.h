
// DataSimulator.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDataSimulatorApp: 
// �йش����ʵ�֣������ DataSimulator.cpp
//

class CDataSimulatorApp : public CWinApp
{
public:
	CDataSimulatorApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDataSimulatorApp theApp;