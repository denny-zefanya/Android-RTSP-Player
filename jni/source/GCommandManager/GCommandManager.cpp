#include "GCommandManager.h"
#include "../../framework/GEngineEvent/GEngineEvent.h"
#include "../../framework/GGameEngine/GGameCommand/GGameCommand.h"
#include "../../framework/GMediaEngine/GMAudio/GMAudio.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//get local IPAddress
//
//add by jarry.jin 2014-04-02
//
/////////////////////////////////////////////////////////////////////////////////////////////////
int GCommandManager::localIPAddress(int index)
{
	return mDeviceDetecter.localIPAddress( index );
}
/////////////////////////////////////////////////////////////////////////////////////////////////

void GCommandManager_DeviceDetecter_C(void *data, const char *ip)
{
	if ( !data || !ip )
	{
		GEngineEvent::sharedInstance().swallowEvent( EET_DDET_ERR, 0, 0 );
		return;
	}
	( ( GCommandManager * )data )->detectedDevice( ip );
}

GCommandManager &GCommandManager::sharedInstance()
{
	static GCommandManager instance;
	return instance;
}

void GCommandManager::detectedDevice(const char *ip)
{
	mDeviceDetecter.parseLocalIPAddress( ip );
	GGameCommand::sharedInstance().controlDevice(ip);
}

