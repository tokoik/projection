#include <iostream>
#include <cstdlib>
#include <opencv2/highgui/highgui.hpp>
#include <atlstr.h>  

//
// ウィンドウ関連の処理
//
#include "Window.h"

// センサ関連の処理
#include "KinectV2.h"

// 描画に用いるメッシュ
#include "Mesh.h"

// 計算に用いるシェーダ
#include "Calculate.h"

// 頂点位置の生成をシェーダ (position.frag) で行うなら 1
#define GENERATE_POSITION 1

// 光源
const GgSimpleShader::Light light =
{
  { 0.2f, 0.2f, 0.2f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 0.0f, 0.0f, 5.0f, 1.0f }
};

// 材質
const GgSimpleShader::Material material =
{
  { 0.8f, 0.8f, 0.8f, 1.0f },                           // 環境光の反射係数
  { 0.8f, 0.8f, 0.8f, 1.0f },                           // 拡散反射係数
  { 0.2f, 0.2f, 0.2f, 1.0f },                           // 鏡面反射係数
  50.0f                                                 // 輝き係数
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
int main() try
{
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
	Window window("Projection Mapping Simulator", 1280, 720);

	// ウィンドウが作成できたか確認する
  if (!window.get()) throw std::runtime_error("GLFW のウィンドウが開けません");

  //
  // OpenCV の設定
  //

  // OpenCV によるビデオキャプチャを初期化する
  cv::VideoCapture camera(0);
  if (!camera.isOpened()) throw std::runtime_error("OpenCV のカメラが使用できません");

  // 入力画像のサイズを取得する
  camera.grab();
  const GLsizei capture_width(GLsizei(camera.get(CV_CAP_PROP_FRAME_WIDTH)));
  const GLsizei capture_height(GLsizei(camera.get(CV_CAP_PROP_FRAME_HEIGHT)));

  //
	// 深度センサの初期化
	//

	// 深度センサを有効にする
	KinectV2 sensor;
	if (sensor.getActivated() == 0) throw std::runtime_error("深度センサが使えません");

	// 深度センサの解像度
	int width, height;
	sensor.getDepthResolution(&width, &height);

	// 描画に使うメッシュ
	const Mesh mesh(width, height, sensor.getCoordBuffer());

	//
	// シェーダの設定
	//

	// 描画用のシェーダを読み込む
	GgSimpleShader simple("simple.vert", "simple.frag");

  // 描画用のシェーダが読み込めたか確認する
  if (simple.get() == 0) throw std::runtime_error("描画用のシェーダが作成できません");

	// テクスチャ変換行列の場所を取り出す
	const GLuint mtLoc(glGetUniformLocation(simple.get(), "mt"));

	// シャドウマップ作成用のシェーダを読み込む
	GgSimpleShader shadow("shadow.vert", "shadow.frag");

	// シャドウマップ作成用のシェーダが読み込めたか確認する
  if (shadow.get() == 0) throw std::runtime_error("シャドウマップ用のシェーダが作成できません");

	// シャドウマップ作成用の変換行列の場所を取り出す
	const GLuint msLoc(glGetUniformLocation(shadow.get(), "ms"));

	// デプスデータから頂点位置を計算するシェーダ
	const Calculate position(width, height, "position.frag");

	// 頂点位置から法線ベクトルを計算するシェーダ
	const Calculate normal(width, height, "normal.frag");

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
	glBindTexture(GL_TEXTURE_2D, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, capture_width, capture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// テクスチャの境界色を指定する (投影像の外側の漏洩光の強度として使う)
	const GLfloat borderColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// FBO のサイズはキャプチャ画像と同じにする
	const GLsizei fbo_width(capture_width), fbo_height(capture_height);

	// シャドウマップを作成する FBO 用のテクスチャを準備する
	GLuint depth;
	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fbo_width, fbo_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// テクスチャの境界深度を指定する (最初の要素がデプステクスチャの外側の深度として使われる)
	const GLfloat borderDepth[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderDepth);

	// 書き込むポリゴンのテクスチャ座標値の r 要素とテクスチャとの比較を行うようにする
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	// もし r 要素の値がテクスチャの値以下なら真 (つまり日向)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// シャドウマップを取得する FBO 用を準備する
	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

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
    // 映像をキャプチャする
    if (camera.grab())
    {
      // キャプチャ映像から画像を切り出す
      cv::Mat frame;
      camera.retrieve(frame);

      // 切り出した画像をテクスチャに転送する
      glBindTexture(GL_TEXTURE_2D, image);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    }

#if GENERATE_POSITION
		// 頂点位置の計算
		position.use();
		glUniform1i(0, 0);
		glActiveTexture(GL_TEXTURE0);
		sensor.getDepth();
		const std::vector<GLuint> &positionTexture(position.calculate());

		// 法線ベクトルの計算
		normal.use();
		glUniform1i(0, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, positionTexture[0]);
		const std::vector<GLuint> &normalTexture(normal.calculate());
#else
		// 法線ベクトルの計算
		normal.use();
		glUniform1i(0, 0);
		glActiveTexture(GL_TEXTURE0);
		sensor.getPoint();
		const std::vector<GLuint> &normalTexture(normal.calculate());
#endif

		// モデルビュー変換行列
		const GgMatrix mw(mv * window.getLtb() * window.getMm());

		// 描画用のシェーダプログラムの使用開始
		simple.use(light, window.getMp(), mw);
		simple.setLight(light);
		simple.setMaterial(material);

		// テクスチャ
#if GENERATE_POSITION
		glUniform1i(0, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, positionTexture[0]);
#endif
		glUniform1i(1, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalTexture[0]);
		glUniform1i(2, 2);
		glActiveTexture(GL_TEXTURE2);
		sensor.getColor();

		// シャドウマッピング用の変換行列
		const GgMatrix ms(ggPerspective(window.getZoom() * 0.01f + 0.3f, aspect, 3.2f, 6.8f) * mv * window.getRtb());

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

		// 背面ポリゴンだけをシャドウマップ用のフレームバッファオブジェクトに描画する
		glCullFace(GL_FRONT);
		mesh.draw();
		glCullFace(GL_BACK);

		// 描画先を通常のフレームバッファに切り替える
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// カラーバッファへの書き込みを有効にする (ダブルバッファリングなのでバックバッファに描く)
		glDrawBuffer(GL_BACK);

		// 画面を消去する
		window.clear();

		// 描画用のシェーダを選択する
		simple.use(light, window.getMp(), mw);

		// シャドウマップの作成に使った投影変換行列を投影像のテクスチャ変換行列に使う
		glUniformMatrix4fv(mtLoc, 1, GL_FALSE, ms.get());

		// 投影する映像のテクスチャユニットを指定する
    glUniform1i(3, 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, image);

		// シャドウマップのテクスチャユニットを指定する
    glUniform1i(4, 4);
    glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depth);

		// 図形を描画する
		mesh.draw();

		// カラーバッファを入れ替えてイベントを取り出す
		window.swapBuffers();
	}
}
catch (const std::runtime_error &e)
{
  // エラーメッセージを表示する
#if defined(_WIN32)
  MessageBox(NULL, CString(e.what()), TEXT("神保"), MB_ICONERROR);
#else
  std::cerr << e.what() << "\n\n[Type enter key] ";
  std::cin.get();
#endif

  // ブログラムを終了する
  return EXIT_FAILURE;
}
