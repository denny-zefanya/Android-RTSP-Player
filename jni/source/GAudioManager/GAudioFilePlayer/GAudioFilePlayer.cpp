#include <fcntl.h>
#include "GAudioFilePlayer.h"
#include "../GAudioEngine/GAudioEngine.h"
#include "../../../framework/GEngineEvent/GEngineEvent.h"

void GAudioFilePlayerCallback( void *buffer, int *size, int maxsize, void *data )
{ if ( data ) ( ( GAudioFilePlayer * )data )->playerCallback( buffer, size, maxsize ); }

void GAudioFilePlayer::install(JNIEnv* env, jobject assetManager)
{
	//��ʼ��ȫ�ֱ���
	mRunning			= false;
	mAssetManager 		= 0;
	mAAsset				= 0;
	mCustomFile			= 0;
	mActionSeed	  		= 0;
	mPlayingAssetFile 	= false;
	memset( mFilePath, 0, 256 );

	//������
	mLock = mUtility.createLock();

	//����AssetManager
	mAssetManager = AAssetManager_fromJava(env, assetManager);
}

void GAudioFilePlayer::unInstall()
{
	//ֹͣ����
	stop( false );

	//��ʼ��ȫ�ֱ���
	mUtility.lock( mLock );
	mRunning = false;
	mUtility.unlock( mLock );
	mAssetManager 		= 0;
	mAAsset				= 0;
	mCustomFile			= 0;
	mActionSeed	  		= 0;
	mPlayingAssetFile 	= false;
	memset( mFilePath, 0, 256 );

	//������
	mUtility.destorylock( &mLock );
}

int GAudioFilePlayer::playAssetFile (JNIEnv* env, jstring path) { return play( env, path, true  ); }
int GAudioFilePlayer::playCustomFile(JNIEnv* env, jstring path) { return play( env, path, false ); }
int GAudioFilePlayer::play			(JNIEnv* env, jstring path, bool asset)
{
	int result = 0;

	do
	{
		//�ж�״̬
		bool isRunning = false;
		mUtility.lock( mLock );
		isRunning = mRunning;
		if ( !isRunning ) mRunning = true;
		mUtility.unlock( mLock );
		if ( isRunning ) break;

		//ȡ���ļ�·����Ϣ
		memset( mFilePath, 0, 256 );
		int length = mConverter.getStringBytes( env, path, mFilePath, 256 );
		if ( length <= 0 ) break;

		//���ļ�
		if ( asset )
		{
			mAAsset = AAssetManager_open( mAssetManager, mFilePath, AASSET_MODE_UNKNOWN );
			if ( !mAAsset ) break;
		}
		else
		{
			//�ж��ļ��Ƿ����
			if ( access( mFilePath, 0 ) != 0 ) break;

			//���ļ�
			mCustomFile = fopen( mFilePath, "rb+" );
			if ( !mCustomFile ) break;
		}
		mPlayingAssetFile = asset;

		//��ʼ�ط�
		if ( !GAudioEngine::sharedInstance().startPlayback( GAudioFilePlayerCallback, this ) ) break;

		//�趨���flag
		mActionSeed++;
		result = mActionSeed;

		//�����¼�
		GEngineEvent::sharedInstance().swallowEvent( EET_AUDIO_FILE_PLAYER_START, 0, mActionSeed );

	} while(0);

	//�ж��Ƿ�ɹ�
	if ( result <= 0 )
	{
		//�ر��ļ�
		if ( mPlayingAssetFile ) AAsset_close( mAAsset );
		if ( mCustomFile 	   ) fclose( mCustomFile );

		//��ʼ��ȫ�ֱ���
		mUtility.lock( mLock );
		mRunning = false;
		mUtility.unlock( mLock );
		mAAsset				= 0;
		mCustomFile			= 0;
		mPlayingAssetFile 	= false;
		memset( mFilePath, 0, 256 );
	}

	return result;
}

void GAudioFilePlayer::stop() { stop( true, true ); }
void GAudioFilePlayer::stop(bool notifyUser) { stop( notifyUser, true ); }
void GAudioFilePlayer::stop(bool notifyUser, bool stopPlayer)
{
	do
	{
		//�ж�״̬
		bool isRunning = false;
		mUtility.lock( mLock );
		isRunning = mRunning;
		if ( isRunning ) mRunning = false;
		mUtility.unlock( mLock );
		if ( !isRunning ) break;

		//ֹͣ�ط�
		if ( stopPlayer ) GAudioEngine::sharedInstance().stopPlayback();

		//�ر��ļ�
		if ( mPlayingAssetFile ) AAsset_close( mAAsset );
		if ( mCustomFile 	   ) fclose( mCustomFile );

		//��ʼ��ȫ�ֱ���
		mAAsset				= 0;
		mCustomFile			= 0;
		mPlayingAssetFile 	= false;
		memset( mFilePath, 0, 256 );

		//�����¼�
		if ( notifyUser ) GEngineEvent::sharedInstance().swallowEvent( EET_AUDIO_FILE_PLAYER_STOP, 0, mActionSeed );

	} while(0);
}

void GAudioFilePlayer::playerCallback(void *buffer, int *size, int maxsize)
{
	if ( mPlayingAssetFile ) ( *size ) = AAsset_read( mAAsset, buffer, maxsize  			);
	else 					 ( *size ) = fread		( buffer , 1     , maxsize, mCustomFile );

	//�ж��Ƿ����
	if ( ( *size ) <= 0 ) stop( true, false );

}

