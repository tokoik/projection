#include <iostream>
#include <cstdlib>
#include <opencv2/highgui/highgui.hpp>
#ifdef _WIN32
#  define CV_VERSION_STR CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
#  ifdef _DEBUG
#    define CV_EXT_STR "d.lib"
#  else
#    define CV_EXT_STR ".lib"
#  endif
#  pragma comment(lib, "opencv_core" CV_VERSION_STR CV_EXT_STR)
#  pragma comment(lib, "opencv_highgui" CV_VERSION_STR CV_EXT_STR)
#endif

//
// �E�B���h�E�֘A�̏���
//
#include "Window.h"

// �`��f�[�^�t�@�C����
const char model[] = "bunny.obj";

// ����
const GgSimpleShader::Light light =
{
  { 0.2f, 0.2f, 0.2f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 0.0f, 0.0f, 0.0f, 1.0f }
};

//
// �v���O�����I�����̏���
//
static void cleanup()
{
  // GLFW �̏I������
  glfwTerminate();
}

//
// ���C���v���O����
//
int main()
{
  //
  // OpenCV �̐ݒ�
  //

  // OpenCV �ɂ��r�f�I�L���v�`��������������
  cv::VideoCapture camera(0);
  if (!camera.isOpened())
  {
    // �J�������g���Ȃ�����
    std::cerr << "Can't open camera." << std::endl;
    return 1;
  }
  
  // �J�����̏����ݒ�
  camera.grab();
  const GLsizei capture_width(GLsizei(camera.get(CV_CAP_PROP_FRAME_WIDTH)));
  const GLsizei capture_height(GLsizei(camera.get(CV_CAP_PROP_FRAME_HEIGHT)));
  
  //
  // GLFW �̐ݒ�
  //

  // GLFW ������������
  if (glfwInit() == GL_FALSE)
  {
    // �������Ɏ��s����
    std::cerr << "Can't initialize GLFW." << std::endl;
    return 1;
  }

  // �v���O�����I�����̏�����o�^����
  atexit(cleanup);

  // OpenGL Version 3.2 Core Profile ��I������
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // �E�B���h�E���쐬����
  Window window("Projection Mapping Simulator", capture_width, capture_height);

  // �E�B���h�E���쐬�ł������m�F����
  if (!window.get())
  {
    // �E�B���h�E���J���Ȃ�����
    std::cerr << "Can't open GLFW window." << std::endl;
    return 1;
  }

  //
  // �V�F�[�_�̐ݒ�
  //

  // �V�F�[�_��ǂݍ���
  GgSimpleShader simple("simple.vert", "simple.frag");

  // �V�F�[�_���ǂݍ��߂����m�F����
  if (!simple.get()) return 1;

  // �e�N�X�`���̃T���v���̏ꏊ�����o��
  const GLuint imageLoc(glGetUniformLocation(simple.get(), "image"));

  // �e�N�X�`���ϊ��s��̏ꏊ�����o��
  const GLuint mtLoc(glGetUniformLocation(simple.get(), "mt"));

  //
  // �`��f�[�^�̐ݒ�
  //

  // �}�`��ǂݍ���
  GgObj obj(model, true);
  
  // �}�`���ǂݍ��߂����m�F����
  if (!obj.get()) return 1;

  // �ǂݍ��񂾐}�`��`���V�F�[�_���w�肷��
  obj.attachShader(simple);

  //
  // OpenGL �̐ݒ�
  //

  // �w�i�F���w�肷��
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

  // �B�ʏ�����L���ɂ���
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  // �e�N�X�`������������
  GLuint image;
  glGenTextures(1, &image);
  glBindTexture(GL_TEXTURE_RECTANGLE, image);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, capture_width, capture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // �e�N�X�`���̋��E�F���w�肷��
  const GLfloat border[] = { 0.1f, 0.1f, 0.1f, 1.0f };
  glTexParameterfv(GL_TEXTURE_RECTANGLE, GL_TEXTURE_BORDER_COLOR, border);

  //
  // �`��
  //

  // �E�B���h�E���J���Ă���ԌJ��Ԃ�
  while (window.shouldClose() == GL_FALSE)
  {
    if (camera.grab())
    {
      // �L���v�`���f������摜��؂�o��
      cv::Mat frame;
      camera.retrieve(frame, 3);

      // �؂�o�����摜���e�N�X�`���ɓ]������
      cv::Mat flipped;
      cv::flip(frame, flipped, 0);
      glBindTexture(GL_TEXTURE_RECTANGLE, image);
      glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, frame.cols, flipped.rows, GL_BGR, GL_UNSIGNED_BYTE, flipped.data);
    }

    // ��ʂ���������
    window.clear();

    // �V�F�[�_��I������
    obj.getShader()->use();

    // �����ʒu���w�肷��
    obj.getShader()->setLight(light);

    // �ϊ��s���ݒ肷��
    obj.getShader()->loadMatrix(window.getMp(), window.getMv() * window.getLtb());

    // �e�N�X�`���ϊ��s���ݒ肷��
    glUniformMatrix4fv(mtLoc, 1, GL_FALSE, window.getRtb());

    // �e�N�X�`�����j�b�g���w�肷��
    glUniform1i(imageLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_RECTANGLE, image);
    
    // �}�`��`�悷��
    obj.draw();

    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
