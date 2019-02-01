#pragma once

//
// �[�x�Z���T�֘A�̏���
//

// Kinect �֘A
#include <Windows.h>
#include <NuiApi.h>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// �[�x�Z���T�֘A�̊��N���X
#include "DepthCamera.h"

// �f�v�X�摜�̃T�C�Y
#define DEPTH_W 320
#define DEPTH_H 240

// �J���[�摜�̃T�C�Y
#define COLOR_W 640
#define COLOR_H 480

// NUI_IMAGE_RESOLUTION �̐ݒ�
#define EXPAND_RESOLUTION(width, height) NUI_IMAGE_RESOLUTION_##width##x##height
#define RESOLUTION(width, height) EXPAND_RESOLUTION(width, height)
#define DEPTH_RESOLUTION RESOLUTION(DEPTH_W, DEPTH_H)
#define COLOR_RESOLUTION RESOLUTION(COLOR_W, COLOR_H)

class KinectV1 : public DepthCamera
{
  // �Z���T�̎��ʎq
  INuiSensor *sensor;

  // �f�v�X�f�[�^�̃X�g���[���n���h��
  HANDLE depthStream;

  // �f�v�X�f�[�^�̃C�x���g�n���h��
  const HANDLE nextDepthFrameEvent;

  // �f�v�X�f�[�^����J�������W�����߂�Ƃ��ɗp����ꎞ������
  GLfloat (*position)[3];

  // �J���[�f�[�^�̃X�g���[���n���h��
  HANDLE colorStream;

  // �J���[�f�[�^�̃C�x���g�n���h��
  const HANDLE nextColorFrameEvent;

  // �f�[�^���擾����
  void getImage(HANDLE event, HANDLE stream,
    GLuint texture, GLsizei width, GLsizei height, GLenum format, GLenum type) const;

  // �R�s�[�R���X�g���N�^ (�R�s�[�֎~)
  KinectV1(const KinectV1 &w);

  // ��� (����֎~)
  KinectV1 &operator=(const KinectV1 &w);

public:

  // �R���X�g���N�^
  KinectV1();

  // �f�X�g���N�^
  virtual ~KinectV1();

  // �f�v�X�f�[�^���擾����
  GLuint getDepth() const
  {
    getImage(nextDepthFrameEvent, depthStream, depthTexture,
      depthWidth, depthHeight, GL_RED, GL_UNSIGNED_SHORT);
    return depthTexture;
  }

  // �J�������W���擾����
  GLuint getPoint() const
  {
    getImage(nextDepthFrameEvent, depthStream, pointTexture,
      depthWidth, depthHeight, GL_RED, GL_UNSIGNED_SHORT);
    return pointTexture;
  }

  // �J���[�f�[�^���擾����
  GLuint getColor() const
  {
    getImage(nextColorFrameEvent, colorStream, colorTexture,
      colorWidth, colorHeight, GL_BGRA, GL_UNSIGNED_BYTE);
    return colorTexture;
  }
};
