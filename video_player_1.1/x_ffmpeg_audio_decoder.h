#ifndef _X_FFMPEG_AUDIO_DECODER_H_
#define _X_FFMPEG_AUDIO_DECODER_H_

# include "x_ffmpeg_decoder.h"
/*
���� רҵ���� ����
1.������
	����Ƶ�ʣ�Ҳ��Ϊ�����ٶȻ��߲����ʣ�������ÿ���
	�����ź�����ȡ�������ɢ�źŵĲ������������ú��ȣ�Hz��
	����ʾ������Ƶ�ʵĵ����ǲ������ڻ��߽�������ʱ�䣬
	���ǲ���֮���ʱ������ͨ�׵Ľ�����Ƶ����ָ����
	��ÿ���Ӳɼ����ٸ��ź�������

	������������:
	22050Hz ���ߵ�㲥���ò����ʣ��㲥����
	44100Hz ��ƵCD��Ҳ������MPEG-1��Ƶ��VCD��SVCD��MP3�����ò�����
	48000Hz miniDV�����ֵ��ӡ�DVD��DAT����Ӱ��רҵ��Ƶ���õ������������ò�����

2.�������ֺ�������
	��������ָ channel_layout.h �������ط�λ������
	AV_CH_FRONT_LEFT	������
	AV_CH_LAYOUT_STEREO(AV_CH_FRONT_LEFT|AV_CH_FRONT_RIGHT)	˫����(������&������)

	������ �����м�������
	˫���� 2 ��

3.��Ƶ�Ĳ�����ʽ
	������ʽָ PCM���ݵĲ������Ⱥʹ洢��ʽ(ע�⣬aac����ѹ���㷨ѹ�����PCM���ݣ�������Ƶ������ʽ)
	����enum AVSampleFormat��

	���������ǵ�λʱ���ڶ������ϵ�һ������������ģ����������(���ֻ�)�Ĵ����ȣ���:
	AV_SAMPLE_FMT_S16����ʾ�����������β���ʹ�� signed 16 bits ���洢������Ϊ[-32768,32767]
	AV_SAMPLE_FMT_FLT����ʾ�����������β���ʹ�� float ���洢������Ϊ[-1.0,1.0]

	��Ƶ�Ĳ�����ʽ��Ϊƽ�棨planar���ʹ����packed���������ͣ���ö��ֵ���ϰ벿����packed
	���ͣ����棨��P��׺�ģ���planar���͡�

	��˫����Ϊ��(L&R)
	ƽ�� ����ͬ����������AVFrame::data[0]�н���洢��LRLRLRLRLRLRLRL......

	��� ��������AVFrame::data[����]�е����洢��
	AVFrame::data[0] LLLLLLLLLLL....
	AVFrame::data[1] RRRRRRRRRRR....

*/
namespace x
{
    namespace ffmpeg
    {
		class xAudioDecoder:
			public x::ffmpeg::xDecoder
		{
		private:
			AVFrame* src_frame_;
		private:
			SwrContext*			swr_ctx_;			//������Ƶ����ת��
			int64_t				swr_ch_layout_;
			enum AVSampleFormat	swr_sample_fmt_;
			int					swr_sample_rate_;
			int					swr_nb_samples_;
		private:
			void decode_function();
		public:
			xAudioDecoder();
			~xAudioDecoder();
		public:
			// ��ȡԭʼ��Ƶ��(PCM) �Ĳ����ʡ���֡������������
			bool GetSrcParameters(int& _sample_rate, int& _nb_samples, int64_t& _ch_layout, enum AVSampleFormat& _sample_fmt);

			// ��ȡ�ز�����Ƶ��(PCM) �Ĳ����ʡ���֡������������
			bool GetDstParameters(int& _sample_rate, int& _nb_samples, int& _channels, enum AVSampleFormat& _sample_fmt);
		public:
			bool Open(xDemultiplexer& _duxer);


			// �����ز������� 
			// @param	_ch_layout		��������
			// @param	_sample_fmt		�ز�����ʽ
			// @param	_sample_rate	�ز���������
			bool SetSwr(int64_t _ch_layout, enum AVSampleFormat _sample_fmt, int _sample_rate);
			bool Start(xDecoderEvent* _event);

			// ���⸴�õõ�����Ƶ�����ݶ�����л���
			bool SendPacket(AVPacket* _pkt);
			void Close();
		};
    }
}

#endif //_X_FFMPEG_AUDIO_DECODER_H_