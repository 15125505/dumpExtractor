
// DumpExtractor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDumpExtractorApp:
// �йش����ʵ�֣������ DumpExtractor.cpp
//

class CDumpExtractorApp : public CWinApp
{
public:
	CDumpExtractorApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDumpExtractorApp theApp;