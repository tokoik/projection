#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// シャドウマップ
uniform sampler2DRectShadow depth;

// 投影像のテクスチャ
uniform sampler2DRect image;

// ラスタライザから受け取る頂点属性の補間値
in vec4 iamb;                                       // 環境光の反射光強度
in vec4 idiff;                                      // 拡散反射光強度
in vec4 ispec;                                      // 鏡面反射光強度
in vec3 texcoord;                                   // 投影像とシャドウマップのテクスチャ座標

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                  // フラグメントの色

void main(void)
{
  // シャドウマップのテクスチャ座標を求める
  vec3 s = vec3(textureSize(depth), 1.0) * texcoord;

  // 投影像に使うキャプチャ画像は上下が反転しているのでテクスチャ座標を上下反転する
  vec2 t = textureSize(image) * vec2(texcoord.x, 1.0 - texcoord.y);
  
  // 光源に用いる投影像の画素値を光源強度としてもちいる
  fc = iamb + (idiff + ispec) * texture(depth, s) * texture(image, t);
}
