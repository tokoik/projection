#include "Shape.h"

//
// �}�`�`��
//

// �R���X�g���N�^
Shape::Shape()
{
  // ���_�z��I�u�W�F�N�g���쐬����
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
}

// �f�X�g���N�^
Shape::~Shape()
{
  // ���_�z��I�u�W�F�N�g���폜����
  glDeleteVertexArrays(1, &vao);
}

// �`��
void Shape::draw() const
{
  // ���_�z��I�u�W�F�N�g���w�肷��
  glBindVertexArray(vao);
}
