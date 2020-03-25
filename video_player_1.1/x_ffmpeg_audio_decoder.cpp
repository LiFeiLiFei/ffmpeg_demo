#include "x_ffmpeg_audio_decoder.h"

namespace x
{
    namespace ffmpeg
    {
		xAudioDecoder::xAudioDecoder()
		{
			src_frame_ = NULL;

			swr_ctx_ = NULL;
			swr_ch_layout_ = -1;
			swr_sample_fmt_ = AVSampleFormat::AV_SAMPLE_FMT_NONE;
			swr_sample_rate_ = -1;
			swr_nb_samples_ = -1;
		}
		xAudioDecoder::~xAudioDecoder()
		{
			Close();
		}

		bool xAudioDecoder::GetSrcParameters(int& _sample_rate, int& _nb_samples, int64_t& _ch_layout, enum AVSampleFormat& _sample_fmt)
		{
			if (av_dec_ctx_ == NULL)
				return false;

			_sample_rate = av_dec_ctx_->sample_rate;
			_nb_samples = av_dec_ctx_->frame_size;
			_ch_layout = av_dec_ctx_->channel_layout;
			_sample_fmt = av_dec_ctx_->sample_fmt;

			return true;
		}
		bool xAudioDecoder::GetDstParameters(int& _sample_rate, int& _nb_samples, int& _channels, enum AVSampleFormat& _sample_fmt)
		{
			if (swr_ctx_ == NULL)
				return false;

			_sample_rate = swr_sample_rate_;
			_nb_samples = swr_nb_samples_;
			_channels = av_get_channel_layout_nb_channels(swr_ch_layout_);//�����������ֻ�ȡ������
			_sample_fmt = swr_sample_fmt_;
			return true;
		}

		bool xAudioDecoder::Open(xDemultiplexer& _duxer)
		{
			if (run_flag_ == true)
				return false;

			AVStream* as = _duxer.FormatContext()->streams[_duxer.AudioStreamIndex()];

			d_timebase_ = av_q2d(as->time_base);	// ��ȡ��Ƶ���� [ʱ�����] ���� ffempg����ʱ��ͳһת��
			duration_ = d_timebase_ * (as->duration * 1.0);	//ʱ�����*����ʱ��(ffmepg��Ƶ��ʱ�䵥λ) = ����ʱ��(��)

			//�����ǳ������������function��ע��

			av_dec_ctx_ = avcodec_alloc_context3(NULL);
			if (NULL == av_dec_ctx_)
				return false;

			avcodec_parameters_to_context(av_dec_ctx_, as->codecpar);

			av_codec_ = avcodec_find_decoder(av_dec_ctx_->codec_id);
			if (NULL == av_codec_)
				return false;

			if (avcodec_open2(av_dec_ctx_, av_codec_, NULL) < 0)
			{
				//printf("[LOG] [ERROR] [xReader] [Init] [video] [avcodec_open2]\n");
				return false;
			}

			rate_ = av_dec_ctx_->sample_rate;

			src_frame_ = av_frame_alloc();

			return true;
		}
		bool xAudioDecoder::SetSwr(int64_t _ch_layout, enum AVSampleFormat _sample_fmt, int _sample_rate)
		{
			if (run_flag_ == true)
				return false;

			swr_ch_layout_ = _ch_layout;
			swr_sample_fmt_ = _sample_fmt;
			swr_sample_rate_ = _sample_rate;

			// ����SwrContext��ز���
			swr_ctx_ = swr_alloc_set_opts(
				NULL,
				swr_ch_layout_,					// Ŀ����������
				swr_sample_fmt_,				// Ŀ�������ʽ
				swr_sample_rate_,				// Ŀ�������
				av_dec_ctx_->channel_layout,	// ԭʼ��������
				av_dec_ctx_->sample_fmt,		// ԭʼ������ʽ
				av_dec_ctx_->sample_rate,		// ԭʼ������
				0,								// ��־����
				NULL);							// ��־������

			if (!swr_ctx_ || swr_init(swr_ctx_) < 0)
				return false;

			// ����ffmepg�ٷ�ʾ���ṩ�ļ����ز���֮��� ��֡������������ �ķ���
			// ע�⣬frame_size ��ԭʼ�� ��֡������������ ����ԭʼ֡��byte����
			// ����AV_SAMPLE_FMT_S16����֡����1024������������֡byte������1024*������*sizeof(short)
			// ��֡�������������Ͳ��������
			swr_nb_samples_ = (int)av_rescale_rnd(
				swr_get_delay(swr_ctx_,av_dec_ctx_->sample_rate) + av_dec_ctx_->frame_size,
				swr_sample_rate_,
				av_dec_ctx_->sample_rate,
				AV_ROUND_INF);

			return true;
		}
		bool xAudioDecoder::Start(xDecoderEvent* _event)
		{
			if (run_flag_ == true)
				return false;

			event_ = _event;
			run_flag_ = true;

			decode_thread_ = std::thread(&xAudioDecoder::decode_function, this);

			return true;
		}
		void xAudioDecoder::Close()
		{
			run_flag_ = false;

			if (decode_thread_.joinable())
			{
				decode_thread_.join();
			}


			if (src_frame_ != NULL)
			{
				av_frame_free(&src_frame_);
				src_frame_ = NULL;
			}

			if (av_dec_ctx_ != NULL)
			{
				avcodec_close(av_dec_ctx_);
				avcodec_free_context(&av_dec_ctx_);
				av_dec_ctx_ = NULL;
				av_codec_ = NULL;
			}

			if (swr_ctx_ != NULL)
			{
				swr_close(swr_ctx_);
				swr_free(&swr_ctx_);
				swr_ctx_ = NULL;
			}

			AVPacket* pkt = NULL;

			while (qpkt_.TryPop(pkt))
			{
				if (pkt != NULL)
				{
					av_packet_free(&pkt);
				}
			}
		}
		bool xAudioDecoder::SendPacket(AVPacket* _pkt)
		{
			AVPacket* tpkt = av_packet_clone(_pkt); //����AVPacket��ʵ�����Ƕ�AVPacket�ڲ������ü���+1
			qpkt_.MaxSziePush(tpkt,&run_flag_);

			return true;
		}

