#include "GAudioRecorder.h"

void GAudioRecorderQueueCallback(SLAndroidSimpleBufferQueueItf queue, void *data)
{ if ( queue && data ) ( ( ( GAudioRecorder * )data )->audioQueueCallback( queue ) ); }

void GAudioRecorder::install(SLEngineItf engine)
{
	//��ʼ��ȫ�ֱ���
	mRecorderObject			= 0;
	mRecorderInterface		= 0;
	mRecorderBufferQueue	= 0;
	mRunning				= false;
	mInstalled				= false;

	//������
	mLock = mUtility.createLock();

	//��ն���
	mAudioQueue.clear();

	//��ʼ��������
	memset( mAudioQueueBuffer, 0, GAUDIORECORDERBUFFERSIZE );

	//����¼����
	createRecorder( engine );
}

void GAudioRecorder::unInstall()
{
	//����¼����
	destoryRecorder();

	//��ʼ��������
	memset( mAudioQueueBuffer, 0, GAUDIORECORDERBUFFERSIZE );

	//��ն���
    mAudioQueue.clear();

	//������
	mUtility.destorylock( &mLock );

	//��ʼ��ȫ�ֱ���
	mRunning 	= false;
	mInstalled 	= false;

}

bool GAudioRecorder::start()
{
	bool result = false;

	do
	{
		//�ж��Ƿ�װ���
		if ( !mInstalled ) break;

		//��ն���
	    mAudioQueue.clear();

		//����
		mUtility.lock  ( mLock );
		mRunning = true;
		mUtility.unlock( mLock );

		//�趨���flag
		result = true;

	} while(false);

	return result;
}

bool GAudioRecorder::stop()
{
	bool result = false;

	do
	{
		//�ж��Ƿ�װ���
		if ( !mInstalled ) break;

		//ֹͣ
		mUtility.lock  ( mLock );
		mRunning = false;
		mUtility.unlock( mLock );

		//��ն���
	    mAudioQueue.clear();

		//�趨���flag
		result = true;

	} while(false);

	return result;
}

int GAudioRecorder::read(void *buffer, int size)
{
	int result = 0;

	do
	{
		//�жϲ����Ƿ���Ч
		if ( !buffer || size <= 0 ) break;

		//�Ӷ����ж�����
		result = mAudioQueue.read( ( char * )buffer, size );

	} while(false);

	return result;
}

void GAudioRecorder::audioQueueCallback(SLAndroidSimpleBufferQueueItf queue)
{
	do
	{
		if ( !queue ) break;

		//ȡ�õ�ǰ״̬
		bool running = false;
		mUtility.lock  ( mLock );
		running = mRunning;
		mUtility.unlock( mLock );

		//����¼�Ƶ�����
		if ( running ) mAudioQueue.write( mAudioQueueBuffer, GAUDIORECORDERBUFFERSIZE );

		//��ն���
		( *queue )->Clear( queue );

		//����¼������
		( *queue )->Enqueue( queue, mAudioQueueBuffer, GAUDIORECORDERBUFFERSIZE );

	} while(0);
}

void GAudioRecorder::createRecorder(SLEngineItf engine)
{
	SLresult result  = SL_RESULT_UNKNOWN_ERROR;
	bool 	 result0 = false;

	do
	{
		//������ƵԴ
	    SLDataLocator_IODevice localDevice = { SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, NULL };
	    SLDataSource 		   audioSource = { &localDevice, NULL };

	    //������Ƶsink
	    SLDataLocator_AndroidSimpleBufferQueue localBufferQueue = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
	    SLDataFormat_PCM 					   audioFormat 	 	= { SL_DATAFORMAT_PCM			, 1							  ,
	    															 SL_SAMPLINGRATE_22_05		,
	    															 SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16 ,
	    															 SL_SPEAKER_FRONT_CENTER	, SL_BYTEORDER_LITTLEENDIAN };
	    SLDataSink 							   audioSink 		= { &localBufferQueue, &audioFormat };

    	//����¼����
	    SLInterfaceID recorderIds[3] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
	    SLboolean 	  recorderReq[3] = { SL_BOOLEAN_TRUE 				 };
    	result = ( *engine )->CreateAudioRecorder( engine, &mRecorderObject, &audioSource, &audioSink, 1, recorderIds, recorderReq );
    	if ( SL_RESULT_SUCCESS != result ) break;

    	//ʵ��¼����
    	result = ( *mRecorderObject )->Realize( mRecorderObject, SL_BOOLEAN_FALSE );
    	if ( SL_RESULT_SUCCESS != result ) break;

    	//ȡ��¼�����ӿ�
    	result = ( *mRecorderObject )->GetInterface( mRecorderObject, SL_IID_RECORD, &mRecorderInterface );
    	if ( SL_RESULT_SUCCESS != result ) break;

    	//ȡ��¼����¼�ƶ���
    	result = ( *mRecorderObject )->GetInterface( mRecorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &mRecorderBufferQueue );
    	if ( SL_RESULT_SUCCESS != result ) break;

    	//ע��¼�ƶ��лص�����
    	result = ( *mRecorderBufferQueue )->RegisterCallback( mRecorderBufferQueue, GAudioRecorderQueueCallback, this );
    	if ( SL_RESULT_SUCCESS != result ) break;
    	//��ն���
    	result = ( *mRecorderBufferQueue )->Clear( mRecorderBufferQueue );
    	if ( SL_RESULT_SUCCESS != result ) break;

    	//������
    	result = ( *mRecorderBufferQueue )->Enqueue( mRecorderBufferQueue, mAudioQueueBuffer, GAUDIORECORDERBUFFERSIZE );
    	if ( SL_RESULT_SUCCESS != result ) break;

    	//����¼����
    	result = ( *mRecorderInterface )->SetRecordState( mRecorderInterface, SL_RECORDSTATE_RECORDING );
    	if ( SL_RESULT_SUCCESS != result ) break;

    	//�趨flag
    	result0 	= true;
    	mInstalled 	= true;

	} while(0);

	if ( !result0 ) destoryRecorder();
}

void GAudioRecorder::destoryRecorder()
{
	//����¼����
    if ( mInstalled )
    {
    	//ֹͣ¼��
    	( *( mRecorderInterface ) )->SetRecordState( mRecorderInterface, SL_RECORDSTATE_STOPPED );

    	//����
        ( *( mRecorderObject ) )->Destroy( mRecorderObject );
    }

	//��ʼ��ȫ�ֱ���
	mRecorderObject			= 0;
	mRecorderInterface		= 0;
	mRecorderBufferQueue	= 0;
}
