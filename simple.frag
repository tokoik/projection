#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// �V���h�E�}�b�v
uniform sampler2DRectShadow depth;

// ���e���̃e�N�X�`��
uniform sampler2DRect image;

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec4 iamb;                                       // �����̔��ˌ����x
in vec4 idiff;                                      // �g�U���ˌ����x
in vec4 ispec;                                      // ���ʔ��ˌ����x
in vec3 texcoord;                                   // ���e���ƃV���h�E�}�b�v�̃e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main(void)
{
  vec2 t = vec2(texcoord.x, textureSize(image).y - texcoord.y);
  fc = iamb + (idiff + ispec) * texture(image, t) * texture(depth, texcoord);
}
