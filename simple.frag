#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

// �J���[
layout (location = 2) uniform sampler2D color;        // �J���[�e�N�X�`��
layout (location = 3) uniform sampler2D image;        // ���e�e�N�X�`��
layout (location = 4) uniform sampler2DShadow depth;  // �f�v�X�}�b�v

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec4 iamb;                                         // �����̋��x
in vec4 idiff;                                        // �g�U���ˌ����x
in vec4 ispec;                                        // ���ʔ��ˌ����x
in vec2 ctex;                                         // �J���[�̃e�N�X�`�����W
in vec3 dtex;                                         // �f�v�X�̃e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                    // �t���O�����g�̐F

void main(void)
{
  // �V���h�E�}�b�v�̃e�N�X�`�����W�����߂�
  vec3 s = vec3(dtex.xy, dtex.z);

  // ���e���Ɏg���L���v�`���摜�͏㉺�����]���Ă���̂Ńe�N�X�`�����W���㉺���]����
  vec2 t = vec2(dtex.x, 1.0 - dtex.y);
  
  // �����ɗp���铊�e���̉�f�l���������x�Ƃ��ėp����
  //fc = texture(image, t);                                               // �A�e�Ȃ��E�V���h�E�}�b�s���O�Ȃ�
  //fc = * texture(depth, s) * texture(image, t);                         // �A�e�Ȃ��E�V���h�E�}�b�s���O����
  //fc = iamb + (idiff + ispec) * texture(image, t);                      // �V���h�E�}�b�s���O�Ȃ�
  fc = iamb + (idiff + ispec) * texture(depth, s) * texture(image, t);  // �V���h�E�}�b�s���O����

}