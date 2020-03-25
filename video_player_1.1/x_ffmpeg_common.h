#ifndef _X_FFMPEG_COMMON_H_
#define _X_FFMPEG_COMMON_H_


#include <thread>
#include <chrono>
#include <mutex>

// ffmepg�Ǵ�C�⣬��C++����������Ҫָ��C�﷨����
extern "C"
{
#include <libavcodec\avcodec.h>
#include <libavformat\avformat.h>
#include <libavutil\avutil.h>
#include <libswscale\swscale.h>
#include <libswresample\swresample.h>
#include <libavutil\imgutils.h>
#include <libavutil\pixdesc.h>
#include <libavutil\time.h>
}


#define X_INT_INDEX_NONE -1
#define X_UINT_INDEX_NONE 0xFFFFFFFF

#define X_FRAME_MAX_CHANNEL 8

#define X_AVSYNC_DYNAMIC_COEFFICIENT 0.0160119  // ��̬֡���㷨��ϵ�� �ⷽ�� (1+x)^6 = 1.1 ��
                                                // �����ʱ��(ffmepgʱ��) Ϊ 6λ����ʱ�򣬿���
                                                // ֡�ʵ���ʱ���ڱ�׼��ʱ�����ӻ�������ʱ���
                                                // (1.1-1)��

#define X_AVSYNC_DYNAMIC_THRESHOLD 0.003        // ����Ƶͬ����̬֡�ʽ��и�Ԥ�Ķ��ߵ�ǰʱ������ֵ

#define X_AVSYNC_SKIP_FRAME -0x1001
//#define X_AVSYNC_NOT_DELAY -0x1002

#endif //_X_FFMPEG_COMMON_H_
