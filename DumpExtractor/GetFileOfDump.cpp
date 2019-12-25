#include "StdAfx.h"
#include "GetFileOfDump.h"
#include <io.h>

CGetFileOfDump::CGetFileOfDump(void)
{
	// ��ʼ��png�ļ�ͷ���ļ�β
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
	// �洢����ļ���
	m_strOutFolder = strOutFolder;
	m_strOutFolder.TrimRight("\\");

	// ���ļ�
	FILE *pFile = NULL;
	fopen_s(&pFile, strDumpFile, "rb");
	if (NULL == pFile) 
	{
		return FALSE;
	}

	// ��ȡ�ļ���С
	__int64 nFileLen = _filelengthi64(_fileno(pFile));
	TRACE("---�ļ���С: %I64d\n", nFileLen);

	// ��ȡ�ļ�
	byte buff[102400] = {0};
	Tool::TBuff<byte> bufRead;
	UINT64 nCount = 0;
	DWORD dwTick = GetTickCount();
	while(1) {

		// ��ȡָ���ֽ�
		size_t nRead = fread(buff, 1, 102400, pFile);
		if (0 == nRead)
		{
			// �ļ��Ѿ���ȡ���
			break;
		}

		nCount += nRead;
		if (GetTickCount() - dwTick > 1000)
		{
			dwTick = GetTickCount();
			TRACE("---��ǰ�Ѿ�����%I64d MB\n", nCount / 1024 / 1024);
		}

		// ���浱ǰ���ݲ����д���
		//bufRead.append(buff, nRead);
		//Process(bufRead);
	}
	fclose(pFile);

	return TRUE;
}

void CGetFileOfDump::Process(Tool::TBuff<byte> &buffRead)
{
	// ������ļ�û���κ��ļ�����ô����Ѱ������ͷ�Ľ׶�
	if (m_buffCurFile.size() == 0)
	{
		// ����ֽڽ��в���
		for (size_t i=0; i + m_pngHead.size() <buffRead.size() + 1; i++)
		{
			// ���û���ҵ������账��
			if (memcmp(&m_pngHead[0], &buffRead[i], m_pngHead.size()) != 0)
			{
				continue;
			}

			// ����ҵ��ˣ���ô��Ҫ���ļ�ͷ�洢����ǰ���������
			m_buffCurFile.append(m_pngHead);

			// ͬʱ����ǰ���������ļ�ͷ�����ݷ���
			m_buffTmp.clear();
			size_t newStartPos = i + m_pngHead.size();
			m_buffTmp.append(&buffRead[newStartPos], buffRead.size() - newStartPos);
			m_buffTmp.swap(buffRead);
			break;
		}

		// ���һֱû���ҵ�����ô��ǰ���������������ݣ���Ҫ����
		if (m_buffCurFile.size() == 0 && buffRead.size() >= m_pngHead.size())
		{
			size_t newStartPos = buffRead.size() - m_pngHead.size();
			m_buffTmp.clear();
			m_buffTmp.append(&buffRead[newStartPos], buffRead.size() - newStartPos);
			m_buffTmp.swap(buffRead);
		}

	}

	// �����ǰ�������ˣ���ô����Ѱ������β�Ľ׶�
	if (m_buffCurFile.size() > 0)
	{
		// ����ֽڽ��в���
		bool bFound = false;
		for (size_t i=0; i + m_pngEnd.size() < buffRead.size() + 1; i++)
		{
			// ���û���ҵ������账��
			if (memcmp(&m_pngEnd[0], &buffRead[i], m_pngEnd.size()) != 0)
			{
				continue;
			}

			// ����ҵ��ˣ���ô��Ҫ��β��֮ǰ������ȫ�����д洢
			bFound = true;
			size_t newStartPos = i + m_pngEnd.size();
			m_buffCurFile.append(&buffRead[0], newStartPos);

			// ���浱ǰ�ļ�
			Save();

			// ͬʱ����ǰ�������Ѿ�����������ݷ���
			m_buffTmp.clear();
			m_buffTmp.append(&buffRead[newStartPos], buffRead.size() - newStartPos);
			m_buffTmp.swap(buffRead);
			break;
		}

		// ���û���ҵ���β����ô��β֮ǰ�����ݣ���Ҫȫ������
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
	TRACE("---��ʼ�����ļ� -- %s\n", strFileName);

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
