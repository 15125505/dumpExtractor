#pragma once
#include "TBuff.h"

class CGetFileOfDump
{
public:
	CGetFileOfDump(void);
	~CGetFileOfDump(void);

	// 执行操作
	BOOL Act(CString strDumpFile, CString strOutFolder);

private:

	// Png文件头尾
	Tool::TBuff<byte> m_pngHead, m_pngEnd;

	// 临时缓冲区
	Tool::TBuff<byte> m_buffTmp;

	// 当前输出文件
	Tool::TBuff<byte> m_buffCurFile;

	// 当前文件计数
	int m_nCount; 

	// 输出目录名称
	CString m_strOutFolder;


	// 处理已读内容的数据（找到需要的数据并删除，保留可能有效的数据）
	void Process(Tool::TBuff<byte> &buffRead);

	// 保存当前文件
	BOOL Save();

};
