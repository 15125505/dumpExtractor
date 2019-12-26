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
	// ��ֹ�ظ�����
	if (NULL != m_hThread)
	{
		Warn("����û�д��������");
		return FALSE;
	}

	// �����û����������
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

	Notice("----��ʼ����...");
	Warn("--�ļ����ͣ�" + strExt);
	Warn("--Dump�ļ���" + strDumpFile);
	Warn("--���Ŀ¼��" + strOutFolder);


	// �����߳̿�ʼ�ɻ�
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
	str.Format("----���������\r\n    �ļ���С - %.3f MB\r\n    �����ļ� - %d ��\r\n    ����ʱ�� - %.3f ��\r\n", pThis->GetFileSize() / 1024.0 / 1024,  pThis->GetOutFileCount(), (GetTickCount() - dwTick) / 1000.0);
	pThis->AddLog(1, str);
	pThis->m_hThread = NULL;
	return 0;
}

void CGetFileOfDump::Work()
{
	// ��ʼ����ǰ����
	m_buffCurFile.clear();
	m_nFileSize = 0;
	m_nFinishedSize = 0;
	m_nOutFileCount = 0;


	// ���ļ�
	FILE *pFile = NULL;
	fopen_s(&pFile, m_strDumpFile, "rb");
	if (NULL == pFile) 
	{
		Err("��ȡdmp�ļ�ʧ�ܣ�");
		return;
	}

	// ��ȡ�ļ���С
	m_nFileSize = _filelengthi64(_fileno(pFile));

	// ��ȡ�ļ�
	byte buff[102400] = {0};
	Tool::TBuff<byte> bufRead;
	while(!m_bStop) {

		// ��ȡָ���ֽ�
		size_t nRead = fread(buff, 1, 102400, pFile);
		if (0 == nRead)
		{
			// �ļ��Ѿ���ȡ���
			break;
		}
		m_nFinishedSize += nRead;

		// ���浱ǰ���ݲ����д���
		bufRead.append(buff, nRead);
		Process(bufRead);
	}
	fclose(pFile);
}

void CGetFileOfDump::Process(Tool::TBuff<byte> &buffRead)
{
	// ������ļ�û���κ��ļ�����ô����Ѱ������ͷ�Ľ׶�
	if (m_buffCurFile.size() == 0)
	{
		// ����ֽڽ��в���
		for (size_t i=0; i + m_bufHead.size() <buffRead.size() + 1; i++)
		{
			// ���û���ҵ������账��
			if (memcmp(&m_bufHead[0], &buffRead[i], m_bufHead.size()) != 0)
			{
				continue;
			}

			// ����ҵ��ˣ���ô��Ҫ���ļ�ͷ�洢����ǰ���������
			m_buffCurFile.append(m_bufHead);

			// ͬʱ����ǰ���������ļ�ͷ�����ݷ���
			m_buffTmp.clear();
			size_t newStartPos = i + m_bufHead.size();
			m_buffTmp.append(&buffRead[newStartPos], buffRead.size() - newStartPos);
			m_buffTmp.swap(buffRead);
			break;
		}

		// ���һֱû���ҵ�����ô��ǰ���������������ݣ���Ҫ����
		if (m_buffCurFile.size() == 0 && buffRead.size() >= m_bufHead.size())
		{
			size_t newStartPos = buffRead.size() - m_bufHead.size();
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
		for (size_t i=0; i + m_bufEnd.size() < buffRead.size() + 1; i++)
		{
			// ���û���ҵ������账��
			if (memcmp(&m_bufEnd[0], &buffRead[i], m_bufEnd.size()) != 0)
			{
				continue;
			}

			// ����ҵ��ˣ���ô��Ҫ��β��֮ǰ������ȫ�����д洢
			bFound = true;
			size_t newStartPos = i + m_bufEnd.size();
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
			Err("д�����ݵ�����ļ�ʧ�ܣ�");
		}
		fclose(pFile);
	}
	else 
	{
		Err("�����ļ�ʧ�ܣ�");
	}
	m_buffCurFile.clear();
	return TRUE;
}
