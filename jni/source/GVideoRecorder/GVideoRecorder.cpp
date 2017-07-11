#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "GVideoRecorder.h"
#include "../../framework/GEngineEvent/GEngineEvent.h"

void GVideoRecorderRunLoop(void *data)
{
	if ( data ) ( ( GVideoRecorder * )data )->runLoop();
}

GVideoRecorder &GVideoRecorder::sharedInstance()
{
	static GVideoRecorder instance;
	return instance;
}

void GVideoRecorder::install()
{
	//��ʼ��ȫ�ֱ���
	mRunning 				= false;
	mLiveLocked				= false;
	mRTSPClientDetectFlag 	= 0; // 1:start   0:none   -1:stop
	mSourceLock 			= mUtility.createLock();
	mLiveLock 				= mUtility.createLock();
	memset( mRecordFilePath, 0, 256 );
	initRecord( true );
}

void GVideoRecorder::unInstall()
{
	//������̨����
	stop( true );

	//�ͷ���Դ
	mUtility.destorylock( &mSourceLock );
	mUtility.destorylock( &mLiveLock   );
}

void GVideoRecorder::start(JNIEnv* env, jstring filePath)
{
	bool result = false;

	do
	{
		//�жϲ����Ƿ���Ч
		if ( !env || !filePath ) break;

		//ȡ��running
		bool running = false;
		mUtility.lock( mSourceLock );
		running = mRunning;
		mUtility.unlock( mSourceLock );
		if ( running ) break;

		//ȡ���ļ�·����Ϣ
		memset( mRecordFilePath, 0, 256 );
		int length = mConverter.getStringBytes( env, filePath, mRecordFilePath, 256 );
		if ( length <= 0 ) break;

		//��ʼ¼��
		if ( !mLiveLocked ) { mLiveLocked = true; mUtility.lock( mLiveLock ); }
		if ( !mUtility.createThread( &mBackgroundWorker, GVideoRecorderRunLoop, this ) ) break;

		//�趨���flag
		result = true;

	} while(0);

	//֪ͨ�ϲ㣬¼��ʧ��
	if ( !result ) startResponse( false );
}

void GVideoRecorder::stop() { stop(false); }
void GVideoRecorder::stop(bool wait)
{
	bool result = false;

	do
	{
		//ȡ��running
		bool running = false;
		mUtility.lock( mSourceLock );
		running = mRunning;
		mUtility.unlock( mSourceLock );
		if ( !running ) break;

		//ֹͣ����
		if ( mLiveLocked ) { mLiveLocked = false; mUtility.unlock( mLiveLock ); }

		//�ȴ���̨��������
		if ( wait ) mUtility.waitingThread( mBackgroundWorker );

		//�趨���flag
		result = true;

	} while(0);

	//֪ͨ�ϲ㣬ֹͣʧ��
	if ( !result && !wait ) stopResponse( false );
}

void GVideoRecorder::runLoop()
{
	bool RTSPClientFinishHandle = false;
	bool recordSuccess			= false;
	bool errorOccur				= false;

	while( !mUtility.trylock( mLiveLock ) )
	{
		//�趨running
		mUtility.lock( mSourceLock );
		mRunning = true;
		mUtility.unlock( mSourceLock );

		//¼��
		do
		{
			//�����ļ�
			FILE *newFile = fopen( mRecordFilePath, "w+" );
			if ( newFile ) fclose( newFile );
			else break;

			//׼��¼��
			if ( !prepareRecord() )
			{
				//֪ͨ�ϲ㣬¼��ʧ��
				startResponse( false );

				//ֹͣ
				errorOccur = true;
				break;
			}

			//��ջ�����
			mDataBuffer.clear();

			//֪ͨRTSP�ͻ��˿�ʼ¼��
			mUtility.lock( mSourceLock );
			mRTSPClientDetectFlag = 1;
			mUtility.unlock( mSourceLock );

			//�ȴ�RTSP�ͻ��˴������
			RTSPClientFinishHandle = false;
			while( true )
			{
				mUtility.lock( mSourceLock );
				RTSPClientFinishHandle = ( mRTSPClientDetectFlag == 0 );
				mUtility.unlock( mSourceLock );
				if ( RTSPClientFinishHandle ) break;
			}

			//֪ͨ�ϲ㣬��ʼ¼��
			startResponse( true );

			//��ʼ¼��
			while( true )
			{
				//ѭ��¼��
				recordRunLoop();

				//�ж��Ƿ�ֹͣ
				if ( mUtility.trylock( mLiveLock ) )
				{
					//����
					mUtility.unlock( mLiveLock );

					//֪ͨRTSP�ͻ���ֹͣ¼��
					mUtility.lock( mSourceLock );
					mRTSPClientDetectFlag = -1;
					mUtility.unlock( mSourceLock );

					//�ȴ�RTSP�ͻ��˴������
					RTSPClientFinishHandle = false;
					while( true )
					{
						mUtility.lock( mSourceLock );
						RTSPClientFinishHandle = ( mRTSPClientDetectFlag == 0 );
						mUtility.unlock( mSourceLock );
						if ( RTSPClientFinishHandle ) break;
					}

					//ֹͣ
					break;
				}
			}

			//����¼��
			finishRecord();

			//֪ͨ�ϲ�
			stopResponse( true );

			//�趨flag
			recordSuccess = true;

		} while(0);

		//ɾ���ļ�
		if ( !recordSuccess ) { if ( access( mRecordFilePath, 0 ) == 0 ) remove( mRecordFilePath ); }

		//�趨running
		mUtility.lock( mSourceLock );
		mRunning = false;
		mUtility.unlock( mSourceLock );

		//�ж��Ƿ�������
		if ( errorOccur ) break;
	}

	//����
	if ( !errorOccur ) mUtility.unlock( mLiveLock );
}

