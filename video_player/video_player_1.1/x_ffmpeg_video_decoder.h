#ifndef _X_FFMPEG_VIDEO_DECODER_H_
#define _X_FFMPEG_VIDEO_DECODER_H_

# include "x_ffmpeg_decoder.h"

namespace x
{
    namespace ffmpeg
    {
		//��Ƶ������
		//�󲿷�ԭ������̶��������Ƶ������
		class xVideoDecoder :
			public xDecoder
		{
		private:
			AVFrame* src_frame_;
			uint8_t* dst_frame_buf_;
			AVFrame* dst_frame_;
		private:
			SwsContext* sws_ctx_;	//����ͼ��ת��
			AVPixelFormat sws_fmt_; //ͼ��ת��Ŀ���ʽ
			int sws_w_;	//ͼ��ת��Ŀ���
			int sws_h_; //ͼ��ת��Ŀ���
		private:
			void decode_function();
		public:
			xVideoDecoder();
			~xVideoDecoder();
		public:
			// ԭʼͼ����� ���ߡ���ʽ
			bool GetSrcParameters(int& _w, int& _h, AVPixelFormat& _fmt);

			// ת��ͼ����� ���ߡ���ʽ
			bool GetDstParameters(int& _w, int& _h, AVPixelFormat& _fmt);
		public:
			// �򿪽�����
			bool Open(xDemultiplexer& _duxer);

			// ����ͼ��ת������
			// @param	_dst_w		Ŀ���
			// @param	_dst_h		Ŀ���
			// @param	_dst_fmt	Ŀ���ʽ
			// @param	scale_flag	ת���㷨 
			bool SetSws(int _dst_w = -1, int _dst_h = -1, AVPixelFormat _dst_fmt = AVPixelFormat::AV_PIX_FMT_NONE, int scale_flag = SWS_FAST_BILINEAR);

			// ��ʼ����
			bool Start(xDecoderEvent* _event);

			// ���⸴�õõ�����Ƶ�����ݶ�����л���
			bool SendPacket(AVPacket* _pkt);
			void Close();
		};
    }
}

#endif //_X_FFMPEG_VIDEO_DECODER_H_