#ifndef _X_FFMPEG_AV_SYNC_H_
#define _X_FFMPEG_AV_SYNC_H_

#include <cmath>
#include <string>

#include "x_ffmpeg_common.h"

namespace x
{
    namespace ffmpeg
    {
		// ����Ƶͬ��ģ�飬����ƵPTSΪ��ʱ�ӣ���Ƶͬ����Ƶ
		class xAVSync
		{
		private:
			volatile double audio_clock_;			//��Ƶʱ�ӣ���ʱ��
			volatile double last_video_pts_;		//��һ֡����ƵPTS
			volatile double video_show_start_time_;	//��Ƶ֡��ʾ���ڵ���ʼʱ��
		public:
			xAVSync()
			{
				audio_clock_ = 0.0;
				last_video_pts_ = 0.0;
				video_show_start_time_ = 0.0;
			}
			~xAVSync()
			{

			}
		public:
			// �趨��ǰ��Ƶʱ��
			void SetAudioClock(double _pts)
			{
				audio_clock_ = _pts;
			}

			// ��ȡ������ͼ����ʾ����ʼʱ��
			void SetVideoShowTime()
			{
				video_show_start_time_ = av_gettime_relative() / AV_TIME_BASE * 1.0;
			}

			// �������֡�ʿ��Ƶ��ӳ�(΢��)
			int64_t CalDelay(double _pts)
			{
				int64_t i64_delay = 0; // ��������ʱ��(΢��)
				double elapsed_time = 0.0;

				if (video_show_start_time_ == 0.0)
					SetVideoShowTime(); // ���� 0ֵ������ֵӰ���һ֡����

				double diff = _pts - audio_clock_;	   //���㵱ǰ��Ƶ֡ʱ������ʱ�����۲�ֵ
				double delay = _pts - last_video_pts_; //���㱾֡��������Ҫ����һ֡����ʱ��֮������ʾ
				int series = std::to_string(static_cast<int64_t>(diff * AV_TIME_BASE)).size();

				last_video_pts_ = _pts; //��¼��һ֡��PTS

				if (diff > X_AVSYNC_DYNAMIC_THRESHOLD)
				{
					// ��� ʱ�Ӳ�ֵΪ���������ʾ��Ƶ������ǰ����ȡ��ֵ�� 
					// ��((1.0 + X_AVSYNC_DYNAMIC_COEFFICIENT)^series - 1.0)��
					// ��Ϊ����֡�ʵ���ʱʱ������������� ʵ���ӳ� = ������ʱ + ������;
					diff = diff * (std::pow(1.0 + X_AVSYNC_DYNAMIC_COEFFICIENT, series) - 1.0);
				}
				else if (diff < -X_AVSYNC_DYNAMIC_THRESHOLD)
				{
					// ��� ʱ�Ӳ�ֵΪ���������ʾ��Ƶ������ǰ����ȡ��ֵ�� 
					// ��((1.0 + X_AVSYNC_DYNAMIC_COEFFICIENT)^series - 1.0)��
					// ��Ϊ����֡�ʵ���ʱʱ������������� ʵ���ӳ� = ������ʱ - |������|;
					diff = diff * (std::pow(1.0 + X_AVSYNC_DYNAMIC_COEFFICIENT, series) - 1.0);
				}
				// ���ˣ�������������㷨���Լ����Դ���ģ�������*0.1Ҳ��

				if (delay > 0.0 && (delay + diff) > 0.0)
				{
					delay += diff;

					//����ʾʱ������������ʱ�� = ������ʾ��Ƶ֡ʱ��-������ʼʱ��
					elapsed_time = av_gettime_relative() / AV_TIME_BASE * 1.0 - video_show_start_time_;

					//�����ӳ�ʱ�� = ʵ���ӳ�ʱ�� - �Ѿ�����ʱ�䡣��ת��Ϊ΢��
					i64_delay = static_cast<int64_t>((delay - elapsed_time) * AV_TIME_BASE);
				}
				else
				{
					// ��������ӳٻ���ʵ���ӳ�Ϊ����������Ҫ������֡����
					i64_delay = X_AVSYNC_SKIP_FRAME;
				}

				printf("%lf\t%lf\n", delay, diff);
				return i64_delay;
			}

		};
    }
}
#endif //_X_FFMPEG_AV_SYNC_H_