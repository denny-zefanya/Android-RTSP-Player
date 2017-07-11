#include "GVideoRecordBuffer.h"

#define GVideoRecordBufferDefaultCapacity  ( 50 * 1024 ) // 50KB

GVideoRecordBuffer::GVideoRecordBuffer (			  ) { init   ( GVideoRecordBufferDefaultCapacity ); }
GVideoRecordBuffer::GVideoRecordBuffer ( int capacity ) { init   ( capacity 					   	 ); }
GVideoRecordBuffer::~GVideoRecordBuffer(			  ) { destory(								   	 ); }

void GVideoRecordBuffer::init(int capacity)
{
	//������
	if ( capacity <= 0 ) capacity = GVideoRecordBufferDefaultCapacity;

	//��ʼ��ȫ�ֱ���
	mDataBytes 			= 0;
	mDataPtrOffset		= 0;
	mIntTypeLength		= sizeof( int );
	mBufferCapacity		= capacity;
	mBufferExpandMargin	= capacity;
	mLock 				= mUtility.createLock();

	//����������
	mBufferPtr = ( char * )malloc( mBufferCapacity );
}

void GVideoRecordBuffer::destory()
{
	mUtility.lock(mLock);
	free( mBufferPtr );
	mDataBytes 			= 0;
	mDataPtrOffset		= 0;
	mBufferPtr			= 0;
	mBufferCapacity		= 0;
	mBufferExpandMargin	= 0;
	mUtility.unlock(mLock);
	mUtility.destorylock( &mLock );
}

void GVideoRecordBuffer::clear()
{
	mUtility.lock(mLock);
	mDataBytes 		= 0;
	mDataPtrOffset	= 0;
	mUtility.unlock(mLock);
}

void GVideoRecordBuffer::writePacket(char *data, int size)
{
	do
	{
		//�жϲ����Ƿ���Ч
		if ( !data || size <= 0 ) break;

		//д������
		write0( ( char * )( &size )	, mIntTypeLength, true , false );
		write0( data 				, size			, false, true  );

	} while(0);
}

int GVideoRecordBuffer::readPacket(char *buffer)
{
	int result = 0;

	if ( read0( ( char * )( &result ), mIntTypeLength, true, false ) > 0 )
	{
		result = read0( buffer, result, false, true );
	}
	else
	{
		//����
		mUtility.unlock(mLock);

		result = 0;
	}

	return result;
}

int GVideoRecordBuffer::read0(char *buffer, int size, bool beginLock, bool endUnLock)
{
	int result = 0;

	do
	{
		//�жϲ����Ƿ���Ч
		if ( !buffer || size <= 0 ) break;

		//����
		if ( beginLock ) mUtility.lock(mLock);

		//��ȡ����
		do
		{
			//�жϵ�ǰ��������������
			if ( mDataBytes <= 0 ) break;

			//�������ζ�ȡ������
			int readSize = ( mDataBytes >= size ? size : mDataBytes );

			//�ж����������Ƿ�
			int lineSize = mBufferCapacity - mDataPtrOffset;
			if ( lineSize >= readSize )
			{
				memcpy( buffer, mBufferPtr + mDataPtrOffset, readSize );
			}
			else
			{
				memcpy( buffer			 , mBufferPtr + mDataPtrOffset, lineSize 		    );
				memcpy( buffer + lineSize, mBufferPtr				  , readSize - lineSize );
			}

			//�趨flag
			mDataPtrOffset 	+= readSize;
			mDataBytes		-= readSize;
			mDataPtrOffset  %= mBufferCapacity;
			result			= readSize;

		} while(0);

		//����
		if ( endUnLock ) mUtility.unlock(mLock);

	} while(0);

	return result;
}

void GVideoRecordBuffer::write0(char *data, int size, bool beginLock, bool endUnLock)
{
	//����
	if ( beginLock ) mUtility.lock(mLock);

	//�жϻ�����ʣ�������Ƿ���
	if ( size <= ( mBufferCapacity - mDataBytes ) )
	{
		//ȡ�õ�ǰ���ݽ���λ��ƫ����
		int dataEndOffset = mDataPtrOffset + mDataBytes;
		dataEndOffset %= mBufferCapacity;

		//�ж������������㹻
		int lineSize = mBufferCapacity - dataEndOffset;
		if ( lineSize >= size )
		{
			//��������
			memcpy( mBufferPtr + dataEndOffset, data, size );
		}
		else
		{
			//��������
			memcpy( mBufferPtr + dataEndOffset, data		   , lineSize 		 );
			memcpy( mBufferPtr				  , data + lineSize, size - lineSize );
		}

		//�ۼ�������
		mDataBytes += size;
	}
	else
	{
		//��������
		expandCapacity();

		//д������
		write0( data, size, false, false );
	}

	//����
	if ( endUnLock ) mUtility.unlock(mLock);
}

void GVideoRecordBuffer::expandCapacity()
{
	//��������buffer
	char *newBuffer = ( char * )malloc( mBufferCapacity + mBufferExpandMargin );

	//������buffer��������buffer
	if ( ( mDataPtrOffset + mDataBytes ) <= mBufferCapacity ) //��ǰ�������Դ��
	{
		memcpy( newBuffer, mBufferPtr + mDataPtrOffset, mDataBytes );
	}
	else
	{
		int lineSize = mBufferCapacity - mDataPtrOffset;
		memcpy( newBuffer			, mBufferPtr + mDataPtrOffset, lineSize 			 );
		memcpy( newBuffer + lineSize, mBufferPtr				 , mDataBytes - lineSize );
	}

	//�ͷž��ڴ�
	free( mBufferPtr );

	//��ʼ��ȫ�ֱ���
	mDataPtrOffset 	=  0;
	mBufferPtr		=  newBuffer;
	mBufferCapacity	+= mBufferExpandMargin;
}
