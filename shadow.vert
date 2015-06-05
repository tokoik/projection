#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// 変換行列
uniform mat4 ms;                                    // 正規化されたシャドウマップの座標系への変換行列

// 頂点属性
layout (location = 0) in vec4 pv;                   // ローカル座標系の頂点位置

void main(void)
{
  // シャドウマップの作成時には陰影付け等は行わないので座標変換だけを行う
  gl_Position = ms * pv;
}
