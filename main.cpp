#include <iostream>
#include <cstdlib>
#include <opencv2/highgui/highgui.hpp>
#include <atlstr.h>  

//
// �E�B���h�E�֘A�̏���
//
#include "Window.h"

// �Z���T�֘A�̏���
#include "KinectV2.h"

// �`��ɗp���郁�b�V��
#include "Mesh.h"

// �v�Z�ɗp����V�F�[�_
#include "Calculate.h"

// ���_�ʒu�̐������V�F�[�_ (position.frag) �ōs���Ȃ� 1
#define GENERATE_POSITION 1

// ����
const GgSimpleShader::Light light =
{
  { 0.2f, 0.2f, 0.2f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 0.0f, 0.0f, 5.0f, 1.0f }
};

// �ގ�
const GgSimpleShader::Material material =
{
  { 0.8f, 0.8f, 0.8f, 1.0f },                           // �����̔��ˌW��
  { 0.8f, 0.8f, 0.8f, 1.0f },                           // �g�U���ˌW��
  { 0.2f, 0.2f, 0.2f, 1.0f },                           // ���ʔ��ˌW��
  50.0f                                                 // �P���W��
};

//
// �v���O�����I�����̏���
//
static void cleanup()
{
	// GLFW �̏I������
	glfwTerminate();
}

