#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

// カラー
layout (location = 2) uniform sampler2D color;        // カラーテクスチャ
layout (location = 3) uniform sampler2D image;        // 投影テクスチャ
layout (location = 4) uniform sampler2DShadow depth;  // デプスマップ

// ラスタライザから受け取る頂点属性の補間値
in vec4 iamb;                                         // 環境光の強度
in vec4 idiff;                                        // 拡散反射光強度
in vec4 ispec;                                        // 鏡面反射光強度
in vec2 ctex;                                         // カラーのテクスチャ座標
in vec3 dtex;                                         // デプスのテクスチャ座標

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                    // フラグメントの色

void main(void)
{
  // シャドウマップのテクスチャ座標を求める
  vec3 s = vec3(dtex.xy, dtex.z);

  // 投影像に使うキャプチャ画像は上下が反転しているのでテクスチャ座標を上下反転する
  vec2 t = vec2(dtex.x, 1.0 - dtex.y);
  
  // 光源に用いる投影像の画素値を光源強度として用いる
  //fc = texture(image, t);                                               // 陰影なし・シャドウマッピングなし
  //fc = * texture(depth, s) * texture(image, t);                         // 陰影なし・シャドウマッピングあり
  //fc = iamb + (idiff + ispec) * texture(image, t);                      // シャドウマッピングなし
  fc = iamb + (idiff + ispec) * texture(depth, s) * texture(image, t);  // シャドウマッピングあり

}