void GVideoRecorder::write(void *data, int size)
{
	if ( data && size > 0 ) mDataBuffer.writePacket( ( char * )data, size );
}

int GVideoRecorder::RTSPClientDetect()
{
	int result = 0;
	mUtility.lock( mSourceLock );
	result = mRTSPClientDetectFlag;
	mRTSPClientDetectFlag = 0;
	mUtility.unlock( mSourceLock );
	return result;
}

void GVideoRecorder::startResponse(bool success)
{
	GEngineEvent::sharedInstance().swallowEvent( EET_VIDEO_RECORD_RESPONSE_START, 0, success ? 0 : -1 );
}

void GVideoRecorder::stopResponse(bool success)
{
	GEngineEvent::sharedInstance().swallowEvent( EET_VIDEO_RECORD_RESPONSE_STOP, 0, success ? 0 : -1 );
}

//=======================================================================================================
//
//��Ƶ¼��
//
//=======================================================================================================
void GVideoRecorder::initRecord(bool init)
{
	//�ͷ���Դ
	if ( !init )
	{
		//�ر��ļ�
		if ( mAVFormatContext && mAVFormatContext->pb )
		{
			avio_close( mAVFormatContext->pb );
			mAVFormatContext->pb = 0;
		}

		//�ͷŸ�ʽ������
		if ( mAVFormatContext )
		{
			avformat_free_context( mAVFormatContext );
			mAVFormatContext = 0;
		}
	}

	//��ʼ��ȫ�ֱ���
	if ( init )
	{
		mVideoWidth  = 0;
		mVideoHeight = 0;
	}
	mAVFormatContext = 0;
	mAVOutputFormat  = 0;
	mAVStream		 = 0;

	//ע�������
	if ( init ) av_register_all();
}

void GVideoRecorder::setVideoSize(int width, int height)
{
	mVideoWidth  = width;
	mVideoHeight = height;
}

bool GVideoRecorder::prepareRecord()
{
	bool result = false;

	do
	{
		//��ʼ��
		initRecord( false );

		//������ʽ������
//		mAVFormatContext = avformat_alloc_context();
		avformat_alloc_output_context2( &mAVFormatContext, 0, 0, mRecordFilePath );
		if ( !mAVFormatContext ) break;
		mAVOutputFormat = mAVFormatContext->oformat;

		//���������ʽ
//		mAVOutputFormat = av_guess_format( 0, mRecordFilePath, 0 );
//		if ( !mAVOutputFormat || mAVOutputFormat->video_codec == CODEC_ID_NONE ) break;

		//���������Ϣ
//		mAVFormatContext->oformat = mAVOutputFormat;
//		sprintf( mAVFormatContext->filename, "%s", mRecordFilePath );

		//���������
//		mAVStream = avformat_new_stream( mAVFormatContext, avcodec_find_encoder( CODEC_ID_H264 ) );
		mAVStream = avformat_new_stream( mAVFormatContext, 0 );
//        mAVStream->codec->codec_id 		= mAVOutputFormat->video_codec;
		mAVStream->codec->codec_id 		= CODEC_ID_H264;
        mAVStream->codec->codec_type 	= AVMEDIA_TYPE_VIDEO;
        mAVStream->codec->bit_rate 		= 400000; //unused for constant quantizer encoding.
        mAVStream->codec->width 		= mVideoWidth;
        mAVStream->codec->height 		= mVideoHeight;
        mAVStream->codec->time_base.num = 1;
        mAVStream->codec->time_base.den	= 11; //֡��
        mAVStream->codec->gop_size 		= 12;
        mAVStream->codec->pix_fmt 		= PIX_FMT_YUV420P;
		if( mAVOutputFormat->flags & AVFMT_GLOBALHEADER) mAVStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

        //��������Ƿ���ȷ
        av_dump_format( mAVFormatContext, 0, mRecordFilePath, 1 );

        //���ļ�
//	    if ( !( mAVOutputFormat->flags & AVFMT_NOFILE ) )
        if ( avio_open( &( mAVFormatContext->pb ), mRecordFilePath, AVIO_FLAG_WRITE) != 0 ) break;

        //д����ͷ
        if ( avformat_write_header( mAVFormatContext, 0 ) != 0 ) break;

        /*
        int ret = avformat_write_header( mAVFormatContext, 0 );
        if ( ret < 0 )
        {
        	char msg[64];
        	memset( msg, 0, 64 );
        	printf("*************************** running 004 002 return = %s \n", av_make_error_string( msg, 64, ret ) );
        	break;
        }
        */


		//�趨���flag
		result = true;

	} while(0);

	//��ʼ��
	if ( !result ) initRecord( false );

	return result;
}

bool GVideoRecorder::recordRunLoop()
{
	bool result = true;

	//��ȡһ����������
	mActivePacketSize = mDataBuffer.readPacket( mPacketData );

	//�ж��Ƿ���Ч
	if ( mActivePacketSize > 0 )
	{
		//������
	    AVPacket packet;
	    av_init_packet( &packet );
	    packet.flags 		|= AV_PKT_FLAG_KEY;
	    packet.stream_index  = mAVStream->index;
	    packet.data 		 = ( unsigned char * )mPacketData;
	    packet.size 		 = mActivePacketSize;

	    //д���
	    if ( av_write_frame( mAVFormatContext, &packet ) < 0 ) result = false;
	}

	return result;
}

void GVideoRecorder::finishRecord()
{
	//д����β
	av_write_trailer( mAVFormatContext );

	//��ʼ��
	initRecord( false );
}

