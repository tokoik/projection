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
  // �V���h�E�}�b�v�̃e�N�X�`�����W�����߂�
  vec3 s = vec3(textureSize(depth), 1.0) * texcoord;

  // ���e���Ɏg���L���v�`���摜�͏㉺�����]���Ă���̂Ńe�N�X�`�����W���㉺���]����
  vec2 t = textureSize(image) * vec2(texcoord.x, 1.0 - texcoord.y);
  
  // �����ɗp���铊�e���̉�f�l���������x�Ƃ��Ă�������
  fc = iamb + (idiff + ispec) * texture(depth, s) * texture(image, t);
}
