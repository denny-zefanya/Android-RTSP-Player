#include "GAudioStreamPlayer.h"
#include "../GAudioEngine/GAudioEngine.h"
#include "../../../framework/GMediaEngine/GMAudio/GMAudio.h"
#include "../../../framework/GGameEngine/CommonInfo/MsgDef.h"
#include "../../../framework/GGameEngine/GGameClient/GGameClient.h"

void GAudioStreamPlayerCallback( void *buffer, int *size, int maxsize, void *data )
{ if ( data ) ( ( GAudioStreamPlayer * )data )->playerCallback( buffer, size, maxsize ); }
void GAudioStreamPlayerStartCallback(void *data)
{ if ( data ) ( ( GAudioStreamPlayer * )data )->RTSPStart(); }
void GAudioStreamPlayerStopCallback (void *data)
{ if ( data ) ( ( GAudioStreamPlayer * )data )->RTSPStop();  }

void GAudioStreamPlayer::install()
{
	//��ʼ��ȫ�ֱ���
	mRunning  = false;
	mSleeping = false;
	memset( mRTSPAddress, 0, 100 );

	//��ն���
	mAudioQueue.clear();

	//������
	mLock = mUtility.createLock();
}

void GAudioStreamPlayer::unInstall()
{
	//ֹͣ
	stop();

	//��ն���
	mAudioQueue.clear();

	//��ʼ��ȫ�ֱ���
	mRunning  = false;
	mSleeping = false;
	memset( mRTSPAddress, 0, 100 );

	//������
	mUtility.destorylock( &mLock );
}

void GAudioStreamPlayer::start()
{
	do
	{
		//�ж�״̬
		bool running = false;
		mUtility.lock  ( mLock );
		running  = mRunning;
		if ( !running )
		{
			mRunning  = true;
			mSleeping = false;
		}
		mUtility.unlock( mLock );
		if ( running ) break;

		//��ջ�����
		mAudioQueue.clear();

		//����RTSP�ͻ���
		GMAudioClientStart( mRTSPAddress, ( void * )GAudioStreamPlayerStartCallback, ( void * )GAudioStreamPlayerStopCallback, this );

	} while(0);
}

void GAudioStreamPlayer::stop()
{
	do
	{
		//�ж�״̬
		bool running = false;
		mUtility.lock  ( mLock );
		running = mRunning;
		mUtility.unlock( mLock );
		if ( !running ) break;

		//ֹͣRTSP�ͻ���
		GMAudioClientStop();

	} while(0);
}

void GAudioStreamPlayer::setRTSPIPAddress(const char *ip)
{
	memset ( mRTSPAddress, 0 , 100 );
	sprintf( mRTSPAddress, "rtsp://%s:18126/ToyAudioServer", ip );
}

void GAudioStreamPlayer::RTSPStart()
{
	//����������
	GAudioEngine::sharedInstance().startPlayback( GAudioStreamPlayerCallback, this );

	//֪ͨ�ϲ�
	int result = 1;
	GGameClient::sharedInstance().swallowMessage( MUIM_M2C_OpenModelSoundToPDAResponse, &result, sizeof(int) );
}

void GAudioStreamPlayer::RTSPStop()
{
	//ֹͣ������
	GAudioEngine::sharedInstance().stopPlayback();

	//�趨״̬
	mUtility.lock  ( mLock );
	mRunning  = false;
	mSleeping = false;
	mUtility.unlock( mLock );

	//��ն���
	mAudioQueue.clear();

	//֪ͨ�ϲ�
	int result = 1;
	GGameClient::sharedInstance().swallowMessage( MUIM_M2C_CloseModelSoundToPDAResponse, &result, sizeof(int) );
}

void GAudioStreamPlayer::write(void *data, int size)
{
	do
	{
		//�ж�״̬
		bool running = false;
		mUtility.lock  ( mLock );
		running = mRunning;
		mUtility.unlock( mLock );
		if ( !running ) break;

		//д�����
		mAudioQueue.write( ( char * )data, size );

		//�ж��Ƿ���
		if ( mSleeping ) GAudioEngine::sharedInstance().wakePlayer();

	} while(0);
}

void GAudioStreamPlayer::playerCallback(void *buffer, int *size, int maxsize)
{
	//��ȡ����
	(*size) = mAudioQueue.read( ( char * )buffer, maxsize );

	//�ж��Ƿ�˯��
	if ( (*size) <= 0 )
	{
		GAudioEngine::sharedInstance().sleepPlayer();
		mSleeping = true;
	}
}
