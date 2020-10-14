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

enum{	LOG_COMM,		// 正常
		LOG_WARN,		// 警告
		LOG_ERROR,		// エラー
};	

class TEcLog
{
private:
	CRITICAL_SECTION cs;						//零界区 書きと読みを守る

protected:
	long			m_lFileSize;				// ログデータサイズ
	char			m_szLogPath[512];			// ログ保存パス
    bool            m_bSave;

	// ファイルサイズを得る関数
	const int GetLogFileSize()
	{
		FILE* fpSize;
		fopen_s(&fpSize, m_szLogPath, "rb");

		if (!fpSize) return 0;	//ファイルが存在しない場合

		fseek(fpSize, 0L, SEEK_END);

		long	lFileSize = ftell(fpSize);
		fclose(fpSize);

		return lFileSize;
	}

	
	// ログ内容の書き込む
	void LogOut(char* szLog)
	{
		EnterCriticalSection (&cs);			//まもる開始

		// ログ時間
		//time_t t = time(0);
		//char szTime[64];
		//strftime(szTime, 64, "%Y-%m-%d %H:%M:%S",localtime(&t));
        char szTime[64];
        SYSTEMTIME sSysTime;
        GetLocalTime (&sSysTime);
        sprintf_s(szTime, "%02d-%02d %02d:%02d:%02d.%03d\0",
						 sSysTime.wMonth, sSysTime.wDay,
						 sSysTime.wHour, sSysTime.wMinute, sSysTime.wSecond, sSysTime.wMilliseconds);

		long	lLastPos;// 前回書いた後のPOSITION 
		
		// ファイルサイズ
		long lFileSize = GetLogFileSize();
		
		FILE* fp;
		if(lFileSize == 0)//  ファイルサイズがゼロの場合
		{
			fopen_s(&fp,m_szLogPath, "a+");
			if (fp == NULL)
				return;
			
			// 今のファイル偏移量をファイルに書き込み 
			lLastPos = 0;
			fprintf (fp, "%08d\n", lLastPos);
			fseek (fp, 0, SEEK_END);

			// 情報をファイルに書き込み 
			fprintf(fp, "%s:%s\r\n",szTime, szLog);
			fclose (fp);

			// 今のファイル偏移量をファイルに書き込む
			fopen_s(&fp,m_szLogPath, "r+");
			if (fp != NULL) {

				// == 今のファイルサイズが制限数を超えるかどうか判断する == //
				fseek (fp, 0, SEEK_END);
				lLastPos = ftell (fp);

				fseek (fp, 0, SEEK_SET);
				fprintf (fp, "%08d\n", lLastPos);
				fclose(fp);
			}
		}
		else if (lFileSize <= m_lFileSize)  // ファイルサイズが制限数以内場合 
		{
			fopen_s(&fp,m_szLogPath, "a+");
			if (fp == NULL)
				return;
			fseek (fp, 0, SEEK_END);

			//  情報をファイルに書き込み 
			fprintf(fp, "%s:%s\r\n",szTime, szLog);
			fclose (fp);

			// 今のファイル偏移量をファイルに書き込む
			fopen_s(&fp,m_szLogPath, "r+");
			if (fp != NULL) {
				// ファイル偏移量を取得する 
				fseek (fp, 0, SEEK_END);
				lLastPos = ftell (fp);

				// ファイルの最初所に偏移量を書き込み 
				fseek (fp, 0, SEEK_SET);
				fprintf (fp, "%08d\n", lLastPos);
				fclose(fp);
			}
		}
		else // ファイルサイズが制限数に超える場合
		{	
			fopen_s(&fp,m_szLogPath, "r+");
			if (fp == NULL)
				return;
			char szLineBuf[64];
			// 一行目のデータを取得する 
			fseek (fp, 0, SEEK_SET);
			fgets (szLineBuf, 64, fp);
			lLastPos = atol (szLineBuf);
			if (lLastPos >= m_lFileSize) 
			{

				// ファイル最後の部本の情報がSPACEに代わります 
				fseek (fp, 0, SEEK_END);
				long lEndPos = ftell (fp);
				if (lEndPos > lLastPos) 
				{
					// 残り部分に対して代わり文字列を作成 
					char * pFillLastPart;
					pFillLastPart = new char [lEndPos-lLastPos+1];
					memset (pFillLastPart, 0x00, lEndPos-lLastPos+1);
					memset (pFillLastPart, 0x20, lEndPos-lLastPos);

					fseek (fp, lLastPos, SEEK_SET);
					fwrite (pFillLastPart, 1, lEndPos-lLastPos, fp);
					delete []pFillLastPart;
				}

				// ファイルの偏移量が最初に設定する 
				lLastPos = 10;
			}
			fseek (fp, lLastPos, SEEK_SET);

			//  情報をファイルに書き込み 
			fprintf(fp, "%s:%s\r\n",szTime, szLog);
			
			//  今のファイル偏移量がファイルに書き込む 
			lLastPos = ftell (fp);
			fseek (fp, 0, SEEK_SET);
			fprintf (fp, "%08d\n", lLastPos);
			fclose(fp);			
		}
		
		LeaveCriticalSection(&cs);			//守る完了
		
	}


public:
	// 構造関数
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
		strcpy_s(m_szLogPath, szPath);					// 一般のパス
		m_lFileSize = lFileSize*1024;					// 1kのサイズ
		InitializeCriticalSection(&cs);					//零界区の初期化
	}

	~TEcLog()
	{
		DeleteCriticalSection (&cs);
	}

	// ログファイルを保存する？
	void SetSave(bool bSave)
	{
        m_bSave = bSave;
	}

	// ログファイルサイズの設定
	void SetFileSize(long lSize)
	{
		m_lFileSize = lSize*1024;
	}

	// ログファイルパスの設定
	void SetPath(char* strLogPath)
	{
		strcpy_s(m_szLogPath, strLogPath);
	}

	// メッセージをプリントする
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
