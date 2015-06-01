#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// ���e���̃e�N�X�`��
uniform sampler2DRect image;

// �e�N�X�`���̃X�P�[��
uniform vec2 scale;

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec4 iamb;                                       // �����̔��ˌ����x
in vec4 idiff;                                      // �g�U���ˌ����x
in vec4 ispec;                                      // ���ʔ��ˌ����x
in vec4 texcoord;                                   // �e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main(void)
{
  vec2 t = textureSize(image) * (scale * texcoord.xy / texcoord.w + 0.5);
  fc = iamb + (idiff + ispec) * texture(image, t);
}
