#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// �ϊ��s��
uniform mat4 ms;                                    // ���K�����ꂽ�V���h�E�}�b�v�̍��W�n�ւ̕ϊ��s��

// ���_����
layout (location = 0) in vec4 pv;                   // ���[�J�����W�n�̒��_�ʒu

void main(void)
{
  // �V���h�E�}�b�v�̍쐬���ɂ͉A�e�t�����͍s��Ȃ��̂ō��W�ϊ��������s��
  gl_Position = ms * pv;
}
