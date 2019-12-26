/*******************************************************************************
* ��Ȩ���� (C) 2008
* 
* �ļ����ƣ� FileEx.h
* �ļ���ʶ�� 
* ����ժҪ�� �ļ����������ࡣ
* ����˵���� 1�������ṩ�Ĳ�����windows/linux�¶���Ч��
*			 2�����ܲ����������õķ�ʽ��������Щ�ӿڣ�ֻ�ܲ��������������'::'
*				�������ṩ����Щ�ӿڡ�
* ��ǰ�汾�� V1.0
* ��    �ߣ� �ܷ�
* ������ڣ� 2008-01-04
*******************************************************************************/
#ifndef _FILE_EX_75894328849318493216789054320573409
#define _FILE_EX_75894328849318493216789054320573409

#include <vector>
#include <string>

#if (!defined _WINDOWS_) && (!defined _WIN32)
#include <unistd.h>
#include <ftw.h>
#include <sys/vfs.h>
#endif

namespace Tool
{

class CFileEx
{
public:
	//��ȡ�ļ��ָ���������windows����'\\'��linux����'/'��
	static char Separator();

	//��ȡ��ǰĿ¼(����ֵ������ĩβ�ġ�\\����/��)
	static std::string GetCurDirectory();

	//��ȡ��ִ�г�������Ŀ¼(����ֵ������ĩβ�ġ�\\����/��)
	static std::string GetExeDirectory();

	//���õ�ǰ����Ŀ¼
	static bool SetCurDirectory(const char * lpszFolder);

	//����ָ���Ķ༶�ļ�Ŀ¼
	static bool CreateFolder(const char * lpszFolder);

	//Ϊ����ָ�����ļ�������Ҫ���ļ�Ŀ¼
	static bool CreateFolderForFile(const char * lpszFile);

	//��ȡָ��Ŀ¼�µ������ļ���������Ŀ¼��
	static void GetSubFiles(const char * lpszFolder, std::vector<std::string> &vecFile);

	//��ȡָ��Ŀ¼�µ�����Ŀ¼���������ļ���
	static void GetSubFoders(const char * lpszFolder, std::vector<std::string> &vecFolder);

	//����ȫ·����ȡ�ļ���
	static std::string Path2FileName(const char *lpszPath);

	//��ȡ����ʣ��ռ�
	static UINT GetFreeDiskSpace(LPCSTR lpszPath);

	//ɾ��ָ���ļ��У������ļ���������������ݣ�
	static bool DelFolder(LPCSTR lpszFolder);

	//ɾ��ָ�����ļ�
	static bool DelFile(LPCSTR lpszPath);

private:
	CFileEx(void){}
	~CFileEx(void){}

#if (!defined _WINDOWS_) && (!defined _WIN32)
	static int m_snDepth;	//Ŀ¼���
	static bool m_sbFile;	//TRUE for file and FALSE for folder
	static std::vector<std::string> m_svecFile;
	static int FileFunc(const char *file, const struct stat *sb, int flag);
#endif

};

}

#endif
