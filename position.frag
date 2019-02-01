#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER)
#define DEPTH_MAXIMUM (-10.0)

// �X�P�[��
const vec2 scale = vec2(
  1.546592,
  1.222434
);

// �e�N�X�`��
layout (location = 0) uniform sampler2D depth;

// �e�N�X�`�����W
in vec2 texcoord;

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec3 position;

// �f�v�X�l���X�P�[�����O����
vec2 s(in float z)
{
  return vec2(z, 1.0 - step(z, 0.0));
}

void main(void)
{
  // �f�v�X�l�����o��
  vec2 sum = s(texture(depth, texcoord).r);
  sum += s(textureOffset(depth, texcoord, ivec2( 1,  0)).r);
  sum += s(textureOffset(depth, texcoord, ivec2(-1,  0)).r);
  sum += s(textureOffset(depth, texcoord, ivec2( 0,  1)).r);
  sum += s(textureOffset(depth, texcoord, ivec2( 0, -1)).r);
  sum += s(textureOffset(depth, texcoord, ivec2( 1,  1)).r);
  sum += s(textureOffset(depth, texcoord, ivec2(-1,  1)).r);
  sum += s(textureOffset(depth, texcoord, ivec2( 1, -1)).r);
  sum += s(textureOffset(depth, texcoord, ivec2(-1, -1)).r);

  float z = sum.x == 0.0 ? DEPTH_MAXIMUM : sum.x * DEPTH_SCALE / sum.y;

  // �f�v�X�l����J�������W�l�����߂�
  position = vec3((texcoord - 0.5) * scale * z, z);
}
