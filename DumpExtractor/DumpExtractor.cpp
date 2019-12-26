
// DumpExtractor.cpp : 定义应用程序的类行为。
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


// CDumpExtractorApp 构造

CDumpExtractorApp::CDumpExtractorApp()
{
}


// 唯一的一个 CDumpExtractorApp 对象

CDumpExtractorApp theApp;


// CDumpExtractorApp 初始化

BOOL CDumpExtractorApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	SetRegistryKey(_T("船长的港湾"));

	AfxInitRichEdit2();


	CDumpExtractorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
