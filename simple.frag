#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// シャドウマップ
uniform sampler2DRectShadow shadow;

// 投影像のテクスチャ
uniform sampler2DRect image;

// ラスタライザから受け取る頂点属性の補間値
in vec4 iamb;                                       // 環境光の反射光強度
in vec4 idiff;                                      // 拡散反射光強度
in vec4 ispec;                                      // 鏡面反射光強度
in vec2 tcoord;                                     // 投影像のテクスチャ座標
in vec3 scoord;                                     // シャドウマップのテクスチャ座標

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                  // フラグメントの色

void main(void)
{
  //fc = iamb + (idiff + ispec) * texture(shadow, scoord) * texture(image, tcoord);
  fc = iamb + (idiff + ispec) * texture(image, tcoord);
}
