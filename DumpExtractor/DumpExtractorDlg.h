
// DumpExtractorDlg.h : 头文件
//

#pragma once
#include "GetFileOfDump.h"
#include "afxcmn.h"


// CDumpExtractorDlg 对话框
class CDumpExtractorDlg : public CDialog
{
// 构造
public:
	CDumpExtractorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DUMPEXTRACTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnBnClickedSelDmp();
	static int CALLBACK BrowseProc(HWND hwnd, UINT msg, LPARAM lp, LPARAM pData);
	afx_msg void OnBnClickedSelOutput();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// 更新控件使能状态
	void Enable(BOOL bEnable = TRUE);

	// 写入配置
	BOOL WriteCfg(CString strKey, int nValue);
	BOOL WriteCfg(CString strKey, CString strValue);

	// 读取配置
	CString ReadCfg(CString strKey, CString strDefault);
	int ReadCfg(CString strKey, int nDefault = 0);

	// 检查日志
	void CheckLog();

	// 获取临时文件夹
	CString GetTmpFolder();

	// 获取用户配置文件名
	CString GetCfgFilePath();

	// 当前是否在工作中
	BOOL m_bWorking;

	// 当前工作类型
	int m_nType;

	// 当前进度条
	CProgressCtrl m_Progress;

	// 实际处理的模块
	CGetFileOfDump m_hander;

	// 日志控件
	CRichEditCtrl m_ctrlLog;
};
