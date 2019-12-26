#include "StdAfx.h"
#include "GetFileOfDump.h"
#include <io.h>
#include "FileEx.h"


#define Log(x) AddLog(0, x)
#define Notice(x) AddLog(1, x)
#define Warn(x) AddLog(2, x)
#define Err(x) AddLog(3, x)

CGetFileOfDump::CGetFileOfDump(void)
{
	m_nFileSize = 0;
	m_nFinishedSize = 0;
	m_nOutFileCount = 0;
	m_hThread = NULL;
	m_bStop = false;
}

CGetFileOfDump::~CGetFileOfDump(void)
{
	Stop();
}

BOOL CGetFileOfDump::Act(CString strDumpFile, CString strOutFolder, CString strExt, const Tool::TBuff<byte> &bufHead, const Tool::TBuff<byte> &bufEnd)
{
	// 防止重复操作
	if (NULL != m_hThread)
	{
		Warn("还有没有处理的任务！");
		return FALSE;
	}

	// 保存用户输入的条件
	m_strDumpFile = strDumpFile;
	m_strDumpFileName = Tool::CFileEx::Path2FileName(strDumpFile).c_str();
	m_strOutFolder = strOutFolder;
	m_strOutFolder.TrimRight("\\");
	m_strExt = strExt;
	m_strExt.TrimLeft(".");
	m_bufHead.clear();
	m_bufHead.append(bufHead);
	m_bufEnd.clear();
	m_bufEnd.append(bufEnd);

	Notice("----开始工作...");
	Warn("--文件类型：" + strExt);
	Warn("--Dump文件：" + strDumpFile);
	Warn("--输出目录：" + strOutFolder);


	// 启动线程开始干活
	m_bStop = false;
	m_hThread = AfxBeginThread(TH_Work, this)->m_hThread;
	return TRUE;
}


UINT CGetFileOfDump::TH_Work(LPVOID lpContext)
{
	CGetFileOfDump *pThis = (CGetFileOfDump *)lpContext;
	DWORD dwTick = GetTickCount();
	pThis->Work();
	CString str;
	str.Format("----任务结束！\r\n    文件大小 - %.3f MB\r\n    导出文件 - %d 个\r\n    花费时间 - %.3f 秒\r\n", pThis->GetFileSize() / 1024.0 / 1024,  pThis->GetOutFileCount(), (GetTickCount() - dwTick) / 1000.0);
	pThis->AddLog(1, str);
	pThis->m_hThread = NULL;
	return 0;
}

void CGetFileOfDump::Work()
{
	// 初始化当前数据
	m_buffCurFile.clear();
	m_nFileSize = 0;
	m_nFinishedSize = 0;
	m_nOutFileCount = 0;


	// 打开文件
	FILE *pFile = NULL;
	fopen_s(&pFile, m_strDumpFile, "rb");
	if (NULL == pFile) 
	{
		Err("读取dmp文件失败！");
		return;
	}

	// 获取文件大小
	m_nFileSize = _filelengthi64(_fileno(pFile));

	// 读取文件
	byte buff[102400] = {0};
	Tool::TBuff<byte> bufRead;
	while(!m_bStop) {

		// 读取指定字节
		size_t nRead = fread(buff, 1, 102400, pFile);
		if (0 == nRead)
		{
			// 文件已经读取完毕
			break;
		}
		m_nFinishedSize += nRead;

		// 保存当前内容并进行处理
		bufRead.append(buff, nRead);
		Process(bufRead);
	}
	fclose(pFile);
}

void CGetFileOfDump::Process(Tool::TBuff<byte> &buffRead)
{
	// 如果但文件没有任何文件，那么处于寻找数据头的阶段
	if (m_buffCurFile.size() == 0)
	{
		// 逐个字节进行查找
		for (size_t i=0; i + m_bufHead.size() <buffRead.size() + 1; i++)
		{
			// 如果没有找到，不予处理
			if (memcmp(&m_bufHead[0], &buffRead[i], m_bufHead.size()) != 0)
			{
				continue;
			}

			// 如果找到了，那么需要将文件头存储到当前输出缓冲区
			m_buffCurFile.append(m_bufHead);

			// 同时，当前缓冲区自文件头的内容放弃
			m_buffTmp.clear();
			size_t newStartPos = i + m_bufHead.size();
			m_buffTmp.append(&buffRead[newStartPos], buffRead.size() - newStartPos);
			m_buffTmp.swap(buffRead);
			break;
		}

		// 如果一直没有找到，那么当前缓冲区的无用数据，需要放弃
		if (m_buffCurFile.size() == 0 && buffRead.size() >= m_bufHead.size())
		{
			size_t newStartPos = buffRead.size() - m_bufHead.size();
			m_buffTmp.clear();
			m_buffTmp.append(&buffRead[newStartPos], buffRead.size() - newStartPos);
			m_buffTmp.swap(buffRead);
		}

	}

	// 如果当前有内容了，那么处于寻找数据尾的阶段
	if (m_buffCurFile.size() > 0)
	{
		// 逐个字节进行查找
		bool bFound = false;
		for (size_t i=0; i + m_bufEnd.size() < buffRead.size() + 1; i++)
		{
			// 如果没有找到，不予处理
			if (memcmp(&m_bufEnd[0], &buffRead[i], m_bufEnd.size()) != 0)
			{
				continue;
			}

			// 如果找到了，那么需要将尾部之前的数据全部进行存储
			bFound = true;
			size_t newStartPos = i + m_bufEnd.size();
			m_buffCurFile.append(&buffRead[0], newStartPos);

			// 保存当前文件
			Save();

			// 同时，当前缓冲区已经处理过的内容放弃
			m_buffTmp.clear();
			m_buffTmp.append(&buffRead[newStartPos], buffRead.size() - newStartPos);
			m_buffTmp.swap(buffRead);
			break;
		}

		// 如果没有找到结尾，那么结尾之前的数据，需要全部存入
		if (!bFound && buffRead.size() >= m_bufEnd.size())
		{
			size_t newStartPos = buffRead.size() - m_bufEnd.size();
			m_buffCurFile.append(&buffRead[0], newStartPos);
			m_buffTmp.clear();
			m_buffTmp.append(&buffRead[newStartPos], buffRead.size() - newStartPos);
			m_buffTmp.swap(buffRead);
		}
	}
}

BOOL CGetFileOfDump::Save()
{
	CString strFileName;
	strFileName.Format("%s\\%s-%04d.%s", m_strOutFolder, m_strDumpFileName, m_nOutFileCount++, m_strExt);
	FILE *pFile = NULL;
	fopen_s(&pFile, strFileName, "w+b");
	if (pFile != NULL)
	{
		size_t ret = fwrite(&m_buffCurFile[0], 1, m_buffCurFile.size(), pFile);
		if (ret < m_buffCurFile.size())
		{
			Err("写入数据到输出文件失败！");
		}
		fclose(pFile);
	}
	else 
	{
		Err("保存文件失败！");
	}
	m_buffCurFile.clear();
	return TRUE;
}
