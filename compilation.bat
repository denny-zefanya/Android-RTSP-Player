REM @ECHO ON

REM ===================================================================================
REM ��������

REM ****************
REM ��������Ϣ
REM ****************
SET CYGDiskNo=D:
SET GYGFullPath=%CYGDiskNo%\AndroidDevelop\soft\cygwin\bin

REM ****************
REM Ŀ����Ŀ��Ϣ
REM ****************
SET ProjectDiskNo=D:
SET ProjectDiskNo1=D
SET ProjectFullPath=%ProjectDiskNo%\ProbeFUN4AD
SET ProjectFullPath1=%ProjectDiskNo1%/ProbeFUN4AD
SET ProjectJNIPath=jni
SET JavahCommandHeader=javah -classpath bin\classes -d %ProjectJNIPath%\source\include 

REM ===================================================================================

REM ##################################
REM �л���ָ���Ĺ���Ŀ¼��
REM #####################
%ProjectDiskNo%
CD %ProjectFullPath%

REM ##################################
REM ����ͷ�ļ�
REM #####################
REM %JavahCommandHeader%glovast.Framework.GFramework

REM ##################################
REM ��ʼ����
REM #####################
%CYGDiskNo%
chdir %GYGFullPath%
REM bash --login -c "cd /cygdrive/%ProjectFullPath1%/%ProjectJNIPath% && $NDK/ndk-build clean && $NDK/ndk-build"
bash --login -c "cd /cygdrive/%ProjectFullPath1%/%ProjectJNIPath% && $NDK/ndk-build"

pause
@ECHO ON