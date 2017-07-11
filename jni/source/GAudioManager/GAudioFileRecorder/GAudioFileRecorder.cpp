#include "GAudioFileRecorder.h"
#include "../GAudioEngine/GAudioEngine.h"
#include "../../../framework/GEngineEvent/GEngineEvent.h"

void GAudioFileRecorderRunLoop(void *data) { if ( data ) ( ( GAudioFileRecorder * )data )->runLoop(); }

void GAudioFileRecorder::install()
{
	//��ʼ��ȫ�ֱ���
	mRunning		= false;
	mActionSeed		= 0;
	mThread			= 0;
	memset( mAudioQueueBuffer, 0, 256 );
	memset( mFilePath		 , 0, 256 );

	//������
	mLock = mUtility.createLock();
}

void GAudioFileRecorder::unInstall()
{
	//ֹͣ����
	bool isRunning = false;
	mUtility.lock( mLock );
	isRunning = mRunning;
	mRunning = false;
	mUtility.unlock( mLock );
	if ( isRunning ) mUtility.waitingThread( mThread );

	//������
	mUtility.destorylock( &mLock );

	//��ʼ��ȫ�ֱ���
	mRunning		= false;
	mActionSeed		= 0;
	mThread			= 0;
	memset( mAudioQueueBuffer, 0, 256 );
	memset( mFilePath		 , 0, 256 );
}

int	 GAudioFileRecorder::start(JNIEnv* env, jstring path)
{
	int result = 0;

	do
	{
		//�жϲ����Ƿ���Ч
		if ( !env || !path ) break;

		//�жϵ�ǰ�Ƿ����ڹ���
		bool isRunning = false;
		mUtility.lock( mLock );
		isRunning = mRunning;
		if ( !isRunning ) mRunning = true;
		mUtility.unlock( mLock );
		if ( isRunning ) break;

		//�����ļ�·����Ϣ
		memset( mFilePath, 0, 256 );
		int length = mConverter.getStringBytes( env, path, mFilePath, 256 );
		if ( length <= 0 ) break;

		//���Դ��ļ�
		FILE *file = fopen( mFilePath, "w+" );
		if ( file ) fclose( file );
		else break;

		//�����߳�
		if ( !mUtility.createThread( &mThread, GAudioFileRecorderRunLoop, this ) ) break;

		//�趨���flag
		mActionSeed++;
		result = mActionSeed;

	} while(0);

	//�ж��Ƿ�ɹ�����
	if ( result <= 0 ) stop();

	return result;
}

void GAudioFileRecorder::stop()
{
	//�趨flag
	mUtility.lock( mLock );
	mRunning = false;
	mUtility.unlock( mLock );
}

void GAudioFileRecorder::runLoop()
{
	//���ɿ�ʼ�¼�
	GEngineEvent::sharedInstance().swallowEvent( EET_AUDIO_FILE_RECORD_START, 0, mActionSeed );

	//�趨״̬flag
	mUtility.lock( mLock );
	mRunning = true;
	mUtility.unlock( mLock );

	//��������
	FILE *file 		= 0;
	int	 readsize	= 0;
	int	 savesize	= 0;
	bool isRunning 	= false;

	do
	{
		//���ļ�
		file = fopen( mFilePath, "w+" );
		if ( !file ) break;

		//����¼����
		if ( !GAudioEngine::sharedInstance().startRecord() ) break;

		//ѭ����ȡ->��������
		while ( true )
		{
			//ȡ��״̬flag
			mUtility.lock( mLock );
			isRunning = mRunning;
			mUtility.unlock( mLock );
			if ( !isRunning ) break;

			//��¼������
			readsize = GAudioEngine::sharedInstance().readRecordedData( mAudioQueueBuffer, 256 );
			if ( readsize <= 0 ) continue;

			//��������
			savesize = fwrite( mAudioQueueBuffer, 1, readsize, file );

			//�ж��Ƿ���������
			if ( savesize != readsize ) break;
		}

		//ֹͣ¼����
		GAudioEngine::sharedInstance().stopRecord();

	} while( 0 );

	//�ر��ļ�
	if ( file ) { fclose( file ); file = 0; }

	//�趨״̬flag
	mUtility.lock( mLock );
	mRunning = false;
	mUtility.unlock( mLock );

	//���ɽ����¼�
	GEngineEvent::sharedInstance().swallowEvent( EET_AUDIO_FILE_RECORD_STOP, 0, mActionSeed );
}

