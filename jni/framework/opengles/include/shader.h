/*******************************************************************************
 *        Module: shader
 *          File: shader.h
 * Functionality: tools for opengl shader programs.
 *       Related: GLES2
 *        System: android
 *      Language: C++
 *        Author: huangxuefeng
 *       Version: V1.0 Copyright(C) 2013 360ANTS, All rights reserved.
 * -----------------------------------------------------------------------------
 * Revisions:
 * Date        Version     Reviser       Description
 * 2014-01-19  v1.0        huangxuefeng  created
 ******************************************************************************/
#ifndef _ANTS360_SHADER_H_
#define _ANTS360_SHADER_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace ants360 {


/*******************************************************************************
 ������      : LoadShader
 ����        : ������Ⱦ��
 �㷨ʵ��    : ��
 ����ȫ�ֱ���: ��
 �������˵��: type      - shader ����
                           GL_VERTEX_SHADER   - vertex shader
                           GL_FRAGMENT_SHADER - fragment shader
               shaderSrc - shader ��Դ����
 ����ֵ˵��  : 0    - ������Ⱦ��ʧ��
               ���� - ������Ⱦ���ɹ�
 -------------------------------------------------------------------------------
 �޸ļ�¼:
 ����        �汾  �޸���  �߶���  �޸�����
 2013/07/12  1.0   л־��          ����
*******************************************************************************/
GLuint LoadShader(GLenum type, const char *shaderSrc);


} // namespace ants360

#endif // _ANTS360_SHADER_H_