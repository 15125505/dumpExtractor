
// DumpExtractorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DumpExtractor.h"
#include "DumpExtractorDlg.h"
#include "FileEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDumpExtractorDlg 对话框

#define APP_NAME		CString("DumpExtractor")		// 软件名称
#define CFG_FILE_NAME	CString("config.ini")			// 配置文件名称
#define FILE_TYPE		CString("FileType")				// 默认提取的文件类型
#define DMP_FILE		CString("DmpFile")				// 默认的Dmp文件路径
#define OUTPUT_FOLDER	CString("OutputFolder")			// 默认的输出目录

// 缺省的文件夹浏览路径
CString g_InitFolder;


void Test()
{

	// 读取文件
	FILE *pFile = NULL;
	fopen_s(&pFile, "D:\\terr.jpg", "rb");
	if (NULL == pFile)
	{
		return;
	}



	while (1)
	{
		WORD wTag;
		if (fread(&wTag, 1, 2, pFile) != 2)
		{
			break;
		}
		if (LOBYTE(wTag) != 0xFF)
		{
			// 文件错误
			ASSERT(0);
			break;
		}
		if (0xD8FF == wTag)
		{
			// 文件头
			continue;
		} 
		else if (0xD9FF == wTag)
		{
			// 文件尾
			continue;
		} 
		else
		{
			// 读取两个字节的文件长度
			WORD wLen;
			if (fread(&wLen, 1, 2, pFile) != 2)
			{
				ASSERT(0);
				break;
			}
			wLen = htons(wLen);
			if (wLen < 2)
			{
				ASSERT(0);
				break;
			}
			fseek(pFile, wLen - 2, SEEK_CUR);
			TRACE("-- %04X : %d\n", htons(wTag), wLen);
		}
	}

	fclose(pFile);

}


CDumpExtractorDlg::CDumpExtractorDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDumpExtractorDlg::IDD, pParent)
, m_nType(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bWorking = FALSE;

	//Test();
}

void CDumpExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_TYPE, m_nType);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_LOG, m_ctrlLog);
}

BEGIN_MESSAGE_MAP(CDumpExtractorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDumpExtractorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDumpExtractorDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SEL_DMP, &CDumpExtractorDlg::OnBnClickedSelDmp)
	ON_BN_CLICKED(IDC_SEL_OUTPUT, &CDumpExtractorDlg::OnBnClickedSelOutput)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDumpExtractorDlg 消息处理程序

BOOL CDumpExtractorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 设置进度条范围
	m_Progress.SetRange(0, 1000);
	m_Progress.SetPos(0);

	SetTimer(1, 100, NULL);

	// 初始化的时候，载入配置信息
	m_nType = ReadCfg(FILE_TYPE, 0);
	SetDlgItemText(IDC_DMP, ReadCfg(DMP_FILE, GetTmpFolder() + "chrome.DMP"));
	SetDlgItemText(IDC_OUTPUT, ReadCfg(OUTPUT_FOLDER, Tool::CFileEx::GetExeDirectory().c_str() + CString("\\output")));
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDumpExtractorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDumpExtractorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDumpExtractorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDumpExtractorDlg::OnBnClickedOk()
{
	// 如果当前正在工作中，那么需要停止
	if (m_bWorking)
	{
		if (AfxMessageBox("确定要停止当前工作吗？", MB_ICONQUESTION | MB_OKCANCEL) == MB_OK)
		{
			return;
		}
		m_hander.Stop();
		return;
	}

	// 获取用户数据
	UpdateData();
	CString strDump, strOutput;
	GetDlgItemText(IDC_DMP, strDump);
	GetDlgItemText(IDC_OUTPUT, strOutput);
	if (strDump.IsEmpty())
	{
		AfxMessageBox("Windows转储文件不能为空！", MB_ICONINFORMATION);
		GetDlgItem(IDC_DMP)->SetFocus();
		return;
	}
	if (strOutput.IsEmpty())
	{
		AfxMessageBox("输出路径不能为空！", MB_ICONINFORMATION);
		GetDlgItem(IDC_OUTPUT)->SetFocus();
		return;
	}
	Tool::CFileEx::CreateFolder(strOutput);

	// 根据类型确定前缀
	Tool::TBuff<byte> bufHead, bufEnd;
	CString strExt;
	if (0 == m_nType)
	{
		strExt = "png";
		byte head[] = {	0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a, 0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52 };
		byte end[] = { 	0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82 };
		bufHead.append(head, sizeof head);
		bufEnd.append(end, sizeof end);
	}
	else if (1 == m_nType)
	{
		strExt = "jpg";
		byte head[] = {	0xff,0xd8,0xff,0xe0,0x00,0x10,0x4a,0x46  };
		byte end[] = { 	0xff, 0xd9 };
		bufHead.append(head, sizeof head);
		bufEnd.append(end, sizeof end);
	}
	else
	{
		AfxMessageBox("该类型暂时不支持！", MB_ICONINFORMATION);
		return;
	}

	// 禁用控件
	Enable(FALSE);

	// 保存用户输入
	WriteCfg(FILE_TYPE, m_nType);
	WriteCfg(DMP_FILE, strDump);
	WriteCfg(OUTPUT_FOLDER, strOutput);


	// 启动开始工作
	m_hander.Act(strDump, strOutput, strExt, bufHead, bufEnd);
	m_bWorking = TRUE;

}

