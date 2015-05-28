#pragma once

// �⏕�v���O����
#include "gg.h"
using namespace gg;

//
// �E�B���h�E�֘A�̏���
//
class Window
{
	// �E�B���h�E�̎��ʎq
	GLFWwindow *const window;

  // ���f���r���[�ϊ��s��
  GgMatrix mw;

  // �������e�ϊ��s��
	GgMatrix mp;

  // �h���b�O�J�n�ʒu
  double cx, cy;

	// �^�C�v�����L�[
	int key;

public:

  //
	// �R���X�g���N�^
  //
  Window(const char *title = "Game Graphics", int width = 640, int height = 480);

  //
	// �f�X�g���N�^
  //
  virtual ~Window();

  //
  // �E�B���h�E�̎��ʎq�̎擾
  //
  const GLFWwindow *get() const
  {
    return window;
  }

  //
  // ��ʃN���A
  //
  //   �E�}�`�̕`��J�n�O�ɌĂяo��
  //   �E��ʂ̏����Ȃǂ��s��
  //
  void clear();

  //
  // �E�B���h�E�����ׂ����𔻒肷��
  //
  //   �E�`�惋�[�v�̌p�������Ƃ��Ďg��
  //
  int shouldClose() const;

  //
  // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
  //
  //   �E�}�`�̕`��I����ɌĂяo��
  //   �E�_�u���o�b�t�@�����O�̃o�b�t�@�̓���ւ����s��
  //   �E�L�[�{�[�h���쓙�̃C�x���g�����o��
  //
  void swapBuffers();

  //
  // �E�B���h�E�̃T�C�Y�ύX���̏���
  //
  //   �E�E�B���h�E�̃T�C�Y�ύX���ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
  //   �E�E�B���h�E�̍쐬���ɂ͖����I�ɌĂяo��
  //
  static void resize(GLFWwindow *window, int width, int height);

  //
  // �L�[�{�[�h���^�C�v�������̏���
  //
  //   �E�L�[�{�[�h���^�C�v�������ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
  //
  static void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods);

  //
  // �}�E�X�{�^���𑀍삵���Ƃ��̏���
  //
  //   �E�}�E�X�{�^�����������Ƃ��ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
  //
  static void mouse(GLFWwindow *window, int button, int action, int mods);

  //
  // �}�E�X�z�C�[�����쎞�̏���
  //
  //   �E�}�E�X�z�C�[���𑀍삵�����ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
  //
  static void wheel(GLFWwindow *window, double x, double y);

  //
  // ���f���r���[�ϊ��s��𓾂�
  //
  const GgMatrix &getMw() const
  {
    return mw;
  }

  //
  // �v���W�F�N�V�����ϊ��s��𓾂�
  //
  const GgMatrix &getMp() const
  {
    return mp;
  }

  //
	// ���O�Ƀ^�C�v���ꂽ�L�[�����o��
  //
	int getKey() const
	{
		return key;
	}

  //
	// ���݂̃L�[�̏�Ԃ𒲂ׂ�
  //
	int testKey(int key) const
	{
		return glfwGetKey(window, key);
	}
};
