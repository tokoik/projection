#include <iostream>
#include <cstdlib>
#include <opencv2/highgui/highgui.hpp>
#ifdef _WIN32
#  define CV_VERSION_STR CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
#  ifdef _DEBUG
#    define CV_EXT_STR "d.lib"
#  else
#    define CV_EXT_STR ".lib"
#  endif
#  pragma comment(lib, "opencv_core" CV_VERSION_STR CV_EXT_STR)
#  pragma comment(lib, "opencv_highgui" CV_VERSION_STR CV_EXT_STR)
#endif

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
  { 0.0f, 0.0f, 0.0f, 1.0f }
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
  
  // カメラの初期設定
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
  Window window("Projection Mapping Simulator", capture_width, capture_height);

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

  // シェーダを読み込む
  GgSimpleShader simple("simple.vert", "simple.frag");

  // シェーダが読み込めたか確認する
  if (!simple.get()) return 1;

  // テクスチャのサンプラの場所を取り出す
  const GLuint imageLoc(glGetUniformLocation(simple.get(), "image"));

  // テクスチャ変換行列の場所を取り出す
  const GLuint mtLoc(glGetUniformLocation(simple.get(), "mt"));

  //
  // 描画データの設定
  //

  // 図形を読み込む
  GgObj obj(model, true);
  
  // 図形が読み込めたか確認する
  if (!obj.get()) return 1;

  // 読み込んだ図形を描くシェーダを指定する
  obj.attachShader(simple);

  //
  // OpenGL の設定
  //

  // 背景色を指定する
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

  // 隠面消去を有効にする
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  // テクスチャを準備する
  GLuint image;
  glGenTextures(1, &image);
  glBindTexture(GL_TEXTURE_RECTANGLE, image);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, capture_width, capture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // テクスチャの境界色を指定する
  const GLfloat border[] = { 0.1f, 0.1f, 0.1f, 1.0f };
  glTexParameterfv(GL_TEXTURE_RECTANGLE, GL_TEXTURE_BORDER_COLOR, border);

  //
  // 描画
  //

  // ウィンドウが開いている間繰り返す
  while (window.shouldClose() == GL_FALSE)
  {
    if (camera.grab())
    {
      // キャプチャ映像から画像を切り出す
      cv::Mat frame;
      camera.retrieve(frame, 3);

      // 切り出した画像をテクスチャに転送する
      cv::Mat flipped;
      cv::flip(frame, flipped, 0);
      glBindTexture(GL_TEXTURE_RECTANGLE, image);
      glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, frame.cols, flipped.rows, GL_BGR, GL_UNSIGNED_BYTE, flipped.data);
    }

    // 画面を消去する
    window.clear();

    // シェーダを選択する
    obj.getShader()->use();

    // 光源位置を指定する
    obj.getShader()->setLight(light);

    // 変換行列を設定する
    obj.getShader()->loadMatrix(window.getMp(), window.getMv() * window.getLtb());

    // テクスチャ変換行列を設定する
    glUniformMatrix4fv(mtLoc, 1, GL_FALSE, window.getRtb());

    // テクスチャユニットを指定する
    glUniform1i(imageLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_RECTANGLE, image);
    
    // 図形を描画する
    obj.draw();

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