void CDumpExtractorDlg::OnBnClickedCancel()
{
	if (m_bWorking)
	{
		AfxMessageBox("操作中不允许退出，请先停止！", MB_ICONINFORMATION);
		return;
	}
	OnCancel();
}

void CDumpExtractorDlg::OnBnClickedSelDmp()
{
	CString strDump, strOutput;
	GetDlgItemText(IDC_DMP, strDump);
	CFileDialog dlg(TRUE, ".dmp", strDump, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "Windows转储文件(*.dmp)|*.dmp|所有文件 (*.*)|*.*||");
	if (dlg.DoModal() != IDOK) 
	{
		return;
	}
	SetDlgItemText(IDC_DMP, dlg.GetPathName());
}

int  CALLBACK   CDumpExtractorDlg::BrowseProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{  
	if (uMsg == BFFM_INITIALIZED)  
	{  
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCSTR)g_InitFolder);  
	}  
	return  0;  
}  


void CDumpExtractorDlg::OnBnClickedSelOutput()
{
	GetDlgItemText(IDC_OUTPUT, g_InitFolder);
	CString str;
	BROWSEINFO bi = {0};
	TCHAR name[_MAX_PATH] = {0};
	bi.hwndOwner = GetSafeHwnd();
	bi.pszDisplayName	= name;
	bi.lpszTitle		= _T("选择提取的文件要存放的目录");
	bi.ulFlags			= BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn				= BrowseProc;  
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if(idl == NULL || !SHGetPathFromIDList(idl, str.GetBufferSetLength(_MAX_PATH)))
	{
		return;
	}
	str.ReleaseBuffer();
	SetDlgItemText(IDC_OUTPUT, str);
}

void CDumpExtractorDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);

	// 检查是否有需要处理的日志
	CheckLog();

	// 更新状态
	CString str;
	str.Format("已处理%dMb(共%dMb)，成功提取%d个文件。", 
		int(m_hander.GetFinishedSize() / 1024 / 1024),
		int(m_hander.GetFileSize() / 1024 / 1024),
		m_hander.GetOutFileCount());
	SetDlgItemText(IDC_TIP, str);
	if (m_hander.GetFileSize() > 0)
	{
		m_Progress.SetPos(UINT(m_hander.GetFinishedSize() * 1000 / m_hander.GetFileSize()));
	}


	// 检查当前工作是否已经完成
	if (m_bWorking && !m_hander.IsWorking())
	{
		m_bWorking = FALSE;
		Enable(TRUE);
	}
}

void CDumpExtractorDlg::Enable(BOOL bEnable /*= TRUE*/)
{
	GetDlgItem(IDC_TYPE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO2)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO3)->EnableWindow(bEnable);
	GetDlgItem(IDC_DMP)->EnableWindow(bEnable);
	GetDlgItem(IDC_SEL_DMP)->EnableWindow(bEnable);
	GetDlgItem(IDC_OUTPUT)->EnableWindow(bEnable);
	GetDlgItem(IDC_SEL_OUTPUT)->EnableWindow(bEnable);
	GetDlgItem(IDCANCEL)->EnableWindow(bEnable);
	SetDlgItemText(IDOK, bEnable ? "开始(&S)" : "停止(&S)");
	GetDlgItem(IDC_TIP)->ShowWindow(bEnable ? SW_HIDE : SW_SHOW);
}

