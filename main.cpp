#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

//
// ウィンドウ関連の処理
//
#include "Window.h"

// 形状データファイル名
const char model[] = "bunny.obj";

// 光源
const GgSimpleShader::Light light =
{
  { 0.2f, 0.2f, 0.2f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 0.0f, 0.0f, 5.0f, 1.0f }
};

//
// プログラム終了時の処理
//
static void cleanup()
{
  // GLFW の終了処理
  glfwTerminate();
}

//
// メインプログラム
//
int main()
{
  //
  // OpenCV の設定
  //

  // OpenCV によるビデオキャプチャを初期化する
  cv::VideoCapture camera(0);
  if (!camera.isOpened())
  {
    // カメラが使えなかった
    std::cerr << "Can't open camera." << std::endl;
    return 1;
  }
  
  // 入力画像のサイズを取得する
  camera.grab();
  const GLsizei capture_width(GLsizei(camera.get(CV_CAP_PROP_FRAME_WIDTH)));
  const GLsizei capture_height(GLsizei(camera.get(CV_CAP_PROP_FRAME_HEIGHT)));
  
  //
  // GLFW の設定
  //

  // GLFW を初期化する
  if (glfwInit() == GL_FALSE)
  {
    // 初期化に失敗した
    std::cerr << "Can't initialize GLFW." << std::endl;
    return 1;
  }

  // プログラム終了時の処理を登録する
  atexit(cleanup);

  // OpenGL Version 3.2 Core Profile を選択する
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // ウィンドウを作成する
  Window window("Projection Mapping Simulator", 800, 800);

  // ウィンドウが作成できたか確認する
  if (!window.get())
  {
    // ウィンドウが開けなかった
    std::cerr << "Can't open GLFW window." << std::endl;
    return 1;
  }

  //
  // シェーダの設定
  //

  // 描画用のシェーダを読み込む
  GgSimpleShader simple("simple.vert", "simple.frag");

  // 描画用のシェーダが読み込めたか確認する
  if (!simple.get()) return 1;

  // 投影する映像のテクスチャのサンプラの場所を取り出す
  const GLuint imageLoc(glGetUniformLocation(simple.get(), "image"));

  // シャドウマップのテクスチャのサンプラの場所を取り出す
  const GLuint depthLoc(glGetUniformLocation(simple.get(), "depth"));

  // テクスチャ変換行列の場所を取り出す
  const GLuint mtLoc(glGetUniformLocation(simple.get(), "mt"));

  // シャドウマップ作成用のシェーダを読み込む
  GgSimpleShader shadow("shadow.vert", "shadow.frag");

  // シャドウマップ作成用のシェーダが読み込めたか確認する
  if (!shadow.get()) return 1;

  // シャドウマップ作成用の変換行列の場所を取り出す
  const GLuint msLoc(glGetUniformLocation(shadow.get(), "ms"));

  //
  // 描画データの設定
  //

  // 図形を読み込む
  GgObj obj(model, true);
  
  // 図形が読み込めたか確認する
  if (!obj.get()) return 1;

  //
  // OpenGL の設定
  //

  // 背景色を指定する
  glClearColor(0.2f, 0.3f, 0.4f, 0.0f);

  // 隠面消去を有効にする
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  // 投影する画像を保存するテクスチャを準備する
  GLuint image;
  glGenTextures(1, &image);
  glBindTexture(GL_TEXTURE_RECTANGLE, image);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, capture_width, capture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // テクスチャの境界色を指定する (投影像の外側の漏洩光の強度として使う)
  const GLfloat borderColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
  glTexParameterfv(GL_TEXTURE_RECTANGLE, GL_TEXTURE_BORDER_COLOR, borderColor);

  // FBO のサイズはキャプチャ画像と同じにする
  const GLsizei fbo_width(capture_width), fbo_height(capture_height);
  
  // シャドウマップを作成する FBO 用のテクスチャを準備する
  GLuint depth;
  glGenTextures(1, &depth);
  glBindTexture(GL_TEXTURE_RECTANGLE, depth);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT, fbo_width, fbo_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // テクスチャの境界深度を指定する (最初の要素がデプステクスチャの外側の深度として使われる)
  const GLfloat borderDepth[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_RECTANGLE, GL_TEXTURE_BORDER_COLOR, borderDepth);

  // 書き込むポリゴンのテクスチャ座標値の r 要素とテクスチャとの比較を行うようにする
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

  // もし r 要素の値がテクスチャの値以下なら真 (つまり日向)
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

  // シャドウマップを取得する FBO 用を準備する
  GLuint fb;
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, depth, 0);

  //
  // 描画
  //

  // 視野変換行列 (視点の位置の初期値を光源の位置にする)
  const GgMatrix mv(ggLookat(light.position[0], light.position[1], light.position[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // 投影像のアスペクト比
  const GLfloat aspect(GLfloat(capture_width) / GLfloat(capture_height));

  // ウィンドウが開いている間繰り返す
  while (window.shouldClose() == GL_FALSE)
  {
    if (camera.grab())
    {
      // キャプチャ映像から画像を切り出す
      cv::Mat frame;
      camera.retrieve(frame);

      // 切り出した画像をテクスチャに転送する
      glBindTexture(GL_TEXTURE_RECTANGLE, image);
      glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    }

    // シャドウマッピング用の変換行列
    const GgMatrix ms(ggPerspective(window.getZoom() * 0.01f + 0.3f, aspect, 3.2f, 6.8f) * mv);

    // 描画先をフレームバッファオブジェクトに切り替える
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    // カラーバッファは無いので読み書きしない
    glDrawBuffer(GL_NONE);

    // デプスバッファだけを消去する
    glClear(GL_DEPTH_BUFFER_BIT);

    // ビューポートを FBO のサイズに設定する
    glViewport(0, 0, fbo_width, fbo_height);

    // シャドウマップに使うデプステクスチャ作成用のシェーダを選択する
    shadow.use();
    
    // シャドウマッピング用の投影変換行列を設定する
    glUniformMatrix4fv(msLoc, 1, GL_FALSE, ms.get());
    
    // デプステクスチャの作成には図形の材質設定は必要ない
    obj.attachShader(nullptr);

    // 背面ポリゴンだけをシャドウマップ用のフレームバッファオブジェクトに描画する
    glCullFace(GL_FRONT);
    obj.draw();
    glCullFace(GL_BACK);

    // 描画先を通常のフレームバッファに切り替える
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // カラーバッファへの書き込みを有効にする (ダブルバッファリングなのでバックバッファに描く)
    glDrawBuffer(GL_BACK);

    // 画面を消去する
    window.clear();

    // 描画用のシェーダを選択する
    simple.use(light, window.getMp(), mv * window.getLtb());

    // 図形の材質設定を行う
    obj.attachShader(simple);
    
    // シャドウマップの作成に使った投影変換行列を投影像のテクスチャ変換行列に使う
    glUniformMatrix4fv(mtLoc, 1, GL_FALSE, ms.get());

    // 投影する映像のテクスチャユニットを指定する
    glUniform1i(imageLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_RECTANGLE, image);
    
    // シャドウマップのテクスチャユニットを指定する
    glUniform1i(depthLoc, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_RECTANGLE, depth);

    // 図形を描画する
    obj.draw();

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