		void xAudioDecoder::decode_function()
		{
			int ret = 0;
			AVPacket* pkt = NULL;
			while (run_flag_)
			{
				if (qpkt_.TryPop(pkt))
				{
					if (pkt == NULL)
					{
						run_flag_ = false;
					}
					else
					{
						if (avcodec_send_packet(av_dec_ctx_, pkt) < 0)
						{
							run_flag_ = false;
						}
						else
						{
							//����������õ�ԭʼPCM֡
							while (ret = avcodec_receive_frame(av_dec_ctx_, src_frame_), run_flag_)
							{
								if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
								{
									break;
								}

								else if (ret < 0)
								{
									run_flag_ = false;
								}
								else if (ret == 0)
								{
									uint8_t* dst_buf = NULL;
									int dst_buf_size = 0;

									// Ϊ�ز���Ŀ��PCM֡����洢�ռ�
									if (av_samples_alloc(
										&dst_buf,		// Ŀ��buffer
										&dst_buf_size,	// Ŀ��buffer��С
										av_get_channel_layout_nb_channels(swr_ch_layout_), //Ŀ��ͨ����
										swr_nb_samples_,	//Ŀ�������
										swr_sample_fmt_,	//Ŀ�������ʽ
										1) < 0)
									{
										run_flag_ = false;
									}
									else
									{
										//�Ӳ��������buffer
										ret = swr_convert(
											swr_ctx_,			//SwrContext
											&dst_buf,			//Ŀ��PCM buffer
											swr_nb_samples_,	//Ŀ�������
											(const uint8_t**)src_frame_->data, //ԭʼPCM buffer
											src_frame_->nb_samples); //ԭʼ������

										if (event_ != NULL)
											event_->AudioEvent(dst_buf, dst_buf_size, src_frame_->best_effort_timestamp, this);

										av_free(dst_buf);
									}
								}
							}
						}

						av_packet_free(&pkt);
					}
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
				}
			}
		}
    }
}