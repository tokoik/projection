#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// �V���h�E�}�b�v
uniform sampler2DRectShadow shadow;

// ���e���̃e�N�X�`��
uniform sampler2DRect image;

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec4 iamb;                                       // �����̔��ˌ����x
in vec4 idiff;                                      // �g�U���ˌ����x
in vec4 ispec;                                      // ���ʔ��ˌ����x
in vec2 tcoord;                                     // ���e���̃e�N�X�`�����W
in vec3 scoord;                                     // �V���h�E�}�b�v�̃e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main(void)
{
  //fc = iamb + (idiff + ispec) * texture(shadow, scoord) * texture(image, tcoord);
  fc = iamb + (idiff + ispec) * texture(image, tcoord);
}
