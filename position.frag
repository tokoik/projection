#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER)
#define DEPTH_MAXIMUM (-10.0)

// スケール
const vec2 scale = vec2(
  1.546592,
  1.222434
);

// テクスチャ
layout (location = 0) uniform sampler2D depth;

// テクスチャ座標
in vec2 texcoord;

// フレームバッファに出力するデータ
layout (location = 0) out vec3 position;

// デプス値をスケーリングする
vec2 s(in float z)
{
  return vec2(z, 1.0 - step(z, 0.0));
}

void main(void)
{
  // デプス値を取り出す
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

  // デプス値からカメラ座標値を求める
  position = vec3((texcoord - 0.5) * scale * z, z);
}
