//
// �W�����C�u����
//
#include <cmath>

//
// �E�B���h�E�֘A�̏���
//
#include "Window.h"

//
// �R���X�g���N�^
//
//    title: �E�B���h�E�̃^�C�g���o�[�ɕ\�����镶����
//    width: �J���E�B���h�E�̕�
//    height: �J���E�B���h�E�̍���
//
Window::Window(const char *title, int width, int height)
  : window(glfwCreateWindow(width, height, title, NULL, NULL))
{
  // �E�B���h�E���쐬�ł��Ȃ������炻�̂܂ܖ߂�
  if (window == NULL) return;

  // �^�C�v�����L�[�̏����l��ݒ肷��
  key = 0;

  // ���e���̃Y�[���t�@�N�^�̏����l
  zoom = 1.5;

  // ���݂̃E�B���h�E�������Ώۂɂ���
  glfwMakeContextCurrent(window);

  // �쐬�����E�B���h�E�ɑ΂���ݒ�
  glfwSwapInterval(1);

  // �Q�[���O���t�B�b�N�X���_�̓s���ɂ��ƂÂ�������
  ggInit();

  // ���̃C���X�^���X�� this �|�C���^���L�^���Ă���
  glfwSetWindowUserPointer(window, this);

  // �L�[�{�[�h���쎞�ɌĂяo�������̓o�^
  glfwSetKeyCallback(window, keyboard);

  // �}�E�X�{�^�����쎞�ɌĂяo�������̓o�^
  glfwSetMouseButtonCallback(window, mouse);

  // �}�E�X�z�C�[�����쎞�ɌĂяo�������̓o�^
  glfwSetScrollCallback(window, wheel);

  // �E�B���h�E�̃T�C�Y�ύX���ɌĂяo��������o�^����
  glfwSetFramebufferSizeCallback(window, resize);

  // �E�B���h�E�̐ݒ������������
  resize(window, width, height);
}

//
// �f�X�g���N�^
//
Window::~Window()
{
  glfwDestroyWindow(window);
}

//
// �E�B���h�E�����ׂ����𔻒肷��
//
//   �E�`�惋�[�v�̌p�������Ƃ��Ďg��
//
int Window::shouldClose() const
{
  return glfwWindowShouldClose(window) | glfwGetKey(window, GLFW_KEY_ESCAPE);
}

//
// ��ʃN���A
//
//   �E�}�`�̕`��J�n�O�ɌĂяo��
//   �E��ʂ̏����Ȃǂ��s��
//
void Window::clear()
{
  // �r���[�|�[�g��ݒ肷��
  glViewport(0, 0, width, height);

  // ��ʃN���A
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//
// �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
//
//   �E�}�`�̕`��I����ɌĂяo��
//   �E�_�u���o�b�t�@�����O�̃o�b�t�@�̓���ւ����s��
//   �E�L�[�{�[�h���쓙�̃C�x���g�����o��
//
void Window::swapBuffers()
{
  // �J���[�o�b�t�@�����ւ���
  glfwSwapBuffers(window);

  // OpenGL �̃G���[���`�F�b�N����
  ggError("SwapBuffers");

  // �C�x���g�����o��
  glfwPollEvents();

  // ���{�^���h���b�O
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
  {
    // �}�E�X�̌��݈ʒu���擾����
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // �g���b�N�{�[������
    ltb.motion(static_cast<float>(x), static_cast<float>(y));
  }

  // �E�{�^���h���b�O
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
  {
    // �}�E�X�̌��݈ʒu���擾����
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // �g���b�N�{�[������
    rtb.motion(static_cast<float>(x), static_cast<float>(y));
  }
}

//
// �E�B���h�E�̃T�C�Y�ύX���̏���
//
//   �E�E�B���h�E�̃T�C�Y�ύX���ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
//   �E�E�B���h�E�̍쐬���ɂ͖����I�ɌĂяo��
//
void Window::resize(GLFWwindow *window, int width, int height)
{
  // �E�B���h�E�S�̂��r���[�|�[�g�ɂ���
  glViewport(0, 0, width, height);

  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance != NULL)
  {
    // �r���[�|�[�g��ۑ�����
    instance->width = width;
    instance->height = height;
    
    // ���e�ϊ��s���ݒ肷��
    instance->mp.loadPerspective(0.5f, (float)width / (float)height, 1.0f, 20.0f);

    // �g���b�N�{�[�������͈̔͂�ݒ肷��
    instance->ltb.region(width, height);
    instance->rtb.region(width, height);
  }
}

//
// �L�[�{�[�h���^�C�v�������̏���
//
//   �E�L�[�{�[�h���^�C�v�������ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
//
void Window::keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    // ���̃C���X�^���X�� this �|�C���^�𓾂�
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance != NULL)
    {
      // �����ꂽ�L�[��ۑ�����
      instance->key = key;
    }
  }
}

//
// �}�E�X�{�^���𑀍삵���Ƃ��̏���
//
//   �E�}�E�X�{�^�����������Ƃ��ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
//
void Window::mouse(GLFWwindow *window, int button, int action, int mods)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    // �}�E�X�̌��݈ʒu�����o��
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // �����ꂽ�{�^���̔���
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_1:
      // ���{�^�������������̏���
      if (action)
      {
        // �g���b�N�{�[�������J�n
        instance->ltb.start(static_cast<float>(x), static_cast<float>(y));
      }
      else
      {
        // �g���b�N�{�[�������I��
        instance->ltb.stop(static_cast<float>(x), static_cast<float>(y));
      }
      break;
    case GLFW_MOUSE_BUTTON_2:
      // �E�{�^�������������̏���
      if (action)
      {
        // �g���b�N�{�[�������J�n
        instance->rtb.start(static_cast<float>(x), static_cast<float>(y));
      }
      else
      {
        // �g���b�N�{�[�������I��
        instance->rtb.stop(static_cast<float>(x), static_cast<float>(y));
      }
      break;
    case GLFW_MOUSE_BUTTON_3:
      // ���{�^�������������̏���
      break;
    default:
      break;
    }
  }
}

//
// �}�E�X�z�C�[�����쎞�̏���
//
//   �E�}�E�X�z�C�[���𑀍삵�����ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
//
void Window::wheel(GLFWwindow *window, double x, double y)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    // ���̈ړ��ʂ��c�̈ړ��ʂ��傫�����
    if (fabs(x) > fabs(y))
    {
      // ���E�Ɉړ�����
    }
    else
    {
      // �㉺�Ɉړ�����
      instance->zoom += y;
    }
  }
}
