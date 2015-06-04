//
// 標準ライブラリ
//
#include <cmath>

//
// ウィンドウ関連の処理
//
#include "Window.h"

//
// コンストラクタ
//
//    title: ウィンドウのタイトルバーに表示する文字列
//    width: 開くウィンドウの幅
//    height: 開くウィンドウの高さ
//
Window::Window(const char *title, int width, int height)
  : window(glfwCreateWindow(width, height, title, NULL, NULL))
{
  // ウィンドウが作成できなかったらそのまま戻る
  if (window == NULL) return;

  // タイプしたキーの初期値を設定する
  key = 0;

  // 投影像のズームファクタの初期値
  zoom = 1.5;

  // 現在のウィンドウを処理対象にする
  glfwMakeContextCurrent(window);

  // 作成したウィンドウに対する設定
  glfwSwapInterval(1);

  // ゲームグラフィックス特論の都合にもとづく初期化
  ggInit();

  // このインスタンスの this ポインタを記録しておく
  glfwSetWindowUserPointer(window, this);

  // キーボード操作時に呼び出す処理の登録
  glfwSetKeyCallback(window, keyboard);

  // マウスボタン操作時に呼び出す処理の登録
  glfwSetMouseButtonCallback(window, mouse);

  // マウスホイール操作時に呼び出す処理の登録
  glfwSetScrollCallback(window, wheel);

  // ウィンドウのサイズ変更時に呼び出す処理を登録する
  glfwSetFramebufferSizeCallback(window, resize);

  // ウィンドウの設定を初期化する
  resize(window, width, height);
}

//
// デストラクタ
//
Window::~Window()
{
  glfwDestroyWindow(window);
}

//
// ウィンドウを閉じるべきかを判定する
//
//   ・描画ループの継続条件として使う
//
int Window::shouldClose() const
{
  return glfwWindowShouldClose(window) | glfwGetKey(window, GLFW_KEY_ESCAPE);
}

//
// 画面クリア
//
//   ・図形の描画開始前に呼び出す
//   ・画面の消去などを行う
//
void Window::clear()
{
  // ビューポートを設定する
  glViewport(0, 0, width, height);

  // 画面クリア
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//
// カラーバッファを入れ替えてイベントを取り出す
//
//   ・図形の描画終了後に呼び出す
//   ・ダブルバッファリングのバッファの入れ替えを行う
//   ・キーボード操作等のイベントを取り出す
//
void Window::swapBuffers()
{
  // カラーバッファを入れ替える
  glfwSwapBuffers(window);

  // OpenGL のエラーをチェックする
  ggError("SwapBuffers");

  // イベントを取り出す
  glfwPollEvents();

  // 左ボタンドラッグ
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
  {
    // マウスの現在位置を取得する
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // トラックボール処理
    ltb.motion(static_cast<float>(x), static_cast<float>(y));
  }

  // 右ボタンドラッグ
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
  {
    // マウスの現在位置を取得する
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // トラックボール処理
    rtb.motion(static_cast<float>(x), static_cast<float>(y));
  }
}

//
// ウィンドウのサイズ変更時の処理
//
//   ・ウィンドウのサイズ変更時にコールバック関数として呼び出される
//   ・ウィンドウの作成時には明示的に呼び出す
//
void Window::resize(GLFWwindow *window, int width, int height)
{
  // ウィンドウ全体をビューポートにする
  glViewport(0, 0, width, height);

  // このインスタンスの this ポインタを得る
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance != NULL)
  {
    // ビューポートを保存する
    instance->width = width;
    instance->height = height;
    
    // 投影変換行列を設定する
    instance->mp.loadPerspective(0.5f, (float)width / (float)height, 1.0f, 20.0f);

    // トラックボール処理の範囲を設定する
    instance->ltb.region(width, height);
    instance->rtb.region(width, height);
  }
}

//
// キーボードをタイプした時の処理
//
//   ・キーボードをタイプした時にコールバック関数として呼び出される
//
void Window::keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance != NULL)
    {
      // 押されたキーを保存する
      instance->key = key;
    }
  }
}

//
// マウスボタンを操作したときの処理
//
//   ・マウスボタンを押したときにコールバック関数として呼び出される
//
void Window::mouse(GLFWwindow *window, int button, int action, int mods)
{
  // このインスタンスの this ポインタを得る
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    // マウスの現在位置を取り出す
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // 押されたボタンの判定
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_1:
      // 左ボタンを押した時の処理
      if (action)
      {
        // トラックボール処理開始
        instance->ltb.start(static_cast<float>(x), static_cast<float>(y));
      }
      else
      {
        // トラックボール処理終了
        instance->ltb.stop(static_cast<float>(x), static_cast<float>(y));
      }
      break;
    case GLFW_MOUSE_BUTTON_2:
      // 右ボタンを押した時の処理
      if (action)
      {
        // トラックボール処理開始
        instance->rtb.start(static_cast<float>(x), static_cast<float>(y));
      }
      else
      {
        // トラックボール処理終了
        instance->rtb.stop(static_cast<float>(x), static_cast<float>(y));
      }
      break;
    case GLFW_MOUSE_BUTTON_3:
      // 中ボタンを押した時の処理
      break;
    default:
      break;
    }
  }
}

//
// マウスホイール操作時の処理
//
//   ・マウスホイールを操作した時にコールバック関数として呼び出される
//
void Window::wheel(GLFWwindow *window, double x, double y)
{
  // このインスタンスの this ポインタを得る
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    // 横の移動量が縦の移動量より大きければ
    if (fabs(x) > fabs(y))
    {
      // 左右に移動する
    }
    else
    {
      // 上下に移動する
      instance->zoom += y;
    }
  }
}
