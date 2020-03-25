#ifndef _X_FFMEPG_DEMULTIPLEXER_H_
#define _X_FFMEPG_DEMULTIPLEXER_H_

#include "x_ffmpeg_common.h"

namespace x
{
    namespace ffmpeg
    {
		// �⸴����ػص��ӿ�
		class xDemultiplexEvent
		{
		public:
			virtual void DuxStart() {};
			virtual void DuxPacket(AVPacket* _data, int _type) {};
			virtual void DuxEnd() {};
		};

		// �⸴����
		class xDemultiplexer
		{
		private:
			AVFormatContext* av_fmt_ctx_;	// ��������Ҳ��֪����ʲô�ã��ٷ����ͽ� Format I/O context
											// ͨ������ṹ������õ� ����(�ļ�)����ϸ��Ϣ(����Ƶ��������֡��������Ƶ�����������ȵ�)
			volatile bool run_flag_;
			std::thread dux_thread_;		// �⸴���߳�
			xDemultiplexEvent* event_;

			int vs_index_;	// ��Ƶ�� ��AVFormatContext::streams�е����� һ����Ƶ�� 0
			int as_index_;	// ��Ƶ�� ��AVFormatContext::streams�е����� һ����Ƶ�� 1
		public:
			xDemultiplexer();
			~xDemultiplexer();
		private:
			void dux_function();
		public:
			// �򿪲���ʼ���⸴�������
			bool Open(const char* _input);

			// ��ʼ�⸴��
			bool Start(xDemultiplexEvent* _event);

			// �ڿ���̨��ӡ��������(�ļ�)�Ļ�����Ϣ
			bool Info();

			// ���� ��Ƶ�� ��AVFormatContext::streams�е�����
			int AudioStreamIndex();

			// ���� ��Ƶ�� ��AVFormatContext::streams�е�����
			int VideoStreamIndex();

			// ���� AVFormatContext
			AVFormatContext* FormatContext();

			// �رս⸴���������ͷ���Դ
			void Close();
		};
    }
}

#endif //_X_FFMEPG_DEMULTIPLEXER_H_