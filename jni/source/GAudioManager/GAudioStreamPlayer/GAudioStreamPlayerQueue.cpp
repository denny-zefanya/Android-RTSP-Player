#include "GAudioStreamPlayerQueue.h"

#define GAudioStreamPlayerQueueDefaultCapacity  ( 50 * 1024 ) // 50KB

GAudioStreamPlayerQueue::GAudioStreamPlayerQueue (			  	) { init   ( GAudioStreamPlayerQueueDefaultCapacity	); }
GAudioStreamPlayerQueue::GAudioStreamPlayerQueue ( int capacity ) { init   ( capacity 					   			); }
GAudioStreamPlayerQueue::~GAudioStreamPlayerQueue(			  	) { destory(								  		); }

void GAudioStreamPlayerQueue::init(int capacity)
{
	//������
	if ( capacity <= 0 ) capacity = GAudioStreamPlayerQueueDefaultCapacity;

	//��ʼ��ȫ�ֱ���
	mDataBytes 			= 0;
	mDataPtrOffset		= 0;
	mBufferCapacity		= capacity;
	mBufferExpandMargin	= capacity;
	mLock 				= mUtility.createLock();

	//����������
	mBufferPtr = ( char * )malloc( mBufferCapacity );
}

void GAudioStreamPlayerQueue::destory()
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

void GAudioStreamPlayerQueue::clear()
{
	mUtility.lock(mLock);
	mDataBytes 		= 0;
	mDataPtrOffset	= 0;
	mUtility.unlock(mLock);
}

void GAudioStreamPlayerQueue::write(char *data, int size)
{
	do
	{
		//�жϲ����Ƿ���Ч
		if ( !data || size <= 0 ) break;

		//д������
		write0( data, size, true );

	} while(0);
}

int GAudioStreamPlayerQueue::read(char *buffer, int maxSize)
{
	int result = 0;

	do
	{
		//�жϲ����Ƿ���Ч
		if ( !buffer || maxSize <= 0 ) break;

		//����
		mUtility.lock(mLock);

		//��ȡ����
		do
		{
			//�жϵ�ǰ��������������
			if ( mDataBytes <= 0 ) break;

			//�������ζ�ȡ������
			int readSize = ( mDataBytes >= maxSize ? maxSize : mDataBytes );

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
		mUtility.unlock(mLock);

	} while(0);

	return result;
}

void GAudioStreamPlayerQueue::write0(char *data, int size, bool lock)
{
	//����
	if ( lock ) mUtility.lock(mLock);

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
		write0( data, size, false );
	}

	//����
	if ( lock ) mUtility.unlock(mLock);
}

void GAudioStreamPlayerQueue::expandCapacity()
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
