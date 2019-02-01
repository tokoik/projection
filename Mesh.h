#pragma once

//
// ���b�V��
//

// �}�`�`��
#include "Shape.h"

class Mesh : public Shape
{
  // ���b�V���̕�
  const GLsizei slices;

  // ���b�V���̍���
  const GLsizei stacks;

  // �f�[�^�Ƃ��ĕێ����钸�_��
  const GLsizei vertices;

  // ���ۂɕ`�悷�钸�_��
  const GLsizei indexes;

  // �f�v�X�f�[�^�̃T���v�����O�Ɏg���e�N�X�`�����W���i�[����o�b�t�@�I�u�W�F�N�g
  GLuint depthCoord;

  // ���_�̃C���f�b�N�X���i�[����o�b�t�@�I�u�W�F�N�g
  GLuint indexBuffer;

  // �e�N�X�`�����W�𐶐����ăo�C���h����Ă���o�b�t�@�I�u�W�F�N�g�ɓ]������
  void genCoord();

public:

  // �R���X�g���N�^
  Mesh(int stacks, int slices, GLuint coordBuffer = 0);

  // �f�X�g���N�^
  virtual ~Mesh();

  // �`��
  virtual void draw() const;
};