//
// ���C���v���O����
//
int main() try
{
	//
	// GLFW �̐ݒ�
	//

	// GLFW ������������
	if (glfwInit() == GL_FALSE)
	{
		// �������Ɏ��s����
		std::cerr << "Can't initialize GLFW." << std::endl;
		return 1;
	}

	// �v���O�����I�����̏�����o�^����
	atexit(cleanup);

	// OpenGL Version 3.2 Core Profile ��I������
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// �E�B���h�E���쐬����
	Window window("Projection Mapping Simulator", 1280, 720);

	// �E�B���h�E���쐬�ł������m�F����
  if (!window.get()) throw std::runtime_error("GLFW �̃E�B���h�E���J���܂���");

  //
  // OpenCV �̐ݒ�
  //

  // OpenCV �ɂ��r�f�I�L���v�`��������������
  cv::VideoCapture camera(0);
  if (!camera.isOpened()) throw std::runtime_error("OpenCV �̃J�������g�p�ł��܂���");

  // ���͉摜�̃T�C�Y���擾����
  camera.grab();
  const GLsizei capture_width(GLsizei(camera.get(CV_CAP_PROP_FRAME_WIDTH)));
  const GLsizei capture_height(GLsizei(camera.get(CV_CAP_PROP_FRAME_HEIGHT)));

  //
	// �[�x�Z���T�̏�����
	//

	// �[�x�Z���T��L���ɂ���
	KinectV2 sensor;
	if (sensor.getActivated() == 0) throw std::runtime_error("�[�x�Z���T���g���܂���");

	// �[�x�Z���T�̉𑜓x
	int width, height;
	sensor.getDepthResolution(&width, &height);

	// �`��Ɏg�����b�V��
	const Mesh mesh(width, height, sensor.getCoordBuffer());

	//
	// �V�F�[�_�̐ݒ�
	//

	// �`��p�̃V�F�[�_��ǂݍ���
	GgSimpleShader simple("simple.vert", "simple.frag");

  // �`��p�̃V�F�[�_���ǂݍ��߂����m�F����
  if (simple.get() == 0) throw std::runtime_error("�`��p�̃V�F�[�_���쐬�ł��܂���");

	// �e�N�X�`���ϊ��s��̏ꏊ�����o��
	const GLuint mtLoc(glGetUniformLocation(simple.get(), "mt"));

	// �V���h�E�}�b�v�쐬�p�̃V�F�[�_��ǂݍ���
	GgSimpleShader shadow("shadow.vert", "shadow.frag");

	// �V���h�E�}�b�v�쐬�p�̃V�F�[�_���ǂݍ��߂����m�F����
  if (shadow.get() == 0) throw std::runtime_error("�V���h�E�}�b�v�p�̃V�F�[�_���쐬�ł��܂���");

	// �V���h�E�}�b�v�쐬�p�̕ϊ��s��̏ꏊ�����o��
	const GLuint msLoc(glGetUniformLocation(shadow.get(), "ms"));

	// �f�v�X�f�[�^���璸�_�ʒu���v�Z����V�F�[�_
	const Calculate position(width, height, "position.frag");

	// ���_�ʒu����@���x�N�g�����v�Z����V�F�[�_
	const Calculate normal(width, height, "normal.frag");

	//
	// OpenGL �̐ݒ�
	//

	// �w�i�F���w�肷��
	glClearColor(0.2f, 0.3f, 0.4f, 0.0f);

	// �B�ʏ�����L���ɂ���
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// ���e����摜��ۑ�����e�N�X�`������������
	GLuint image;
	glGenTextures(1, &image);
	glBindTexture(GL_TEXTURE_2D, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, capture_width, capture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// �e�N�X�`���̋��E�F���w�肷�� (���e���̊O���̘R�k���̋��x�Ƃ��Ďg��)
	const GLfloat borderColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// FBO �̃T�C�Y�̓L���v�`���摜�Ɠ����ɂ���
	const GLsizei fbo_width(capture_width), fbo_height(capture_height);

	// �V���h�E�}�b�v���쐬���� FBO �p�̃e�N�X�`������������
	GLuint depth;
	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fbo_width, fbo_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// �e�N�X�`���̋��E�[�x���w�肷�� (�ŏ��̗v�f���f�v�X�e�N�X�`���̊O���̐[�x�Ƃ��Ďg����)
	const GLfloat borderDepth[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderDepth);

	// �������ރ|���S���̃e�N�X�`�����W�l�� r �v�f�ƃe�N�X�`���Ƃ̔�r���s���悤�ɂ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	// ���� r �v�f�̒l���e�N�X�`���̒l�ȉ��Ȃ�^ (�܂����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// �V���h�E�}�b�v���擾���� FBO �p����������
	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

	//
	// �`��
	//

	// ����ϊ��s�� (���_�̈ʒu�̏����l�������̈ʒu�ɂ���)
	const GgMatrix mv(ggLookat(light.position[0], light.position[1], light.position[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

	// ���e���̃A�X�y�N�g��
	const GLfloat aspect(GLfloat(capture_width) / GLfloat(capture_height));

	// �E�B���h�E���J���Ă���ԌJ��Ԃ�
	while (window.shouldClose() == GL_FALSE)
	{
    // �f�����L���v�`������
    if (camera.grab())
    {
      // �L���v�`���f������摜��؂�o��
      cv::Mat frame;
      camera.retrieve(frame);

      // �؂�o�����摜���e�N�X�`���ɓ]������
      glBindTexture(GL_TEXTURE_2D, image);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    }

#if GENERATE_POSITION
		// ���_�ʒu�̌v�Z
		position.use();
		glUniform1i(0, 0);
		glActiveTexture(GL_TEXTURE0);
		sensor.getDepth();
		const std::vector<GLuint> &positionTexture(position.calculate());

		// �@���x�N�g���̌v�Z
		normal.use();
		glUniform1i(0, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, positionTexture[0]);
		const std::vector<GLuint> &normalTexture(normal.calculate());
#else
		// �@���x�N�g���̌v�Z
		normal.use();
		glUniform1i(0, 0);
		glActiveTexture(GL_TEXTURE0);
		sensor.getPoint();
		const std::vector<GLuint> &normalTexture(normal.calculate());
#endif

		// ���f���r���[�ϊ��s��
		const GgMatrix mw(mv * window.getLtb() * window.getMm());

		// �`��p�̃V�F�[�_�v���O�����̎g�p�J�n
		simple.use(light, window.getMp(), mw);
		simple.setLight(light);
		simple.setMaterial(material);

		// �e�N�X�`��
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

		// �V���h�E�}�b�s���O�p�̕ϊ��s��
		const GgMatrix ms(ggPerspective(window.getZoom() * 0.01f + 0.3f, aspect, 3.2f, 6.8f) * mv * window.getRtb());

    // �`�����t���[���o�b�t�@�I�u�W�F�N�g�ɐ؂�ւ���
		glBindFramebuffer(GL_FRAMEBUFFER, fb);

		// �J���[�o�b�t�@�͖����̂œǂݏ������Ȃ�
		glDrawBuffer(GL_NONE);

		// �f�v�X�o�b�t�@��������������
		glClear(GL_DEPTH_BUFFER_BIT);

		// �r���[�|�[�g�� FBO �̃T�C�Y�ɐݒ肷��
		glViewport(0, 0, fbo_width, fbo_height);

		// �V���h�E�}�b�v�Ɏg���f�v�X�e�N�X�`���쐬�p�̃V�F�[�_��I������
		shadow.use();

		// �V���h�E�}�b�s���O�p�̓��e�ϊ��s���ݒ肷��
		glUniformMatrix4fv(msLoc, 1, GL_FALSE, ms.get());

		// �w�ʃ|���S���������V���h�E�}�b�v�p�̃t���[���o�b�t�@�I�u�W�F�N�g�ɕ`�悷��
		glCullFace(GL_FRONT);
		mesh.draw();
		glCullFace(GL_BACK);

		// �`����ʏ�̃t���[���o�b�t�@�ɐ؂�ւ���
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// �J���[�o�b�t�@�ւ̏������݂�L���ɂ��� (�_�u���o�b�t�@�����O�Ȃ̂Ńo�b�N�o�b�t�@�ɕ`��)
		glDrawBuffer(GL_BACK);

		// ��ʂ���������
		window.clear();

		// �`��p�̃V�F�[�_��I������
		simple.use(light, window.getMp(), mw);

		// �V���h�E�}�b�v�̍쐬�Ɏg�������e�ϊ��s��𓊉e���̃e�N�X�`���ϊ��s��Ɏg��
		glUniformMatrix4fv(mtLoc, 1, GL_FALSE, ms.get());

		// ���e����f���̃e�N�X�`�����j�b�g���w�肷��
    glUniform1i(3, 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, image);

		// �V���h�E�}�b�v�̃e�N�X�`�����j�b�g���w�肷��
    glUniform1i(4, 4);
    glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depth);

		// �}�`��`�悷��
		mesh.draw();

		// �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
		window.swapBuffers();
	}
}
catch (const std::runtime_error &e)
{
  // �G���[���b�Z�[�W��\������
#if defined(_WIN32)
  MessageBox(NULL, CString(e.what()), TEXT("�_��"), MB_ICONERROR);
#else
  std::cerr << e.what() << "\n\n[Type enter key] ";
  std::cin.get();
#endif

  // �u���O�������I������
  return EXIT_FAILURE;
}
