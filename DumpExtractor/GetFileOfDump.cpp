#include "StdAfx.h"
#include "GetFileOfDump.h"
#include <io.h>

CGetFileOfDump::CGetFileOfDump(void)
{
	// 初始化png文件头和文件尾
	byte start[] = { 0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a, 0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52 };
	byte end[] = { 0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82 };
	m_pngHead.append(start, sizeof(start));
	m_pngEnd.append(end, sizeof(end));

	m_nCount = 0;
}

CGetFileOfDump::~CGetFileOfDump(void)
{
}

BOOL CGetFileOfDump::Act(CString strDumpFile, CString strOutFolder)
{
	// 存储输出文件名
	m_strOutFolder = strOutFolder;
	m_strOutFolder.TrimRight("\\");

	// 打开文件
	FILE *pFile = NULL;
	fopen_s(&pFile, strDumpFile, "rb");
	if (NULL == pFile) 
	{
		return FALSE;
	}

	// 获取文件大小
	__int64 nFileLen = _filelengthi64(_fileno(pFile));
	TRACE("---文件大小: %I64d\n", nFileLen);

	// 读取文件
	byte buff[102400] = {0};
	Tool::TBuff<byte> bufRead;
	UINT64 nCount = 0;
	DWORD dwTick = GetTickCount();
	while(1) {

		// 读取指定字节
		size_t nRead = fread(buff, 1, 102400, pFile);
		if (0 == nRead)
		{
			// 文件已经读取完毕
			break;
		}

		nCount += nRead;
		if (GetTickCount() - dwTick > 1000)
		{
			dwTick = GetTickCount();
			TRACE("---当前已经处理：%I64d MB\n", nCount / 1024 / 1024);
		}

		// 保存当前内容并进行处理
		//bufRead.append(buff, nRead);
		//Process(bufRead);
	}
	fclose(pFile);

	return TRUE;
}

void CGetFileOfDump::Process(Tool::TBuff<byte> &buffRead)
{
	// 如果但文件没有任何文件，那么处于寻找数据头的阶段
	if (m_buffCurFile.size() == 0)
	{
		// 逐个字节进行查找
		for (size_t i=0; i + m_pngHead.size() <buffRead.size() + 1; i++)
		{
			// 如果没有找到，不予处理
			if (memcmp(&m_pngHead[0], &buffRead[i], m_pngHead.size()) != 0)
			{
				continue;
			}

			// 如果找到了，那么需要将文件头存储到当前输出缓冲区
			m_buffCurFile.append(m_pngHead);

			// 同时，当前缓冲区自文件头的内容放弃
			m_buffTmp.clear();
			size_t newStartPos = i + m_pngHead.size();
			m_buffTmp.append(&buffRead[newStartPos], buffRead.size() - newStartPos);
			m_buffTmp.swap(buffRead);
			break;
		}

		// 如果一直没有找到，那么当前缓冲区的无用数据，需要放弃
		if (m_buffCurFile.size() == 0 && buffRead.size() >= m_pngHead.size())
		{
			size_t newStartPos = buffRead.size() - m_pngHead.size();
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
		for (size_t i=0; i + m_pngEnd.size() < buffRead.size() + 1; i++)
		{
			// 如果没有找到，不予处理
			if (memcmp(&m_pngEnd[0], &buffRead[i], m_pngEnd.size()) != 0)
			{
				continue;
			}

			// 如果找到了，那么需要将尾部之前的数据全部进行存储
			bFound = true;
			size_t newStartPos = i + m_pngEnd.size();
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
		if (!bFound && buffRead.size() >= m_pngEnd.size())
		{
			size_t newStartPos = buffRead.size() - m_pngEnd.size();
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
	strFileName.Format("%s\\%04d.png", m_strOutFolder, m_nCount++);
	TRACE("---开始保存文件 -- %s\n", strFileName);

	FILE *pFile = NULL;
	fopen_s(&pFile, strFileName, "w+b");
	if (pFile != NULL)
	{
		fwrite(&m_buffCurFile[0], 1, m_buffCurFile.size(), pFile);
		fclose(pFile);
	}
	m_buffCurFile.clear();
	return TRUE;
}
