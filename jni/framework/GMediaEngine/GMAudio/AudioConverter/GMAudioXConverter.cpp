#include <pthread.h>
#include <fcntl.h>
#include "GMAudioXConverter.h"
#define GMAudioXConverterFrameBytes 2

GMAudioXConverter::GMAudioXConverter() { init(); }
GMAudioXConverter::~GMAudioXConverter() { destory(); }

void GMAudioXConverter::init()
{
	mFile 				= 0;
	mFileSize			= 0;
	mFileRemainSize		= 0;
	mActiveReadSize		= 0;
	mActiveReadedSize	= 0;
	mLoaded				= false;
}

void GMAudioXConverter::destory()
{
	if ( mFile ) fclose( mFile );
	init();
}

int  GMAudioXConverter::fileSize()
{
	int result = 0;

	do
	{
		if ( !mFile ) break;
        fseek( mFile, 0, SEEK_END);
        result = ftell( mFile );
        fseek( mFile, 0, SEEK_SET);

	} while(0);

	return result;
}

bool   GMAudioXConverter::loadFile(const char *path)
{
	bool result = false;
	mLoaded = false;

	do
	{
	    if ( !path ) break;
	    if ( access(path, 0) != 0 ) break;

		//���ļ�
	    mFile = fopen( path, "rb+" );
		if ( !mFile ) break;

		//�����ļ���С
		mFileSize = fileSize();
		mFileRemainSize = mFileSize;
		if ( mFileSize <= 0 ) break;

		//�趨���flag
		mLoaded = true;
		result  = true;

	} while( 0 );

	if ( !result && mFile )
	{
		fclose( mFile );
		mFile = 0;
	}

	return result;
}

void   GMAudioXConverter::readData(void *buffer, unsigned &actualframes, unsigned maxframes)
{
	do
	{
		if ( !mLoaded || !buffer || maxframes <= 0 || mFileRemainSize <= 0 ) { actualframes = 0; break; }

		//���㱾�ζ�ȡ���ֽ���
		mActiveReadSize = ( maxframes * GMAudioXConverterFrameBytes );
		mActiveReadSize = ( ( mActiveReadSize > mFileRemainSize ) ? mFileRemainSize : mActiveReadSize );

		//��ȡ����
		mActiveReadedSize = fread( buffer, 1, mActiveReadSize, mFile );
		if ( mActiveReadedSize <= 0 ) mActiveReadedSize = 0;

		//�����ȡ������֡��
		actualframes = ( mActiveReadedSize / GMAudioXConverterFrameBytes );

		//����ʣ��������
		mFileRemainSize -= mActiveReadedSize;

	} while( 0 );
}

void   GMAudioXConverter::seek(double time)
{
	if ( !mLoaded ) return;

	//����ƫ����
	int offsetsize = ( int )( 44100.0 /* 44100.0 = 22050.0 * 2.0 */ * time );

	//�ض�λ�ļ�
	fseek( mFile, offsetsize, SEEK_SET );

	//����ʣ���ֽ���
	mFileRemainSize = mFileSize - offsetsize;
	mFileRemainSize = ( ( mFileRemainSize < 0 ) ? 0 : mFileRemainSize );
}

double GMAudioXConverter::duration()
{
	return ( ( ( double )( mFileSize * 1.0 ) ) / ( 44100.0 /* 44100.0 = 22050.0 * 2.0 */ ) - 0.0 );
}

bool   GMAudioXConverter::isEOF()
{
	return ( mFileRemainSize <= 0 );
}

float  GMAudioXConverter::mediaDuration(char const* fileName)
{
	float result = 0.0;

	do
	{
	    if ( !fileName ) break;
	    if ( access(fileName, 0) != 0 ) break;

	    FILE *_file = fopen( fileName, "rb+" );
	    if ( !_file ) break;
        fseek( _file, 0, SEEK_END);
        result = ( ftell( _file ) / ( 44100.0 /* 44100.0 = 22050.0 * 2.0 */ ) - 0.0 );
        fclose( _file );

	} while(0);

	return result;
}
