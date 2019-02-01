#include "DepthCamera.h"

//
// �[�x�Z���T�֘A�̊��N���X
//

// OpenCV
#include <opencv2/highgui/highgui.hpp>
#ifdef _WIN32
#  define CV_VERSION_STR CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
#  ifdef _DEBUG
#    define CV_EXT_STR "d.lib"
#  else
#    define CV_EXT_STR ".lib"
#  endif
#  pragma comment(lib, "opencv_world" CV_VERSION_STR CV_EXT_STR)
#endif

// depthCount �� colorCount ���v�Z���ăe�N�X�`���ƃo�b�t�@�I�u�W�F�N�g���쐬����
void DepthCamera::makeTexture()
{
  // �f�v�X�f�[�^�ƃJ���[�f�[�^�̉�f�������߂�
  depthCount = depthWidth * depthHeight;
  colorCount = colorWidth * colorHeight;

  // �f�v�X�f�[�^���i�[����e�N�X�`������������
  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, depthWidth, depthHeight, 0, GL_RED, GL_UNSIGNED_SHORT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // �f�v�X�f�[�^���狁�߂��J�������W���i�[����e�N�X�`������������
  glGenTextures(1, &pointTexture);
  glBindTexture(GL_TEXTURE_2D, pointTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, depthWidth, depthHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // �J���[�f�[�^���i�[����e�N�X�`������������
  glGenTextures(1, &colorTexture);
  glBindTexture(GL_TEXTURE_2D, colorTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, colorWidth, colorHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // �f�v�X�f�[�^�̉�f�ʒu�̃J���[�̃e�N�X�`�����W���i�[����o�b�t�@�I�u�W�F�N�g����������
  glGenBuffers(1, &coordBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
  glBufferData(GL_ARRAY_BUFFER, depthCount * 2 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  // �g�p���Ă���Z���T�̐��𐔂���
  ++activated;
}

// �f�X�g���N�^
DepthCamera::~DepthCamera()
{
  // �Z���T���L���ɂȂ��Ă�����
  if (activated > 0)
  {
    // �e�N�X�`�����폜����
    glDeleteTextures(1, &depthTexture);
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &pointTexture);

    // �o�b�t�@�I�u�W�F�N�g���폜����
    glDeleteBuffers(1, &coordBuffer);

    // �g�p���Ă���Z���T�̐������炷
    --activated;
  }
}

// �ڑ����Ă���Z���T�̐�
int DepthCamera::connected(0);

// �g�p���Ă���Z���T�̐�
int DepthCamera::activated(0);
