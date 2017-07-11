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
#include "GAudioHornUpdate.h"
#include "../../../framework/GEngineEvent/GEngineEvent.h"
#include "../../../framework/GGameEngine/GGameCommand/GGameCommand.h"
#include "../../../framework/GGameEngine/GGameClient/GGameClient.h"
#include "../../../framework/GGameEngine/CommonInfo/MsgIdDef.h"

void GAudioHornUpdateFileServerRunLoop(void *data) { if ( data ) ( ( GAudioHornUpdate * )data )->runLoop(); }
int GAudioHornUpdate::currentAction() { return mActionSeed; }

void GAudioHornUpdate::install(JNIEnv* env, jobject assetManager)
{
	//��ʼ��ȫ�ֱ���
	mRunning		= false;
	mThread 		= 0;
	mActionSeed		= 0;
	mAssetManager	= 0;
	mAAsset			= 0;
	mCustomFile		= 0;
	mIsAssetFile	= false;
	mUpdateTarget	= 0;
	memset( mFilePath		 , 0, 256  );
	memset( mLocalIPAddress	 , 0, 256  );
	memset( mFileServerBuffer, 0, 1024 );

	//����AssetManager
	mAssetManager = AAssetManager_fromJava(env, assetManager);

	//������
	mLock = mUtility.createLock();
}

void GAudioHornUpdate::unInstall()
{
	//ȡ��״̬flag
	bool isRunning = false;
	mUtility.lock( mLock );
	isRunning = mRunning;
	mUtility.unlock( mLock );

	//�ȴ��������
	if ( isRunning ) mUtility.waitingThread( mThread );

	//������
	mUtility.destorylock( &mLock );

	//��ʼ��ȫ�ֱ���
	mRunning		= false;
	mThread 		= 0;
	mActionSeed		= 0;
	mAssetManager	= 0;
	mAAsset			= 0;
	mCustomFile		= 0;
	mIsAssetFile	= false;
	mUpdateTarget	= 0;
	memset( mFilePath		 , 0, 256  );
	memset( mLocalIPAddress	 , 0, 256  );
	memset( mFileServerBuffer, 0, 1024 );
}

int GAudioHornUpdate::updateCustomFile(JNIEnv* env, jstring ip, jstring path, jint target) { return update( env, ip, path, target, false ); }
int GAudioHornUpdate::updateAssetFile (JNIEnv* env, jstring ip, jstring path, jint target) { return update( env, ip, path, target, true  ); }
int GAudioHornUpdate::update(JNIEnv* env, jstring ip, jstring path, jint target, bool asset)
{
	int result = 0;

	do
	{
		//�жϲ����Ƿ���Ч
		if ( !env || !ip || !path || target < 0 || target > 2 ) break;

		//�жϵ�ǰ�Ƿ���������
		bool isRunning = false;
		mUtility.lock( mLock );
		isRunning = mRunning;
		mUtility.unlock( mLock );
		if ( isRunning ) break;

		//���������Ϣ
		memset( mLocalIPAddress, 0, 256 );
		memset( mFilePath	   , 0, 256 );
		int length0 = mConverter.getStringBytes( env, ip  , mLocalIPAddress, 256 );
		int length1 = mConverter.getStringBytes( env, path, mFilePath	   , 256 );
		if ( length0 <= 0 || length1 <= 0 ) break;
		mIsAssetFile  = asset;
		mUpdateTarget = target;

		//���������߳�
		if ( !mUtility.createThread( &mThread, GAudioHornUpdateFileServerRunLoop, this ) ) break;

		//�趨���flag
		mActionSeed++;
		result = mActionSeed;

	} while(0);

	return result;
}

//==========================================================================================================
//
//�ļ�������
//
//==========================================================================================================
int GAudioHornUpdate::parseFileSize(AAsset *asset, FILE *file)
{
	int result = 0;

	do
	{
		if ( !asset && !file ) break;
		if ( asset )
		{
			result = AAsset_getLength( asset );
		}
		else
		{
            fseek( file, 0, SEEK_END);
            result = ftell( file );
            fseek( file, 0, SEEK_SET);
		}

	} while(0);

	return result;
}

int GAudioHornUpdate::copyDataFromFileToBuffer()
{
	int result = 0;
    if ( mIsAssetFile ) result = AAsset_read( mAAsset, mFileServerBuffer, 1024 );
    else 				result = fread( mFileServerBuffer, 1, 1024, mCustomFile );
	return result;
}

void GAudioHornUpdate::runLoop()
{
	bool result 	= false;
	int  resultStep = 0;

	//�趨Running
	mUtility.lock( mLock );
	mRunning = true;
	mUtility.unlock( mLock );

	do
	{
		//��ʼ��ȫ�ֱ���
		mAAsset 	= 0;
		mCustomFile = 0;

		//���ļ�
		if ( mIsAssetFile )
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

		//�����ļ���С
		int fileSize = parseFileSize( mAAsset, mCustomFile );
		if ( fileSize <= 0 ) break;

        //��������
		struct sockaddr_in 	sin;
		int 				socketID, fileID, clientID, readDataBytes;
		socklen_t 			acceptLength;

        //������������
        sin.sin_family 		= AF_INET;
        sin.sin_port 		= htons(8688);
        sin.sin_addr.s_addr = htonl(INADDR_ANY);

        //����socket
        if ( ( socketID = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) break;

        //��ַ���˿�����
        int _reuseFlag = 1;
        setsockopt( socketID, SOL_SOCKET, SO_REUSEADDR, &_reuseFlag, sizeof(int) );
//        setsockopt(socketID, SOL_SOCKET, SO_REUSEPORT, &_reuseFlag, sizeof(int));

        //��socket
        if ( bind( socketID, (struct sockaddr *)(&sin), sizeof(sin) ) < 0 ) { close(socketID); break; }

        //��ʼ����ָ���˿�
        if ( listen(socketID, 5) < 0 )  { close(socketID); break; }

        //֪ͨ���׼��������Ч
        GGameCommand::sharedInstance().updateSpeakerMusic( mUpdateTarget, mLocalIPAddress );

        //�ȴ��������
        clientID = accept( socketID, (struct sockaddr *)(&sin), &acceptLength );

        //�ж������Ƿ�ɹ�
        if (clientID < 0) { close(socketID); break; }
        resultStep = 1;

        //���͵�ǰ�ļ���С��Ϣ
        memset( mFileServerBuffer, 0				, 1024 		  );
        memcpy( mFileServerBuffer, &fileSize		, sizeof(int) );
        write ( clientID		 , mFileServerBuffer, sizeof(int) );

        //��������
        while ( ( readDataBytes = copyDataFromFileToBuffer() ) > 0 )
        {
            write( clientID, mFileServerBuffer, readDataBytes );
        }

        //�ر�socket
        close(socketID);

		//�趨���flag
		result = true;

	} while(0);

	//�ر��ļ�
	if ( mIsAssetFile ) AAsset_close( mAAsset );
	if ( mCustomFile  ) fclose( mCustomFile );
	mCustomFile	= 0;
	mAAsset 	= 0;

	//�жϸ��½��
	if ( !result && resultStep <= 0)
	{
		//�ڲ�����
		int updateResult = 0;
		GGameClient::sharedInstance().swallowMessage(MUIM_M2C_SaveSpeakerMusicPCMDataResponse, &updateResult, sizeof(updateResult));
	}

	//�趨Running
	mUtility.lock( mLock );
	mRunning = false;
	mUtility.unlock( mLock );
}
