#pragma once

// 補助プログラム
#include "gg.h"
using namespace gg;

//
// ウィンドウ関連の処理
//
class Window
{
	// ウィンドウの識別子
	GLFWwindow *const window;

  // モデルビュー変換行列
  GgMatrix mw;

  // 透視投影変換行列
	GgMatrix mp;

  // ドラッグ開始位置
  double cx, cy;

	// タイプしたキー
	int key;

public:

  //
	// コンストラクタ
  //
  Window(const char *title = "Game Graphics", int width = 640, int height = 480);

  //
	// デストラクタ
  //
  virtual ~Window();

  //
  // ウィンドウの識別子の取得
  //
  const GLFWwindow *get() const
  {
    return window;
  }

  //
  // 画面クリア
  //
  //   ・図形の描画開始前に呼び出す
  //   ・画面の消去などを行う
  //
  void clear();

  //
  // ウィンドウを閉じるべきかを判定する
  //
  //   ・描画ループの継続条件として使う
  //
  int shouldClose() const;

  //
  // カラーバッファを入れ替えてイベントを取り出す
  //
  //   ・図形の描画終了後に呼び出す
  //   ・ダブルバッファリングのバッファの入れ替えを行う
  //   ・キーボード操作等のイベントを取り出す
  //
  void swapBuffers();

  //
  // ウィンドウのサイズ変更時の処理
  //
  //   ・ウィンドウのサイズ変更時にコールバック関数として呼び出される
  //   ・ウィンドウの作成時には明示的に呼び出す
  //
  static void resize(GLFWwindow *window, int width, int height);

  //
  // キーボードをタイプした時の処理
  //
  //   ・キーボードをタイプした時にコールバック関数として呼び出される
  //
  static void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods);

  //
  // マウスボタンを操作したときの処理
  //
  //   ・マウスボタンを押したときにコールバック関数として呼び出される
  //
  static void mouse(GLFWwindow *window, int button, int action, int mods);

  //
  // マウスホイール操作時の処理
  //
  //   ・マウスホイールを操作した時にコールバック関数として呼び出される
  //
  static void wheel(GLFWwindow *window, double x, double y);

  //
  // モデルビュー変換行列を得る
  //
  const GgMatrix &getMw() const
  {
    return mw;
  }

  //
  // プロジェクション変換行列を得る
  //
  const GgMatrix &getMp() const
  {
    return mp;
  }

  //
	// 直前にタイプされたキーを取り出す
  //
	int getKey() const
	{
		return key;
	}

  //
	// 現在のキーの状態を調べる
  //
	int testKey(int key) const
	{
		return glfwGetKey(window, key);
	}
};
