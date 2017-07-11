#include "GEngine.h"
#include "../GGameEngine/GGameCommand/GGameCMDManager.h"
#include "../GGameEngine/GGameClient/GGameClient.h"


#include "../GMediaEngine/GMAudio/GMAudio.h"

GEngine &GEngine::sharedInstance()
{
    static GEngine instance;
    return instance;
}

void GEngine::install()
{
	//����¼�����
	mEventDataSize = sizeof( GEventData );
    mEventQueue.clear();

    //��װ������RTSP������
    GMAudioInstall();
    GMAudioServerStart();

    //��װ����ָ����������ͻ���
    GGameCMDManager::sharedInstance().install();
    GGameClient::sharedInstance().install();
}

void GEngine::unInstall()
{
	//ж������ָ����������ͻ���
    GGameClient::sharedInstance().unInstall();
    GGameCMDManager::sharedInstance().unInstall();

    //ֹͣ��ж��RTSP������
    GMAudioServerStop();
    GMAudioUnInstall();

    //����¼�����
    mEventQueue.clear();
}

void GEngine::reset()
{
    GGameCMDManager::sharedInstance().clear();
    mEventQueue.clear();
}

bool GEngine::eventRunLoop(GEventData *data)
{
    bool result = false;
    
    do
    {
        if ( !data ) break;

        //�Ӷ����ж�ȡ����
        if ( mEventQueue.read( ( char * )data, mEventDataSize ) == mEventDataSize ) result = true;
        
    } while (0);
    
    return result;
}

void GEngine::swallowNetMessage(GGameNetMessage message)
{
    mNewEvent.event         = GEVT_NETMSG;
    mNewEvent.netMessage	= message;
    mEventQueue.write( ( char * )( &mNewEvent ), mEventDataSize );
}

void GEngine::swallowEngineEvent(GEngineEventData event)
{
    mNewEvent.event         = GEVT_ENGEVT;
    mNewEvent.engineEvent   = event;
    mEventQueue.write( ( char * )( &mNewEvent ), mEventDataSize );
}
