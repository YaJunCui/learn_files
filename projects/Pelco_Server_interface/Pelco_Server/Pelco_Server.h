
// Pelco_Server.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPelco_ServerApp: 
// �йش����ʵ�֣������ Pelco_Server.cpp
//

class CPelco_ServerApp : public CWinApp
{
public:
	CPelco_ServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()

};

extern CPelco_ServerApp theApp;