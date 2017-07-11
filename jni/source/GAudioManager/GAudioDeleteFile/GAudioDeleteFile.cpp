#include <stdio.h>
#include <fcntl.h>
#include "GAudioDeleteFile.h"

void GAudioDeleteFile::install  () { };
void GAudioDeleteFile::unInstall() { };

int GAudioDeleteFile::deleteFile(JNIEnv* env, jstring path)
{
	int result = -1;

	do
	{
		if ( !env || !path ) break;

		//ȡ���ļ�ȫ·����Ϣ
		memset( mFilePath, 0, 256 );
		mFilePathLength = mConverter.getStringBytes( env, path, mFilePath, 256 );
		if ( mFilePathLength <= 0 ) break;

		//�ж��ļ��Ƿ����
		if ( access( mFilePath, 0 ) != 0 ) break;

		//ɾ���ļ�
		if ( remove( mFilePath ) == 0 ) result = 0;

	} while(0);

	return result;
}
