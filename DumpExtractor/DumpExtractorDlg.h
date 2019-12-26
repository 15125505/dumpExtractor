
// DumpExtractorDlg.h : ͷ�ļ�
//

#pragma once
#include "GetFileOfDump.h"
#include "afxcmn.h"


// CDumpExtractorDlg �Ի���
class CDumpExtractorDlg : public CDialog
{
// ����
public:
	CDumpExtractorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DUMPEXTRACTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

	// ���¿ؼ�ʹ��״̬
	void Enable(BOOL bEnable = TRUE);

	// д������
	BOOL WriteCfg(CString strKey, int nValue);
	BOOL WriteCfg(CString strKey, CString strValue);

	// ��ȡ����
	CString ReadCfg(CString strKey, CString strDefault);
	int ReadCfg(CString strKey, int nDefault = 0);

	// �����־
	void CheckLog();

	// ��ȡ��ʱ�ļ���
	CString GetTmpFolder();

	// ��ȡ�û������ļ���
	CString GetCfgFilePath();

	// ��ǰ�Ƿ��ڹ�����
	BOOL m_bWorking;

	// ��ǰ��������
	int m_nType;

	// ��ǰ������
	CProgressCtrl m_Progress;

	// ʵ�ʴ����ģ��
	CGetFileOfDump m_hander;

	// ��־�ؼ�
	CRichEditCtrl m_ctrlLog;
};
