#pragma once

//
// ��`
//

// �}�`�`��
#include "Shape.h"

class Rect : public Shape
{
  // ���_�o�b�t�@�I�u�W�F�N�g
  GLuint vbo;

public:

  // �R���X�g���N�^
  Rect();

  // �f�X�g���N�^
  virtual ~Rect();

  // �`��
  virtual void draw() const;
};