void GCommandManager::setStreamingMusicPath(JNIEnv *env, jstring sysMusicPath, jstring customMusicPath)
{
	do
	{
		mGettedMusicPath = false;
		if ( !env || !sysMusicPath || !customMusicPath ) break;
		memset( mSystemMusicPath, 0, 256 );
		memset( mCustomMusicPath, 0, 256 );
		int size0 = mDataConverter.getStringBytes( env, sysMusicPath   , mSystemMusicPath, 256 );
		int size1 = mDataConverter.getStringBytes( env, customMusicPath, mCustomMusicPath, 256 );
		if ( size0 <= 0 || size1 <= 0 ) break;
		mGettedMusicPath = true;

	} while(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GCommandManager::controlDevice(bool detect)
{
	if ( detect )
	{
		mDeviceDetecter.detect( (void *)GCommandManager_DeviceDetecter_C, this);
	}
	else
	{
		mDeviceDetecter.parseLocalIPAddress( "192.168.0.1" );
		GGameCommand::sharedInstance().controlDevice( "192.168.0.1" );
	}
}

void GCommandManager::disconnectWithDevice()
{
	GGameCommand::sharedInstance().disconnectWithDevice();
}

void GCommandManager::customDisconnectWithDevice(JNIEnv *env, jstring reason)
{
	if ( env && reason )
	{
		char _reason[256];
		memset( _reason, 0, 256 );
		int _size = mDataConverter.getStringBytes( env, reason, _reason, 256 );
		if ( _size > 0 )
		{
			GGameCommand::sharedInstance().customDisconnectWithDevice( _reason, 0 );
		}
		else
		{
			GGameCommand::sharedInstance().customDisconnectWithDevice( "", 0 );
		}
	}
	else
	{
		GGameCommand::sharedInstance().customDisconnectWithDevice( "", 0 );
	}
}

void GCommandManager::submitConfigruation(JNIEnv *env, jstring ssid, bool channelUF, int channel, bool volumeUF, int volume, bool calibrationUF, int calibration)
{
	if ( ssid )
	{
		char _ssid[256];
		memset( _ssid, 0, 256 );
		int _size = mDataConverter.getStringBytes( env, ssid, _ssid, 256 );
		if ( _size > 0 )
		{
			GGameCommand::sharedInstance().cacheConfigruation( _ssid, channelUF, channel, volumeUF, volume, calibrationUF, calibration);
		}
		else
		{
			GGameCommand::sharedInstance().cacheConfigruation( 0, channelUF, channel, volumeUF, volume, calibrationUF, calibration);
		}
	}
	else
	{
		GGameCommand::sharedInstance().cacheConfigruation( 0, channelUF, channel, volumeUF, volume, calibrationUF, calibration);
	}
}

void GCommandManager::updateConfigruation()
{
	GGameCommand::sharedInstance().updateConfigruation( true );
}

void GCommandManager::forward(int ratio)
{
	GGameCommand::sharedInstance().forward( ratio );
}

void GCommandManager::backward(int ratio)
{
	GGameCommand::sharedInstance().backward( ratio );
}

void GCommandManager::stop()
{
	GGameCommand::sharedInstance().stop();
}

void GCommandManager::startLeftTurn(int ratio)
{
	GGameCommand::sharedInstance().startLeftTurn( ratio );
}

void GCommandManager::stopLeftTurn()
{
	GGameCommand::sharedInstance().stopLeftTurn();
}

void GCommandManager::startRightTurn(int ratio)
{
	GGameCommand::sharedInstance().startRightTurn( ratio );
}

void GCommandManager::stopRightTurn()
{
	GGameCommand::sharedInstance().stopRightTurn();
}

void GCommandManager::cameraMoveUp()
{
	GGameCommand::sharedInstance().cameraMoveUp();
}

void GCommandManager::cameraMoveDown()
{
	GGameCommand::sharedInstance().cameraMoveDown();
}

void GCommandManager::cameraMoveStop()
{
	GGameCommand::sharedInstance().cameraMoveStop();
}

void GCommandManager::openLeftLED(int on, int off)
{
	GGameCommand::sharedInstance().openLeftLED( on, off );
}

void GCommandManager::closeLeftLED()
{
	GGameCommand::sharedInstance().closeLeftLED();
}

void GCommandManager::openRightLED(int on, int off)
{
	GGameCommand::sharedInstance().openLeftLED( on, off );
}

void GCommandManager::closeRightLED()
{
	GGameCommand::sharedInstance().closeLeftLED();
}

void GCommandManager::openTailLED(int on, int off)
{
	GGameCommand::sharedInstance().openTailLED( on, off );
}

void GCommandManager::closeTailLED()
{
	GGameCommand::sharedInstance().closeTailLED();
}

void GCommandManager::openHeadLight()
{
	GGameCommand::sharedInstance().openHeadLight();
}

void GCommandManager::closeHeadLight()
{
	GGameCommand::sharedInstance().closeHeadLight();
}

void GCommandManager::openSpeaker(bool response)
{
	GGameCommand::sharedInstance().openSpeaker( "defaultSpeakerMusic.pcm", response );
}

void GCommandManager::closeSpeaker(bool response)
{
	GGameCommand::sharedInstance().closeSpeaker( response );
}

void GCommandManager::openSpeaking(JNIEnv *env, jstring ip, bool response)
{
	if ( !env || !ip ) return;
	char _ip[256];
	memset( _ip, 0, 256 );
	int _size = mDataConverter.getStringBytes( env, ip, _ip, 256 );
	if ( _size > 0 )
	{
		GGameCommand::sharedInstance().openSpeaking( _ip, GMAudioServerPort(), 300, response );
	}
}

void GCommandManager::closeSpeaking(bool response)
{
	GGameCommand::sharedInstance().closeSpeaking( response );
}

void GCommandManager::openListening(bool response)
{
	GGameCommand::sharedInstance().openListening( response );
}

void GCommandManager::closeListening(bool response)
{
	GGameCommand::sharedInstance().closeListening( response );
}

void GCommandManager::playMusic(JNIEnv *env, jstring filename, jstring ip, jint fileType, bool reset, bool response)
{
	if ( !env || !filename || !ip ) return;
	if ( !mGettedMusicPath ) return;

	//ȡ���ļ���Ϣ
	char _ip[256];
	memset( mActiveStreamingFileName, 0, 256 );
	memset( _ip		 				, 0, 256 );
	int _size0 = mDataConverter.getStringBytes( env, filename, mActiveStreamingFileName	, 256 );
	int _size1 = mDataConverter.getStringBytes( env, ip		 , _ip						, 256 );
	if ( _size0 <= 0 || _size1 <= 0 ) return;

	//�ж��Ƿ�λ
	if ( reset )
	{
		//ȡ���ļ�·����Ϣ
		char _filename[256];
		memset ( _filename, 0, 256 );
		sprintf( _filename, "%s%s", ( fileType == 0 ? mSystemMusicPath : mCustomMusicPath ), mActiveStreamingFileName );

		//����DSP��Ϣ
		GMAudioParseFileDuration( _filename, ( ( fileType == 0 ) ? 1 : 0 ) );

		//���ʱ����Ϣ
		mStreamMusicDistanceTime.tv_sec  = 0;
		mStreamMusicDistanceTime.tv_usec = 0;
	}

	//ȡ�õ�ǰʱ����Ϣ
	gettimeofday( &mStreamMusicBeginDateTime, 0 );

	//��ʼ������
	GGameCommand::sharedInstance().playMusic( ( fileType == 0 ? MUSIC_STREAM_SYSTEM : MUSIC_STREAM_CUSTOM ), _ip, 300, mStreamMusicDistanceTime, 18126, response );
}

const char *GCommandManager::getStreamingFilePath(char const *fileName, int *isSystemMusic)
{
	if ( fileName )
	{
		memset ( mActiveStreamingFilePath, 0, 256 );
		if ( strcmp( fileName, MUSIC_STREAM_SYSTEM ) == 0 ) // system music
		{
			if ( isSystemMusic ) ( *isSystemMusic ) = 1;
			sprintf( mActiveStreamingFilePath, "%s%s", mSystemMusicPath, mActiveStreamingFileName );
			return mActiveStreamingFilePath;
		}
		else if ( strcmp( fileName, MUSIC_STREAM_CUSTOM ) == 0 ) // custom music
		{
			if ( isSystemMusic ) ( *isSystemMusic ) = 0;
			sprintf( mActiveStreamingFilePath, "%s%s", mCustomMusicPath, mActiveStreamingFileName );
			return mActiveStreamingFilePath;
		}
	}

	return NULL;
}

void GCommandManager::stopMusic(bool response)
{
	//ȡ�õ�ǰʱ����Ϣ
	gettimeofday( &mStreamMusicActiveDateTime, 0 );

	//�ۼ�ʱ���
	unsigned long long distance = ( mStreamMusicActiveDateTime.tv_sec - mStreamMusicBeginDateTime.tv_sec ) * 1000000
								+ ( mStreamMusicActiveDateTime.tv_usec - mStreamMusicBeginDateTime.tv_usec )
								+ mStreamMusicDistanceTime.tv_sec * 1000000 + mStreamMusicDistanceTime.tv_usec;
	mStreamMusicDistanceTime.tv_sec  = ( ( distance - ( distance % 1000000 ) ) / 1000000 );
	mStreamMusicDistanceTime.tv_usec = ( distance % 1000000 );

	//����ָֹͣ��
	GGameCommand::sharedInstance().stopMusic( response );
}

void GCommandManager::updateSpeakerMusic(JNIEnv *env, int type, jstring ip)
{
	if ( !env || !ip ) return;

	char _ip[256];
	memset( _ip, 0, 256 );
	int _size = mDataConverter.getStringBytes( env, ip, _ip, 256 );
	if ( _size > 0 )
	{
		GGameCommand::sharedInstance().updateSpeakerMusic( type, _ip );
	}
}

void GCommandManager::requestAdvanceConfigruation()
{
	GGameCommand::sharedInstance().requestAdvanceConfigruation();
}

void GCommandManager::GP5SM3nSForward(int stall)
{
	GGameCommand::sharedInstance().GP5SM3nSForward( stall );
}

void GCommandManager::GP5SM3nSBackward(int stall)
{
	GGameCommand::sharedInstance().GP5SM3nSBackward( stall );
}

void GCommandManager::GP5SM3nSTurnLeft(int stall)
{
	GGameCommand::sharedInstance().GP5SM3nSTurnLeft( stall );
}

void GCommandManager::GP5SM3nSTurnRight(int stall)
{
	GGameCommand::sharedInstance().GP5SM3nSTurnRight( stall );
}

void GCommandManager::GP5SM3nSStopMove()
{
	GGameCommand::sharedInstance().GP5SM3nSStopMove();
}

void GCommandManager::GP5SM3nSStopTurn()
{
	GGameCommand::sharedInstance().GP5SM3nSStopTurn();
}

void GCommandManager::GP5SM3nSBeginBattle()
{
	GGameCommand::sharedInstance().GP5SM3nSBeginBattle();
}

void GCommandManager::GP5SM3nSFinishBattle(bool fail)
{
	GGameCommand::sharedInstance().GP5SM3nSFinishBattle(fail);
}

void GCommandManager::GP5SM3nSFire()
{
	GGameCommand::sharedInstance().GP5SM3nSFire();
}

void GCommandManager::GWGKickGamePlayer(int gameId)
{
	GGameCommand::sharedInstance().GWGKickGamePlayer( gameId );
}

void GCommandManager::GWGDisbandGame()
{
	GGameCommand::sharedInstance().GWGDisbandGame();
}

void GCommandManager::GWGLeaveGameWhileWaiting()
{
	GGameCommand::sharedInstance().GWGLeaveGameWhileWaiting();
}

void GCommandManager::GWGStartGame()
{
	GGameCommand::sharedInstance().GWGStartGame();
}

void GCommandManager::GWGSearchGameServer()
{
	GGameCommand::sharedInstance().GWGSearchGameServer();
}

void GCommandManager::GWGStopSearchGameServer()
{
	GGameCommand::sharedInstance().GWGStopSearchGameServer();
}

void GCommandManager::GWGJoinGameServer(JNIEnv *env, jstring macaddr, int team, jstring name)
{
	if ( !env || !macaddr || !name ) return;

	char _macaddr[256];
	char _name[256];
	memset( _macaddr, 0, 256 );
	memset( _name   , 0, 256 );
	int _size0 = mDataConverter.getStringBytes( env, macaddr, _macaddr, 256 );
	int _size1 = mDataConverter.getStringBytes( env, name	, _name	  , 256 );
	if ( _size0 <= 0 || _size1 <= 0 ) return;

	GGameCommand::sharedInstance().GWGJoinGameServer( _macaddr, team, _name );
}

void GCommandManager::GWGRequestTeamInfo(int team)
{
	GGameCommand::sharedInstance().GWGRequestTeamInfo( team );
}

void GCommandManager::GWGCreateGame(JNIEnv *env, jstring gamename, jstring nickname, int gametype, int gametime, int gameteam)
{
	if ( !env || !gamename || !nickname ) return;

	char _gamename[256];
	char _nickname[256];
	memset( _gamename, 0, 256 );
	memset( _nickname, 0, 256 );
	int _size0 = mDataConverter.getStringBytes( env, gamename, _gamename, 256 );
	int _size1 = mDataConverter.getStringBytes( env, nickname, _nickname, 256 );
	if ( _size0 <= 0 || _size1 <= 0 ) return;

	GGameCommand::sharedInstance().GWGCreateGame( _gamename, _nickname, gametype, gametime, gameteam );
}

void GCommandManager::GWGLeaveGameWhileBattling()
{
	GGameCommand::sharedInstance().GWGLeaveGameWhileBattling();
}

void GCommandManager::GWGRequestTeamMemberIDList()
{
	GGameCommand::sharedInstance().GWGRequestTeamMemberIDList();
}

