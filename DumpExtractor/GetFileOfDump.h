#pragma once
#include "TBuff.h"

class CGetFileOfDump
{
public:
	CGetFileOfDump(void);
	~CGetFileOfDump(void);

	// ִ�в���
	BOOL Act(CString strDumpFile, CString strOutFolder);

private:

	// Png�ļ�ͷβ
	Tool::TBuff<byte> m_pngHead, m_pngEnd;

	// ��ʱ������
	Tool::TBuff<byte> m_buffTmp;

	// ��ǰ����ļ�
	Tool::TBuff<byte> m_buffCurFile;

	// ��ǰ�ļ�����
	int m_nCount; 

	// ���Ŀ¼����
	CString m_strOutFolder;


	// �����Ѷ����ݵ����ݣ��ҵ���Ҫ�����ݲ�ɾ��������������Ч�����ݣ�
	void Process(Tool::TBuff<byte> &buffRead);

	// ���浱ǰ�ļ�
	BOOL Save();

};
