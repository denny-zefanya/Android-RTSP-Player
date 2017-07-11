#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <dirent.h>
#include "GAudioSaveFile.h"
#include "../../../framework/GEngineEvent/GEngineEvent.h"

void GAudioSaveFileRunLoop(void *data) { if ( data ) ( ( GAudioSaveFile * )data )->runLoop(); }

void GAudioSaveFile::install()
{
	//��ʼ��ȫ�ֱ���
	mRunning 	= false;
	mActionSeed	= 0;
	mThread		= 0;
	memset( mOldFilePath	, 0, 256  );
	memset( mNewFilePath	, 0, 256  );
	memset( mFileDateBuffer	, 0, 1024 );

	//������
	mLock = mUtility.createLock();
}

void GAudioSaveFile::unInstall()
{
	//�жϵ�ǰ�Ƿ����ڹ���
	bool isRunning = false;
	mUtility.lock( mLock );
	isRunning = mRunning;
	mUtility.unlock( mLock );

	//�ȴ���������
	if ( isRunning ) mUtility.waitingThread( mThread );

	//������
	mUtility.destorylock( &mLock );

	//��ʼ��ȫ�ֱ���
	mRunning 	= false;
	mActionSeed	= 0;
	mThread		= 0;
	memset( mOldFilePath	, 0, 256  );
	memset( mNewFilePath	, 0, 256  );
	memset( mFileDateBuffer	, 0, 1024 );
}

int GAudioSaveFile::saveFile(JNIEnv* env, jstring oldFilePath, jstring newFilePath)
{
	int result = 0;

	do
	{
		//�жϲ����Ƿ���Ч
		if ( !env || !oldFilePath || !newFilePath ) break;

		//�жϵ�ǰ�Ƿ����ڹ���
		bool isRunning = false;
		mUtility.lock( mLock );
		isRunning = mRunning;
		mUtility.unlock( mLock );
		if ( isRunning ) break;

		//�����ļ�·����Ϣ
		memset( mOldFilePath, 0, 256 );
		memset( mNewFilePath, 0, 256 );
		int length0 = mConverter.getStringBytes( env, oldFilePath, mOldFilePath, 256 );
		int length1 = mConverter.getStringBytes( env, newFilePath, mNewFilePath, 256 );
		if ( length0 <= 0 || length1 <= 0 ) break;

		//�����߳�
		if ( !mUtility.createThread( &mThread, GAudioSaveFileRunLoop, this ) ) break;

		//�趨���flag
		mActionSeed++;
		result = mActionSeed;

	} while(0);

	return result;
}

void GAudioSaveFile::runLoop()
{
	//�趨Running
	mUtility.lock( mLock );
	mRunning = true;
	mUtility.unlock( mLock );

	bool result 	= false;
	FILE *oldFile 	= NULL;
	FILE *newFile 	= NULL;

	do
	{
		//�ж��ļ��Ƿ����
		if ( access( mOldFilePath, 0 ) != 0 ) break;

		//ɾ�����ļ�
		if ( access( mNewFilePath, 0 ) == 0 ) { if ( remove( mNewFilePath ) != 0 ) break; }

		//���ļ�
		oldFile = fopen( mOldFilePath, "rb+" );
		newFile = fopen( mNewFilePath, "w+" );

		//�ж��ļ��Ƿ�򿪳ɹ�
		if ( !oldFile || !newFile ) break;

		//ѭ�������ļ�����
		int  copyOutSize = 0;
		int  copyInSize  = 0;
		bool copyResult  = false;
		do
		{
			//�Ӿ��ļ��п���������������
			copyOutSize = fread( mFileDateBuffer, 1, 1024, oldFile );
			if ( copyOutSize <= 0 ) { copyResult = true; break; }

			//�ӻ������п������������ļ�
			copyInSize = fwrite( mFileDateBuffer, 1, copyOutSize, newFile );
			if ( copyInSize != copyOutSize ) break;

		} while( true );

		//�趨���flag
		result = copyResult;

	} while(0);

	//�ر��ļ�
	if ( oldFile ) fclose( oldFile );
	if ( newFile ) fclose( newFile );
	oldFile = 0;
	newFile = 0;

	//ǿ�Ƶȴ�
	usleep( 1000 * 1000 );

	//�����¼�
	int action = mActionSeed;
	GEngineEvent::sharedInstance().swallowEvent( EET_AUDIO_FILE_SAVE_FINISH, ( void * )( &action ), result ? 1 : 0 );

	//�趨Running
	mUtility.lock( mLock );
	mRunning = false;
	mUtility.unlock( mLock );
}


