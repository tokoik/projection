#include "Rect.h"

//
// ��`
//

// �R���X�g���N�^
Rect::Rect()
{
  // ���_�o�b�t�@�I�u�W�F�N�g���쐬����
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // ���_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ��ăf�[�^��]������
  static const GLfloat points[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f };
  glBufferData(GL_ARRAY_BUFFER, sizeof points, points, GL_STATIC_DRAW);

  // 0 �Ԃ� attribute �ϐ�����f�[�^����͂���
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
}

// �f�X�g���N�^
Rect::~Rect()
{
  // ���_�o�b�t�@�I�u�W�F�N�g���폜����
  glDeleteBuffers(1, &vbo);
}

// �`��
void Rect::draw() const
{
  // ���_�z��I�u�W�F�N�g���w�肵�ĕ`�悷��
  Shape::draw();
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
