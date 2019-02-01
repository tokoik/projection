#pragma once

//
// �摜����
//

// �E�B���h�E�֘A�̏���
#include "Window.h"

// ��`
#include "Rect.h"

// �W�����C�u����
#include <vector>

class Calculate
{
  // �摜�����Ɏg���t���[���o�b�t�@�I�u�W�F�N�g
  GLuint fbo;

  // �v�Z���ʂ�ۑ�����t���[���o�b�t�@�̃^�[�Q�b�g�Ɏg���e�N�X�`��
  std::vector<GLuint> texture;

  // �����_�����O�^�[�Q�b�g
  std::vector<GLenum> bufs;

  // �t���[���o�b�t�@�I�u�W�F�N�g�̃T�C�Y
  const GLsizei width, height;

  // �v�Z�p�̃V�F�[�_�v���O����
  const GLuint program;

  // �v�Z�p�̃V�F�[�_�v���O�����Ŏg�p���Ă���T���v���� uniform �ϐ��̐�
  const int uniforms;

  // �v�Z�Ɏg����`
  static const Rect *rectangle;

  // ���t�@�����X�J�E���g
  static unsigned int count;

public:

  // �R���X�g���N�^
  Calculate(int width, int height, const char *source, int uniforms = 1, int targets = 1);

  // �f�X�g���N�^
  virtual ~Calculate();

  // �V�F�[�_�v���O�����𓾂�
  GLuint get() const
  {
    return program;
  }

  // �v�Z���ʂ����o���e�N�X�`�����𓾂�
  const std::vector<GLuint> &getTexture() const
  {
    return texture;
  }

  // �v�Z�p�̃V�F�[�_�v���O�����̎g�p���J�n����
  void use() const
  {
    glUseProgram(program);
  }

  // �v�Z�����s����
  const std::vector<GLuint> &calculate() const;
};
