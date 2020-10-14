////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Log File Class
// Create by SHENLI [2009-7-6]
// For Windows
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef EcLogH
#define EcLogH

#include	<string>
#include	<time.h>
#include	<stdarg.h>
#include	<stdio.h>
#include	<windows.h>

enum{	LOG_COMM,		// ����
		LOG_WARN,		// �x��
		LOG_ERROR,		// �G���[
};	

class TEcLog
{
private:
	CRITICAL_SECTION cs;						//��E�� �����Ɠǂ݂����

protected:
	long			m_lFileSize;				// ���O�f�[�^�T�C�Y
	char			m_szLogPath[512];			// ���O�ۑ��p�X
    bool            m_bSave;

	// �t�@�C���T�C�Y�𓾂�֐�
	const int GetLogFileSize()
	{
		FILE* fpSize;
		fopen_s(&fpSize, m_szLogPath, "rb");

		if (!fpSize) return 0;	//�t�@�C�������݂��Ȃ��ꍇ

		fseek(fpSize, 0L, SEEK_END);

		long	lFileSize = ftell(fpSize);
		fclose(fpSize);

		return lFileSize;
	}

	
	// ���O���e�̏�������
	void LogOut(char* szLog)
	{
		EnterCriticalSection (&cs);			//�܂���J�n

		// ���O����
		//time_t t = time(0);
		//char szTime[64];
		//strftime(szTime, 64, "%Y-%m-%d %H:%M:%S",localtime(&t));
        char szTime[64];
        SYSTEMTIME sSysTime;
        GetLocalTime (&sSysTime);
        sprintf_s(szTime, "%02d-%02d %02d:%02d:%02d.%03d\0",
						 sSysTime.wMonth, sSysTime.wDay,
						 sSysTime.wHour, sSysTime.wMinute, sSysTime.wSecond, sSysTime.wMilliseconds);

		long	lLastPos;// �O�񏑂������POSITION 
		
		// �t�@�C���T�C�Y
		long lFileSize = GetLogFileSize();
		
		FILE* fp;
		if(lFileSize == 0)//  �t�@�C���T�C�Y���[���̏ꍇ
		{
			fopen_s(&fp,m_szLogPath, "a+");
			if (fp == NULL)
				return;
			
			// ���̃t�@�C���Έڗʂ��t�@�C���ɏ������� 
			lLastPos = 0;
			fprintf (fp, "%08d\n", lLastPos);
			fseek (fp, 0, SEEK_END);

			// �����t�@�C���ɏ������� 
			fprintf(fp, "%s:%s\r\n",szTime, szLog);
			fclose (fp);

			// ���̃t�@�C���Έڗʂ��t�@�C���ɏ�������
			fopen_s(&fp,m_szLogPath, "r+");
			if (fp != NULL) {

				// == ���̃t�@�C���T�C�Y���������𒴂��邩�ǂ������f���� == //
				fseek (fp, 0, SEEK_END);
				lLastPos = ftell (fp);

				fseek (fp, 0, SEEK_SET);
				fprintf (fp, "%08d\n", lLastPos);
				fclose(fp);
			}
		}
		else if (lFileSize <= m_lFileSize)  // �t�@�C���T�C�Y���������ȓ��ꍇ 
		{
			fopen_s(&fp,m_szLogPath, "a+");
			if (fp == NULL)
				return;
			fseek (fp, 0, SEEK_END);

			//  �����t�@�C���ɏ������� 
			fprintf(fp, "%s:%s\r\n",szTime, szLog);
			fclose (fp);

			// ���̃t�@�C���Έڗʂ��t�@�C���ɏ�������
			fopen_s(&fp,m_szLogPath, "r+");
			if (fp != NULL) {
				// �t�@�C���Έڗʂ��擾���� 
				fseek (fp, 0, SEEK_END);
				lLastPos = ftell (fp);

				// �t�@�C���̍ŏ����ɕΈڗʂ��������� 
				fseek (fp, 0, SEEK_SET);
				fprintf (fp, "%08d\n", lLastPos);
				fclose(fp);
			}
		}
		else // �t�@�C���T�C�Y���������ɒ�����ꍇ
		{	
			fopen_s(&fp,m_szLogPath, "r+");
			if (fp == NULL)
				return;
			char szLineBuf[64];
			// ��s�ڂ̃f�[�^���擾���� 
			fseek (fp, 0, SEEK_SET);
			fgets (szLineBuf, 64, fp);
			lLastPos = atol (szLineBuf);
			if (lLastPos >= m_lFileSize) 
			{

				// �t�@�C���Ō�̕��{�̏��SPACE�ɑ���܂� 
				fseek (fp, 0, SEEK_END);
				long lEndPos = ftell (fp);
				if (lEndPos > lLastPos) 
				{
					// �c�蕔���ɑ΂��đ��蕶������쐬 
					char * pFillLastPart;
					pFillLastPart = new char [lEndPos-lLastPos+1];
					memset (pFillLastPart, 0x00, lEndPos-lLastPos+1);
					memset (pFillLastPart, 0x20, lEndPos-lLastPos);

					fseek (fp, lLastPos, SEEK_SET);
					fwrite (pFillLastPart, 1, lEndPos-lLastPos, fp);
					delete []pFillLastPart;
				}

				// �t�@�C���̕Έڗʂ��ŏ��ɐݒ肷�� 
				lLastPos = 10;
			}
			fseek (fp, lLastPos, SEEK_SET);

			//  �����t�@�C���ɏ������� 
			fprintf(fp, "%s:%s\r\n",szTime, szLog);
			
			//  ���̃t�@�C���Έڗʂ��t�@�C���ɏ������� 
			lLastPos = ftell (fp);
			fseek (fp, 0, SEEK_SET);
			fprintf (fp, "%08d\n", lLastPos);
			fclose(fp);			
		}
		
		LeaveCriticalSection(&cs);			//��銮��
		
	}


public:
	// �\���֐�
	TEcLog(bool bSave=true, long lFileSize=128, char* szPath="D:\\Project\\Log.txt")
	{
		int nLen=strlen(szPath)-1;
		for(int i=nLen;i>0;i--)
		{
			if(szPath[i]=='\\')
			{
				char szTmp[512];
				memset(szTmp,0x00,sizeof(szTmp));
				//wcscpy(szTmp,szPath,i+1);
				CString sTmp;
				sTmp.Format(_T("%f"),*szPath);
				CreateDirectoryW(sTmp,NULL);
				break;
			}
		}
        m_bSave = bSave;
		strcpy_s(m_szLogPath, szPath);					// ��ʂ̃p�X
		m_lFileSize = lFileSize*1024;					// 1k�̃T�C�Y
		InitializeCriticalSection(&cs);					//��E��̏�����
	}

	~TEcLog()
	{
		DeleteCriticalSection (&cs);
	}

	// ���O�t�@�C����ۑ�����H
	void SetSave(bool bSave)
	{
        m_bSave = bSave;
	}

	// ���O�t�@�C���T�C�Y�̐ݒ�
	void SetFileSize(long lSize)
	{
		m_lFileSize = lSize*1024;
	}

	// ���O�t�@�C���p�X�̐ݒ�
	void SetPath(char* strLogPath)
	{
		strcpy_s(m_szLogPath, strLogPath);
	}

	// ���b�Z�[�W���v�����g����
	void Print(char* szFormat, ...)
	{
        if(!m_bSave) return;
		char szMsg[1024]={0};
		va_list vas;
		va_start(vas, szFormat);
		vsprintf_s(szMsg, szFormat, vas);
		LogOut(szMsg);
        va_end(vas);
	}
};

#endif
