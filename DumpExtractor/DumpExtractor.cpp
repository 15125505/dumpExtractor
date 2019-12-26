
// DumpExtractor.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "DumpExtractor.h"
#include "DumpExtractorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDumpExtractorApp

BEGIN_MESSAGE_MAP(CDumpExtractorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDumpExtractorApp ����

CDumpExtractorApp::CDumpExtractorApp()
{
}


// Ψһ��һ�� CDumpExtractorApp ����

CDumpExtractorApp theApp;


// CDumpExtractorApp ��ʼ��

BOOL CDumpExtractorApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	SetRegistryKey(_T("�����ĸ���"));

	AfxInitRichEdit2();


	CDumpExtractorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
