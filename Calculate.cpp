#include "Calculate.h"

//
// �摜����
//

// �R���X�g���N�^
Calculate::Calculate(int width, int height, const char *source, int uniforms, int targets)
  : width(width)
  , height(height)
  , program(ggLoadShader("rectangle.vert", source))
  , uniforms(uniforms)
{
  // �v�Z���ʂ��i�[����t���[���o�b�t�@�I�u�W�F�N�g���쐬����
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  for (int i = 0; i < targets; ++i)
  {
    // �t���[���o�b�t�@�I�u�W�F�N�g�̃^�[�Q�b�g�Ɏg���e�N�X�`�����쐬����
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // �t���[���o�b�t�@�I�u�W�F�N�g�Ƀe�N�X�`����ǉ�����
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, tex, 0);

    // �����_�����O�^�[�Q�b�g��ݒ肷��
    texture.push_back(tex);
    bufs.push_back(GL_COLOR_ATTACHMENT0 + i);
  }

  // �t���[���o�b�t�@�I�u�W�F�N�g�ւ̕`��Ɏg����`���쐬����
  if (count++ == 0) rectangle = new Rect;
}

// �f�X�g���N�^
Calculate::~Calculate()
{
  // �V�F�[�_�v���O�������폜����
  glDeleteShader(program);

  // �t���[���o�b�t�@�I�u�W�F�N�g���폜����
  glDeleteFramebuffers(1, &fbo);

  // �t���[���o�b�t�@�I�u�W�F�N�g�ւ̕`��Ɏg����`���폜����
  if (--count == 0) delete rectangle;
}

// �v�Z�����s����
const std::vector<GLuint> &Calculate::calculate() const
{
  // �t���[���o�b�t�@�I�u�W�F�N�g�Ƀ����_�����O
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glDrawBuffers(GLsizei(bufs.size()), bufs.data());

  // �B�ʏ����𖳌��ɂ���
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  // �r���[�|�[�g���t���[���o�b�t�@�I�u�W�F�N�g�̃T�C�Y�ɐݒ肷��
  glViewport(0, 0, width, height);

  // �N���b�s���O��Ԃ����ς��̋�`�������_�����O����
  rectangle->draw();

  // ���̃����_�����O��ɖ߂�
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);

  // �B�ʏ�����L���ɂ���
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  return texture;
}

// �v�Z�Ɏg����`
const Rect *Calculate::rectangle;

// ���t�@�����X�J�E���g
unsigned int Calculate::count(0);
