#include "KinectV2.h"

//
// �[�x�Z���T�֘A�̏���
//

// �W�����C�u����
#include <cassert>

// Kinect �֘A
#pragma comment(lib, "Kinect20.lib")

// �v���s�\�_�̃f�t�H���g����
const GLfloat maxDepth(10.0f);

// �R���X�g���N�^
KinectV2::KinectV2()
{
  // �Z���T���擾����
  if (sensor == NULL && GetDefaultKinectSensor(&sensor) == S_OK)
  {
    HRESULT hr;

    // �Z���T�̎g�p���J�n����
    hr = sensor->Open();
    assert(hr == S_OK);

    // �f�v�X�f�[�^�̓ǂݍ��ݐݒ�
    IDepthFrameSource *depthSource;
    hr = sensor->get_DepthFrameSource(&depthSource);
    assert(hr == S_OK);
    hr = depthSource->OpenReader(&depthReader);
    assert(hr == S_OK);
    IFrameDescription *depthDescription;
    hr = depthSource->get_FrameDescription(&depthDescription);
    assert(hr == S_OK);
    depthSource->Release();

    // �f�v�X�f�[�^�̃T�C�Y�𓾂�
    depthDescription->get_Width(&depthWidth);
    depthDescription->get_Height(&depthHeight);
    depthDescription->Release();

    // �J���[�f�[�^�̓ǂݍ��ݐݒ�
    IColorFrameSource *colorSource;
    hr = sensor->get_ColorFrameSource(&colorSource);
    assert(hr == S_OK);
    hr = colorSource->OpenReader(&colorReader);
    assert(hr == S_OK);
    IFrameDescription *colorDescription;
    hr = colorSource->get_FrameDescription(&colorDescription);
    assert(hr == S_OK);
    colorSource->Release();

    // �J���[�f�[�^�̃T�C�Y�𓾂�
    colorDescription->get_Width(&colorWidth);
    colorDescription->get_Height(&colorHeight);
    colorDescription->Release();

    // ���W�̃}�b�s���O
    hr = sensor->get_CoordinateMapper(&coordinateMapper);
    assert(hr == S_OK);

    // depthCount �� colorCount ���v�Z���ăe�N�X�`���ƃo�b�t�@�I�u�W�F�N�g���쐬����
    makeTexture();

    // �f�v�X�f�[�^����J�������W�����߂�Ƃ��ɗp����ꎞ���������m�ۂ���
    position = new GLfloat[depthCount][3];

    // �J���[�f�[�^��ϊ�����p����ꎞ���������m�ۂ���
    color = new GLubyte[colorCount * 4];
  }
}

// �f�X�g���N�^
KinectV2::~KinectV2()
{
  if (getActivated() > 0)
  {
    // �f�[�^�ϊ��p�̃��������폜����
    delete[] position;
    delete[] color;

    // �Z���T���J������
    colorReader->Release();
    depthReader->Release();
    coordinateMapper->Release();
    sensor->Close();
    sensor->Release();

    // �Z���T���J���������Ƃ��L�^����
    sensor = NULL;
  }
}

// �f�v�X�f�[�^���擾����
GLuint KinectV2::getDepth() const
{
  // �f�v�X�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, depthTexture);

  // ���̃f�v�X�̃t���[���f�[�^���������Ă����
  IDepthFrame *depthFrame;
  if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
  {
    // �f�v�X�f�[�^�̃T�C�Y�Ɗi�[�ꏊ�𓾂�
    UINT depthSize;
    UINT16 *depthBuffer;
    depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

    // �J���[�̃e�N�X�`�����W�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    ColorSpacePoint *const texcoord(static_cast<ColorSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
    coordinateMapper->MapDepthFrameToColorSpace(depthCount, depthBuffer, depthCount, texcoord);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // �f�v�X�f�[�^���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RED, GL_UNSIGNED_SHORT, depthBuffer);

    // �f�v�X�t���[�����J������
    depthFrame->Release();
  }

  return depthTexture;
}

// �J�������W���擾����
GLuint KinectV2::getPoint() const
{
  // �J�������W�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, pointTexture);

  // ���̃f�v�X�̃t���[���f�[�^���������Ă����
  IDepthFrame *depthFrame;
  if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
  {
    // �f�v�X�f�[�^�̃T�C�Y�Ɗi�[�ꏊ�𓾂�
    UINT depthSize;
    UINT16 *depthBuffer;
    depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

    // �J���[�̃e�N�X�`�����W�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    ColorSpacePoint *const texcoord(static_cast<ColorSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
    coordinateMapper->MapDepthFrameToColorSpace(depthCount, depthBuffer, depthCount, texcoord);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // �J�������W�ւ̕ϊ��e�[�u���𓾂�
    UINT32 entry;
    PointF *table;
    coordinateMapper->GetDepthFrameToCameraSpaceTable(&entry, &table);

    // ���ׂĂ̓_�ɂ���
    for (unsigned int i = 0; i < entry; ++i)
    {
      // �f�v�X�l�̒P�ʂ����[�g���Ɋ��Z����W��
      static const GLfloat zScale(-0.001f);

      // ���̓_�̃f�v�X�l�𓾂�
      const unsigned short d(depthBuffer[i]);

      // �f�v�X�l�̒P�ʂ����[�g���Ɋ��Z���� (�v���s�\�_�� maxDepth �ɂ���)
      const GLfloat z(d == 0 ? -maxDepth : GLfloat(d) * zScale);

      // ���̓_�̃X�N���[����̈ʒu�����߂�
      const GLfloat x(table[i].X);
      const GLfloat y(-table[i].Y);

      // ���̓_�̃J�������W�����߂�
      position[i][0] = x * z;
      position[i][1] = y * z;
      position[i][2] = z;
    }

    // �J�������W��]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RGB, GL_FLOAT, position);

    // �e�[�u�����J������
    CoTaskMemFree(table);

    // �f�v�X�t���[�����J������
    depthFrame->Release();
  }

  return pointTexture;
}

// �J���[�f�[�^���擾����
GLuint KinectV2::getColor() const
{
  // �J���[�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, colorTexture);

  // ���̃J���[�̃t���[���f�[�^���������Ă����
  IColorFrame *colorFrame;
  if (colorReader->AcquireLatestFrame(&colorFrame) == S_OK)
  {
    // �J���[�f�[�^���擾���� RGBA �`���ɕϊ�����
    colorFrame->CopyConvertedFrameDataToArray(colorCount * 4,
      static_cast<BYTE *>(color), ColorImageFormat::ColorImageFormat_Bgra);

    // �J���[�t���[�����J������
    colorFrame->Release();

    // �J���[�f�[�^���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorWidth, colorHeight, GL_BGRA, GL_UNSIGNED_BYTE, color);
  }

  return colorTexture;
}

// �Z���T�̎��ʎq
IKinectSensor *KinectV2::sensor(NULL);
