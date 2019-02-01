#include "KinectV1.h"

//
// �[�x�Z���T�֘A�̏���
//

// �W�����C�u����
#include <cassert>

// Kinect �֘A
#pragma comment(lib, "Kinect10.lib")

// �v���s�\�_�̃f�t�H���g����
const GLfloat maxDepth(8.0f);

// �R���X�g���N�^
KinectV1::KinectV1()
  : DepthCamera(DEPTH_W, DEPTH_H, COLOR_W, COLOR_H)
  , nextColorFrameEvent(CreateEvent(NULL, TRUE, FALSE, NULL))
  , nextDepthFrameEvent(CreateEvent(NULL, TRUE, FALSE, NULL))
{
  HRESULT hr(S_OK);

  // �ŏ��̃C���X�^���X�𐶐�����Ƃ�����
  if (getActivated() == 0)
  {
    // �ڑ�����Ă���Z���T�̐��𒲂ׂ�
    hr = NuiGetSensorCount(&connected);
  }

  // �Z���T���ڑ�����Ă���g�p�䐔���ڑ��䐔�ɒB���Ă��Ȃ����
  if (hr == S_OK && getActivated() < connected)
  {
    // �Z���T�̎g�p���J�n����
    hr = NuiCreateSensorByIndex(getActivated(), &sensor);

    // �Z���T���g�p�\�ł����
    if (hr == S_OK && sensor->NuiStatus() == S_OK)
    {
      // �Z���T������������ (�J���[�ƃf�v�X���擾����)
      hr = sensor->NuiInitialize(
        NUI_INITIALIZE_FLAG_USES_COLOR |
        NUI_INITIALIZE_FLAG_USES_DEPTH |
        0);
      assert(hr == S_OK);

      // �Z���T�̋p������������
      hr = sensor->NuiCameraElevationSetAngle(0L);
      assert(hr == S_OK);

      // �f�v�X�X�g���[���̎擾�ݒ�
      hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, DEPTH_RESOLUTION,
        0, 2, nextDepthFrameEvent, &depthStream);
      assert(hr == S_OK);

      // �J���[�X�g���[���̎擾�ݒ�
      hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, COLOR_RESOLUTION,
        0, 2, nextColorFrameEvent, &colorStream);
      assert(hr == S_OK);

      // depthCount �� colorCount ���v�Z���ăe�N�X�`���ƃo�b�t�@�I�u�W�F�N�g���쐬����
      makeTexture();

      // �f�v�X�f�[�^����J�������W�����߂�Ƃ��ɗp����ꎞ���������m�ۂ���
      position = new GLfloat[depthCount][3];
    }
  }
}

// �f�X�g���N�^
KinectV1::~KinectV1()
{
  // �C�x���g�n���h�������
  CloseHandle(nextDepthFrameEvent);
  CloseHandle(nextColorFrameEvent);

  // �Z���T���L���ɂȂ��Ă�����
  if (getActivated() > 0)
  {
    // �f�[�^�ϊ��p�̃��������폜����
    delete[] position;

    // �Z���T���V���b�g�_�E������
    sensor->NuiShutdown();
  }
}

// �f�[�^���擾����
void KinectV1::getImage(HANDLE event, HANDLE stream,
  GLuint texture, GLsizei width, GLsizei height, GLenum format, GLenum type) const
{
  // �J���[�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, texture);

  // ���̃t���[���f�[�^���������Ă����
  if (WaitForSingleObject(event, 0) != WAIT_TIMEOUT)
  {
    // �擾�����t���[���f�[�^�̊i�[�ꏊ
    NUI_IMAGE_FRAME frame;

    // �t���[���f�[�^�̊i�[�ꏊ�� frame �Ɏ擾����
    if (sensor->NuiImageStreamGetNextFrame(stream, 0, &frame) == S_OK)
    {
      // ���ꂩ�珈�������܂Ńf�[�^���ύX����Ȃ��悤�Ƀ��b�N����
      NUI_LOCKED_RECT rect;
      frame.pFrameTexture->LockRect(0, &rect, NULL, 0);

      // ���b�N�ɐ���������
      if (rect.Pitch)
      {
        // �擾�����̂��f�v�X�f�[�^���J���[�f�[�^�ł����
        if (texture == depthTexture || texture == colorTexture)
        {
          // pBits �ɓ����Ă���f�[�^���e�N�X�`���ɓ]������
          glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, rect.pBits);
        }

        // �f�v�X�f�[�^���J�������W���擾����Ƃ���
        if (texture == depthTexture || texture == pointTexture)
        {
          // �J���[�f�[�^�̃e�N�X�`�����W�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
          glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
          GLfloat (*const texcoord)[2](static_cast<GLfloat (*)[2]>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
          for (int i = 0; i < depthCount; ++i)
          {
            // �f�v�X�̉�f�ʒu����J���[�̉�f�ʒu�����߂�
            LONG x, y;
            sensor->NuiImageGetColorPixelCoordinatesFromDepthPixel(COLOR_RESOLUTION,
              NULL, i % depthWidth, i / depthWidth, reinterpret_cast<USHORT *>(rect.pBits)[i], &x, &y);

            // �J���[�f�[�^�̃e�N�X�`�����W�ɕϊ�����
            texcoord[i][0] = GLfloat(x) + 0.5f;
            texcoord[i][1] = GLfloat(y) + 0.5f;
          }
          glUnmapBuffer(GL_ARRAY_BUFFER);

          // �J�������W���擾����Ƃ���
          if (texture == pointTexture)
          {
            // ���ׂĂ̓_�ɂ���
            for (int i = 0; i < depthCount; ++i)
            {
              // �f�v�X�l�̒P�ʂ����[�g���Ɋ��Z����W��
              static const GLfloat zScale(-0.001f / GLfloat(1 << NUI_IMAGE_PLAYER_INDEX_SHIFT));

              // ���̓_�̃f�v�X�l�𓾂�
              const unsigned short d(reinterpret_cast<USHORT *>(rect.pBits)[i]);

              // �f�v�X�l�̒P�ʂ����[�g���Ɋ��Z���� (�v���s�\�_�� maxDepth �ɂ���)
              const GLfloat z(d == 0 ? -maxDepth : GLfloat(d) * zScale);

              // �����Y�̉�p�ɂ��ƂÂ��X�P�[�������߂�
              const GLfloat s(NUI_CAMERA_DEPTH_NOMINAL_INVERSE_FOCAL_LENGTH_IN_PIXELS * z);

              // ���̓_�̃X�N���[����̈ʒu�����߂�
              const GLfloat x(float(i % depthWidth - depthWidth / 2) + 0.5f);
              const GLfloat y(float(i / depthWidth - depthHeight / 2) + 0.5f);

              // ���̓_�̃J�������W�����߂�
              position[i][0] = x * s;
              position[i][1] = y * s;
              position[i][2] = z;
            }

            // �J�������W���e�N�X�`���ɓ]������
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RGB, GL_FLOAT, position);
          }
        }

        // ���b�N�����f�[�^���J������
        const HRESULT hr(sensor->NuiImageStreamReleaseFrame(stream, &frame));
        assert(hr == S_OK);
      }
    }
  }
}
