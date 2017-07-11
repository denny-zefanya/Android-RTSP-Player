#include "GAudioStreamRecorder.h"
#include "../GAudioEngine/GAudioEngine.h"

void GAudioStreamRecorder::install	() { };
void GAudioStreamRecorder::unInstall() { };

void GAudioStreamRecorder::start()
{
	//����¼����
	bool result = GAudioEngine::sharedInstance().startRecord();
}

int GAudioStreamRecorder::read(void *buffer, int maxSize)
{
	//��ȡ����
	return GAudioEngine::sharedInstance().readRecordedData( buffer, maxSize );
}

void GAudioStreamRecorder::stop()
{
	//ֹͣ¼����
	GAudioEngine::sharedInstance().stopRecord();
}

