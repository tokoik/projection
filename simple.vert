#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

// ����
uniform vec4 lamb;                                  // ��������
uniform vec4 ldiff;                                 // �g�U���ˌ�����
uniform vec4 lspec;                                 // ���ʔ��ˌ�����
uniform vec4 pl;                                    // �ʒu

// �ގ�
uniform vec4 kamb;                                  // �����̔��ˌW��
uniform vec4 kdiff;                                 // �g�U���ˌW��
uniform vec4 kspec;                                 // ���ʔ��ˌW��
uniform float kshi;                                 // �P���W��

// �ϊ��s��
uniform mat4 mw;                                      // ���_���W�n�ւ̕ϊ��s��
uniform mat4 mc;                                      // �N���b�s���O���W�n�ւ̕ϊ��s��
uniform mat4 mg;                                      // �@���x�N�g���̕ϊ��s��
uniform mat4 mt;                                      // �e�N�X�`�����W�̕ϊ��s��

// �e�N�X�`��
layout (location = 0) uniform sampler2D position;     // ���_�ʒu�̃e�N�X�`��
layout (location = 1) uniform sampler2D normal;       // �@���x�N�g���̃e�N�X�`��
layout (location = 2) uniform sampler2D color;        // �J���[�̃e�N�X�`��

// ���_����
layout (location = 0) in vec2 pc;                     // ���_�̃e�N�X�`�����W
layout (location = 1) in vec2 cc;                     // �J���[�̃e�N�X�`�����W

// ���X�^���C�U�ɑ��钸�_����
out vec4 iamb;                                        // �����̔��ˌ����x
out vec4 idiff;                                       // �g�U���ˌ����x
out vec4 ispec;                                       // ���ʔ��ˌ����x
out vec2 ctex;                                        // �J���[�̃e�N�X�`�����W
out vec3 dtex;                                        // �f�v�X�̃e�N�X�`�����W

void main(void)
{
  // ���_�ʒu
  vec4 pv = texture(position, pc);

  // �@���x�N�g��
  vec4 nv = texture(normal, pc);

  // ���W�v�Z
  vec4 p = mw * pv;                                   // ���_���W�n�̒��_�̈ʒu
  vec4 q = mw * pl;                                   // ���_���W�n�̌����̈ʒu
  vec3 v = normalize(p.xyz / p.w);                    // �����x�N�g��
  vec3 l = normalize((q * p.w - p * q.w).xyz);        // �����x�N�g��
  vec3 n = normalize((mg * nv).xyz);                  // �@���x�N�g��
  vec3 h = normalize(l - v);                          // ���ԃx�N�g��

  // �A�e�v�Z
  iamb = kamb * lamb;
  idiff = max(dot(n, l), 0.0) * kdiff * ldiff;
  ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

  // ���e���̃X�N���[�����W
  vec4 t = mt * pv;

  // �J���[�̃e�N�X�`�����W
  ctex = cc / vec2(textureSize(color, 0));

  // ���e���ƃV���h�E�}�b�v�̃e�N�X�`�����W�̓X�N���[�����W�� 1 �𑫂��� 2 �Ŋ���
  dtex = (t.xyz / t.w + 1.0) * 0.5;

  // �N���b�s���O���W�n�ɂ�������W�l
  gl_Position = mc * pv;
}
