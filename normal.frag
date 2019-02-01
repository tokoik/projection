#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

// �e�N�X�`��
layout (location = 0) uniform sampler2D position;

// �e�N�X�`�����W
in vec2 texcoord;

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec3 normal;

void main(void)
{
  // �ߖT�̌��z�����߂�
  vec3 vx = vec3(textureOffset(position, texcoord, ivec2(1, 0)) - textureOffset(position, texcoord, ivec2(-1, 0)));
  vec3 vy = vec3(textureOffset(position, texcoord, ivec2(0, 1)) - textureOffset(position, texcoord, ivec2(0, -1)));

  // ���z���炩��@���x�N�g�������߂�
  normal = normalize(cross(vx, vy));
}
