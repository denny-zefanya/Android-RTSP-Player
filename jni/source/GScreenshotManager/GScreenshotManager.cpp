#include "GScreenshotManager.h"

GScreenshotManager &GScreenshotManager::sharedInstance()
{
	static GScreenshotManager instance;
	return instance;
}

GScreenshotManager::GScreenshotManager()
{
	gettimeofday( &mActionDateTime, 0 );
	mActiveDateTime = mActionDateTime;
}

GScreenshotManager::~GScreenshotManager()
{

}

void GScreenshotManager::start()
{
	//��¼��ʼʱ��
	gettimeofday( &mActionDateTime, 0 );
}

int  GScreenshotManager::stop()
{
	int result = -1;

	//ȡ�õ�ǰʱ��
	gettimeofday( &mActiveDateTime, 0 );

	//�Ƚ�ʱ����Ƿ�����
	if ( ( ( mActiveDateTime.tv_sec - mActionDateTime.tv_sec ) * 1000000
			+ ( mActiveDateTime.tv_usec - mActionDateTime.tv_usec ) ) >= GScreenshotManagerActionInterval )
	{
		mActionDateTime = mActiveDateTime;
		result = 0;
	}

	return result;
}
