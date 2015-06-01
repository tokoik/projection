#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// 投影像のテクスチャ
uniform sampler2DRect image;

// テクスチャのスケール
uniform vec2 scale;

// ラスタライザから受け取る頂点属性の補間値
in vec4 iamb;                                       // 環境光の反射光強度
in vec4 idiff;                                      // 拡散反射光強度
in vec4 ispec;                                      // 鏡面反射光強度
in vec4 texcoord;                                   // テクスチャ座標

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                  // フラグメントの色

void main(void)
{
  vec2 t = textureSize(image) * (scale * texcoord.xy / texcoord.w + 0.5);
  fc = iamb + (idiff + ispec) * texture(image, t);
}
