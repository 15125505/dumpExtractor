#pragma once
#include "TBuff.h"
#include <vector>
#include <afxmt.h>

class CGetFileOfDump
{
public:
	CGetFileOfDump(void);
	~CGetFileOfDump(void);

	// ִ�в���
	BOOL Act(
		CString strDumpFile,					// windowsת���ļ���
		CString strOutFolder,					// ����ļ���Ŀ¼��
		CString strExt,							// ����ļ�����չ��
		const Tool::TBuff<byte> &bufHead,		// �������ļ���ͷ������
		const Tool::TBuff<byte> &bufEnd			// �������ļ���β������
		);

	// ��ȡ��ǰ�ļ���С
	INT64 GetFileSize() { return m_nFileSize;}

	// ��ȡ��ǰ�Ѿ�������ֽ���
	INT64 GetFinishedSize() { return m_nFinishedSize;}

	// ��ȡ��ǰ�Ѿ�������ļ�����
	int GetOutFileCount() {return m_nOutFileCount;}

	// ֹͣ��ǰ����
	void Stop() 
	{
		AddLog(1, "--ֹͣ...");
		m_bStop = true;
		if (NULL != m_hThread)
		{
			WaitForSingleObject(m_hThread, INFINITE);
			m_hThread = NULL;
		}
	}

	// ��ǰ�Ƿ����ڹ�����
	BOOL IsWorking() {return m_hThread != NULL;}

	// ������־�ڵ�s
	struct LogNode 
	{
		BOOL bTextOnly;	// �Ƿ����֣�������ģʽ�£�����ͷ������ʱ���β�����뻻��
		int nType;		// ��־���ͣ�0.��ͨ 1.��ʾ 2.���� 3.����
		CTime tmTime;	// ��־������ʱ��
		CString strLog;	// ��־�ļ�����
	};

	// ��ȡ����Ĳ�����־
	void GetLog(std::vector<LogNode> &vec)
	{
		vec.clear();
		CSingleLock lock(&m_csLock, TRUE);
		vec.swap(m_vecLog);
	}
	

private:

	// ת���ļ���
	CString m_strDumpFile;

	// ����ļ���Ŀ¼��
	CString m_strOutFolder;

	// ����ļ�����չ��
	CString m_strExt;

	// ��ǰdmp�ļ���
	CString m_strDumpFileName;

	// �ļ�ͷβ����
	Tool::TBuff<byte> m_bufHead, m_bufEnd;

	// ��ʱ������
	Tool::TBuff<byte> m_buffTmp;

	// ��ǰ����ļ�
	Tool::TBuff<byte> m_buffCurFile;

	// ��ǰ�ļ���С
	INT64 m_nFileSize;

	// ��ǰ�Ѿ�������ֽ���
	INT64 m_nFinishedSize;

	// ��ǰ�Ѿ�������ļ�����
	int m_nOutFileCount; 

	// ��ǰ��־
	std::vector<LogNode> m_vecLog;
	CCriticalSection m_csLock;

	// �߳����
	HANDLE m_hThread;
	volatile bool m_bStop;

	// �̺߳���
	static UINT TH_Work(LPVOID lpContext);

	// ʵ�ʹ�������
	void Work();

	// �����Ѷ����ݵ����ݣ��ҵ���Ҫ�����ݲ�ɾ��������������Ч�����ݣ�
	void Process(Tool::TBuff<byte> &buffRead);

	// ���浱ǰ�ļ�
	BOOL Save();

	// �����־
	void AddLog(int nType, CString strText)
	{
		LogNode node;
		node.bTextOnly = false;
		node.nType = nType;
		node.strLog = strText;
		CSingleLock lock(&m_csLock, TRUE);
		if (m_vecLog.size() < 10000)
		{
			m_vecLog.push_back(node);
		}
	}

};
