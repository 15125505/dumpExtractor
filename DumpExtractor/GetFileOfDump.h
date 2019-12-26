#pragma once
#include "TBuff.h"
#include <vector>
#include <afxmt.h>

class CGetFileOfDump
{
public:
	CGetFileOfDump(void);
	~CGetFileOfDump(void);

	// 执行操作
	BOOL Act(
		CString strDumpFile,					// windows转储文件名
		CString strOutFolder,					// 输出文件的目录名
		CString strExt,							// 输出文件的扩展名
		const Tool::TBuff<byte> &bufHead,		// 该类型文件的头部特征
		const Tool::TBuff<byte> &bufEnd			// 该类型文件的尾部特征
		);

	// 获取当前文件大小
	INT64 GetFileSize() { return m_nFileSize;}

	// 获取当前已经处理的字节数
	INT64 GetFinishedSize() { return m_nFinishedSize;}

	// 获取当前已经输出的文件个数
	int GetOutFileCount() {return m_nOutFileCount;}

	// 停止当前操作
	void Stop() 
	{
		AddLog(1, "--停止...");
		m_bStop = true;
		if (NULL != m_hThread)
		{
			WaitForSingleObject(m_hThread, INFINITE);
			m_hThread = NULL;
		}
	}

	// 当前是否正在工作中
	BOOL IsWorking() {return m_hThread != NULL;}

	// 操作日志节点s
	struct LogNode 
	{
		BOOL bTextOnly;	// 是否纯文字，纯文字模式下，不在头部加入时间和尾部加入换行
		int nType;		// 日志类型：0.普通 1.提示 2.警告 3.错误
		CTime tmTime;	// 日志创建的时间
		CString strLog;	// 日志文件内容
	};

	// 读取最近的操作日志
	void GetLog(std::vector<LogNode> &vec)
	{
		vec.clear();
		CSingleLock lock(&m_csLock, TRUE);
		vec.swap(m_vecLog);
	}
	

private:

	// 转储文件名
	CString m_strDumpFile;

	// 输出文件的目录名
	CString m_strOutFolder;

	// 输出文件的扩展名
	CString m_strExt;

	// 当前dmp文件名
	CString m_strDumpFileName;

	// 文件头尾特征
	Tool::TBuff<byte> m_bufHead, m_bufEnd;

	// 临时缓冲区
	Tool::TBuff<byte> m_buffTmp;

	// 当前输出文件
	Tool::TBuff<byte> m_buffCurFile;

	// 当前文件大小
	INT64 m_nFileSize;

	// 当前已经处理的字节数
	INT64 m_nFinishedSize;

	// 当前已经输出的文件个数
	int m_nOutFileCount; 

	// 当前日志
	std::vector<LogNode> m_vecLog;
	CCriticalSection m_csLock;

	// 线程相关
	HANDLE m_hThread;
	volatile bool m_bStop;

	// 线程函数
	static UINT TH_Work(LPVOID lpContext);

	// 实际工作函数
	void Work();

	// 处理已读内容的数据（找到需要的数据并删除，保留可能有效的数据）
	void Process(Tool::TBuff<byte> &buffRead);

	// 保存当前文件
	BOOL Save();

	// 添加日志
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
