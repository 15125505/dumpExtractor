
// DumpExtractorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DumpExtractor.h"
#include "DumpExtractorDlg.h"
#include "FileEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


// CDumpExtractorDlg �Ի���

#define APP_NAME		CString("DumpExtractor")		// �������
#define CFG_FILE_NAME	CString("config.ini")			// �����ļ�����
#define FILE_TYPE		CString("FileType")				// Ĭ����ȡ���ļ�����
#define DMP_FILE		CString("DmpFile")				// Ĭ�ϵ�Dmp�ļ�·��
#define OUTPUT_FOLDER	CString("OutputFolder")			// Ĭ�ϵ����Ŀ¼

// ȱʡ���ļ������·��
CString g_InitFolder;


void Test()
{

	// ��ȡ�ļ�
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
			// �ļ�����
			ASSERT(0);
			break;
		}
		if (0xD8FF == wTag)
		{
			// �ļ�ͷ
			continue;
		} 
		else if (0xD9FF == wTag)
		{
			// �ļ�β
			continue;
		} 
		else
		{
			// ��ȡ�����ֽڵ��ļ�����
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


// CDumpExtractorDlg ��Ϣ�������

BOOL CDumpExtractorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// ���ý�������Χ
	m_Progress.SetRange(0, 1000);
	m_Progress.SetPos(0);

	SetTimer(1, 100, NULL);

	// ��ʼ����ʱ������������Ϣ
	m_nType = ReadCfg(FILE_TYPE, 0);
	SetDlgItemText(IDC_DMP, ReadCfg(DMP_FILE, GetTmpFolder() + "chrome.DMP"));
	SetDlgItemText(IDC_OUTPUT, ReadCfg(OUTPUT_FOLDER, Tool::CFileEx::GetExeDirectory().c_str() + CString("\\output")));
	UpdateData(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDumpExtractorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDumpExtractorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDumpExtractorDlg::OnBnClickedOk()
{
	// �����ǰ���ڹ����У���ô��Ҫֹͣ
	if (m_bWorking)
	{
		if (AfxMessageBox("ȷ��Ҫֹͣ��ǰ������", MB_ICONQUESTION | MB_OKCANCEL) == MB_OK)
		{
			return;
		}
		m_hander.Stop();
		return;
	}

	// ��ȡ�û�����
	UpdateData();
	CString strDump, strOutput;
	GetDlgItemText(IDC_DMP, strDump);
	GetDlgItemText(IDC_OUTPUT, strOutput);
	if (strDump.IsEmpty())
	{
		AfxMessageBox("Windowsת���ļ�����Ϊ�գ�", MB_ICONINFORMATION);
		GetDlgItem(IDC_DMP)->SetFocus();
		return;
	}
	if (strOutput.IsEmpty())
	{
		AfxMessageBox("���·������Ϊ�գ�", MB_ICONINFORMATION);
		GetDlgItem(IDC_OUTPUT)->SetFocus();
		return;
	}
	Tool::CFileEx::CreateFolder(strOutput);

	// ��������ȷ��ǰ׺
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
		AfxMessageBox("��������ʱ��֧�֣�", MB_ICONINFORMATION);
		return;
	}

	// ���ÿؼ�
	Enable(FALSE);

	// �����û�����
	WriteCfg(FILE_TYPE, m_nType);
	WriteCfg(DMP_FILE, strDump);
	WriteCfg(OUTPUT_FOLDER, strOutput);


	// ������ʼ����
	m_hander.Act(strDump, strOutput, strExt, bufHead, bufEnd);
	m_bWorking = TRUE;

}

void CDumpExtractorDlg::OnBnClickedCancel()
{
	if (m_bWorking)
	{
		AfxMessageBox("�����в������˳�������ֹͣ��", MB_ICONINFORMATION);
		return;
	}
	OnCancel();
}

void CDumpExtractorDlg::OnBnClickedSelDmp()
{
	CString strDump, strOutput;
	GetDlgItemText(IDC_DMP, strDump);
	CFileDialog dlg(TRUE, ".dmp", strDump, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "Windowsת���ļ�(*.dmp)|*.dmp|�����ļ� (*.*)|*.*||");
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
	bi.lpszTitle		= _T("ѡ����ȡ���ļ�Ҫ��ŵ�Ŀ¼");
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

	// ����Ƿ�����Ҫ�������־
	CheckLog();

	// ����״̬
	CString str;
	str.Format("�Ѵ���%dMb(��%dMb)���ɹ���ȡ%d���ļ���", 
		int(m_hander.GetFinishedSize() / 1024 / 1024),
		int(m_hander.GetFileSize() / 1024 / 1024),
		m_hander.GetOutFileCount());
	SetDlgItemText(IDC_TIP, str);
	if (m_hander.GetFileSize() > 0)
	{
		m_Progress.SetPos(UINT(m_hander.GetFinishedSize() * 1000 / m_hander.GetFileSize()));
	}


	// ��鵱ǰ�����Ƿ��Ѿ����
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
	SetDlgItemText(IDOK, bEnable ? "��ʼ(&S)" : "ֹͣ(&S)");
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
	//��ȡ��ǰ����������
	static std::vector<CGetFileOfDump::LogNode> vec;
	m_hander.GetLog(vec);
	if (vec.empty())
	{
		return;
	}

	//����ѡ��λ��
	CHARRANGE cr;
	m_ctrlLog.GetSel(cr);

	//���ô�����ı���ʽ
	CHARFORMAT cf = {0};
	cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE;
	cf.yHeight	= 180;
	sprintf_s(cf.szFaceName, "����");
	m_ctrlLog.SetSel(-1, -1);
	COLORREF aColor[] = 
	{
		RGB(0, 0, 255),
		RGB(0, 150, 0),
		RGB(112, 48, 160),
		RGB(48, 112, 160),
	};

	//��һ������־��Ϣ
	m_ctrlLog.SetRedraw(FALSE);
	for (size_t i=0; i<vec.size(); i++)
	{
		const CGetFileOfDump::LogNode &node = vec[i];

		// ɾ���������
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

		// ����ʱ����Ϣ
		if (!node.bTextOnly)
		{
			cf.crTextColor = aColor[0];
			m_ctrlLog.SetSelectionCharFormat(cf);
			m_ctrlLog.ReplaceSel(CTime::GetCurrentTime().Format("%m-%d %H:%M:%S  "));
		}

		// ������־����
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

	//�趨���λ��
	if (cr.cpMin != cr.cpMax)
	{
		//�ָ��û�ѡ��λ��
		m_ctrlLog.SetSel(cr);
	}
	else
	{
		//����������λ��
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
