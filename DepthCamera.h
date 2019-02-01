#pragma once

//
// �[�x�Z���T�֘A�̊��N���X
//

// �E�B���h�E�֘A�̏���
#include "Window.h"

class DepthCamera
{
  // �L�������ꂽ�f�v�X�J�����̑䐔
  static int activated;

protected:

  // �ڑ����Ă���Z���T�̐�
  static int connected;

  // �f�v�X�J�����̃T�C�Y�Ɖ�f��
  int depthWidth, depthHeight, depthCount;

  // �f�v�X�f�[�^���i�[����e�N�X�`��
  GLuint depthTexture;

  // �f�v�X�f�[�^����ϊ������|�C���g�̃J�������W���i�[����e�N�X�`��
  GLuint pointTexture;

  // �J���[�J�����̃T�C�Y�Ɖ�f��
  int colorWidth, colorHeight, colorCount;

  // �J���[�f�[�^���i�[����e�N�X�`��
  GLuint colorTexture;

  // �f�v�X�f�[�^�̉�f�ɂ�����J���[�f�[�^�̃e�N�X�`�����W���i�[����o�b�t�@�I�u�W�F�N�g
  GLuint coordBuffer;

  // depthCount �� colorCount ���v�Z���ăe�N�X�`���ƃo�b�t�@�I�u�W�F�N�g���쐬����
  void makeTexture();

public:

  // �R���X�g���N�^
  DepthCamera()
  {
  }
  DepthCamera(int depthWidth, int depthHeight, int colorWidth, int colorHeight)
    : depthWidth(depthWidth)
    , depthHeight(depthHeight)
    , colorWidth(colorWidth)
    , colorHeight(colorHeight)
  {
  }

  // �f�X�g���N�^
  virtual ~DepthCamera();

  // �ڑ�����Ă���Z���T�̐��𒲂ׂ�
  static int getConnected()
  {
    return connected;
  }

  // �g�p���Ă���Z���T�̐��𒲂ׂ�
  static int getActivated()
  {
    return activated;
  }

  // �f�v�X�f�[�^���擾����
  GLuint getDepth() const
  {
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    return depthTexture;
  }

  // �J�������W���擾����
  GLuint getPoint() const
  {
    glBindTexture(GL_TEXTURE_2D, pointTexture);
    return pointTexture;
  }

  // �J���[�f�[�^���擾����
  GLuint getColor() const
  {
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    return colorTexture;
  }

  // �f�v�X�J�����̃T�C�Y�𓾂�
  void getDepthResolution(int *width, int *height) const
  {
    *width = depthWidth;
    *height = depthHeight;
  }

  // �J���[�J�����̃T�C�Y�𓾂�
  void getColorResolution(int *width, int *height) const
  {
    *width = colorWidth;
    *height = colorHeight;
  }

  // �J���[�f�[�^�̃e�N�X�`�����W���i�[����o�b�t�@�I�u�W�F�N�g�𓾂�
  GLuint getCoordBuffer() const
  {
    return coordBuffer;
  }
};