BOOL CDumpExtractorDlg::WriteCfg(CString strKey, int nValue)
{
	CString strValue;
	strValue.Format("%d", nValue);
	return WriteCfg(strKey, strValue);
}

BOOL CDumpExtractorDlg::WriteCfg(CString strKey, CString strValue)
{
	CString strFile = GetCfgFilePath();
	Tool::CFileEx::CreateFolderForFile(strFile);
	return WritePrivateProfileString(APP_NAME, strKey, strValue, strFile);
}

CString CDumpExtractorDlg::ReadCfg(CString strKey, CString strDefault)
{
	CString strRet;
	GetPrivateProfileString(APP_NAME, strKey, strDefault, strRet.GetBufferSetLength(512), 512, GetCfgFilePath());
	strRet.ReleaseBuffer();
	return strRet;
}

int CDumpExtractorDlg::ReadCfg(CString strKey, int nDefault /*= 0*/)
{
	CString strRet = ReadCfg(strKey, "");
	if (!strRet.IsEmpty())
	{
		nDefault = atoi(strRet);
	}
	return nDefault;
}

void CDumpExtractorDlg::CheckLog()
{
	//获取当前待处理数据
	static std::vector<CGetFileOfDump::LogNode> vec;
	m_hander.GetLog(vec);
	if (vec.empty())
	{
		return;
	}

	//保存选中位置
	CHARRANGE cr;
	m_ctrlLog.GetSel(cr);

	//设置待输出文本格式
	CHARFORMAT cf = {0};
	cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE;
	cf.yHeight	= 180;
	sprintf_s(cf.szFaceName, "宋体");
	m_ctrlLog.SetSel(-1, -1);
	COLORREF aColor[] = 
	{
		RGB(0, 0, 255),
		RGB(0, 150, 0),
		RGB(112, 48, 160),
		RGB(48, 112, 160),
	};

	//逐一处理日志信息
	m_ctrlLog.SetRedraw(FALSE);
	for (size_t i=0; i<vec.size(); i++)
	{
		const CGetFileOfDump::LogNode &node = vec[i];

		// 删除多余的行
		while (m_ctrlLog.GetLineCount() > 5000)
		{
			int nPos = m_ctrlLog.LineLength(0);
			m_ctrlLog.SetSel(0, nPos + 1);
			m_ctrlLog.ReplaceSel("");
			cr.cpMax -= nPos + 1;
			cr.cpMin -= nPos + 1;
			if (cr.cpMax < 0)
			{
				cr.cpMax = 0;
			}
			if (cr.cpMin < 0)
			{
				cr.cpMin = 0;
			}
			m_ctrlLog.SetSel(-1, -1);
		}

		// 插入时间信息
		if (!node.bTextOnly)
		{
			cf.crTextColor = aColor[0];
			m_ctrlLog.SetSelectionCharFormat(cf);
			m_ctrlLog.ReplaceSel(CTime::GetCurrentTime().Format("%m-%d %H:%M:%S  "));
		}

		// 插入日志内容
		cf.crTextColor = aColor[node.nType % 4];
		m_ctrlLog.SetSelectionCharFormat(cf);
		m_ctrlLog.ReplaceSel(node.strLog);
		if (!node.bTextOnly)
		{
			m_ctrlLog.ReplaceSel("\r\n");
		}
	}
	m_ctrlLog.SetRedraw(TRUE);
	m_ctrlLog.Invalidate();
	vec.clear();

	//设定光标位置
	if (cr.cpMin != cr.cpMax)
	{
		//恢复用户选中位置
		m_ctrlLog.SetSel(cr);
	}
	else
	{
		//滚动到最新位置
		m_ctrlLog.LineScroll(m_ctrlLog.GetLineCount() - 1);
	}
}

CString CDumpExtractorDlg::GetTmpFolder()
{
	CString strRet;
	GetTempPath(_MAX_PATH, strRet.GetBufferSetLength(_MAX_PATH));
	strRet.ReleaseBuffer();
	return strRet;
}

CString CDumpExtractorDlg::GetCfgFilePath()
{
	CString strRet;
	SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, strRet.GetBufferSetLength(MAX_PATH) );
	strRet.ReleaseBuffer();
	strRet += "\\";
	strRet += APP_NAME;
	strRet += "\\";
	strRet += CFG_FILE_NAME;
	return strRet;
}
