#pragma once

//
// �`��}�`
//

// �E�B���h�E�֘A�̏���
#include "Window.h"

class Shape
{
  // ���_�z��I�u�W�F�N�g
  GLuint vao;

public:

  // �R���X�g���N�^
  Shape();

  // �f�X�g���N�^
  virtual ~Shape();

  // �`��
  virtual void draw() const = 0;
};
